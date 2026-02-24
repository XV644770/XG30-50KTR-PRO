/******************************************************************
*	Copyright (c) 2020-2021, Invt Co.Lrd.
*	ALL  RIGHTS  RESERVED

*	Product:           three phase inveter 
*	Filename:        driver_uart_com.h
*	Purpose:          lcd screen task
*	Date              Version           Author            Note   
*   20/11/18          V1.01             csx            none
***************************************************************/


#ifndef __DRIVER_UART_COM_H
#define __DRIVER_UART_COM_H

//#include "F2807x_Sci.h"

/**************SCI Tx Status**************************/
#define SCI_TX_READY		0
#define SCI_TX_BUSY			1
/**************SCI Rx Status**************************/
#define SCI_RX_READY		0
#define SCI_RX_BUF_EMPTY	1
/**************MAX Sci Number**************************/
#define SCI_MAX_NUM			4

#define DSP_F107_SCID		0
#define DSP_F042_SCIB		1
#define RS485_UART4			2
#define DSP_AFCI_SCIC		3

/**************MODBUS Command**************************/
#define FUNCTION_CODE_03	0x03
#define FUNCTION_CODE_06	0x06
#define FUNCTION_CODE_10	0x10

#define DSP_RX_SIZE		64		// START(1) + CMD (1) + LEN_H(1) + LEN_L(1) + DATA(4-61) + CRC(2) + V-I Curve
#define DSP_TX_SIZE		550			// START(1) + CMD (1) + LEN_H(1) + LEN_L(1) + DATA(4-61) + CRC(2) 

#define DSP_TX_M0_SIZE	12
#define DSP_RX_M0_SIZE	24

/**************SCI Tx Status**************************/
typedef	struct{
	Uint16	bStartTxCnt;
	Uint16	bStopTxCnt;
	Uint16	uwTxLength;
	Uint16	uwTxCnt;
	Uint16	*bpTxBuffer;
}ST_SCI_TX_STATUS;

/**************SCI Rx Status**************************/

typedef	struct
{
	Uint16	bRxStatus;
	Uint16	uwRxLength;
	Uint16	uwRxCnt;
	Uint16	uwReadCnt;
	Uint16	uwRxSize;
	Uint16	*bpRxBuffer;
}ST_SCI_RX_STATUS;

extern INT16U	wBinaryMode;
extern INT16U	wHighByte;
extern INT16U	wLowByte;
extern INT16U	wSendHighHalfByte;

extern void UartReceDriverInit(Uint16 uwUartIndex, Uint16 uwRxSize, Uint16 *puwUart);

extern void UART_TxData(Uint16 uwUartIndex,volatile struct SCI_REGS * pstSciRegs);
extern void UART_RXData(Uint16 uwUartIndex, Uint16 uwRxData);

extern Uint16 uwSciWrite(Uint16 uwUartIndex, Uint16* pbTxBuf, Uint16* pbTxBufTmp, Uint16 uwLength);
extern Uint16 uwSciRead(Uint16  uwUartIndex, Uint16  *puwOutBuf);
extern Uint16 uwSciWriteBinary(Uint16 uwUartIndex, Uint16* pbTxBuf,  Uint16 uwLength);
extern void sSplit(INT16U wSendData);
#endif
