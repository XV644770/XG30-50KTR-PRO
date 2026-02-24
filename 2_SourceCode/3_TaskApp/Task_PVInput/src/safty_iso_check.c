/*
 * safty_iso_check.c
 *
 *  Created on: 2021年5月21日
 *      Author: Administrator
 */


#include "dsp_user_include.h"

void PVISOCheck(void)
{
	static Uint16	suwISOCheckCnt=0;
	static int16	swISOVolt1Avg=0,	swISOVolt2Avg=0;
	static int32	sdISOVoltSum=0;
	static Uint16	suwISODetectTimes=0;

	int16	wInternalResistance;
	int32	dISOResistanceGxy;

//	unSysFlag.bit.ISOCheckFinished= 1;
//	return;

	stDebug.ReadData.uwIsoCheckCnt = suwISOCheckCnt;
	if((cInitialStatus == eInverterStatus)
	 ||(cInverterStatus == eInverterStatus)
	 ||(1==unSysFlag.bit.PvVoltLow)
	 ||(1==unSysFlag.bit.ISOCheckFinished)
	 ||(FREQ_NULL == stFreqRang.uwFreqFlag)
	 ||((0!=stSysFaultReg.unFaultBit.all)&&(0==stSysFaultReg.unFaultBit.bit.ISO)))		// inverter fault & no ISO fault
	{
		ISO_RELAY1_CLEAR;
		ISO_RELAY2_CLEAR;
		suwISOCheckCnt = 0;
		suwISODetectTimes = 0;
	}
	else if(ENABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.ISO)		// enable ISO check
	{
		suwISOCheckCnt++;
		if(suwISOCheckCnt <= 5)		// 5*20ms = 100ms
		{
			stDCSample.wISORelay1Volt = 0;
			stDCSample.wISORelay2Volt = 0;
			ISO_RELAY1_CLEAR;
			ISO_RELAY2_CLEAR;
		}
		else if(suwISOCheckCnt <= 55)		// (55-5)*20ms = 1000ms		/**************step1: ISO1-High ISO2-Low*******************/
		{
			sdISOVoltSum = 0;
			ISO_RELAY1_SET;
			ISO_RELAY2_CLEAR;
		}
		else if(suwISOCheckCnt <= 87)		// (87-55)*20ms = 640ms
		{
			sdISOVoltSum += (int32)stDCSample.wISOVolt;
		}
		else if(suwISOCheckCnt <= 88)
		{
			swISOVolt1Avg = (sdISOVoltSum>>5);
			stDCSample.wISORelay1Volt = swISOVolt1Avg;
		}
		else if(suwISOCheckCnt <= 138)		// (138-88)*20ms = 1000ms /**************step1: ISO1-High ISO2-Low*******************/
		{
			sdISOVoltSum = 0;
			ISO_RELAY1_CLEAR;
			ISO_RELAY2_SET;
		}
		else if(suwISOCheckCnt <= 170)		// (170-138)*20ms = 640ms
		{
			sdISOVoltSum += (int32)stDCSample.wISOVolt;
		}
		else if(suwISOCheckCnt <= 171)
		{
			swISOVolt2Avg = (sdISOVoltSum>>5);
			stDCSample.wISORelay2Volt = swISOVolt2Avg;
		}
		else if(suwISOCheckCnt <= 172)		// 172*20ms = 3.44s
		{
			ISO_RELAY1_CLEAR;
			ISO_RELAY2_CLEAR;
			if(swISOVolt1Avg>swISOVolt2Avg)
			{
				dISOResistanceGxy = ((int32)stDCSample.wBusVolt*5988)/(swISOVolt1Avg-swISOVolt2Avg) - 7988;
				if(TRUE==stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)
				{
					wInternalResistance = stMachineCfg.wISOInternalRes_N;		// 346K
				}
				else
				{
					wInternalResistance = stMachineCfg.wISOInternalRes;		// 346K
				}
				dISOResistanceGxy = dISOResistanceGxy - ((int32)6988000/wInternalResistance);

				if(dISOResistanceGxy > 1)		// positive number
				{
					stDCSample.uwISOResistance = (Uint16)((int32)6988000/dISOResistanceGxy);
				}
				else
				{
					stDCSample.uwISOResistance = 65000;	// ISO Resistance Max = 65M
				}
			}
			else
			{
				stDCSample.uwISOResistance = 0;
			}
		}
		else if(suwISOCheckCnt <= 173)		// 83  (1660ms)
		{
			if(1==stSysFaultReg.unFaultBit.bit.ISO)
			{
				if(stDCSample.uwISOResistance > (stF107Data.wISOValue+20))		// 70k
				{
					stSysFaultReg.unFaultBit.bit.ISO = 0;		// Clear ISO Fault Logic
					unSysFlag.bit.ISOCheckFinished = 1;			// ISO Check Finished
				}
			}
			else
			{
				if(stDCSample.uwISOResistance < stF107Data.wISOValue)		// CQC 11000V/30mA=36.6k //50k
				{
					stSysFaultReg.unFaultBit.bit.ISO = 1;		// Set ISO Fault Logic
					unSysFlag.bit.ISOCheckFinished = 0;			// ISO Check Not Finished
				}
				else
				{
					stSysFaultReg.unFaultBit.bit.ISO = 0;
					unSysFlag.bit.ISOCheckFinished = 1;			// ISO Check Finished
				}
			}
			suwISODetectTimes++;
			if(suwISODetectTimes>=3)		// 3 times
			{
				suwISODetectTimes = 0;		// delay (15000*20ms = 300s) restart detect 3 times
			}
			else
			{
				suwISOCheckCnt = 0;			// restart detect 3 times
			}
		}
		else if(suwISOCheckCnt >= 15173)		// Check after 5 minutes
		{
			suwISOCheckCnt = 0;
		}
	}
	else			// disable ISO check
	{
		stDCSample.uwISOResistance = 1;
		unSysFlag.bit.ISOCheckFinished = 1;			// ISO Check Finished
		stSysFaultReg.unFaultBit.bit.ISO = 0;		// clear ISO Fault
		suwISODetectTimes = 0;
		suwISOCheckCnt = 0;
	}

}
