/*
 * calc_ac_sample.c
 *
 *  Created on: 2020年11月26日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

ST_AC_SAMPLE stACSample;

/****************************************************************************
*
*	FunctionName:	CalcACVoltCurrRms()
*	Input:	none
*	Output:	none
*	Description:	calculate ac volt rms,include phase volt and line volt
*	Note:	none.
***************************************************************/
void CalcACVoltCurrRms(void)
{
	static Uint16 suwACParaCalcCnt=0;
	static int32 sdGridFreqSum=0, sdLineVoltRmsMinSum=0, sdLineVoltRmsMaxSum=0;
	int16	wGridLineVoltTmp=0;
	int16 wRNPhaseCurrRmsTmp=0,wSNPhaseCurrRmsTmp=0,wTNPhaseCurrRmsTmp=0;
	int16	wGridPhaseCurrHalfTmp;

	/***********************Phase&Line&Inv Voltage Sample Calculate*********************************/
	stACSample.PhaseVoltRms.wRN = (uwCalcRms(stAdcPool.RGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10
	stACSample.PhaseVoltRms.wSN = (uwCalcRms(stAdcPool.SGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10
	stACSample.PhaseVoltRms.wTN = (uwCalcRms(stAdcPool.TGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10

	wGridLineVoltTmp = (uwCalcRms(stAdcPool.RSGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10
	stACSample.LineVoltRms.wRS = (int16)((int32)wGridLineVoltTmp*stF107Data.wRSVoltAdjRatio/10000);
	
	wGridLineVoltTmp = (uwCalcRms(stAdcPool.STGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10
	stACSample.LineVoltRms.wST = (int16)((int32)wGridLineVoltTmp*stF107Data.wSTVoltAdjRatio/10000);
	
	wGridLineVoltTmp = (uwCalcRms(stAdcPool.TRGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10
	stACSample.LineVoltRms.wTR = (int16)((int32)wGridLineVoltTmp*stF107Data.wTRVoltAdjRatio/10000);

	stACSample.RSTGridVoltRms = (uwCalcRms(stAdcPool.RSTGridVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;	// sqrt(2^4) = 4*5/2 = 10

	stACSample.InvVoltRms.wRN = (uwCalcRms(stAdcPool.RInvVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10
	stACSample.InvVoltRms.wSN = (uwCalcRms(stAdcPool.SInvVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10
	stACSample.InvVoltRms.wTN = (uwCalcRms(stAdcPool.TInvVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10

	stACSample.RlyVoltRms.wRN = (uwCalcRms(stAdcPool.RRlyVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10
	stACSample.RlyVoltRms.wSN = (uwCalcRms(stAdcPool.SRlyVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10
	stACSample.RlyVoltRms.wTN = (uwCalcRms(stAdcPool.TRlyVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10

	stACSample.RlyLineVoltRms.wRS = (uwCalcRms(stAdcPool.RSRlyVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10
	stACSample.RlyLineVoltRms.wST = (uwCalcRms(stAdcPool.STRlyVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10
	stACSample.RlyLineVoltRms.wTR = (uwCalcRms(stAdcPool.TRRlyVolt.dSquareAddSum,stAdcPool.uwSumCnt)*5)>>1;		// sqrt(2^4) = 4*5/2 = 10

	stACSample.wVoltNPERms = uwCalcRms(stAdcPool.NPEVolt.dSquareAddSum ,stAdcPool.uwMux4Ch2SumCnt) * 4;		// sqrt(2^4) = 4*5/2 = 10

#if OLD_INV_VOLT_SAMPLE
	/***********************Calculation of DC component of voltage*********************************/
	stACSample.PhaseInvVoltAvg.wRN = (stAdcPool.RInvVolt.dAddSum/stAdcPool.uwSumCnt);		// Q5 = 32
	stACSample.PhaseInvVoltAvg.wSN = (stAdcPool.SInvVolt.dAddSum/stAdcPool.uwSumCnt);
	stACSample.PhaseInvVoltAvg.wTN = (stAdcPool.TInvVolt.dAddSum/stAdcPool.uwSumCnt);
	stACSample.LineInvVoltAvg.wRS = ((stACSample.PhaseInvVoltAvg.wRN-stACSample.PhaseInvVoltAvg.wSN)*10)>>5;	// 32*10/32 = *10
	stACSample.LineInvVoltAvg.wST = ((stACSample.PhaseInvVoltAvg.wSN-stACSample.PhaseInvVoltAvg.wTN)*10)>>5;
	stACSample.LineInvVoltAvg.wTR = ((stACSample.PhaseInvVoltAvg.wTN-stACSample.PhaseInvVoltAvg.wRN)*10)>>5;
#endif
	/***********************Calculation of Inv Current Sample***************************************/
	wRNPhaseCurrRmsTmp = (uwCalcRms(stAdcPool.RInvCurr.dSquareAddSum,stAdcPool.uwSumCnt)*50)>>2;   // *100
	wSNPhaseCurrRmsTmp = (uwCalcRms(stAdcPool.SInvCurr.dSquareAddSum,stAdcPool.uwSumCnt)*50)>>2;   // *100
	wTNPhaseCurrRmsTmp = (uwCalcRms(stAdcPool.TInvCurr.dSquareAddSum,stAdcPool.uwSumCnt)*50)>>2;   // *100

	wGridPhaseCurrHalfTmp = ((stLoadLimit.wRatedApparentCurr)*86/3);

	if(wRNPhaseCurrRmsTmp < wGridPhaseCurrHalfTmp)        //*100,stLoadLimit.wRatedApparentCurr/3(Phase)/2
	{
	    stACSample.PhaseCurrRms.wRN = (int16)((int32)wRNPhaseCurrRmsTmp*stF107Data.wRNCurr30AdjRatio/10000);   // *100
	}
	else
	{
	    stACSample.PhaseCurrRms.wRN = (int16)((int32)wRNPhaseCurrRmsTmp*stF107Data.wRNCurr70AdjRatio/10000);   // *100
	}

	if(wSNPhaseCurrRmsTmp < wGridPhaseCurrHalfTmp)        //*100,stLoadLimit.wRatedApparentCurr/3(Phase)/2
	{
	    stACSample.PhaseCurrRms.wSN = (int16)((int32)wSNPhaseCurrRmsTmp*stF107Data.wSNCurr30AdjRatio/10000);   // *100
	}
	else
	{
	    stACSample.PhaseCurrRms.wSN = (int16)((int32)wSNPhaseCurrRmsTmp*stF107Data.wSNCurr70AdjRatio/10000);   // *100
	}

	if(wTNPhaseCurrRmsTmp < wGridPhaseCurrHalfTmp)        //*100,stLoadLimit.wRatedApparentCurr/3(Phase)/2
	{
	    stACSample.PhaseCurrRms.wTN = (int16)((int32)wTNPhaseCurrRmsTmp*stF107Data.wTNCurr30AdjRatio/10000);   // *100
	}
	else
	{
	    stACSample.PhaseCurrRms.wTN = (int16)((int32)wTNPhaseCurrRmsTmp*stF107Data.wTNCurr70AdjRatio/10000);   // *100
	}

	/***********************Calculation of software DCI Sample***************************************/
	stACSample.SoftDCI.wRN = (stAdcPool.RSoftDCI.dAddSum*1000/stAdcPool.uwSumCnt)>>5;				// 1mA
	stACSample.SoftDCI.wSN = (stAdcPool.SSoftDCI.dAddSum*1000/stAdcPool.uwSumCnt)>>5;

	// stACSample.HardDCI.wRN = stAdcPool.RHardDCI.dAddSum / stAdcPool.uwMux4Ch2SumCnt;				// 1mA
	// stACSample.HardDCI.wSN = stAdcPool.SHardDCI.dAddSum /stAdcPool.uwMux4Ch2SumCnt;
	/***********************Calculation of GFCI Sample***************************************/
	stACSample.wGfciRms = uwCalcRms(stAdcPool.GFCI.dSquareAddSum,stAdcPool.uwMux4Ch0SumCnt)*4;	// 1mA
	stACSample.wGfciAvg = (stAdcPool.GFCI.dAddSum/stAdcPool.uwSumCnt);

	/***********************Calculation of Grid Voltage Freqency*************************************/
//	stACSample.wGridFreqReal = ((int64)305577*stPllPara.stOut.dTheta>>24) + 8;			// 19200*100/(2*PI) >> 2^24
//	if((stACSample.wGridFreqReal<FREQ_50HZ_MIN)||(stACSample.wGridFreqReal>FREQ_60HZ_MAX))
//	{
//		stACSample.wGridFreqReal = 0;
//	}
	
	/***********************Calculation of Phase Voltage Rms*************************************/
	stACSample.wPhaseVoltRmsMax = MAX3(stACSample.PhaseVoltRms.wRN,stACSample.PhaseVoltRms.wSN,stACSample.PhaseVoltRms.wTN);
	stACSample.wPhaseVoltRmsMin = MIN3(stACSample.PhaseVoltRms.wRN,stACSample.PhaseVoltRms.wSN,stACSample.PhaseVoltRms.wTN);
	/***********************Calculation of Line Voltage Rms*************************************/
	stACSample.wLineVoltRmsMax = MAX3(stACSample.LineVoltRms.wRS,stACSample.LineVoltRms.wST,stACSample.LineVoltRms.wTR);
	stACSample.wLineVoltRmsMin = MIN3(stACSample.LineVoltRms.wRS,stACSample.LineVoltRms.wST,stACSample.LineVoltRms.wTR);
	/***********************Calculation of Grid Volt Period Time*************************************/
	stACSample.d1DivGridTimFlt = 16777216/stPllPara.stOut.uwGridPrdTimFlt;			// 2^24/Time

	/***********************Calculation of AC Ferq*************************************/
	stGridFreq.wGridFreqMax =  MAX3(stGridFreq.uwRGridFreqNew,stGridFreq.uwSGridFreqNew,stGridFreq.uwTGridFreqNew);
	stGridFreq.wGridFreqMin =  MIN3(stGridFreq.uwRGridFreqNew,stGridFreq.uwSGridFreqNew,stGridFreq.uwTGridFreqNew);

	if((stACSample.wPhaseVoltRmsMax <= 50)&&(cInverterStatus != eInverterStatus))      // Grid Loss, Freq Clear
	{
	    stACSample.wGridFreqReal = 0;
	}
	else
	{
	    stACSample.wGridFreqReal = stGridFreq.wGridFreqMax;
	}
	stSysCfg.Rated_CapCurrPeak = (((int32)stACSample.wLineVoltRmsMax * 63)>>14) ;// 2*PI*F*C*U,U=u/sqrt3*sqrt2
	if(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough)
	{
		sdGridFreqSum += (int32)stACSample.wGridFreqReal;
		sdLineVoltRmsMinSum += (int32)stACSample.wLineVoltRmsMin;
		sdLineVoltRmsMaxSum += (int32)stACSample.wLineVoltRmsMax;

		suwACParaCalcCnt++;
		if(suwACParaCalcCnt>=32)	// 32*20ms = 640ms
		{
			suwACParaCalcCnt = 0;

			stACSample.wGridFreqAvg = (sdGridFreqSum>>5);
			stACSample.wLineVoltRmsMinAvg = (sdLineVoltRmsMinSum>>5);
			stACSample.wLineVoltRmsMaxAvg = (sdLineVoltRmsMaxSum>>5);

			sdLineVoltRmsMinSum = 0;
			sdLineVoltRmsMaxSum = 0;
			sdGridFreqSum = 0;
		}
	}
	else
	{
		suwACParaCalcCnt = 0;
	}
}


/****************************************************************************
*
*	FunctionName:	CalcACVoltCurrRms()
*	Input:	none
*	Output:	none
*	Description:	calculate ac output Power
*	Note:	none.
***************************************************************/
void CalcOutputPower(void)
{
	static int16	wAcParaCalcCnt=0;
	static int32	sdActivePowerSum=0,sdApparentPowerSum=0;
	int32	dReactivePowerSquare;
	

	if(cInverterStatus == eInverterStatus)
	{
		stACSample.dActivePower = ((stAdcPool.ActivePower.dAddSum/stAdcPool.uwSumCnt)*10)>>4;	// *10
		if(ATE_ADJUST_NORMAL == stF107Data.uwAdjustMode)
		{
			if(stACSample.dActivePower < (stLoadLimit.dActivePower>>1))
			{
				stACSample.dActivePower = (((int32)stACSample.dActivePower*stF107Data.wActPower30AdjRatio)>>11);
			}
			else
			{
				stACSample.dActivePower = (((int32)stACSample.dActivePower*stF107Data.wActPower70AdjRatio)>>11);
			}
		}
		stACSample.dApparentPower = ((int32)stACSample.PhaseVoltRms.wRN*stACSample.PhaseCurrRms.wRN
									+(int32)stACSample.PhaseVoltRms.wSN*stACSample.PhaseCurrRms.wSN
									+(int32)stACSample.PhaseVoltRms.wTN*stACSample.PhaseCurrRms.wTN)/100;

		sdActivePowerSum += stACSample.dActivePower;
		sdApparentPowerSum += stACSample.dApparentPower;

		wAcParaCalcCnt++;
		if(wAcParaCalcCnt>=32)		// 32*20ms = 640ms
		{
			wAcParaCalcCnt = 0;
			stACSample.dActivePowerAvg = (sdActivePowerSum>>5);
			stACSample.dApparentPowerAvg = (sdApparentPowerSum>>5);
			sdActivePowerSum = 0;
			sdApparentPowerSum = 0;

			if(ATE_ADJUST_NORMAL != stF107Data.uwAdjustMode)
			{
				if(stACSample.dActivePowerAvg < (stLoadLimit.dActivePower>>1))
				{
					stACSample.dActivePowerAvg = (((int32)stACSample.dActivePowerAvg*stF107Data.wActPower30AdjRatio)>>11);
				}
				else
				{
					stACSample.dActivePowerAvg = (((int32)stACSample.dActivePowerAvg*stF107Data.wActPower70AdjRatio)>>11);
				}
			}
			if(stACSample.dActivePowerAvg > stACSample.dApparentPowerAvg)	// P>S; S=P
			{
				stACSample.dReactivePower = 0;
				stACSample.dApparentPowerAvg = stACSample.dActivePowerAvg;
				stACSample.wPowerFactor = 1000;			// PF = 1
			}
			else
			{
				/**********Q^2 = [(S^2)-(P^2)] = [(S+P)*(S-P)]*******/
				dReactivePowerSquare = (((stACSample.dApparentPowerAvg+stACSample.dActivePowerAvg)>>4)
									   *((stACSample.dApparentPowerAvg-stACSample.dActivePowerAvg)>>4));
				stACSample.dReactivePower = uwCalcRms(dReactivePowerSquare,1);
				stACSample.dReactivePower = stACSample.dReactivePower*16;
				stACSample.wPowerFactor = (stACSample.dActivePowerAvg*1000/stACSample.dApparentPowerAvg);
			}
			UPDNLMT(stACSample.wPowerFactor,1000,-1000);

			stACSample.wEfficiency = (stACSample.dActivePowerAvg*970)/stDCSample.dPVPowerTotal;
			UPDNLMT(stACSample.wEfficiency,990,930);
		}
	//	stACSample.dReactivePower =	uwCalcRms((stACSample.dApparentPower*stACSample.dApparentPower - stACSample.dActivePower*stACSample.dActivePower))
	}
	else
	{
		sdApparentPowerSum = 0;
		sdActivePowerSum = 0;
		wAcParaCalcCnt = 0;
		stACSample.dActivePowerAvg = 0;
		stACSample.dApparentPowerAvg=0;
		stACSample.dReactivePower=0;
		stACSample.dActivePower = 0;
		stACSample.dApparentPower = 0;
		stACSample.wPowerFactor = 1000;
		stACSample.dReactivePower = 0;
		stACSample.wEfficiency = 0;
	}
}


void EnergizeDegreeCalc(void)
{
	static Uint32	sudEnergyPowerTotal=0;
	if(stACSample.dActivePowerAvg > 0)
	{
		sudEnergyPowerTotal += stACSample.dActivePowerAvg;
	}

	if(sudEnergyPowerTotal>=7200000)		// 0.1kwh
	{
		sudEnergyPowerTotal -= 7200000;
		stACSample.uwPowerEnergySum++;
		if(stACSample.uwPowerEnergySum>=10)		// 1kwh
		{
			stACSample.uwPowerEnergySum = 0;
		}
	}
}





