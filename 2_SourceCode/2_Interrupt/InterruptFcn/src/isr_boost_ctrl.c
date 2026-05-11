/*-------------------------------------------------------
* Solar_BoostCtrl.c
*
*	Product:			three phase inveter
*	Filename:			Solar_BoostCtrl.c
*	Purpose:			DC-DC Control loop
*	Date				Version		Author			Note
*   20/12/11			V1.01		chensx			none
*--------------------------------------------------------*/
#include "dsp_user_include.h"

ST_BTCtrl stBTCtrl[PV_MAX_NUM];
ST_CTRL_LOOP stBusVoltSoftStartCtrl[PV_MAX_NUM];
ST_CTRL_LOOP stPVVoltCtrl[PV_MAX_NUM];
ST_CTRL_LOOP stBTCurrCtrl[PV_MAX_NUM];
ST_CTRL_LOOP stActivePowerCtrl;

/*=============================================================================*
 * FUNCTION: BoostCtrlParaInit()
 * PURPOSE : reset boost control para
 * INPUT: 
 * CALLED BY: EPWM1_zero_isr()
 *============================================================================*/
void BoostCtrlParaInit(void)
{
	Uint16 uwPVIndexTmp;
	for(uwPVIndexTmp=0;	uwPVIndexTmp<PV_MAX_NUM;	uwPVIndexTmp++)
	{
		memset((void*)&stBTCtrl[uwPVIndexTmp], 0, sizeof(stBTCtrl[uwPVIndexTmp]));
		memset((void*)&stBusVoltSoftStartCtrl[uwPVIndexTmp], 0, sizeof(stBusVoltSoftStartCtrl[uwPVIndexTmp]));
		memset((void*)&stPVVoltCtrl[uwPVIndexTmp], 0, sizeof(stPVVoltCtrl[uwPVIndexTmp]));
		memset((void*)&stBTCurrCtrl[uwPVIndexTmp], 0, sizeof(stBTCurrCtrl[uwPVIndexTmp]));

		stBusVoltSoftStartCtrl[uwPVIndexTmp].stPID.stIn.dKp = 1025;
		stBusVoltSoftStartCtrl[uwPVIndexTmp].stPID.stIn.dKi = 60;
		stBusVoltSoftStartCtrl[uwPVIndexTmp].stPID.stIn.dPIMax = 0;
		stBusVoltSoftStartCtrl[uwPVIndexTmp].stPID.stIn.dPIMin = 0;

		stPVVoltCtrl[uwPVIndexTmp].stPID.stIn.dKp = 345;
		stPVVoltCtrl[uwPVIndexTmp].stPID.stIn.dKi = 48;
		stPVVoltCtrl[uwPVIndexTmp].stPID.stIn.dPIMax = 0;
		stPVVoltCtrl[uwPVIndexTmp].stPID.stIn.dPIMin = 0;

		stBTCurrCtrl[uwPVIndexTmp].stPID.stIn.dKp = 245;
		stBTCurrCtrl[uwPVIndexTmp].stPID.stIn.dKi = 56;
		stBTCurrCtrl[uwPVIndexTmp].stPID.stIn.dPIMax = stSysCfg.uwPWMBoostDutyMax ;
		stBTCurrCtrl[uwPVIndexTmp].stPID.stIn.dPIMin = 0;
		stBTCurrCtrl[uwPVIndexTmp].stIn.dCurrLimitMax = 0;
	}
	stActivePowerCtrl.stPID.stIn.dKp = 816;
	stActivePowerCtrl.stPID.stIn.dKi = 88;
}	

/****************************************************************************
*	FunctionName:	IncrementalPID
*	Input:	none
*	Output:	none
*	Description:	△U(t) = U(t) - U(t-1) = Kp[e(t)-e(t-1)] + Ki*e(t) = (Kp+Ki)*e(t) - Kp*e(t-1)
*	Note:	none.
***************************************************************/
#pragma CODE_SECTION(IncrementalPID,"ramfuncs");
void IncrementalPID(ST_CTRL_LOOP *pstCtrlLoop)
{
	int32 dPidCtrlLoopOutputTmp = 0;
	if(POSITIVE_PID == pstCtrlLoop->stIn.uwCtrlLoopEnable)
	{
		pstCtrlLoop->stPID.stIn.dError = (pstCtrlLoop->stIn.dRef - pstCtrlLoop->stIn.dReal);
	}
	else if(NEGATIVE_PID == pstCtrlLoop->stIn.uwCtrlLoopEnable)
	{
		pstCtrlLoop->stPID.stIn.dError = (pstCtrlLoop->stIn.dReal - pstCtrlLoop->stIn.dRef);
	}
	else
	{
		pstCtrlLoop->stPID.stOut.dErrorOld = 0;
		pstCtrlLoop->stPID.stIn.dError = 0;
		pstCtrlLoop->stOut.dCtrlLoopOutput = 0;
		pstCtrlLoop->stOut.dCtrlLoopRemainder = 0;
		return;
	}
	pstCtrlLoop->stPID.stOut.dPIDOut = ((pstCtrlLoop->stOut.dCtrlLoopOutput<<14)
				  + (pstCtrlLoop->stPID.stIn.dKp + pstCtrlLoop->stPID.stIn.dKi) * pstCtrlLoop->stPID.stIn.dError
				  - (pstCtrlLoop->stPID.stIn.dKp*pstCtrlLoop->stPID.stOut.dErrorOld)
				  + pstCtrlLoop->stOut.dCtrlLoopRemainder);
	pstCtrlLoop->stPID.stOut.dErrorOld = pstCtrlLoop->stPID.stIn.dError;
	//pstCtrlLoop->stOut.dCtrlLoopOutput = (pstCtrlLoop->stPID.stOut.dPIDOut>>14);
	dPidCtrlLoopOutputTmp = (pstCtrlLoop->stPID.stOut.dPIDOut>>14);
	pstCtrlLoop->stOut.dCtrlLoopRemainder = (pstCtrlLoop->stPID.stOut.dPIDOut&0x3FFF);

	if(dPidCtrlLoopOutputTmp > pstCtrlLoop->stPID.stIn.dPIMax)
	{
		dPidCtrlLoopOutputTmp = pstCtrlLoop->stPID.stIn.dPIMax;
		pstCtrlLoop->stOut.dCtrlLoopRemainder = 0;
		pstCtrlLoop->stPID.stOut.dErrorOld = 0;
	}
	else if(dPidCtrlLoopOutputTmp < pstCtrlLoop->stPID.stIn.dPIMin)
	{
		dPidCtrlLoopOutputTmp = pstCtrlLoop->stPID.stIn.dPIMin;
		pstCtrlLoop->stOut.dCtrlLoopRemainder = 0;
		pstCtrlLoop->stPID.stOut.dErrorOld = 0;
	}
	pstCtrlLoop->stOut.dCtrlLoopOutput = dPidCtrlLoopOutputTmp;

}
/*************************************************************************************************
*
*	FunctionName:	WaitBusVoltLoopCtrl
*	Input		:	@uwPvId - PV Input ID Number
*	Output:	none
*	Description:	WaitState Bus Voltage soft start control loop;Raise the Bus Voltage to 800V
*	Note:	none.
*************************************************************************************************/
#pragma CODE_SECTION(WaitBusVoltLoopCtrl,"ramfuncs");
void WaitBusVoltLoopCtrl(Uint16 uwPVId)
{
	if((cWaitStatus == eInverterStatus)
	&&(stWaitStatus.eStandbyStep >= cBusSoftStart)
	&&(TRUE == stBTCtrl[uwPVId].uwBtPwmEnable))
	{
		stBusVoltSoftStartCtrl[uwPVId].stIn.uwCtrlLoopEnable = POSITIVE_PID;

		// Bus Voltage Reference
		stBusVoltSoftStartCtrl[uwPVId].stIn.dRef = stMpptTskCtrl.uwBusRef;
		stBusVoltSoftStartCtrl[uwPVId].stIn.dReal = stAdcPool.BusVolt.wArithVal;
		if(stBusVoltSoftStartCtrl[uwPVId].stIn.dRef < stAdcPool.BusVolt.wArithVal - 200)
		{
			stBusVoltSoftStartCtrl[uwPVId].stIn.dRef = (stAdcPool.BusVolt.wArithVal - 200);
		}

		if(stBusVoltSoftStartCtrl[uwPVId].stPID.stIn.dPIMax < stSysCfg.uwPWMBoostDutyMax )
		{
			stBusVoltSoftStartCtrl[uwPVId].stPID.stIn.dPIMax+= 1;
		}
		else
		{
			stBusVoltSoftStartCtrl[uwPVId].stPID.stIn.dPIMax = stSysCfg.uwPWMBoostDutyMax ;
		}
		stBusVoltSoftStartCtrl[uwPVId].stPID.stIn.dPIMin = 0;
	}
	else
	{
		stBusVoltSoftStartCtrl[uwPVId].stPID.stIn.dPIMax = 0;
		stBusVoltSoftStartCtrl[uwPVId].stPID.stIn.dPIMin = 0;
		stBusVoltSoftStartCtrl[uwPVId].stIn.uwCtrlLoopEnable	= PID_DISABLE;
	}
	IncrementalPID(&stBusVoltSoftStartCtrl[uwPVId]);
}
/****************************************************************************
*	FunctionName:	InvPVVoltLoopCtrl
*	Input:			@uwPVId: PV Input Index   (0-PV_MAX_NUM)
*	Output:	none
*	Description:	MPPT Model;PV Voltage Control
*	Note:	running every 1.04ms  1.04ms~(1/32)A -> 30A~998.4ms(parallel)/3993.6ms(independent)
***************************************************************/
#pragma CODE_SECTION(InvPVVoltLoopCtrl,"ramfuncs");
void InvPVVoltLoopCtrl(Uint16 uwPVId)
{
	static Uint16 suwBTCurrLimitBTCurrRef[PV_MAX_NUM];
	static Uint16 suwPowerLimitBTCurrRef[PV_MAX_NUM];
	static Uint16 suwAcPowerLimitCnt[PV_MAX_NUM];
	static int32 sdAcPowerSum5ms[PV_MAX_NUM];
    static Uint16 suwAcPowerLimitavgCnt;
	int32  dActivePowerIsrTmp;
	Uint16 suwPVIndex=0;
	if((cInverterStatus == eInverterStatus)&&(TRUE == stBTCtrl[uwPVId].uwBtPwmEnable)
	&&  (Runing == stMpptDisturb[uwPVId].eTrackStatus))
	{
		stPVVoltCtrl[uwPVId].stIn.uwCtrlLoopEnable	= NEGATIVE_PID;
		/***********************Over Power BT Current Reference Limit**************************************/
		
		dActivePowerIsrTmp = (stAdcPool.ActivePower.dOffsetSumIsr>>4);		// [(2^5 * 2^5)  / 2^6] = 2^4
		if( stDCSample.wBusVolt1ms > stMpptTskCtrl.uwBusRef + VDC30V)
		{
			if(stMpptDisturb[uwPVId].wMpptVoltRef < (stAdcPool.PVVolt[uwPVId].wArithVal  - VDC20V))
			{
				stMpptDisturb[uwPVId].wMpptVoltRef = stAdcPool.PVVolt[uwPVId].wArithVal;
				UPDNLMT(stMpptDisturb[uwPVId].wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
			 	stMpptDisturb[uwPVId].wPVVoltRef = stMpptDisturb[uwPVId].wMpptVoltRef;
			}
			else
			{
				stMpptDisturb[uwPVId].wMpptVoltRef += 5;
				UPDNLMT(stMpptDisturb[uwPVId].wMpptVoltRef, stMpptPara->wPVOpenVolt + VDC5V ,MPPT_VOLT_MIN);
				stMpptDisturb[uwPVId].wPVVoltRef = stMpptDisturb[uwPVId].wMpptVoltRef;
			}
			 
		}

		
		
		sdAcPowerSum5ms[uwPVId] += (stAdcPool.ActivePower.dOffsetSumIsr>>4);
		suwAcPowerLimitCnt[uwPVId]++;
        if(FREQ_60HZ == stFreqRang.uwFreqFlag)
        {
            suwAcPowerLimitavgCnt = 17;
        }
        else
        {
            suwAcPowerLimitavgCnt = 20;
        }
		if(suwAcPowerLimitCnt[uwPVId] >= (suwAcPowerLimitavgCnt>>1))
		{
			suwAcPowerLimitCnt[uwPVId] = 0;
			dActivePowerIsrTmp = sdAcPowerSum5ms[uwPVId] / suwAcPowerLimitavgCnt;
			sdAcPowerSum5ms[uwPVId] = 0;
			
			if(dActivePowerIsrTmp>stLoadLimit.dActPowerOutputRef + ACT300W)	
			{	
				if(dActivePowerIsrTmp>stLoadLimit.dActPowerOutputRef + ACT3000W)	
				{
					if(stBTCurrCtrl[uwPVId].stIn.dRef>2)
					{
						suwPowerLimitBTCurrRef[uwPVId] = stBTCurrCtrl[uwPVId].stIn.dRef-2;
					}
					else
					{
						suwPowerLimitBTCurrRef[uwPVId] = 0;
					}
				}
				else if(((stLoadLimit.uwDeratingMode == SETTING_DERATING)
					   ||(stLoadLimit.uwDeratingMode == MULTI_ANTI_FLOW_DERATING)
					   ||(stLoadLimit.uwDeratingMode == ANTI_FLOW_DERATING)))
				{
					if(stPVVoltCtrl[uwPVId].stIn.dRef < (stAdcPool.PVVolt[uwPVId].wArithVal - VDC5V)  &&  (stPVVoltCtrl[uwPVId].stIn.dRef > MPPT_VOLT_MIN + VDC20V))
					{
						stMpptDisturb[uwPVId].wMpptVoltRef = stAdcPool.PVVolt[uwPVId].wArithVal - VDC5V;						
					}
					stMpptDisturb[uwPVId].wMpptVoltRef += 2;
					UPDNLMT(stMpptDisturb[uwPVId].wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
				}
				else if(stLoadLimit.uwDeratingMode != SETTING_DERATING)
				{
//					for(suwPVIndex = 0; suwPVIndex < stSysCfg.uwPVNumber; suwPVIndex++)
//					{
//						//Average to each MPPT
//						stMpptDisturb[suwPVIndex].wMpptVoltRef += 1;
//					}
				    stMpptDisturb[uwPVId].wMpptVoltRef += 1;
					UPDNLMT(stMpptDisturb[uwPVId].wMpptVoltRef, MPPT_VOLT_MAX ,MPPT_VOLT_MIN);
				}
				stMpptDisturb[uwPVId].wPVVoltRef = stMpptDisturb[uwPVId].wMpptVoltRef;
			}

			if((stACSample.dActivePower<stLoadLimit.dActPowerLimitOutput-200)
			|| (((stLoadLimit.uwDeratingMode == MULTI_ANTI_FLOW_DERATING) || (stLoadLimit.uwDeratingMode == ANTI_FLOW_DERATING))
				&& (ANTIFLOW_DownLoading != stMpptDisturb[uwPVId].unMpptBits.bit.AntiOverPower)))		// (CurrentPower < RatedPower)
			{
				suwPowerLimitBTCurrRef[uwPVId]++;										// 1.04ms*4 + (1/32)A
				if(suwPowerLimitBTCurrRef[uwPVId] > (stSysCfg.wBTCurrLimitMax[uwPVId]+32))
				{
					suwPowerLimitBTCurrRef[uwPVId] = (stSysCfg.wBTCurrLimitMax[uwPVId]+32);
				}
			}
		}
		
		/***********************Over BT Current **************************************/
		if(stAdcPool.BTCurr[uwPVId].wArithVal > (stSysCfg.wBTCurrLimitMax[uwPVId]+32))	// BT Current
		{
			if(stBTCurrCtrl[uwPVId].stIn.dRef>2)
			{
				suwBTCurrLimitBTCurrRef[uwPVId] = stBTCurrCtrl[uwPVId].stIn.dRef-2;
			}
			else
			{
				suwBTCurrLimitBTCurrRef[uwPVId] = 0;
			}

		}
		else if(stAdcPool.BTCurr[uwPVId].wArithVal < (stSysCfg.wBTCurrLimitMax[uwPVId]-16))	// BT Current
		{
			suwBTCurrLimitBTCurrRef[uwPVId]++;										// 1.04ms*4 + (1/32)A
			if(suwBTCurrLimitBTCurrRef[uwPVId] > (stSysCfg.wBTCurrLimitMax[uwPVId]+32))
			{
				suwBTCurrLimitBTCurrRef[uwPVId] = (stSysCfg.wBTCurrLimitMax[uwPVId]+32);
			}
		}
		/**********************BT Current Reference Limit***********************************/
		if(stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax >= (stSysCfg.wBTCurrLimitMax[uwPVId]+32))
		{
			stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax = (stSysCfg.wBTCurrLimitMax[uwPVId]+32);
		}
		else
		{
			stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax++;
		}
		if(stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax >= suwPowerLimitBTCurrRef[uwPVId])
		{
			stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax = suwPowerLimitBTCurrRef[uwPVId];
		}
		if(stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax >= suwBTCurrLimitBTCurrRef[uwPVId])
		{
			stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax = suwBTCurrLimitBTCurrRef[uwPVId];
		}
		if(stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax > stLoadLimit.wBTCurrLimit[uwPVId])
		{
			stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax =  stLoadLimit.wBTCurrLimit[uwPVId];
		}

		// boost current refine limit
		stPVVoltCtrl[uwPVId].stIn.dRef = stMpptDisturb[uwPVId].wPVVoltRef;
		if(stPVVoltCtrl[uwPVId].stIn.dRef < (stAdcPool.PVVolt[uwPVId].wArithVal - VDC10V)  &&  (stPVVoltCtrl[uwPVId].stIn.dRef > MPPT_VOLT_MIN + VDC20V))
		{
			stPVVoltCtrl[uwPVId].stIn.dRef = stAdcPool.PVVolt[uwPVId].wArithVal - VDC10V;
			stMpptDisturb[uwPVId].wMpptVoltRef = stPVVoltCtrl[uwPVId].stIn.dRef;
		}

		stPVVoltCtrl[uwPVId].stIn.dReal = stAdcPool.PVVolt[uwPVId].wArithVal;
	}
	else
	{

		stPVVoltCtrl[uwPVId].stIn.uwCtrlLoopEnable = PID_DISABLE;
		if(eInverterStatus != cInverterStatus)
		{
			stPVVoltCtrl[uwPVId].stPID.stIn.dPIMax = 0;
			suwPowerLimitBTCurrRef[uwPVId] = 0;
			suwBTCurrLimitBTCurrRef[uwPVId] = 0;
		}
		stPVVoltCtrl[uwPVId].stPID.stIn.dPIMin = 0;
	}
	IncrementalPID(&stPVVoltCtrl[uwPVId]);
}

#pragma CODE_SECTION(InvActivePowerLoopCtrl,"ramfuncs");
void InvActivePowerLoopCtrl(void)
{
	if((ENABLE == unSysFlag.bit.InvPwmEnable)&&(cInverterStatus == eInverterStatus))
	{
		stActivePowerCtrl.stIn.uwCtrlLoopEnable	= POSITIVE_PID;
		if(stActivePowerCtrl.stPID.stIn.dPIMax >= stSysCfg.wBTCurrRefLimitMax)
		{
			stActivePowerCtrl.stPID.stIn.dPIMax = stSysCfg.wBTCurrRefLimitMax;
		}
		else
		{
			stActivePowerCtrl.stPID.stIn.dPIMax ++;
		}

		if(stActivePowerCtrl.stPID.stIn.dPIMax > stLoadLimit.wBTCurrLimitMax)
		{
			stActivePowerCtrl.stPID.stIn.dPIMax = stLoadLimit.wBTCurrLimitMax;
		}

		/************************Active Power Reference ********************************/
		/*************************************BT Current Reference Limit*****************************/
		if(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard)
		{
			stActivePowerCtrl.stIn.dRef = stLoadLimit.dActPowerOutputRef>>6;
			stActivePowerCtrl.stIn.dReal = (stAdcPool.ActivePower.dOffsetSumIsr>>10);		// [(2^5 * 2^5)  / 2^6] = 2^4
			if(stActivePowerCtrl.stIn.dRef > (stActivePowerCtrl.stIn.dReal + 100))		// 50*256 = 12.8kw
			{
				stActivePowerCtrl.stIn.dRef = stActivePowerCtrl.stIn.dReal + 100;
			}
			else if(stActivePowerCtrl.stIn.dRef < (stActivePowerCtrl.stIn.dReal - 100))
			{
				stActivePowerCtrl.stIn.dRef = stActivePowerCtrl.stIn.dReal - 100;		// 50*256 = 12.8kw
			}
		}
		else
		{
			stActivePowerCtrl.stIn.dRef = stLoadLimit.dActPowerOutputRef>>8;
			stActivePowerCtrl.stIn.dReal = (stAdcPool.ActivePower.dOffsetSumIsr>>12);		// [(2^5 * 2^5)  / 2^6] = 2^4
			if(stActivePowerCtrl.stIn.dRef > (stActivePowerCtrl.stIn.dReal + 50))		// 50*256 = 12.8kw
			{
				stActivePowerCtrl.stIn.dRef = stActivePowerCtrl.stIn.dReal + 50;
			}
			else if(stActivePowerCtrl.stIn.dRef < (stActivePowerCtrl.stIn.dReal - 50))
			{
				stActivePowerCtrl.stIn.dRef = stActivePowerCtrl.stIn.dReal - 50;		// 50*256 = 12.8kw
			}
		}
	}
	else
	{
		stActivePowerCtrl.stPID.stIn.dPIMax = 0;
		stActivePowerCtrl.stIn.uwCtrlLoopEnable = PID_DISABLE;
		stActivePowerCtrl.stPID.stIn.dPIMin = 0;
	}
	IncrementalPID(&stActivePowerCtrl);
}

#pragma CODE_SECTION(InvBTCurrLoopCtrl,"ramfuncs");
void InvBTCurrLoopCtrl(Uint16 uwPVId)
{
	int16 wBusErrTmp;
	//static int16 swBTCurrLimitMax[uwPVId]={0,0,0,0};
	if((cInverterStatus == eInverterStatus)&&(TRUE == stBTCtrl[uwPVId].uwBtPwmEnable))
	{
		if(stDCSample.wBusVolt1ms>BOOST_OFF_BUS_VOLT_MAX)
		{
			stBTCurrCtrl[uwPVId].stIn.uwCtrlLoopEnable = PID_DISABLE;
		}
		else if((stDCSample.wBusVolt1ms<BOOST_OFF_BUS_VOLT_MAX-300))
		{
			stBTCurrCtrl[uwPVId].stIn.uwCtrlLoopEnable = POSITIVE_PID;
		}
	   // add for 230V Jump change to196v immediatelay can  cause the BUS too high begin
	    wBusErrTmp = stMpptTskCtrl.uwBusRef - stInvBusVoltCtrl.stIn.dReal + 200;
	    if(wBusErrTmp<-300)
	    {
	     		if(stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax>1)
		    	{
		    		stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax--;
		    	}
		    	else
		    	{
				stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax= 1;
		    	}
		      /*if(stBTCurrCtrl.stOut.dCtrlLoopOutput>1)
		    	{
		    		swBTCurrLimitMax[uwPVId] = stBTCurrCtrl.stOut.dCtrlLoopOutput--;
		    	}
		    	else
		    	{
		    		swBTCurrLimitMax[uwPVId] = 1;
		    	}*/

		    	/*if(stBTCurrCtrl[uwPVId].stIn.dReal>1)
		    	{
		    		swBTCurrLimitMax[uwPVId] = stBTCurrCtrl[uwPVId].stIn.dReal--;
		    	}
		    	else
		    	{
		    		swBTCurrLimitMax[uwPVId] = 1;
		    	}*/
	    }
	    else
	    {
		    	if(stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax>stSysCfg.uwPWMBoostDutyMax )
		    	{
		    		stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax = stSysCfg.uwPWMBoostDutyMax ;
		    	}
		    	else
		    	{
				stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax++;
		    	}
	    }
		   // add for 230V Jump change to196v immediatelay can  cause the BUS too high end		
		if(DC_SOURCE == stSysCfg.eMpptMode)
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stActivePowerCtrl.stOut.dCtrlLoopOutput; // DC source mode don't need voltage loop
			//stBTCurrCtrl[uwPVId].stIn.dRef = stDebug.SetData.wBTCurrRef[PVA];
		}
		else if(PARALLEL == stSysCfg.eMpptMode)
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stPVVoltCtrl[stMpptTskCtrl.uwPVVoltMaxID].stOut.dCtrlLoopOutput;
		}
		else
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stPVVoltCtrl[uwPVId].stOut.dCtrlLoopOutput;
		}
		//stBTCurrCtrl[uwPVId].stPID.stIn.dPIMax = stSysCfg.uwPWMBoostDutyMax ;
		stBTCurrCtrl[uwPVId].stPID.stIn.dPIMax = stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax;
		stBTCurrCtrl[uwPVId].stPID.stIn.dPIMin = 0;

		/*************************************BT Current Reference Limit*****************************/
		stBTCurrCtrl[uwPVId].stIn.dReal = stAdcPool.BTCurr[uwPVId].wArithVal;
		//stBTCurrCtrl[uwPVId].stIn.dReal = stAdcPool.BTCurrCtrl[uwPVId].wArithVal;
		if(stBTCurrCtrl[uwPVId].stIn.dRef > (stBTCurrCtrl[uwPVId].stIn.dReal + 16))
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stBTCurrCtrl[uwPVId].stIn.dReal + 16;	// max = 0.5A * 32 = 16
		}
		if(stBTCurrCtrl[uwPVId].stIn.dRef > stSysCfg.wBTCurrLimitMax[uwPVId])
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stSysCfg.wBTCurrLimitMax[uwPVId];
		}
		if(stBTCurrCtrl[uwPVId].stIn.dRef > stLoadLimit.wBTCurrLimit[uwPVId])
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stLoadLimit.wBTCurrLimit[uwPVId];
		}

		if(stBTCurrCtrl[uwPVId].stIn.dRef > stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax)
		{
			stBTCurrCtrl[uwPVId].stIn.dRef = stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax;
		}


	}
	else
	{
		stBTCurrCtrl[uwPVId].stIn.dCurrLimitMax= 0;
		stBTCurrCtrl[uwPVId].stIn.uwCtrlLoopEnable = PID_DISABLE;
	}
	IncrementalPID(&stBTCurrCtrl[uwPVId]);
}

#if PWM_OPEN_LOOP_ENABLE
#pragma CODE_SECTION(BoostPwmOpenLoop,"ramfuncs");
void BoostPwmOpenLoop(void)
{
	EPwm8Regs.CMPA.bit.CMPA = 1042;//stDebug.SetData.uwBtOpenLoopDuty;
	EPwm8Regs.AQCSFRC.all = 0x00;

	EPwm9Regs.CMPA.bit.CMPA = 1042;//stDebug.SetData.uwBtOpenLoopDuty;
	EPwm9Regs.AQCSFRC.all = 0x00;

	EPwm10Regs.CMPA.bit.CMPA = 1042;//stDebug.SetData.uwBtOpenLoopDuty;
	EPwm10Regs.AQCSFRC.all = 0x00;

	EPwm11Regs.CMPA.bit.CMPA = 1042;//stDebug.SetData.uwBtOpenLoopDuty;
	EPwm11Regs.AQCSFRC.all = 0x00;
}
#else
#pragma CODE_SECTION(BoostPwmOnff,"ramfuncs");
void BoostPwmOnff(void)
{

	static Uint16 suwBoostOnOrder = 0;
	static Uint16	suwBtPwmConditionOKFlag = 0;
	static Uint16 suwBTSetDuty=0;
	static Uint16 suwBTSetDutyCnt=0;

	if((cInverterStatus == eInverterStatus)
	||((cWaitStatus == eInverterStatus)&&(stWaitStatus.eStandbyStep >= cBusSoftStart)))
	{
		if(0 != stSysFaultReg.unFaultBit.all)			// Fault->Close PWM
		{
			suwBoostOnOrder = 0;
			stBTCtrl[PVA].uwBtPwmEnable = DISABLE;
			stBTCtrl[PVB].uwBtPwmEnable = DISABLE;
			stBTCtrl[PVC].uwBtPwmEnable = DISABLE;
			stBTCtrl[PVD].uwBtPwmEnable = DISABLE;
		}
		else
		{

/*			 if(((DISABLE == unSysFlag.bit.InvPwmEnableOld)&&(ENABLE == unSysFlag.bit.InvPwmEnable))
			 	||((1 == stWaitStatus.stFanInvtOpenCheck.Flag.Success)&&(cWaitStatus == eInverterStatus)
			 	&&(stWaitStatus.eStandbyStep >= cBusSoftStart)&&(stWaitStatus.stRelayCheck.Flag.InvPwmoff == 0)))
			{
				suwBoostOnOrder = 1;
			}
			 else if((ENABLE == unSysFlag.bit.InvPwmEnableOld)&&(DISABLE == unSysFlag.bit.InvPwmEnable))
			{
				suwBoostOnOrder = 0;
			}*/

           if((ENABLE == unSysFlag.bit.InvPwmEnableOld)&&(DISABLE == unSysFlag.bit.InvPwmEnable))
            {
                suwBoostOnOrder = 0;
            }
            else if((DISABLE == unSysFlag.bit.InvPwmEnableOld)&&(ENABLE == unSysFlag.bit.InvPwmEnable))
            {
                suwBoostOnOrder = 1;
            }

			if((1 == suwBoostOnOrder)&&(SvgParam.bEnterSVGStateFlag == false))
			{
				stBTCtrl[PVA].uwBtPwmEnable = stMpptTskCtrl.uwBTOnOff[PVA];
				stBTCtrl[PVB].uwBtPwmEnable = stMpptTskCtrl.uwBTOnOff[PVB];
				stBTCtrl[PVC].uwBtPwmEnable = stMpptTskCtrl.uwBTOnOff[PVC];
				stBTCtrl[PVD].uwBtPwmEnable = stMpptTskCtrl.uwBTOnOff[PVD];
			}
			else
			{
				stBTCtrl[PVA].uwBtPwmEnable = DISABLE;
				stBTCtrl[PVB].uwBtPwmEnable = DISABLE;
				stBTCtrl[PVC].uwBtPwmEnable = DISABLE;
				stBTCtrl[PVD].uwBtPwmEnable = DISABLE;
			}
		}
	}
	else
	{
		suwBoostOnOrder = 1;

		stBTCtrl[PVA].uwBtPwmEnable = DISABLE;
		stBTCtrl[PVB].uwBtPwmEnable = DISABLE;
		stBTCtrl[PVC].uwBtPwmEnable = DISABLE;
		stBTCtrl[PVD].uwBtPwmEnable = DISABLE;
	}

	if(cInverterStatus == eInverterStatus)
	{
		suwBtPwmConditionOKFlag = TRUE;
		if(suwBTSetDuty!=stDebug.SetData.uwBTSetDuty)
		{
			stBTCtrl[PVA].uwBtPwmDuty = stSysCfg.uwPWMBoostDutyMax ;
			stBTCtrl[PVB].uwBtPwmDuty = stSysCfg.uwPWMBoostDutyMax ;
			stBTCtrl[PVC].uwBtPwmDuty = stSysCfg.uwPWMBoostDutyMax ;
			stBTCtrl[PVD].uwBtPwmDuty = stSysCfg.uwPWMBoostDutyMax ;
			suwBTSetDuty =1;
			//suwBTSetDutyCnt++;
			//if(suwBTSetDutyCnt>=2)
			//{
				//suwBTSetDuty=1;
			//}
		}
		else
		{
			stBTCtrl[PVA].uwBtPwmDuty = (Uint16)stBTCurrCtrl[PVA].stOut.dCtrlLoopOutput;
			stBTCtrl[PVB].uwBtPwmDuty = (Uint16)stBTCurrCtrl[PVB].stOut.dCtrlLoopOutput;
			stBTCtrl[PVC].uwBtPwmDuty = (Uint16)stBTCurrCtrl[PVC].stOut.dCtrlLoopOutput;
			stBTCtrl[PVD].uwBtPwmDuty = (Uint16)stBTCurrCtrl[PVD].stOut.dCtrlLoopOutput;
		}
		
		UPDNLMT(stBTCtrl[PVA].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
		UPDNLMT(stBTCtrl[PVB].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
		UPDNLMT(stBTCtrl[PVC].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
		UPDNLMT(stBTCtrl[PVD].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
	}
	else if((cWaitStatus == eInverterStatus)&&(stWaitStatus.eStandbyStep >= cBusSoftStart))
	{
		suwBtPwmConditionOKFlag = TRUE;
		stBTCtrl[PVA].uwBtPwmDuty = (Uint16)stBusVoltSoftStartCtrl[PVA].stOut.dCtrlLoopOutput;
		stBTCtrl[PVB].uwBtPwmDuty = (Uint16)stBusVoltSoftStartCtrl[PVB].stOut.dCtrlLoopOutput;
		stBTCtrl[PVC].uwBtPwmDuty = (Uint16)stBusVoltSoftStartCtrl[PVC].stOut.dCtrlLoopOutput;
		stBTCtrl[PVD].uwBtPwmDuty = (Uint16)stBusVoltSoftStartCtrl[PVD].stOut.dCtrlLoopOutput;

		UPDNLMT(stBTCtrl[PVA].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
		UPDNLMT(stBTCtrl[PVB].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
		UPDNLMT(stBTCtrl[PVC].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
		UPDNLMT(stBTCtrl[PVD].uwBtPwmDuty,stSysCfg.uwPWMBoostDutyMax ,1);
	}
	else
	{
		suwBtPwmConditionOKFlag = FALSE;
		stBTCtrl[PVA].uwBtPwmDuty = 1;
		stBTCtrl[PVB].uwBtPwmDuty = 1;
		stBTCtrl[PVC].uwBtPwmDuty = 1;
		stBTCtrl[PVD].uwBtPwmDuty = 1;
	}

	if((TRUE == stBTCtrl[PVA].uwBtPwmEnable)&&(TRUE == suwBtPwmConditionOKFlag))
	{
		EPwm8Regs.CMPA.bit.CMPA = stBTCtrl[PVA].uwBtPwmDuty;
		EPwm8Regs.AQCSFRC.all = 0x00;
	}
	else
	{
		EPwm8Regs.CMPA.bit.CMPA = 1;
		EPwm8Regs.AQCSFRC.all = 0x09;
	}

	if((TRUE == stBTCtrl[PVB].uwBtPwmEnable)&&(TRUE == suwBtPwmConditionOKFlag))
	{
		EPwm9Regs.CMPA.bit.CMPA = stBTCtrl[PVB].uwBtPwmDuty;
		EPwm9Regs.AQCSFRC.all = 0x00;
	}
	else
	{
		EPwm9Regs.CMPA.bit.CMPA = 1;
		EPwm9Regs.AQCSFRC.all = 0x09;
	}

	if((TRUE == stBTCtrl[PVC].uwBtPwmEnable)&&(TRUE == suwBtPwmConditionOKFlag))
	{
		EPwm10Regs.CMPA.bit.CMPA = stBTCtrl[PVC].uwBtPwmDuty;
		EPwm10Regs.AQCSFRC.all = 0x00;
	}
	else
	{
		EPwm10Regs.CMPA.bit.CMPA = 1;
		EPwm10Regs.AQCSFRC.all = 0x09;
	}

	if((TRUE == stBTCtrl[PVD].uwBtPwmEnable)&&(TRUE == suwBtPwmConditionOKFlag))
	{
		EPwm11Regs.CMPA.bit.CMPA = stBTCtrl[PVD].uwBtPwmDuty;
		EPwm11Regs.AQCSFRC.all = 0x00;
	}
	else
	{
		EPwm11Regs.CMPA.bit.CMPA = 1;
		EPwm11Regs.AQCSFRC.all = 0x09;
	}
}
#endif


//end of file


