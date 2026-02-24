/******************************************************************
*	Copyright (c) 2020-2021, Invt Co.Lrd.
*	ALL  RIGHTS  RESERVED

*	Product:           three phase inveter 
*	Filename:        driver_uart_com.c
*	Purpose:          uart driver programm
*	Date              Version           Author            Note   
*   20/11/28          V1.01             csx            none
***************************************************************/
#include "dsp_user_include.h"
#include "F2807x_Sci.h"

ST_SCI_RX_STATUS stSciRx[SCI_MAX_NUM];
ST_SCI_TX_STATUS stSciTx[SCI_MAX_NUM];

INT16U	wBinaryMode=0;
INT16U	wHighByte=0;
INT16U	wLowByte=0;
INT16U	wSendHighHalfByte=0;

/****************************************************************************
*
*	FunctionName:	UartReceDriverInit
*	Input:	
*		@bUartIndex:	UART index value, representing his array
*		@uwRxSize:	receive array length
*		@pbUart:		A pointer to the address of the received array
*	Output:	none
*	Description:	uart receiver driver init; includes peripherals and variable
*	Note:	none.
***************************************************************/
void UartReceDriverInit(Uint16 uwUartIndex, Uint16 uwRxSize, Uint16 *puwUart)
{
	ST_SCI_RX_STATUS *pstUartRx;
	memset(puwUart, 0x00, uwRxSize);

	pstUartRx = &stSciRx[uwUartIndex];
	pstUartRx->bRxStatus = SCI_RX_BUF_EMPTY;
	pstUartRx->uwRxLength = 0;
	pstUartRx->uwRxCnt = 0;
	pstUartRx->uwReadCnt = 0;
	pstUartRx->bpRxBuffer = puwUart;
	pstUartRx->uwRxSize = uwRxSize;	
}

/****************************************************************************
*
*	FunctionName:	UART_RXData
*	Input:	
*		@bUartIndex:	UART index value, representing his array
*		@bRxData:	data form receive
*	Output:	none
*	Description:		 get data form uart
*	Note:	none.
***************************************************************/
void UART_RXData(Uint16 uwUartIndex, Uint16 uwRxData)
{
	ST_SCI_RX_STATUS *pstUartRx;
	
	static Uint16 suwF107TranLen = 0, suwF042TranLen = 0, suwAfciTranLen = 0;

	pstUartRx = &stSciRx[uwUartIndex];
	if(pstUartRx->uwReadCnt == pstUartRx->uwRxCnt)
	{
		if(pstUartRx->uwRxCnt != 0)
		{
			pstUartRx->uwReadCnt = 0;
			pstUartRx->uwRxCnt = 0;
		}
	}
	if(pstUartRx->uwRxCnt < pstUartRx->uwRxSize)
	{

		*(pstUartRx->bpRxBuffer + pstUartRx->uwRxCnt) = uwRxData;
		if(DSP_F107_SCID == uwUartIndex)
		{
			if((*(pstUartRx->bpRxBuffer + 0)) == 0xAA)		// STM32F107 Send Flag 0xAA	01011010
			{
				pstUartRx->uwRxCnt++;
				if(pstUartRx->uwRxCnt==4)
				{
					suwF107TranLen = *(pstUartRx->bpRxBuffer+2);		// Len_Low
					suwF107TranLen = suwF107TranLen*256+*(pstUartRx->bpRxBuffer+3)+6;	// START+CMD+Len_L+Len_H+CRC_L+CRC_H
					if(suwF107TranLen>=(DSP_RX_SIZE+1))
					{
						pstUartRx->uwRxCnt = 0;
					}
				}
				else
				{
					if((pstUartRx->uwRxCnt==suwF107TranLen)&&(suwF107TranLen>6))
					{
						pstUartRx->uwRxCnt = 0;
						memcpy(stDspUart.uwRxBuf, pstUartRx->bpRxBuffer, suwF107TranLen);				// add this line for avoid to change the tx_buf in the task before send finish
						OSISREventSend(COM_F107_PRIO, COM_107_RX_ENDED);
					 	OSTaskRdy(COM_F107_PRIO);
					}
				}							
			}					
			else 
			{
				pstUartRx->uwRxCnt = 0;
			}			
		}
		else if(DSP_F042_SCIB == uwUartIndex)
		{
			if((*(pstUartRx->bpRxBuffer + 0)) == 0xAA)		// Dsp Send Flag 0x55 01010101
			{
				pstUartRx->uwRxCnt++;
				if(pstUartRx->uwRxCnt==4)
				{
					suwF042TranLen = *(pstUartRx->bpRxBuffer+2);
					suwF042TranLen = suwF042TranLen*256+*(pstUartRx->bpRxBuffer+3)+6;
					if(suwF042TranLen>DSP_RX_M0_SIZE)
					{
						pstUartRx->uwRxCnt = 0;
					}
				}
				else
				{
					if((pstUartRx->uwRxCnt==suwF042TranLen)&&(suwF042TranLen>6))
					{
						pstUartRx->uwRxCnt = 0;
						memcpy(stDspM0Uart.uwRxBuf, pstUartRx->bpRxBuffer, suwF042TranLen);				// add this line for avoid to change the tx_buf in the task before send finish
						OSISREventSend(COM_F042_PRIO, COM_F042_RX_ENDED);
						OSTaskRdy(COM_F042_PRIO);
					}
				}							
			}					
			else 
			{
				pstUartRx->uwRxCnt = 0;
			}			
		}
		else if(DSP_AFCI_SCIC == uwUartIndex)
		{
			//if((*(pstUartRx->bpRxBuffer + 0)) == AFCI_ADDRESS)		// Dsp Send Flag 0x55 01010101
			if((*(pstUartRx->bpRxBuffer + 0)) == stAfciUart.uwCommAddr)
			{
				pstUartRx->uwRxCnt++;
				if(pstUartRx->uwRxCnt==3)
				{
					if((*(pstUartRx->bpRxBuffer + 1)) == 0x03)	// 03 read function code
					{
						suwAfciTranLen = *(pstUartRx->bpRxBuffer + 2) + 5;
						if((suwAfciTranLen>AFCI_RX_SIZE)||(suwAfciTranLen<=6))
						{
							pstUartRx->uwRxCnt = 0;
						}
					}
					else
					{
						pstUartRx->uwRxCnt = 0;
					}
				}
				else
				{
					if((pstUartRx->uwRxCnt>=suwAfciTranLen)&&(suwAfciTranLen>6))
					{
						pstUartRx->uwRxCnt = 0;
						memcpy(stAfciUart.uwRxBuf, pstUartRx->bpRxBuffer, suwAfciTranLen);				// add this line for avoid to change the tx_buf in the task before send finish
						OSISREventSend(COM_AFCI_PRIO, COM_AFCI_RX_ENDED);
						OSTaskRdy(COM_AFCI_PRIO);
					}
				}
			}
			else
			{
				pstUartRx->uwRxCnt = 0;
			}
		}
		else 	// (bUartIndex == RS485_UART4)
		{
			pstUartRx->uwRxCnt++;
		}
	}
	else 
	{
		//pstUartRx->uwRxCnt = pstUartRx->uwRxSize;
		pstUartRx->uwRxCnt = 0;
	}
}

/****************************************************************************
*
*	FunctionName:	UART_TxData
*	Input:	
*		@bUartIndex:	UART index value, representing his array
*		@USARTx:	A pointer to the address of the uart
*	Output:	none
*	Description:		Transmits single data through the USARTx peripheral.
*	Note:	none.
*****************************************************************************/
void UART_TxData(Uint16 uwUartIndex,volatile struct SCI_REGS * pstSciRegs)
{
	ST_SCI_TX_STATUS *pstUartTx;
	pstUartTx = &stSciTx[uwUartIndex];

	if(pstUartTx->uwTxCnt < pstUartTx->uwTxLength)
	{
		if(uwUartIndex == RS485_UART4 && wBinaryMode==1)
		{
			pstSciRegs->SCICTL2.bit.TXINTENA = 1;
			if(wSendHighHalfByte==1)
			{
				sSplit(*(pstUartTx->bpTxBuffer+ pstUartTx->uwTxCnt));
				pstSciRegs->SCITXBUF.all = wHighByte;
				wSendHighHalfByte=0;
			}
			else
			{						
				wSendHighHalfByte=1;
				pstSciRegs->SCITXBUF.all = wLowByte;				
				
				pstUartTx->uwTxCnt ++;
			}
		}
		else
		{
			pstSciRegs->SCICTL2.bit.TXINTENA = 1;
			pstSciRegs->SCITXBUF.all = *(pstUartTx->bpTxBuffer + pstUartTx->uwTxCnt);
			pstUartTx->uwTxCnt ++;
		}
		
	}
	else if((pstUartTx->uwTxCnt >= pstUartTx->uwTxLength) && (pstUartTx->uwTxLength != 0))
	{
		pstSciRegs->SCICTL2.bit.TXINTENA = 0;
		
		if(uwUartIndex == RS485_UART4)
			wBinaryMode=0;
	}

}

/****************************************************************************
*
*	FunctionName:	bSciRead
*	Input:	
*		@bUartIndex:	UART index value, representing his array
*		@pbOutBuf:	A pointer to the receiver buffer of the uart
*	Output:	
*		@SCI_RX_BUF_EMPTY:	SCI Receive Buffer is empty.
*		@SCI_RX_READY:	SCI Receive Buffer is ready,we can read data
*	Description:		 after the data is completely sent, pull the enable pin down
*					Pull up the pin in advance and send data
*	Note:	none.
**************************************************************************/

Uint16 uwSciRead(Uint16  uwUartIndex, Uint16  *puwOutBuf)
{ 
	ST_SCI_RX_STATUS *pstUartRx;
	pstUartRx = &stSciRx[uwUartIndex];
	if(pstUartRx->uwReadCnt == pstUartRx->uwRxCnt)
	{
		return (SCI_RX_BUF_EMPTY);
	}
	if(pstUartRx->uwReadCnt < pstUartRx->uwRxSize)
	{
		*puwOutBuf = *(pstUartRx->bpRxBuffer + pstUartRx->uwReadCnt);
		pstUartRx->uwReadCnt++;
		return (SCI_RX_READY);
	}
	else
	{
		pstUartRx->uwReadCnt = pstUartRx->uwRxSize;
		return (SCI_RX_BUF_EMPTY);
	}
}

/****************************************************************************
*
*	FunctionName:	uwSciWrite
*	Input:	
*		@bUartIndex:	UART index value, representing his array
*		@pbTxBuf:	A pointer to the send buffer of the uart
*		@pTxBufTmp:	A pointer to the send bakup buffer of the uart,Prevent data from being changed during transmission
*		@wLength:	the length of tx buffer
*	Output:	
*		@SCI_TX_BUSY:	SCI sending,UASRT is busy;we can not send data
*		@SCI_TX_READY:	UART is idle,we can send data now
*	Description:		 uart send data 
*	Note:	none.
**************************************************************************/
Uint16 uwSciWrite(Uint16 uwUartIndex, Uint16* pbTxBuf, Uint16* pbTxBufTmp, Uint16 uwLength)
{
	ST_SCI_TX_STATUS *pstUartTx;

	pstUartTx = &stSciTx[uwUartIndex];
	
	if(uwLength == 0)
	{
		return(SCI_TX_READY);
	}
	memcpy(pbTxBuf, pbTxBufTmp, uwLength);				// add this line for avoid to change the tx_buf in the task before send finish
	pstUartTx->uwTxLength = uwLength;
	pstUartTx->uwTxCnt = 0;
	pstUartTx->bpTxBuffer = pbTxBuf;
	
	if(uwUartIndex == DSP_F107_SCID)
	{
		UART_TxData(uwUartIndex, &ScidRegs);
	}
	else if(uwUartIndex == DSP_F042_SCIB)
	{
		UART_TxData(uwUartIndex, &ScibRegs);
	}
	else if(uwUartIndex == RS485_UART4)
	{
		UART_TxData(uwUartIndex, &SciaRegs);
	}
	else if(uwUartIndex == DSP_AFCI_SCIC)
	{
		UART_TxData(uwUartIndex, &ScicRegs);
	}
	else
	{
		return(SCI_TX_BUSY);
	}
	return(SCI_TX_READY);
}

/******************************************************************
* Function Name: 	sSplit                                 *
*                                                                 *
* Parameters                                                      *
*                                                                 *
* Retruns                                                         *
*                                                                 *
*                                                                 *
*******************************************************************/
		
void sSplit(INT16U wSendData)
{                
	
	wHighByte=wSendData>>8;
	wLowByte=wSendData&0x00FF;
}


/******************************************************************
* Function Name: 	uwSciWriteBinary                      	      *
*                                                                 *
* Parameters                                                      *
*                                                                 *
* Retruns                                                         *
*                                                                 *
*                                                                 *
*******************************************************************/
Uint16 uwSciWriteBinary(Uint16 uwUartIndex, Uint16* pbTxBuf,  Uint16 uwLength)
{
	ST_SCI_TX_STATUS *pstUartTx;

	pstUartTx = &stSciTx[uwUartIndex];
	
	if(uwLength == 0)
	{
		return(SCI_TX_READY);
	}
	OS_ENTER_CRITICAL();
	wBinaryMode=1;
	pstUartTx->uwTxLength = uwLength;
	pstUartTx->uwTxCnt = 0;
	pstUartTx->bpTxBuffer = pbTxBuf;
	wSendHighHalfByte=1;
	
	if(uwUartIndex == RS485_UART4)
	{
		UART_TxData(uwUartIndex, &SciaRegs);
	}
	else
	{
		OS_EXIT_CRITICAL();
		return(SCI_TX_BUSY);
	}
	OS_EXIT_CRITICAL();
	return(SCI_TX_READY);
}
