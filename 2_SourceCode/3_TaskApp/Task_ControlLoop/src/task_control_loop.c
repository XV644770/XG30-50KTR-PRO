/*
 * task_control_loop.c
 *
 *  Created on: 2021年5月15日
 *      Author: Administrator
 */

#include "dsp_user_include.h"


/****************************************************************************
*
*	FunctionName:	StatusTask
*	Input:	none
*	Output:	none
*	Description:	DSP Status Change Task, quicly on/off inverter
*	Note:	none.
***************************************************************/
void ControlLoopTask(void)
{
	static TASK_EVENT event = 0;
	Uint16 suwPVIndex=0;
    int16 shutdown=0;

	while(1)
	{
		event = OSEventPend();		// idle

		if(event&(1<<CONTROL_LOOP_PERIOD))		// task period 1ms
		{
			 CalcAdcRealValueSumTemp();
			if(DC_SOURCE == stSysCfg.eMpptMode)
			{
				if(stSysCfg.uwFastDCMode == 0)
				{
					InvActivePowerLoopCtrl();
				}
			}
			else if(PARALLEL == stSysCfg.eMpptMode)
			{
				InvPVVoltLoopCtrl(stMpptTskCtrl.uwPVVoltMaxID);
			}
			else
			{
				for(suwPVIndex=0;	suwPVIndex<stSysCfg.uwPVNumber;	suwPVIndex++)
				{
					InvPVVoltLoopCtrl(suwPVIndex);
				}
			}
			FanSpeedDetect();                              // 1.7us
			 InvCurrCtrlParmUpdate();
			 shutdown++;
			if(shutdown > 70)
			{
			    shutdown = 0;
			    sSPSShutDownChk();
			}
		}
	}
}

void sSPSShutDownChk(void)
{
	static INT16U wShutDownCnt;
	if((cInverterStatus != eInverterStatus) && (cInitialStatus != eInverterStatus)
		&& (stDCSample.wBusVolt1ms < VDC150V))
	{
		if(++wShutDownCnt > 60)
		{
			wShutDownCnt = 0;
			mSetSPS_SHUTDOWN();
		}
	}
	else if(stDCSample.wBusVolt1ms > VDC160V)
	{
		wShutDownCnt = 0;
		mClrSPS_SHUTDOWN();
	}
}

