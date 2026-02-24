/*
 * Task_pv_input.c
 *
 *  Created on: 2020年11月26日
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
***************************************************************************/
void PVInputTaskParaInit(void)
{
	MpptInit();
	stTaskTimer.uwStartDelayTime = 60;		// 30s
}

/****************************************************************************
*
*	FunctionName:	StatusTask
*	Input:	none
*	Output:	none
*	Description:	DSP Status Change Task, quicly on/off inverter
*	Note:	none.
***************************************************************/
void PVInputTask(void)
{
	static TASK_EVENT event = 0;
	Uint16 uwPVIndexTmp = 0;

	while(1)
	{
		event = OSEventPend();		// idle

		if(event&(1<<PVINPUT_GRID_ZERO))		// Zero crossing of power grid
		{
			CalcPVVoltCurrAvg();
			CheckPVVoltRange();		// PV voltage range check
			CalcPVVoltMax();		// calc PVVolt and PVRef max value

			PVPanelReserveCheck();
			PVPanelShortCheck();
			PVISOCheck();
			BusVoltSampleCheck();
			BTCurrSampleCheck();
			/****Warning Check*********/
			InverterSPDCheck();
			InverterNPECheck();

			if(INDEPENDENT == stSysCfg.eMpptMode)
			{
				for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
				{
					MPPT(uwPVIndexTmp);
				}
			}
			else if(PARALLEL == stSysCfg.eMpptMode)
			{
				MPPT(stMpptTskCtrl.uwPVVoltMaxID);
			}
		}

		if(event&(1<<PVINPUT_TASK_PERIOD))		// task period 20ms
		{
			PVOnOffCheck();			// PV connect or disconnect  check
			ConnectCountDown();		// Grid connection countdown
			BusRefCalc(stSysCfg.eMpptMode);
			ReadMachineModel();
			InvPwmChange();
		}
	}
}


