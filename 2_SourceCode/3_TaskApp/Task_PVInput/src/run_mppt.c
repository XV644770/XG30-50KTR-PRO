/*
 * run_mppt.c
 *
 *  Created on: 2020年12月31日
 *      Author: Administrator
 */
#include "dsp_user_include.h"

ST_BT_CTRL_TASK stMpptTskCtrl;
ST_MPPT_PARA stMpptPara[PV_MAX_NUM];
ST_MPPT_DISTURB stMpptDisturb[PV_MAX_NUM];

static void TrackingMppt(ST_MPPT_DISTURB *pstMpptDisturb, ST_MPPT_PARA *pstMpptPara);
static void DisturbObserveMppt(ST_MPPT_DISTURB *pstMpptDisturb, int16 wStepSize, int32 dMpptPowerError);
static void TrackingMpptInit(ST_MPPT_DISTURB *pstMpptDisturb);
static void LowPowerCalcMppt(ST_MPPT_DISTURB *pstMpptDisturb, int32 dLowPowerLimit);

void MpptInit(void)
{
	Uint16 uwPVIndexTmp = 0;
	memset((void*)&stMpptTskCtrl,0,sizeof(stMpptTskCtrl));
	for(uwPVIndexTmp = 0; uwPVIndexTmp < PV_MAX_NUM; uwPVIndexTmp++)
	{
		memset((void*)&stMpptPara[uwPVIndexTmp],0,sizeof(stMpptPara[uwPVIndexTmp]));
		memset((void*)&stMpptDisturb[uwPVIndexTmp],0,sizeof(stMpptDisturb[uwPVIndexTmp]));
		ClearPVCurveAnalyCtrlPara(uwPVIndexTmp);
	}
	stMpptTskCtrl.uwPVONNum = 0;
}

void MPPTStrategy(Uint16 uwPVIndex)
{
	Uint16 uwPVIndexTmp = 0; 
	Uint16 uwVoltStepTmp = 0;
    Uint32 udCtActivePowerAvg = 0;

    if((REACTIVE_QV_MODE==stF107Data.uwPFCtrlMode)&&(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard)&&(MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType)
		|| (ENABLE == stDspReceData.unFuncEnable.bit.ExportLimit || ENABLE == stDspReceData.unFuncEnable.bit.MultiExportLimit)
		&& (stMpptDisturb[uwPVIndex].wMpptVoltRef >(int16) ((int32)stMpptPara[uwPVIndex].wPVOpenVolt * 880>>10))
		&& (cInverterStatus == eInverterStatus))					//use Anti Strategy
    {
		/************* AntiFlow Mode Switch *************/
        if((ENABLE == stDebug.ReadData.wQVPFFlag)
		|| (ENABLE == stDspReceData.unFuncEnable.bit.ExportLimit || ENABLE == stDspReceData.unFuncEnable.bit.MultiExportLimit))
        {
			stMpptDisturb[uwPVIndex].uwAntiFlowCnt ++;
            stMpptDisturb[uwPVIndex].uwFastMpptFlag = 1;			// 关闭快追
			stMpptDisturb[uwPVIndex].unMpptBits.bit.OverPower = 1;

            if((stLoadLimit.dActPowerLimitOutput - stACSample.dActivePower) > AC200W)       //wMpptVoltRef-- 1000
            {
                stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower = ANTIFLOW_Loading;
            }
            else if((stLoadLimit.dActPowerLimitOutput - stACSample.dActivePower) > 0)     //stop Tracking 1000
            {
                stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower = ANTIFLOW_Stop;
				stMpptDisturb[uwPVIndex].dAntiStep = 0;
				return;
            }
            else if((stLoadLimit.dActPowerLimitOutput - stACSample.dActivePower) < -AC200W)     //wMpptVoltRef++  1000
            {
                stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower = ANTIFLOW_DownLoading;
            }
        }
        else
        {
            stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower = ANTIFLOW_NormalMppt;
        }

		/************* AntiFlow Step Calculation *************/
		if(ANTIFLOW_Loading == stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower)
		{
			// 大载，斜率跟踪
			if (stLoadLimit.dActPowerLimitOutput > (AC10000W*stMpptTskCtrl.uwPVONNum))
			{   // Single Load > 12KW：0.8*P -5.8
				uwVoltStepTmp = VDC0_8V*((stLoadLimit.dActPowerLimitOutput/stMpptTskCtrl.uwPVONNum)/AC1000W)-45;
			}	
			else if(stLoadLimit.dActPowerLimitOutput > (AC1000W*stMpptTskCtrl.uwPVONNum))
			{   // Single Load > 1KW：2 + 0.15*P
				uwVoltStepTmp = VDC2V + (((stLoadLimit.dActPowerLimitOutput/stMpptTskCtrl.uwPVONNum)/AC1000W)*15/10);
			}   // Low Load
			else if((stLoadLimit.dActPowerLimitOutput <= (AC1000W*stMpptTskCtrl.uwPVONNum))
					&& (stLoadLimit.dActPowerLimitOutput > AC1000W))
			{
				uwVoltStepTmp = VDC1V;
			}
		}
		else if(ANTIFLOW_DownLoading == stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower)
		{
			// 大载，斜率跟踪
			if (stACSample.dActivePower > (AC10000W*stMpptTskCtrl.uwPVONNum))
			{   // Single Load > 12KW：0.8*P -5.8
				uwVoltStepTmp = VDC0_8V*((stACSample.dActivePower/stMpptTskCtrl.uwPVONNum)/AC1000W)-45;
			}	
			else if(stACSample.dActivePower > (AC1000W*stMpptTskCtrl.uwPVONNum))
			{   // Single Load > 1KW：2 + 0.15*P
				uwVoltStepTmp = VDC2V + (((stACSample.dActivePower/stMpptTskCtrl.uwPVONNum)/AC1000W)*15/10);
			}   // Low Load
			else if((stACSample.dActivePower <= (AC1000W*stMpptTskCtrl.uwPVONNum))
					&& (stACSample.dActivePower > AC1000W))
			{
				uwVoltStepTmp = VDC1V;
			}
			else
			{
				uwVoltStepTmp = VDC1V;
			}
		}


		/************* Adjust the step size based on errors *************/
		udCtActivePowerAvg = labs(stLoadLimit.dActPowerLimitOutput - stACSample.dActivePower);

		if(udCtActivePowerAvg > AC8000W)
		{
			stMpptDisturb[uwPVIndex].dAntiStep = uwVoltStepTmp;
		}
		else if(stLoadLimit.dActPowerLimitOutput <= AC300W
		&&  ANTIFLOW_DownLoading == stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower)
		{
			stMpptDisturb[uwPVIndex].dAntiStep = uwVoltStepTmp;
		}
		else if(stLoadLimit.dActPowerLimitOutput < AC1000W
		&&  ANTIFLOW_Loading == stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower)
		{
			if(PVA == uwPVIndex)
			{	// 极小载，单路追踪
				for(uwPVIndexTmp = 0; uwPVIndexTmp < PV_MAX_NUM; uwPVIndexTmp++)
				{
					stMpptDisturb[uwPVIndexTmp].dAntiStep = 0;
				}
				for(uwPVIndexTmp = 0; uwPVIndexTmp < PV_MAX_NUM; uwPVIndexTmp++)
				{
					if(TRUE == stMpptTskCtrl.uwBTOnOff[uwPVIndexTmp])
					{
						stMpptDisturb[uwPVIndexTmp].dAntiStep = VDC0_1V;
						break;
					}
				}
			}
		}
		else
		{
			uwVoltStepTmp -= stMpptTskCtrl.uwPVONNum;

			if(udCtActivePowerAvg > AC2000W)
			{
				stMpptDisturb[uwPVIndex].dAntiStep = uwVoltStepTmp >> 1;
			}
			else if(udCtActivePowerAvg > AC200W)
			{
				stMpptDisturb[uwPVIndex].dAntiStep = VDC0_1V;
			}
			else if(udCtActivePowerAvg < AC100W)
			{
				stMpptDisturb[uwPVIndex].dAntiStep = 0;
			}
		}
	}
    else
    {
		stMpptDisturb[uwPVIndex].dAntiStep = 0;
        stMpptDisturb[uwPVIndex].uwAntiFlowCnt = 0;
        stMpptDisturb[uwPVIndex].unMpptBits.bit.AntiOverPower = ANTIFLOW_NormalMppt;
    }
}

void MPPT(Uint16 uwPVIndex)
{
	static Uint16 swIntialFlag = 1;
	Uint16 uwPVIndexTmp = 0;
	if((cInverterStatus == eInverterStatus)
	&&(TRUE == stMpptTskCtrl.uwPVConnect[uwPVIndex])&&(false == SvgParam.bEnterSVGStateFlag))
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.Enable = ENABLE;
	}
	else
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.Enable = DISABLE;
	}
	/***********************Over Power **************************************/
	if(stACSample.dActivePower>stLoadLimit.dActPowerLimitOutput + AC500W)		// (CurrentPower > RatedPower+500w)
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.OverPower = 2;
	}
	else if(stACSample.dActivePower>stLoadLimit.dActPowerLimitOutput + AC200W)		// (CurrentPower > RatedPower)
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.OverPower = 1;
	}
	else if(stACSample.dActivePower<stLoadLimit.dActPowerLimitOutput)		// (CurrentPower < RatedPower)
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.OverPower = 0;
	}

	MPPTStrategy(uwPVIndex);

	/***********************Over BT Current **************************************/
	if(stDCSample.wBTCurr[uwPVIndex] > (((int32)stSysCfg.wBTCurrLimitMax[uwPVIndex]*100+3200)>>5))	// BT Current
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.OverBTCurr = 1;
	}
	else if(stDCSample.wBTCurr[uwPVIndex] < (((int32)stSysCfg.wBTCurrLimitMax[uwPVIndex]*100)>>5))	// BT Current
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.OverBTCurr = 0;
	}
	/***********************PV Voltage Low Limit BT Current**************************************/
	if(stDCSample.wBTCurr[uwPVIndex] > (((int32)stLoadLimit.wBTCurrLimit[uwPVIndex]*100+3200)>>5))	// BT Current
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.LimitBTCurr = 1;
	}
	else if(stDCSample.wBTCurr[uwPVIndex] < (((int32)stLoadLimit.wBTCurrLimit[uwPVIndex]*100)>>5))	// BT Current
	{
		stMpptDisturb[uwPVIndex].unMpptBits.bit.LimitBTCurr = 0;
	}
	if(0 == stIVCurve.unIVCurveBits.bit.StartEnable)
	{
		TrackingMppt(&stMpptDisturb[uwPVIndex],&stMpptPara[uwPVIndex]);
		#if 1
		//	if(stMpptDisturb[uwPVIndex].wMpptVoltRef <= stDebug.SetData.wMpptVoltRef[uwPVIndex])
		//	{
		//		stMpptDisturb[uwPVIndex].wMpptVoltRef = stDebug.SetData.wMpptVoltRef[uwPVIndex];
		//	}
		
		stMpptDisturb[uwPVIndex].wPVVoltRef = stMpptDisturb[uwPVIndex].wMpptVoltRef;
		#else
		stMpptDisturb[uwPVIndex].wPVVoltRef = stDebug.SetData.wMpptVoltRef[uwPVIndex];
		#endif

		if(swIntialFlag == 1)
		{
			swIntialFlag = 0;
			for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)//reset para
			{
				m_PVCurveParaReset(&stPvCurveAnalyCtrl[uwPVIndexTmp]);
			}
		}
	}
	else
	{	
		swIntialFlag = 1;
		IVCurve(uwPVIndex);		
	}

}

static void TrackingMppt(ST_MPPT_DISTURB *pstMpptDisturb, ST_MPPT_PARA *pstMpptPara)  // 20ms
{
	int16 wMpptVoltStepSizeTmp=0;
	int32 dMpptPowerErrorTmp=0;
	if(0==pstMpptDisturb->unMpptBits.bit.Enable)
 	{
		pstMpptDisturb->wMpptVoltRef = pstMpptPara->wPVOpenVolt;
		TrackingMpptInit(pstMpptDisturb);
 	}
 	else
 	{
 		if(Initing == pstMpptDisturb->eTrackStatus)		// Enter inv status Mppt Initing
 		{
 		    pstMpptDisturb->uwFastMpptOffCnt = 0;
			if(pstMpptDisturb->wMpptVoltRef < (pstMpptPara->wPVOpenVolt>>1))
 			{
 				pstMpptDisturb->wMpptVoltRef = (pstMpptPara->wPVOpenVolt>>1);
 				pstMpptDisturb->uwMpptTrackInitCnt = 0;
 			}
			  	
 			else if((pstMpptDisturb->wMpptVoltRef  < pstMpptPara->wPVOpenVolt)
 				 &&(pstMpptDisturb->wMpptVoltRef < (MPPT_VOLT_MAX - 200)))
 			{
 				pstMpptDisturb->wMpptVoltRef += 15;			// +1.5V
 				pstMpptDisturb->uwMpptTrackInitCnt = 0;
 			}
 			else
 			{
 				pstMpptDisturb->uwMpptTrackInitCnt++;
 				if(pstMpptDisturb->uwMpptTrackInitCnt > 100) // 20*100=2000ms=2s
 				{
 					pstMpptDisturb->uwMpptTrackInitCnt = 0;
 					pstMpptDisturb->wMpptVoltRefOld = pstMpptPara->wPVVolt;
					if(DISABLE == stDspReceData.unFuncEnable.bit.ExportLimit 
					&& DISABLE == stDspReceData.unFuncEnable.bit.MultiExportLimit)
					{
 						pstMpptDisturb->wMpptVoltRef = pstMpptPara->wPVVolt - 200;			//		pstMpptDisturb->wMpptVoltRef = pstMpptPara->wPVVolt - 100;
					}
 					pstMpptDisturb->eTrackStatus = Runing;
 					/***************ZeroPower Flag & Counter Clear Zero***************/
 					pstMpptDisturb->unMpptBits.bit.ZeroPower = 0;
 					pstMpptDisturb->uwZeroPowerCnt = 0;
 					/***************LowPower Flag & Counter Clear Zero***************/
					pstMpptDisturb->unMpptBits.bit.LowPower = 0;
					pstMpptDisturb->uwLowPowerCnt = 0;
					pstMpptDisturb->uwFastMpptCnt = 0;
					pstMpptDisturb->uwFastMpptFlag =0;
 				}
 			}
 			UPDNLMT(pstMpptDisturb->wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
 		}
 		else if(Runing == pstMpptDisturb->eTrackStatus)			// MPPT Tracking	
 		{
 			pstMpptDisturb->uwMpptTrackInitCnt  = 0;
			/*****************MPPT Power Calculate****************************/
 			if((2==pstMpptDisturb->unMpptBits.bit.OverPower)
 			|| (1==pstMpptDisturb->unMpptBits.bit.OverBTCurr)
			|| (1==pstMpptDisturb->unMpptBits.bit.LimitBTCurr))
 			{
 				pstMpptDisturb->wMpptVoltRef += 2;		// +0.4V~20ms
 				pstMpptPara->dMpptPowerSum = 0;
				UPDNLMT(pstMpptDisturb->wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
				return;
 			} 	
			else if(pstMpptDisturb->uwFastMpptFlag ==0)
			{
				if((pstMpptDisturb->wMpptVoltRef >(int16) ((int32)pstMpptPara->wPVOpenVolt * 890>>10))
				&&(pstMpptDisturb->wMpptVoltRef > (MPPT_VOLT_MIN - 200)))
				{
					pstMpptDisturb->uwFastMpptCnt++;
					if(pstMpptDisturb->uwFastMpptCnt >= 2)
					{
						pstMpptDisturb->uwFastMpptCnt = 0;
		 				pstMpptDisturb->wMpptVoltRef -= 1;			// +0.2V				
						UPDNLMT(pstMpptDisturb->wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
					}
				}
				else
				{
					pstMpptDisturb->uwFastMpptCnt = 0;
					pstMpptDisturb->uwFastMpptFlag =1;
					pstMpptDisturb->uwMpptTrackInitCnt = 0;
				}
 			}
			else if(pstMpptDisturb->unMpptBits.bit.AntiOverPower != ANTIFLOW_NormalMppt)
            {

				if(ANTIFLOW_DownLoading == pstMpptDisturb->unMpptBits.bit.AntiOverPower)
				{
					stDebug.ReadData.wDebug7[5] = stLoadLimit.dActPowerLimitOutput*25/stLoadLimit.dActivePower;
				}
				else
				{
					stDebug.ReadData.wDebug7[5] = 25 - (stLoadLimit.dActPowerLimitOutput*25/stLoadLimit.dActivePower);
				}
				UPDNLMT(stDebug.ReadData.wDebug7[5], 25, 2);

                if(pstMpptDisturb->uwAntiFlowCnt > stDebug.ReadData.wDebug7[5])
                {
                    pstMpptDisturb->uwAntiFlowCnt = 0;
					pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
                    pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
                    if(pstMpptDisturb->unMpptBits.bit.AntiOverPower == ANTIFLOW_Loading)
                    {
                        pstMpptDisturb->wMpptVoltRef -= pstMpptDisturb->dAntiStep;
                        UPDNLMT(pstMpptDisturb->wMpptVoltRef, pstMpptPara->wPVOpenVolt, MPPT_VOLT_MIN);
                    }
                    else if(pstMpptDisturb->unMpptBits.bit.AntiOverPower == ANTIFLOW_Stop)
                    {
                        UPDNLMT(pstMpptDisturb->wMpptVoltRef, pstMpptPara->wPVOpenVolt, MPPT_VOLT_MIN);
                    }
                    else if(pstMpptDisturb->unMpptBits.bit.AntiOverPower == ANTIFLOW_DownLoading)
                    {
                        pstMpptDisturb->wMpptVoltRef += pstMpptDisturb->dAntiStep;
                        UPDNLMT(pstMpptDisturb->wMpptVoltRef, pstMpptPara->wPVOpenVolt + VDC5V, MPPT_VOLT_MIN);
                    }
                }
            }
 			else if(pstMpptDisturb->uwMpptPowerCalcCnt < 50)		// 50*20ms = 1s
 			{
 				pstMpptDisturb->uwMpptPowerCalcCnt++;
 				if(pstMpptDisturb->uwMpptPowerCalcCnt>=34)
 				{
 					pstMpptPara->dMpptPowerSum += pstMpptPara->dMpptPower;
 				}
 			}
 			else  // 0.5s
 			{
 				//pstMpptPara->dMpptPowerAvg = (pstMpptPara->dMpptPowerSum / pstMpptDisturb->uwMpptPowerCalcCnt);		// 100*16>>4 = 100
 				pstMpptPara->dMpptPowerAvg = (pstMpptPara->dMpptPowerSum>>4);		// 100*16>>4 = 100
 				pstMpptPara->dMpptPowerSum = 0;
				pstMpptDisturb->uwMpptPowerCalcCnt = 0;
				pstMpptDisturb->dMpptPower = pstMpptPara->dMpptPowerAvg;
				// stMpptDisturb.dMpptPower(100-1w);		stLoadLimit.dActivePower(10-1w)
				// 60kw -> 600000/10 = 60000 = 600w
				LowPowerCalcMppt(pstMpptDisturb,(stLoadLimit.dActivePower/10));
				if(1==pstMpptDisturb->unMpptBits.bit.ZeroPower)
				{
					pstMpptDisturb->unMpptBits.bit.ZeroPower = 0;
					pstMpptDisturb->eTrackStatus = Initing;			// AcitvePower Low;Remppt
				}
				else
				{
					if(((pstMpptDisturb->wMpptVoltRef+VDC20V)<pstMpptPara->wPVVolt)
					|| (pstMpptDisturb->wMpptVoltRef>(pstMpptPara->wPVVolt+VDC20V)))
		 			{
		 				pstMpptDisturb->wMpptVoltRef = (pstMpptPara->wPVVolt-VDC10V);
						pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
						pstMpptDisturb->wMpptVoltRefOld = pstMpptPara->wPVVolt;
						pstMpptDisturb->eTrackDir = Increase;
						pstMpptDisturb->uwMpptPowerCalcCnt = 0;
		 				pstMpptPara->dMpptPowerSum = 0;
						UPDNLMT(pstMpptDisturb->wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
						
						return;
		 			}
#if 0
					if(pstMpptPara->dMpptPower< MPPT10W && stACSample.dActivePower < stLoadLimit.dActPowerLimitOutput - AC100W)		// 0.04w
		 			{
		 				pstMpptDisturb->wMpptVoltRef = (pstMpptPara->wPVVolt-VDC10V);		// -1V ~ 20ms
		 				pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
						pstMpptDisturb->wMpptVoltRefOld = pstMpptPara->wPVVolt;
						pstMpptDisturb->eTrackDir = Increase;
		 				pstMpptDisturb->uwMpptPowerCalcCnt = 0;
		 				pstMpptPara->dMpptPowerSum = 0;
						UPDNLMT(pstMpptDisturb->wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
						return;
		 			}
#endif
//					if(1==pstMpptDisturb->unMpptBits.bit.LowPower)
//					{
//						wMpptVoltStepSizeTmp = 40;		// 4V
//						dMpptPowerErrorTmp = 1000;		// 10w
//					}
					if(pstMpptPara->dMpptPower< MPPT1000W)
					{
						wMpptVoltStepSizeTmp = 60;		// 12V
						dMpptPowerErrorTmp = 100;		// 1w
					}
					else if(stACSample.dActivePower>stLoadLimit.dActPowerLimitOutput - AC1000W)
					{
						wMpptVoltStepSizeTmp = 10;		//  1V
						dMpptPowerErrorTmp = 100;		// 1w
					}
					else
					{
						wMpptVoltStepSizeTmp = 60;		// 6V
						dMpptPowerErrorTmp = 200;		// 2w
					}
					pstMpptDisturb->wMpptVoltRefMax = MPPT_VOLT_MAX;//pstMpptPara->wPVOpenVolt;
					pstMpptDisturb->wMpptVoltRefMin = MPPT_VOLT_MIN;//(pstMpptPara->wPVOpenVolt>>1);		// 0.5*Vopen
					pstMpptDisturb->uwFastMpptCnt++;
					if(pstMpptDisturb->uwFastMpptCnt>=180)	// 180*1s = 180s = 3min
					{
						pstMpptDisturb->uwFastMpptCnt = 180;
					}
					DisturbObserveMppt(pstMpptDisturb,wMpptVoltStepSizeTmp,dMpptPowerErrorTmp);
					UPDNLMT(pstMpptDisturb->wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
				}
 			}
 		}
 	}
}


static void DisturbObserveMppt(ST_MPPT_DISTURB *pstMpptDisturb, int16 wStepSize, int32 dMpptPowerError)
{
	int16 wMpptVoltStepSizeTmp;
	int32 dStepDeltPvWattAbs;
	int16 wMpptStepWattPer;
	if(1 == pstMpptDisturb->unMpptBits.bit.OverPower)					// full load not mppt
		return;
	/*********************CurrentPower > LastPower************************************/
//	if(pstMpptDisturb->dMpptPower > (pstMpptDisturb->dMpptPowerOld+dMpptPowerError))
//	{
//		pstMpptDisturb->dDecreasePowerErrorOld = 0;
//		pstMpptDisturb->dDecreasePowerError = 0;
//		pstMpptDisturb->dIncreasePowerErrorOld = pstMpptDisturb->dIncreasePowerError;
//		pstMpptDisturb->dIncreasePowerError = pstMpptDisturb->dMpptPower - pstMpptDisturb->dMpptPowerOld;
//		if((pstMpptDisturb->dMpptPower > (pstMpptDisturb->dMpptPowerOld+(dMpptPowerError*5)))
//		||(Increase == pstMpptDisturb->eTrackDir)
//		||(PredictIncrease == pstMpptDisturb->eTrackDir))
//		{
//			pstMpptDisturb->eTrackDir = Increase;
//		}
//		else if((Decrease == pstMpptDisturb->eTrackDir)
//		        ||(PredictDecrease == pstMpptDisturb->eTrackDir))
//		{
//			pstMpptDisturb->eTrackDir = PredictIncrease;
//		}
//		else
//		{
//			pstMpptDisturb->eTrackDir = Increase;
//		}
//		pstMpptDisturb->eTrackDirOld = pstMpptDisturb->eTrackDir;
//		if(Increase == pstMpptDisturb->eTrackDir)
//		{
//			pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
//		}
//		pstMpptDisturb->uwMpptDisturbCnt = 0;
//	}
//	/*********************CurrentPower < LastPower************************************/
//	else if((pstMpptDisturb->dMpptPower+dMpptPowerError)<pstMpptDisturb->dMpptPowerOld)
//	{
//		pstMpptDisturb->dIncreasePowerErrorOld = 0;
//		pstMpptDisturb->dIncreasePowerError = 0;
//		pstMpptDisturb->dDecreasePowerErrorOld = pstMpptDisturb->dIncreasePowerError;
//		pstMpptDisturb->dDecreasePowerError = pstMpptDisturb->dMpptPower - pstMpptDisturb->dMpptPowerOld;
//		if(((pstMpptDisturb->dMpptPower+(dMpptPowerError*5)) > pstMpptDisturb->dMpptPowerOld)
//		||(Decrease == pstMpptDisturb->eTrackDir)
//		||(PredictDecrease == pstMpptDisturb->eTrackDir))
//		{
//			pstMpptDisturb->eTrackDir = Decrease;
//		}
//		else if((Increase == pstMpptDisturb->eTrackDir)
//		        ||(PredictIncrease == pstMpptDisturb->eTrackDir))
//		{
//			pstMpptDisturb->eTrackDir = PredictDecrease;
//		}
//		else
//		{
//			pstMpptDisturb->eTrackDir = Decrease;
//		}
//		pstMpptDisturb->eTrackDirOld = pstMpptDisturb->eTrackDir;
//		if(Decrease == pstMpptDisturb->eTrackDir)
//		{
//			pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
//		}
//		pstMpptDisturb->uwMpptDisturbCnt = 0;
//	}
//	else
//	{
//		pstMpptDisturb->dIncreasePowerErrorOld = 0;
//		pstMpptDisturb->dIncreasePowerError = 0;
//		pstMpptDisturb->dDecreasePowerErrorOld = 0;
//		pstMpptDisturb->dDecreasePowerError = 0;
//		
//		pstMpptDisturb->uwMpptDisturbCnt++;
//		if(pstMpptDisturb->uwMpptDisturbCnt==5)			// 20ms * 5 = 100ms
//		{
//			pstMpptDisturb->eTrackDir = Increase;
//			pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
//			pstMpptDisturb->eTrackDirOld = pstMpptDisturb->eTrackDir;
//		}
//		else if(pstMpptDisturb->uwMpptDisturbCnt>20)		// 20ms * 20 = 400ms
//		{
//			pstMpptDisturb->uwMpptDisturbCnt = 0;
//			pstMpptDisturb->eTrackDir = Decrease;
//			pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
//			pstMpptDisturb->eTrackDirOld = pstMpptDisturb->eTrackDir;
//		}
//	}
//	
//	if(Decrease == pstMpptDisturb->eTrackDir)		// MPPT Power Decrease 
//	{
//		/******************MPPT Reference Voltage Step Size Calculate******************************/
//		if(pstMpptDisturb->dDecreasePowerError > (pstMpptDisturb->dDecreasePowerErrorOld+2*dMpptPowerError)
//			&& stACSample.dActivePower < stLoadLimit.dActPowerLimitOutput - AC1000W)
//		{
//			/****************step = (1+0.5)*step*******************/
//			wMpptVoltStepSizeTmp =  (wStepSize+(wStepSize>>1));
//		}
//		else if(pstMpptDisturb->dDecreasePowerError > (pstMpptDisturb->dDecreasePowerErrorOld+dMpptPowerError))
//		{
//			/****************step = (1+0)*step*******************/
//			wMpptVoltStepSizeTmp =  wStepSize;
//		}
//		else
//		{
//			/****************step = 0.5*step*******************/
//			wMpptVoltStepSizeTmp = (wStepSize>>1);
//		}
//		/************ MPPT Volt Ref < MPPT Volt Ref Old(The MPPT curve is on the left)*****************/
//		if(pstMpptDisturb->wMpptVoltRef <= pstMpptDisturb->wMpptVoltRefOld)
//		{
//			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
//			if(pstMpptDisturb->wMpptVoltRef < (pstMpptDisturb->wMpptVoltRefMax-wMpptVoltStepSizeTmp))
//			{	
//				pstMpptDisturb->wMpptVoltRef += wMpptVoltStepSizeTmp;
//				pstMpptDisturb->uwMpptAddCnt++;
//				pstMpptDisturb->uwMpptSubCnt = 0;
//			}
//			else
//			{
//				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMax;
//			}
//		}
//		else		/*******************MPPT Volt Ref > MPPT Volt Ref Old(The MPPT curve is on the right)************/
//		{
//			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
//			if(pstMpptDisturb->wMpptVoltRef > (pstMpptDisturb->wMpptVoltRefMin + wMpptVoltStepSizeTmp))
//			{	
//				pstMpptDisturb->wMpptVoltRef -= wMpptVoltStepSizeTmp;
//				pstMpptDisturb->uwMpptAddCnt = 0;
//				pstMpptDisturb->uwMpptSubCnt++;
//			}
//			else
//			{
//				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMin;
//			}
//		}
//	}
//	else if(Increase == pstMpptDisturb->eTrackDir)		// MPPT Power Increase
//	{
//		/******************MPPT Reference Voltage Step Size Calculate******************************/
//		if(pstMpptDisturb->dIncreasePowerError > (pstMpptDisturb->dIncreasePowerErrorOld+2*dMpptPowerError)
//			&& stACSample.dActivePower < stLoadLimit.dActPowerLimitOutput - AC1000W)
//		{
//			/****************step = (1+0.5)*step*******************/
//			wMpptVoltStepSizeTmp =  (wStepSize+(wStepSize>>1));
//		}
//		else if(pstMpptDisturb->dIncreasePowerError > (pstMpptDisturb->dIncreasePowerErrorOld+dMpptPowerError))
//		{
//			/****************step = (1+0)*step*******************/
//			wMpptVoltStepSizeTmp =  wStepSize;
//		}
//		else
//		{
//			/****************step = 0.5*step*******************/
//			wMpptVoltStepSizeTmp = (wStepSize>>1);
//		}
//		/************ MPPT Volt Ref < MPPT Volt Ref Old(The MPPT curve is on the right)*****************/
//		if(pstMpptDisturb->wMpptVoltRef <= pstMpptDisturb->wMpptVoltRefOld)
//		{
//			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
//			if(pstMpptDisturb->wMpptVoltRef > (pstMpptDisturb->wMpptVoltRefMin + wMpptVoltStepSizeTmp))
//			{	
//				pstMpptDisturb->wMpptVoltRef -= wMpptVoltStepSizeTmp;
//				pstMpptDisturb->uwMpptAddCnt = 0;
//				pstMpptDisturb->uwMpptSubCnt++;
//			}
//			else
//			{
//				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMin;
//			}
//		}
//		else		/*******************MPPT Volt Ref > MPPT Volt Ref Old(The MPPT curve is on the left)************/
//		{
//			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
//			if(pstMpptDisturb->wMpptVoltRef < (pstMpptDisturb->wMpptVoltRefMax-wMpptVoltStepSizeTmp))
//			{	
//				pstMpptDisturb->wMpptVoltRef += wMpptVoltStepSizeTmp;
//				pstMpptDisturb->uwMpptAddCnt++;
//				pstMpptDisturb->uwMpptSubCnt = 0;
//			}
//			else
//			{
//				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMax;
//			}
//		}
//	}
	//////////////// MPPT DYNAMIC ////////////////////////////
	pstMpptDisturb->dStepDeltPvWatt = pstMpptDisturb->dMpptPower -pstMpptDisturb->dMpptPowerOld;
	if(pstMpptDisturb->dStepDeltPvWatt >= 0)
	{
		dStepDeltPvWattAbs = pstMpptDisturb->dStepDeltPvWatt;
	}
		
	else
	{
		dStepDeltPvWattAbs = -pstMpptDisturb->dStepDeltPvWatt;
	}
	
	if(pstMpptDisturb->dMpptPowerOld <= 0)
		pstMpptDisturb->wMpptStepWattPer = 1000;
	else
		pstMpptDisturb->wMpptStepWattPer = dStepDeltPvWattAbs * 1000 / pstMpptDisturb->dMpptPowerOld;
	
	if(pstMpptDisturb->wMpptStepWattPer > 1000)
		pstMpptDisturb->wMpptStepWattPer = 1000;

	
	wMpptVoltStepSizeTmp =  wStepSize;
	if(pstMpptDisturb->dStepDeltPvWatt < -dMpptPowerError)		// MPPT Power Decrease 
	{
		pstMpptDisturb->uwMpptWattIncreaCnt = 0;
		pstMpptDisturb->uwMpptWattReducCnt++;
		pstMpptDisturb->uwDeadCnt = 0;
		/******************MPPT Reference Voltage Step Size Calculate******************************/

		/************ MPPT Volt Ref < MPPT Volt Ref Old(The MPPT curve is on the left)*****************/
		if(pstMpptDisturb->wMpptVoltRef <= pstMpptDisturb->wMpptVoltRefOld)
		{
			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
			if(pstMpptDisturb->wMpptVoltRef < (pstMpptDisturb->wMpptVoltRefMax-wMpptVoltStepSizeTmp))
			{	
				pstMpptDisturb->wMpptVoltRef += wMpptVoltStepSizeTmp;
				pstMpptDisturb->uwMpptAddCnt++;
				pstMpptDisturb->uwMpptSubCnt = 0;
			}
			else
			{
				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMax;
			}
		}
		else		/*******************MPPT Volt Ref > MPPT Volt Ref Old(The MPPT curve is on the right)************/
		{
			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
			if(pstMpptDisturb->wMpptVoltRef > (pstMpptDisturb->wMpptVoltRefMin + wMpptVoltStepSizeTmp))
			{	
				pstMpptDisturb->wMpptVoltRef -= wMpptVoltStepSizeTmp;
				pstMpptDisturb->uwMpptAddCnt = 0;
				pstMpptDisturb->uwMpptSubCnt++;
			}
			else
			{
				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMin;
			}
		}

		pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
	}
	else if(pstMpptDisturb->dStepDeltPvWatt > dMpptPowerError)		// MPPT Power Increase
	{
		pstMpptDisturb->uwMpptWattIncreaCnt++;
		pstMpptDisturb->uwMpptWattReducCnt = 0;
		pstMpptDisturb->uwDeadCnt = 0;
		/******************MPPT Reference Voltage Step Size Calculate******************************/

		/************ MPPT Volt Ref < MPPT Volt Ref Old(The MPPT curve is on the right)*****************/
		if(pstMpptDisturb->wMpptVoltRef <= pstMpptDisturb->wMpptVoltRefOld)
		{
			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
			if(pstMpptDisturb->wMpptVoltRef > (pstMpptDisturb->wMpptVoltRefMin + wMpptVoltStepSizeTmp))
			{	
				pstMpptDisturb->wMpptVoltRef -= wMpptVoltStepSizeTmp;
				pstMpptDisturb->uwMpptAddCnt = 0;
				pstMpptDisturb->uwMpptSubCnt++;
			}
			else
			{
				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMin;
			}
		}
		else		/*******************MPPT Volt Ref > MPPT Volt Ref Old(The MPPT curve is on the left)************/
		{
			pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
			if(pstMpptDisturb->wMpptVoltRef < (pstMpptDisturb->wMpptVoltRefMax-wMpptVoltStepSizeTmp))
			{	
				pstMpptDisturb->wMpptVoltRef += wMpptVoltStepSizeTmp;
				pstMpptDisturb->uwMpptAddCnt++;
				pstMpptDisturb->uwMpptSubCnt = 0;
			}
			else
			{
				pstMpptDisturb->wMpptVoltRef = pstMpptDisturb->wMpptVoltRefMax;
			}
		}
		pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
	}
	else
	{
		pstMpptDisturb->uwDeadCnt++;
		if(pstMpptDisturb->uwDeadCnt > 2) 	// 2s
		{
			if(pstMpptDisturb->uwMpptDeadStus == 0)
			{
				pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
				pstMpptDisturb->wMpptVoltRef -= VDC5V;
				pstMpptDisturb->uwMpptAddCnt = 0;
				pstMpptDisturb->uwMpptSubCnt++;
				pstMpptDisturb->uwMpptDeadStus = 1;
			}
			else
			{
				pstMpptDisturb->wMpptVoltRefOld = pstMpptDisturb->wMpptVoltRef;
				pstMpptDisturb->wMpptVoltRef += VDC5V;
				pstMpptDisturb->uwMpptAddCnt++; 
				pstMpptDisturb->uwMpptSubCnt = 0;
				pstMpptDisturb->uwMpptDeadStus = 0;
			}
			pstMpptDisturb->uwDeadCnt = 0;
		}
		pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
	}

//	if((pstMpptDisturb->dStepDeltPvWatt > 0 && pstMpptDisturb->wMpptStepWattPer > 50) || pstMpptDisturb->uwMpptWattIncreaCnt >= 5)
//		pstMpptDisturb->uwMpptLeftSideFlag = TRUE;	
//	if(pstMpptDisturb->uwMpptLeftSideFlag == TRUE)
//	{
//		pstMpptDisturb->uwMpptLeftSideFlag = FALSE;
//		pstMpptDisturb->wMpptVoltRef += 2 * wMpptVoltStepSizeTmp;
//		pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
//		pstMpptDisturb->uwMpptAddCnt =0;
//		pstMpptDisturb->uwMpptSubCnt = 0;
//	}
	
	if(pstMpptDisturb->uwFastMpptCnt >= 180)	// 180
	{
		if(pstMpptDisturb->uwMpptAddCnt >= 3)
		{
			pstMpptDisturb->uwMpptAddCnt = 0;
			pstMpptDisturb->wMpptVoltRef -= 2 * wMpptVoltStepSizeTmp;
			pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;
		}
		if(pstMpptDisturb->uwMpptSubCnt >= 3)
		{
			pstMpptDisturb->uwMpptSubCnt = 0;
			pstMpptDisturb->wMpptVoltRef += 2 * wMpptVoltStepSizeTmp;
			pstMpptDisturb->dMpptPowerOld = pstMpptDisturb->dMpptPower;

		}
	}
}


static void LowPowerCalcMppt(ST_MPPT_DISTURB *pstMpptDisturb, int32 dLowPowerLimit)
{
	/********************Zero Power & Counter Calculate****************************/
	if((pstMpptDisturb->dMpptPower<dLowPowerLimit)	// < 1%*RatedPower
	 &&(pstMpptDisturb->dMpptPower<30000))			// 300w
	{
		pstMpptDisturb->uwZeroPowerCnt++;
		if(pstMpptDisturb->uwZeroPowerCnt>=MPPT_POWER_LOW_TIME)
		{
			pstMpptDisturb->uwZeroPowerCnt = 0;
			pstMpptDisturb->unMpptBits.bit.ZeroPower = 1;
		}
	}
	else
	{
		pstMpptDisturb->uwZeroPowerCnt = 0;
	}

	/********************Low Power & Counter Calculate****************************/
	if(pstMpptDisturb->dMpptPower<dLowPowerLimit)
	{
		pstMpptDisturb->uwLowPowerCnt ++;
		if(pstMpptDisturb->uwLowPowerCnt>=(MPPT_POWER_LOW_TIME>>1))
		{
			pstMpptDisturb->uwLowPowerCnt = 0;
			pstMpptDisturb->unMpptBits.bit.LowPower = 1;
		}
	}
	else
	{
		pstMpptDisturb->unMpptBits.bit.LowPower = 0;
		pstMpptDisturb->uwLowPowerCnt =0;
	}
}
static void TrackingMpptInit(ST_MPPT_DISTURB *pstMpptDisturb)  // 20ms
{
	pstMpptDisturb->eTrackStatus = Initing;
	pstMpptDisturb->wMpptVoltRefMax = MPPT_VOLT_MAX;
	pstMpptDisturb->wMpptVoltRefMin = MPPT_VOLT_MIN;
	pstMpptDisturb->dMpptPower = 0;		// Current MPPT Power
	pstMpptDisturb->dMpptPowerOld = 0;		  	// Last MPPT Power
	pstMpptDisturb->dDecreasePowerError = 0;
	pstMpptDisturb->dDecreasePowerErrorOld = 0;
	pstMpptDisturb->dIncreasePowerError = 0;
	pstMpptDisturb->dIncreasePowerErrorOld = 0;
	pstMpptDisturb->eTrackDir = Increase;
	pstMpptDisturb->eTrackDirOld = Increase;
	pstMpptDisturb->uwMpptDisturbCnt = 0;
	pstMpptDisturb->uwMpptPowerCalcCnt = 0;
	pstMpptDisturb->uwMpptTrackInitCnt = 0;
	pstMpptDisturb->unMpptBits.bit.ZeroPower = 0;
	pstMpptDisturb->uwFastMpptFlag = 0;
}
