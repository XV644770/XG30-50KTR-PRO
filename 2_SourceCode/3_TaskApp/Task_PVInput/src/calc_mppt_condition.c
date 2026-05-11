/*
 * calc_mppt_condition.c
 *
 *  Created on: 2020年12月30日
 *      Author: Administrator
 */
#include "dsp_user_include.h"

void PVOnOffCheck(void)
{
	Uint16 uwPVIndexTmp = 0;
	Uint16 uwPvOnNumTmp;
	Uint16 uwPvHighNumTmp;
	static Uint16 uwPVConnectCnt[PV_MAX_NUM]={0}, uwPVDisConnectCnt[PV_MAX_NUM]={0};

	if(cInitialStatus == eInverterStatus)
		return;
	uwPvOnNumTmp = 0;
	uwPvHighNumTmp = 0;
	/*******************Determine whether PV is connected***************************/
	for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
	{
		if(FALSE == stMpptTskCtrl.uwPVConnect[uwPVIndexTmp])
		{
			if((stDCSample.wPVVolt[uwPVIndexTmp] > stF107Data.uwPVStartUnderVolt)
	                &&(stDCSample.wPVVolt[uwPVIndexTmp] > PV_TURN_OFF_VOLT))//
			{
				uwPVConnectCnt[uwPVIndexTmp]++;
				if(uwPVConnectCnt[uwPVIndexTmp] > PV_CONNECT_TIME) // 4s
				{
					uwPVConnectCnt[uwPVIndexTmp] = 0;
					stMpptTskCtrl.uwPVConnect[uwPVIndexTmp] = TRUE;
					stMpptTskCtrl.uwPVONNum++;
				}
			}
            else
            {
                uwPVConnectCnt[uwPVIndexTmp] = 0;
            }
			uwPVDisConnectCnt[uwPVIndexTmp] = 0;
		}
		else
		{
			if(stDCSample.wPVVolt[uwPVIndexTmp]  < (PV_TURN_OFF_VOLT -VDC20V))//180V
			{
				uwPVDisConnectCnt[uwPVIndexTmp]++;
				if(uwPVDisConnectCnt[uwPVIndexTmp] > PV_DISCONNECT_TIME) // 1s
				{
					uwPVDisConnectCnt[uwPVIndexTmp] = 0;
					stMpptTskCtrl.uwPVConnect[uwPVIndexTmp] = FALSE;
					stMpptTskCtrl.uwPVONNum--;
					if(stMpptTskCtrl.uwPVONNum < 1)
					{
						stMpptTskCtrl.uwPVONNum = 1;
					}
				}
			}
            else
            {
                uwPVDisConnectCnt[uwPVIndexTmp] = 0;
            }
			uwPVConnectCnt[uwPVIndexTmp] = 0;
		}
		/************************PV Voltage High******************************/
		if(stDCSample.wPVVolt[uwPVIndexTmp] > PV_VOLT_MAX)
		{
			uwPvHighNumTmp++;
		}

		if(stMpptTskCtrl.uwPVConnect[uwPVIndexTmp] == TRUE)
		{
			uwPvOnNumTmp ++;
		}
	}

	if((FALSE == uwPvOnNumTmp)&&(false == SvgParam.bEnterSVGStateFlag))   //disable SVG
	{
		unSysFlag.bit.PvVoltLow = TRUE;
	}
	else
	{
		unSysFlag.bit.PvVoltLow = FALSE;
	}

	if(uwPvHighNumTmp > 0)
	{
		unSysFlag.bit.PvVoltHigh = TRUE;
	}
	else
	{
		unSysFlag.bit.PvVoltHigh = FALSE;
	}
}

void CalcPVVoltMax(void)
{
	Uint16 uwPVIndexTmp = 0;
	Uint16 uwPVVoltMaxMpptIDTmp=0, uwPVVoltRefMaxMpptIDTmp=0;
	int16 wPVVoltMaxTmp=0, wPVVoltRefMaxTmp=0;
	int32 dPVPowerSum=0;

	/******************Calculate the MAX Value of PV Voltage & PV Voltage Reference***********************/
	uwPVVoltMaxMpptIDTmp = 0;
	wPVVoltMaxTmp = 0;

	uwPVVoltRefMaxMpptIDTmp = 0;
	wPVVoltRefMaxTmp = 0;

	for(uwPVIndexTmp = 0; uwPVIndexTmp < stSysCfg.uwPVNumber; uwPVIndexTmp++)
	{
		/**************Calculate PV Voltage Max******************/
		if((wPVVoltMaxTmp+200) < stDCSample.wPVVolt[uwPVIndexTmp])
		{
			wPVVoltMaxTmp = stDCSample.wPVVolt[uwPVIndexTmp];
			uwPVVoltMaxMpptIDTmp = uwPVIndexTmp;
		}

		/**************Calculate PV Voltage  Reference Max******************/
		if((wPVVoltRefMaxTmp < stMpptDisturb[uwPVIndexTmp].wPVVoltRef)
		&& (TRUE == stMpptTskCtrl.uwPVConnect[uwPVIndexTmp]))
		{
			wPVVoltRefMaxTmp = stMpptDisturb[uwPVIndexTmp].wPVVoltRef;
			uwPVVoltRefMaxMpptIDTmp = uwPVIndexTmp;
		}
	}

	if((cWaitStatus == eInverterStatus)&&(cInit == stWaitStatus.eStandbyStep))
	{
		stMpptTskCtrl.uwPVVoltMaxID = uwPVVoltMaxMpptIDTmp;
	}
	else
	{
		// Prevent PV disconnection during operation
		if(FALSE == stMpptTskCtrl.uwPVConnect[stMpptTskCtrl.uwPVVoltMaxID])
		{
			stMpptTskCtrl.uwPVVoltMaxID = uwPVVoltMaxMpptIDTmp;
		}
	}
	stMpptTskCtrl.uwPVVoltRefMaxID = uwPVVoltRefMaxMpptIDTmp;
	
	/******************Calculate the Vaule of PV Voltage/Power***********************/
	dPVPowerSum = 0;
	for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
	{
		// PV Connect Solar Panel
		if(TRUE == stMpptTskCtrl.uwPVConnect[uwPVIndexTmp])
		{
			stMpptPara[uwPVIndexTmp].wPVVolt = stDCSample.wPVVolt[uwPVIndexTmp];
			// calculate Open Voltage
			if((cWaitStatus == eInverterStatus)&&(cInit == stWaitStatus.eStandbyStep))
			{
				stMpptPara[uwPVIndexTmp].wPVOpenVolt = stMpptPara[uwPVIndexTmp].wPVVolt;
			}	
			else if((cInverterStatus==eInverterStatus) && (stMpptPara[uwPVIndexTmp].wPVOpenVolt < stMpptPara[uwPVIndexTmp].wPVVolt))
			{
				stMpptPara[uwPVIndexTmp].wPVOpenVolt = stMpptPara[uwPVIndexTmp].wPVVolt;	// update Open Voltage →
			}
			else if((cInverterStatus==eInverterStatus) 
				 && ((stMpptPara[uwPVIndexTmp].wPVOpenVolt > stMpptPara[uwPVIndexTmp].wPVVolt) && stDCSample.wBTCurr[uwPVIndexTmp] < 5))
			{
				stMpptPara[uwPVIndexTmp].wPVOpenVolt = stMpptPara[uwPVIndexTmp].wPVVolt;	// update Open Voltage ←
			}
			// Calculate the power of each PV 		P = V*I
			stMpptPara[uwPVIndexTmp].dMpptPower = ((int32)stMpptPara[uwPVIndexTmp].wPVVolt*stDCSample.wMPPTCurr[uwPVIndexTmp]/10);	//unit 0.01w
		}
		else
		{
			stMpptPara[uwPVIndexTmp].wPVOpenVolt = 0;
			stMpptPara[uwPVIndexTmp].dMpptPower = 0;
		}
		// Calculation of total PV power
		if(PARALLEL == stSysCfg.eMpptMode)
		{
			dPVPowerSum += stMpptPara[uwPVIndexTmp].dMpptPower;
		}
	}

	if(PARALLEL == stSysCfg.eMpptMode)
	{
		stMpptPara[stMpptTskCtrl.uwPVVoltMaxID].dMpptPower = dPVPowerSum;
	}
}

void BusRefCalc(ENUM_MPPT_MODE  eMpptMode_s)
{
	int16	wBusVoltRefTmp,wACVoltPeakTmp;
	Uint16  uwPVIndexTmp=0, uwPVVoltMaxIDTmp=0;
	int16	wBoostOnVoltTmp=0, wBoostOnPVJudgeVoltTmp=0;
	static	int16 swBusVoltRefBak=0,swBusVoltInvRefBak=0,swSetBusBakRef=0;
	/****************************************AC Voltage Peak Value Calculate*******************************/
	if(cInverterStatus  == eInverterStatus)		// SVPWM		BusVolt = (AC_Line*1.414)
	{
		// example:	ACVolt=230V		BusVolt = 230*1.732*1.414
		if(MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType)
		{
			wACVoltPeakTmp  = ((((int32)stACSample.wLineVoltRmsMax * 1448)>>10) + BUS_VOLT_ADD+VOLT_20V);//add 20v  for inv harmonic issue when input volt is 576V add 25% load
		}
		else
		{
			wACVoltPeakTmp  = ((((int32)stACSample.wLineVoltRmsMax * 1448)>>10) + BUS_VOLT_ADD);
		}
	}
	else										// SPWM			BusVolt = (AC_Phase*1.414*2)
	{
		// example:	ACVolt=230V		BusVolt = 230*1.414*2
		wACVoltPeakTmp  = ((((int32)stACSample.wPhaseVoltRmsMax * 2896)>>10) + 500);
	}
	
	stMpptTskCtrl.wBusStandardVolt = wACVoltPeakTmp;
	stDebug.ReadData.wACVoltPeakValue = stMpptTskCtrl.wBusStandardVolt;
	/*******************************Bus Reference Soft Start Slowly***********************************************/
	if(swSetBusBakRef<stDebug.SetData.uwBusRefMin)
	{
		swSetBusBakRef += 10;			// add 10V every 20ms
	}
	else if(swSetBusBakRef>stDebug.SetData.uwBusRefMin)
	{
		swSetBusBakRef -= 10;			// decrease 10V every 20ms
	}
	else
	{
		swSetBusBakRef = stDebug.SetData.uwBusRefMin;
	}
	UPDNLMT(stMpptTskCtrl.wBusStandardVolt,BUS_REF_MAX,swSetBusBakRef);	// max = (277V*1.732)*1.2*1.414 + 50v
	UPDNLMT(stMpptTskCtrl.wBusStandardVolt,BUS_REF_MAX,BUS_REF_MIN);
	/****************************************BOOST ON-OFF Judge Voltage*******************************/
	if(cInverterStatus  == eInverterStatus)		// Inverter Status:Boost On/off & BusRef Calculate
	{
		if(DC_SOURCE == eMpptMode_s)
		{
			wBoostOnVoltTmp = PARALLEL_BOOST_ON_VOLT;
			uwPVVoltMaxIDTmp = stMpptTskCtrl.uwPVVoltMaxID;
			wBoostOnPVJudgeVoltTmp = stDCSample.wPVVolt[uwPVVoltMaxIDTmp]+VOLT_30V;
		}
		else if(PARALLEL == eMpptMode_s)
		{
			wBoostOnVoltTmp = PARALLEL_BOOST_ON_VOLT;
			uwPVVoltMaxIDTmp = stMpptTskCtrl.uwPVVoltMaxID;
//			wBoostOnPVJudgeVoltTmp = stMpptDisturb[stMpptTskCtrl.uwPVVoltRefMaxID].wPVVoltRef+VOLT_30V;
			wBoostOnPVJudgeVoltTmp = stMpptDisturb[uwPVVoltMaxIDTmp].wPVVoltRef+VOLT_30V;
		}
		else
		{
			wBoostOnVoltTmp = INDEPENDENT_BOOST_ON_VOLT;
			uwPVVoltMaxIDTmp = stMpptTskCtrl.uwPVVoltRefMaxID;
			if(stMpptDisturb[uwPVVoltMaxIDTmp].wPVVoltRef <= (INDEPENDENT_BOOST_ON_VOLT-VDC30V))	// <= 840V
			{
				wBoostOnPVJudgeVoltTmp = (stMpptDisturb[uwPVVoltMaxIDTmp].wPVVoltRef + VDC30V);		// <= 870V
			}
			else if(stMpptDisturb[uwPVVoltMaxIDTmp].wPVVoltRef <= INDEPENDENT_BOOST_ON_VOLT)		// 840V~870V
			{
				wBoostOnPVJudgeVoltTmp = INDEPENDENT_BOOST_ON_VOLT;									// 870V
			}
			else	// > 870V
			{
				wBoostOnPVJudgeVoltTmp = stMpptDisturb[uwPVVoltMaxIDTmp].wPVVoltRef;
			}
		}
		if((wBoostOnPVJudgeVoltTmp < stMpptTskCtrl.wBusStandardVolt) || (wBoostOnPVJudgeVoltTmp < wBoostOnVoltTmp))
		{
			stMpptTskCtrl.uwBTOnOff[uwPVVoltMaxIDTmp] = TRUE;		// on boost
		}
		else if((wBoostOnPVJudgeVoltTmp > (stMpptTskCtrl.wBusStandardVolt+VOLT_30V))&& (wBoostOnPVJudgeVoltTmp > (wBoostOnVoltTmp+VOLT_30V)))
		{
			stMpptTskCtrl.uwBTOnOff[uwPVVoltMaxIDTmp] = FALSE;		// off boost
		}
#if 0
		stMpptPara.uwBTOnOff[uwPVVoltMaxIDTmp] = FALSE;		// off boost
		wBusVoltRefTmp = stDebug.SetData.uwBusRef;
#else
		wBusVoltRefTmp = MAX2(wBoostOnPVJudgeVoltTmp,stMpptTskCtrl.wBusStandardVolt);
#endif
		UPDNLMT(wBusVoltRefTmp,BUS_REF_MAX,BUS_REF_MIN);
		// Bus voltage is soft starting, 1V is added every 20ms
		if(swBusVoltInvRefBak > wBusVoltRefTmp)
		{
			swBusVoltInvRefBak -= 10;		// decrease 10V every 20ms
		}
		else
		{
			swBusVoltInvRefBak = wBusVoltRefTmp;
		}
		swBusVoltRefBak = swBusVoltInvRefBak;
	}
	else if((cWaitStatus  == eInverterStatus) && (stWaitStatus.eStandbyStep >= cBusSoftStart))
	{
		uwPVVoltMaxIDTmp = stMpptTskCtrl.uwPVVoltMaxID;
		wBoostOnPVJudgeVoltTmp = stDCSample.wPVVolt[uwPVVoltMaxIDTmp];

		if((wBoostOnPVJudgeVoltTmp < stMpptTskCtrl.wBusStandardVolt) || (wBoostOnPVJudgeVoltTmp < BUS_SOFT_START_VOLT))
		{
			stMpptTskCtrl.uwBTOnOff[uwPVVoltMaxIDTmp] = TRUE;		// on boost
		}
		else if((wBoostOnPVJudgeVoltTmp > (stMpptTskCtrl.wBusStandardVolt+VOLT_20V)) && (wBoostOnPVJudgeVoltTmp > (BUS_SOFT_START_VOLT+VOLT_20V)))
		{
			stMpptTskCtrl.uwBTOnOff[uwPVVoltMaxIDTmp] = FALSE;		// off boost
		}
		// Calculate the BUS reference voltage
//		wBusVoltRefTmp = MAX3(wBoostOnPVJudgeVoltTmp,stMpptTskCtrl.wBusStandardVolt,BUS_SOFT_START_VOLT);
		wBusVoltRefTmp = MAX2(wBoostOnPVJudgeVoltTmp,stMpptTskCtrl.wBusStandardVolt);
		UPDNLMT(wBusVoltRefTmp,BUS_REF_MAX,BUS_WAIT_MIN);
		

		// Bus voltage is soft starting, 1V is added every 20ms
		if(swBusVoltRefBak < wBusVoltRefTmp)
		{
			swBusVoltRefBak += 10;		// add 1V every 20ms
		}
		else
		{
			swBusVoltRefBak = wBusVoltRefTmp;
		}
		
		swBusVoltInvRefBak = stDCSample.wBusVolt;
	}
	else if(true == SvgParam.bEnterSVGStateFlag)
	{
	    swBusVoltRefBak = stMpptTskCtrl.wBusStandardVolt;  //ACVolt*1.414*1.732+50
	}
	else		// Others Status:Boost On/off & BusRef Calculate
	{
		wBusVoltRefTmp = stDCSample.wBusVolt;
		swBusVoltInvRefBak = stDCSample.wBusVolt;
		swBusVoltRefBak = wBusVoltRefTmp;
	}

	/****************************************BOOST ON-OFF Judge Condition*******************************/
	for(uwPVIndexTmp = 0; uwPVIndexTmp < PV_MAX_NUM; uwPVIndexTmp++)
	{
		
		if((cInverterStatus  == eInverterStatus)
	      ||((cWaitStatus  == eInverterStatus) && (stWaitStatus.eStandbyStep >= cBusSoftStart)))
		{
			if(uwPVIndexTmp >= stSysCfg.uwPVNumber)
			{
				stMpptTskCtrl.uwBTOnOff[uwPVIndexTmp] = FALSE;
				continue;
			}
			
			if(uwPVIndexTmp != uwPVVoltMaxIDTmp)
			{
				if(TRUE == stMpptTskCtrl.uwPVConnect[uwPVIndexTmp])
				{
					if(cInverterStatus  == eInverterStatus)
					{
						/* In the grid-connected state of the inverter,
						the maximum Boost switch of one road is determined by PV voltage,
						and Boost switch of other PV channels must be turned on. */
						stMpptTskCtrl.uwBTOnOff[uwPVIndexTmp] = TRUE;		// wait status : Max-MPPT open , leftover also open;
					}
					else
					{
						/* In the waiting state of the inverter,
						the largest one BOOST switch is determined by the PV voltage,
						and the other PV channels keep the same switching state. */
						stMpptTskCtrl.uwBTOnOff[uwPVIndexTmp] = stMpptTskCtrl.uwBTOnOff[uwPVVoltMaxIDTmp];
					}
				}
				else
				{
					stMpptTskCtrl.uwBTOnOff[uwPVIndexTmp] = FALSE;
				}
			}
		}
	}
	stMpptTskCtrl.uwBusRef = swBusVoltRefBak;
}

void InvPwmChange(void)
{
	static Uint16	suwInvPWmChangeCnt=0, suwInvDwpmPWmCnt=0;
	int16	wACPhaseVoltRmsMinTmp=0;

	if(cInverterStatus  == eInverterStatus)
	{
		wACPhaseVoltRmsMinTmp  = ((int32)stACSample.wPhaseVoltRmsMin * 2508)>>10;

		suwInvPWmChangeCnt++;
		if(suwInvPWmChangeCnt >= 500)		// 20ms*500=1s
		{
			suwInvPWmChangeCnt = 500;
			if(stDCSample.wBusVolt > (wACPhaseVoltRmsMinTmp+700))	//
			{
				suwInvDwpmPWmCnt=0;
				stDebug.SetData.unSetReg.bit.PWMMode = 0;
			}
			else if(stDCSample.wBusVolt < (wACPhaseVoltRmsMinTmp+500))
			{
				suwInvDwpmPWmCnt++;
				if(suwInvDwpmPWmCnt>=500)	// 20ms*500 = 1000ms
				{
					suwInvDwpmPWmCnt = 500;
					stDebug.SetData.unSetReg.bit.PWMMode = 1;
				}
			}
		}
	}
	else
	{
		suwInvPWmChangeCnt = 0;
	}
}


