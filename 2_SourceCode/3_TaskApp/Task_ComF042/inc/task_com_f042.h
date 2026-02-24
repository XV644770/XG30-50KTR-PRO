/*
* task_com_f042.h
*
*	Product:			three phase inveter
*	Filename:			task_com_f042.h
*	Purpose:			dsp and stm32f0 communicate
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef TASK_COM_F042_H
#define TASK_COM_F042_H

#define CLR_F042_COM_FAULT_CNT				20		// 100ms * 20 = 2s
#define SET_F042_COM_FAULT_CNT				100		// 100ms * 100 = 10s

/**************SCI Tx/Rx Buffer**************************/
typedef	struct{
	Uint16 uwRxBuf[DSP_RX_M0_SIZE];
	Uint16 uwRxBufBak[DSP_RX_M0_SIZE];
	Uint16 uwTxBuf[DSP_TX_M0_SIZE];
	Uint16 uwTxBufBak[DSP_TX_M0_SIZE];
	Uint16 uwRxDataLen;
}ST_DSP_M0_UART;

typedef struct
{
    Uint16 uwFaultCode;
    Uint16 uwFrameIndex;
    Uint16 uwSoftWareVersion;
    Uint16 uwSoftWareChar;
}ST_F042_PARA;   //The structure for F042 System Parameter

extern ST_F042_PARA	  stF042Para;
extern ST_DSP_M0_UART stDspM0Uart;

extern void ComF042Task(void);
extern void ComF042ParaInit(void);

#endif /* TASK_COM_F107_H */
