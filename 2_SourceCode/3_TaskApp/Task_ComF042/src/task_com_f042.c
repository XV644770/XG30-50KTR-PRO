/*
 * task_com_f042.c
 *
 *  Created on: 2020年12月26日
 *      Author: Administrator
 */
#include "dsp_user_include.h"

ST_DSP_M0_UART stDspM0Uart;
ST_F042_PARA   stF042Para;

/****************************************************************************
*
*	FunctionName:	ComF107Task
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate
*	Note:	none.
***************************************************************/
void ComF042ParaInit(void)
{
    memset((void*)&stDspM0Uart,0,sizeof(stDspM0Uart));
}

/****************************************************************************
*
*	FunctionName:	ComF042Task
*	Input:	none
*	Output:	none
*	Description:	DSP and m0 Internal communicate
*	Note:	none.
***************************************************************/
void ComF042Task(void)
{
	static TASK_EVENT event = 0;
	Uint16 uwCheckSumTmp = 0;
	Uint16 uwRxLenTmp=0;
	static Uint16 suwF042FaultSetCnt=0, suwF042FaultClrCnt=0;

	while(1)
	{
		event = OSEventPend();

		//receive stm32f042 data
		if(event&(1<<COM_F042_RX_ENDED))
		{
			uwRxLenTmp = stDspM0Uart.uwRxBuf[2];
			uwRxLenTmp = uwRxLenTmp*256+stDspM0Uart.uwRxBuf[3]+6;
			if((uwRxLenTmp<=DSP_RX_M0_SIZE)&&(uwRxLenTmp>2))
			{
				uwCheckSumTmp = uwCalcCheckSum(stDspM0Uart.uwRxBuf, (uwRxLenTmp-2));

				if(((uwCheckSumTmp&0xFF)==stDspM0Uart.uwRxBuf[uwRxLenTmp-2])
				&&((uwCheckSumTmp>>8)==stDspM0Uart.uwRxBuf[uwRxLenTmp-1]))
				{
					stF042Para.uwFrameIndex = stDspM0Uart.uwRxBuf[1];
					stF042Para.uwSoftWareVersion = (Uint16)(((Uint16)stDspM0Uart.uwRxBuf[4]*100) + stDspM0Uart.uwRxBuf[5]);
					stF042Para.uwSoftWareChar = stDspM0Uart.uwRxBuf[6];

					suwF042FaultSetCnt = 0;
					if((++suwF042FaultClrCnt) > CLR_F042_COM_FAULT_CNT)
					{
						suwF042FaultClrCnt = CLR_F042_COM_FAULT_CNT;
						stSysFaultReg.unCommunicateFault.bit.F042Sci = 0;
					}
				}
			}
		}
		else if(event&(1<<COM_F042_TIMER))
		{
			suwF042FaultSetCnt++;
			if(suwF042FaultSetCnt > SET_F042_COM_FAULT_CNT)		//10S
			{
				suwF042FaultSetCnt = 0;
				suwF042FaultClrCnt = 0;

				if(ENABLE == stDspReceData.unFuncEnable.bit.RelayCheck)		// double relay: M0 contrl Relay B
				{
					stSysFaultReg.unFaultBit.bit.CommunicateFault = 1;
					stSysFaultReg.unCommunicateFault.bit.F042Sci = 1;
				}
				else														// single relay: delete M0
				{
					stSysFaultReg.unFaultBit.bit.CommunicateFault = 0;
					stSysFaultReg.unCommunicateFault.bit.F042Sci = 0;
				}
			}
		}

		// DSP send message to stm32F042
		if(event&(1<<COM_F042_TIMER))
		{
			stDspM0Uart.uwTxBuf[0] = 0x55;
			stDspM0Uart.uwTxBuf[1] = 0x00;		// frame index(rsv)
			stDspM0Uart.uwTxBuf[2] = 0x00;		// DataLen High
			stDspM0Uart.uwTxBuf[3] = 6;			// DataLen Low     6bytes

			stDspM0Uart.uwTxBuf[4] = stF107Data.unSet.bit.FlashReady;		// flash flag
			if(1 == stWaitStatus.stRelayCheck.uwF042OnOff)
			{
				stDspM0Uart.uwTxBuf[5] = stMachineCfg.wRelayM0Duty;		// relay pwm duty control - 65%
			}
			else
			{
				stDspM0Uart.uwTxBuf[5] = 0;			// relay pwm duty control - 65%
			}
			stDspM0Uart.uwTxBuf[6] = 0x00;										// freq flag
			stDspM0Uart.uwTxBuf[7] = 0x00;
			stDspM0Uart.uwTxBuf[8] = 0x00;
			stDspM0Uart.uwTxBuf[9] = 0x00;

			uwCheckSumTmp = uwCalcCheckSum(stDspM0Uart.uwTxBuf,10);

			stDspM0Uart.uwTxBuf[10] = (uwCheckSumTmp&0x00FF);
			stDspM0Uart.uwTxBuf[11] = ((uwCheckSumTmp&0xFF00)>>8);
			uwSciWrite(DSP_F042_SCIB,stDspM0Uart.uwTxBufBak,stDspM0Uart.uwTxBuf,12);
		}
	}
}


