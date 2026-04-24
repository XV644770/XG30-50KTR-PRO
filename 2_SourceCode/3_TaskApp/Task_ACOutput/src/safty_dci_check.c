/*
 * safty_dci_check.c
 *
 *  Created on: 2021年3月11日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

int16 wRDCI_PI,wSDCI_PI;
int16 wDCI_PI_SET;
int16 wDCI_Injection_Enable;
ST_CTRL_LOOP stDCICtrlLoop[PHASE];

void DCICtrlLoopInit(void)
{
	int phase;
	for(phase = 0; phase < PHASE; phase++)
	{
		memset((void*)&stDCICtrlLoop[phase], 0, sizeof(stDCICtrlLoop[phase]));

		stDCICtrlLoop[phase].stIn.uwCtrlLoopEnable = POSITIVE_PID;
		stDCICtrlLoop[phase].stIn.dRef = 0;
		stDCICtrlLoop[phase].stIn.dReal = 0;

		stDCICtrlLoop[phase].stOut.dCtrlLoopOutput = 0;
		stDCICtrlLoop[phase].stOut.dCtrlLoopRemainder = 0;

		stDCICtrlLoop[phase].stPID.stIn.dKp = 800;
		stDCICtrlLoop[phase].stPID.stIn.dKi = 1200;
		stDCICtrlLoop[phase].stPID.stIn.dPIMax = 200;
		stDCICtrlLoop[phase].stPID.stIn.dPIMin = -200;
		stDCICtrlLoop[phase].stPID.stIn.dError = 0;
		stDCICtrlLoop[phase].stPID.stOut.dErrorOld = 0;
		stDCICtrlLoop[phase].stPID.stOut.dPIDOut = 0;
	}
}

void DCIAdjust(void)
{
	static int32 sdRDCISoftSum,sdSDCISoftSum;
	static Uint16 uwDCIAdjustCnt;
	static Uint16 uwDCICalcCnt;
	static int16 wRDCISoftAvg,wSDCISoftAvg;
	if((cInverterStatus == eInverterStatus)&&(DISABLE == wDCI_Injection_Enable))
	{
		if(uwDCIAdjustCnt<=1500)		// 1500*20 ms = 30s
		{
			uwDCIAdjustCnt++;
			uwDCICalcCnt = 0;
			wRDCI_PI = 0;
			wSDCI_PI = 0;
			return ;
		}

		sdRDCISoftSum += (stACSample.SoftDCI.wRN - stAdcPool.RHardDCI.wOffset);
		sdSDCISoftSum += (stACSample.SoftDCI.wSN - stAdcPool.SHardDCI.wOffset);

		uwDCICalcCnt++;
		if(uwDCICalcCnt>=stDebug.ReadData.wDebug7[0])
		{
			wRDCISoftAvg = (sdRDCISoftSum/uwDCICalcCnt);
			wSDCISoftAvg = (sdSDCISoftSum/uwDCICalcCnt);
			stDebug.ReadData.wDebug5[1] = wRDCISoftAvg;
			stDebug.ReadData.wDebug5[2] = wSDCISoftAvg;
			uwDCICalcCnt = 0;

			sdRDCISoftSum = 0;
			sdSDCISoftSum = 0;

			if(stACSample.dActivePower > AC500W)	// 500w
			{
				stDCICtrlLoop[R_Phase].stPID.stIn.dKp = stDebug.ReadData.wDebug7[2];
				stDCICtrlLoop[R_Phase].stPID.stIn.dKi = stDebug.ReadData.wDebug7[3];
				stDCICtrlLoop[S_Phase].stPID.stIn.dKp = stDebug.ReadData.wDebug7[2];
				stDCICtrlLoop[S_Phase].stPID.stIn.dKi = stDebug.ReadData.wDebug7[3];

				stDCICtrlLoop[R_Phase].stIn.dReal = wRDCISoftAvg;
				stDCICtrlLoop[S_Phase].stIn.dReal = wSDCISoftAvg;
				IncrementalPID(&stDCICtrlLoop[R_Phase]);
				IncrementalPID(&stDCICtrlLoop[S_Phase]);

				wRDCI_PI = stDCICtrlLoop[R_Phase].stOut.dCtrlLoopOutput;
				wSDCI_PI = stDCICtrlLoop[S_Phase].stOut.dCtrlLoopOutput;

				stDebug.ReadData.wDebug5[3] = wRDCI_PI;
				stDebug.ReadData.wDebug5[4] = wSDCI_PI;
			}
		}
	}
	else
	{
		uwDCICalcCnt = 0;
		uwDCIAdjustCnt = 0;
		sdRDCISoftSum = 0;
		sdSDCISoftSum = 0;

		stDCICtrlLoop[R_Phase].stOut.dCtrlLoopOutput = 0;
		stDCICtrlLoop[R_Phase].stOut.dCtrlLoopRemainder = 0;
		stDCICtrlLoop[R_Phase].stPID.stOut.dErrorOld = 0;

		stDCICtrlLoop[S_Phase].stOut.dCtrlLoopOutput = 0;
		stDCICtrlLoop[S_Phase].stOut.dCtrlLoopRemainder = 0;
		stDCICtrlLoop[S_Phase].stPID.stOut.dErrorOld = 0;

		if(DISABLE == wDCI_Injection_Enable)
		{
			wRDCI_PI=0;
			wSDCI_PI=0;
		}
		else
		{
			if(wDCI_PI_SET>=300)
			{
				wRDCI_PI = 300;
				wSDCI_PI = 300;
			}
			else
			{
				wRDCI_PI = wDCI_PI_SET;
				wSDCI_PI = wDCI_PI_SET;
			}

		}

	}
}

void DCIProtection(void)
{
	static Uint16 uwDCIFaultCnt=0;
	static Uint16 uwDCCalcCnt200ms=0;
	static Uint16 uwDCCalcCnt1s=0;
	static Uint16 uwDCCalcTemp=0;
	static Uint16 uwDCIFaultRecoverCnt=0;
	if((cInverterStatus == eInverterStatus)&&(ENABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.DCI)&&(STRANDARD_ITALY_CEIO16==stDspReceData.unSafetyOdm.bit.SafetyStandard))
	{
		if(uwDCIFaultCnt<=1500)		// 1500*20 ms = 30s
		{
			uwDCIFaultCnt++;
			uwDCCalcCnt200ms = 0;
			uwDCCalcCnt1s = 0;
			uwDCIFaultRecoverCnt = 0;
			
			return ;
		}
		//uwDCCalcTemp =stLoadLimit.dApparPowerActual/2300/3*0.005*1000;//In=S/V/3*0.5%=25000/2300/3*0.005*1000=181mA
		uwDCCalcTemp =stLoadLimit.dApparPowerActual*30>>12;
		if(stACSample.dActivePower > 5000)	// 500w
		{
			if(stSysFaultReg.unDCIIFault.bit.RDCISoftWare ==0)
			{			  
				 if((abs(stACSample.HardDCI.wRN)> 10000)||(abs(stACSample.HardDCI.wSN)> 10000))		// 1000mA
				{				
					uwDCCalcCnt200ms++;
					uwDCCalcCnt1s++;
					if(uwDCCalcCnt200ms>=9) //< 20ms*10=200ms
					{
						uwDCCalcCnt200ms = 0;
						uwDCCalcCnt1s = 0;
						stSysFaultReg.unDCIIFault.bit.RDCISoftWare =1;
						stSysFaultReg.unFaultBit.bit.DCIOverCurr = 1;
					}
				}
				else if((abs(stACSample.HardDCI.wRN)> uwDCCalcTemp)||(abs(stACSample.HardDCI.wSN)> uwDCCalcTemp))//181mA
				{
					uwDCCalcCnt200ms=0;
					uwDCCalcCnt1s++;
					if(uwDCCalcCnt1s>=45) // <50ms*20=1000ms
					{
						uwDCCalcCnt1s = 0;
						stSysFaultReg.unFaultBit.bit.DCIOverCurr = 1;
						stSysFaultReg.unDCIIFault.bit.RDCISoftWare = 1;
					}
				}
				else
				{
					uwDCCalcCnt200ms = 0;
					uwDCCalcCnt1s = 0;
				}
			}
			
		}
	}
	else
	{
		uwDCIFaultCnt = 0;
		uwDCCalcCnt200ms = 0;
		uwDCCalcCnt1s = 0;
		if(stSysFaultReg.unDCIIFault.bit.RDCISoftWare ==1)
		{
			uwDCIFaultRecoverCnt++;
			if(uwDCIFaultRecoverCnt >250)	// 10S
			{
				uwDCIFaultRecoverCnt = 0;
				stSysFaultReg.unDCIIFault.bit.RDCISoftWare = 0;
				stSysFaultReg.unDCIIFault.bit.SDCISoftWare =0;
				stSysFaultReg.unDCIIFault.bit.TDCISoftWare =0;
				stSysFaultReg.unFaultBit.bit.DCIOverCurr = 0;
			}
		}
	}
}

