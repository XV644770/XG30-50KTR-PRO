/*-------------------------------------------------------
* Solar_Protect.c
*
*	Product:			three phase inveter
*	Filename:			Solar_Protect.c
*	Purpose:			Protect in intrrupt loop
*	Date				Version		Author			Note
*   20/12/11			V1.01		csx				none
*--------------------------------------------------------*/
#include "dsp_user_include.h"

/****************************************************************************************
*Functions	:	Functions For Boost Current protect in zero interrupt
*Functions	:	Functions For Inv Current protect in zero interrupt
*Functions	:	Functions For Bus Volt protect in zero interrupt
 * FUNCTION: isrLossQuickCheckInZero()
*Global var :	
*
*****************************************************************************************/
#pragma CODE_SECTION(SoftOverProtect,"ramfuncs");
void SoftOverProtect(void)
{
/*****************************BTSoftOverCurrProtect*************************************/
	static Uint32 sudBTOverCurr1Cnt,sudBTOverCurr2Cnt;
	static Uint32 sudBTOverCurrClrCnt=0;
	int16 wBoostCurrTmp;
	int16 wBoostOtherCurrTmp;
	
	static Uint32 sudOverCurrRecoverCnt = 0;
	static Uint16 suwInvCurrRelayCnt=0;
	Uint16 uwInvCurrMaxTmp;

	static Uint32 sudBusErrorClrCnt = 0;

	static Uint32 sudIslandErrorClrCnt=0, sudIslandErrorSetCnt=0;
	static Uint32 sudAcOverVoltClrCnt=0;
	int32 udACVoltSqrtSumTmp=0;
	int16 uwACVoltMaxTmp = 0;
	static Uint32 suwInvOpenLoopCheckCnt=0,suwInvOpenLoopCheckTimers=0;
	static Uint32 sudAcUnderVoltSetCnt=0, sudAcUnderVoltClrCnt=0;

	// Calculate the maximum of boost current
	wBoostCurrTmp = MAX4(stAdcPool.BTCurr[PVA].wArithVal,stAdcPool.BTCurr[PVB].wArithVal,
	                     stAdcPool.BTCurr[PVC].wArithVal,stAdcPool.BTCurr[PVD].wArithVal);
	wBoostOtherCurrTmp = stAdcPool.BTCurr[PVA].wArithVal;       // XG25K,PVA--26A

	if(0 == stSysFaultReg.unOverCurrFault.bit.BTSoftWare)
	{
		/*The instantaneous current exceeds the maximum value*/
	    if((wBoostCurrTmp > stSysCfg.wBTOverCurrFaultMax)||(wBoostOtherCurrTmp > stSysCfg.wBTOverCurrFaultMax1))
	    {
#if FAULT_SHUTDOWN
			stSysFaultReg.unFaultBit.bit.OverCurr = 1;
			stSysFaultReg.unOverCurrFault.bit.BTSoftWare = 1;
#else
			stInvPwm.unPwmDisableBit.bit.SoftBoostOCP = 1;
			stInvPwm.uwPwmDisableDelayCnt = 1;
			stInvPwm.uwSoftBoostOCPTimes++;
#endif
		}

		/*The average current exceeds the maximum value*/
		if((wBoostCurrTmp >= (stSysCfg.wBTOverCurrFaultAvg+48))||
		   (wBoostOtherCurrTmp >= (stSysCfg.wBTOverCurrFaultAvg1+48)))	// 1.5A * 32 = 48
		{
			sudBTOverCurr1Cnt++;
		}
		else if((wBoostCurrTmp <= (stSysCfg.wBTOverCurrFaultAvg+32))||
		        (wBoostOtherCurrTmp <= (stSysCfg.wBTOverCurrFaultAvg1+32)))	// 1.0A * 32 = 32
		{
			sudBTOverCurr1Cnt = 0;
		}
		if(sudBTOverCurr1Cnt>=ISR_BT_OCP1_TIME)		// 60S
		{
			stSysFaultReg.unFaultBit.bit.OverCurr = 1;
			stSysFaultReg.unOverCurrFault.bit.BTSoftWare = 1;
		}

		/*The average current exceeds the maximum value*/
		if((wBoostCurrTmp >= (stSysCfg.wBTOverCurrFaultAvg+96))||
		   (wBoostOtherCurrTmp >= (stSysCfg.wBTOverCurrFaultAvg1+96)))	// 3.0A * 32 = 96
		{
			sudBTOverCurr2Cnt++;
		}
		else if((wBoostCurrTmp <= (stSysCfg.wBTOverCurrFaultAvg+80))||
		        (wBoostOtherCurrTmp <= (stSysCfg.wBTOverCurrFaultAvg1+80)))	// 2.5A * 32 = 80
		{
			sudBTOverCurr2Cnt = 0;
		}
		if(sudBTOverCurr2Cnt>=ISR_BT_OCP2_TIME)		// 10S
		{
			sudBTOverCurr2Cnt = 0;
			stSysFaultReg.unFaultBit.bit.OverCurr = 1;
			stSysFaultReg.unOverCurrFault.bit.BTSoftWare = 1;
		}
		sudBTOverCurrClrCnt = 0;
	}
	else
	{
		if((wBoostCurrTmp <= (stSysCfg.wBTOverCurrFaultAvg-128))||
		   (wBoostOtherCurrTmp <= (stSysCfg.wBTOverCurrFaultAvg1-128)))//BOOST current protect
		{
			sudBTOverCurrClrCnt++;
			if(sudBTOverCurrClrCnt >= ISR_CLR_ERROR_TIME)
			{
				sudBTOverCurrClrCnt = 0;

				sudBTOverCurr1Cnt = 0;
				sudBTOverCurr2Cnt = 0;
				stSysFaultReg.unOverCurrFault.bit.BTSoftWare = 0;
			}
		}
		else
		{
			sudBTOverCurrClrCnt = 0;
		}
	}
/*****************************INVSoftOverCurrProtect************************************/
	// Calculate the maximum of three phase current
	uwInvCurrMaxTmp = MAX3(abs(stAdcPool.RInvCurr.wArithVal),abs(stAdcPool.SInvCurr.wArithVal),abs(stAdcPool.TInvCurr.wArithVal));

	if(0 == stSysFaultReg.unOverCurrFault.bit.INVSoftWare)
	{
		sudOverCurrRecoverCnt=0;
		if(uwInvCurrMaxTmp >= stSysCfg.wInvCurrFaultMax)//RST current protect
		{
			if((cFaultStatus != eInverterStatus) && (1 == unSysFlag.bit.InvPwmEnableOld))
			{
#if FAULT_SHUTDOWN
				stSysFaultReg.unFaultBit.bit.OverCurr = 1;
				stSysFaultReg.unOverCurrFault.bit.INVSoftWare = 1;
#else
				stInvPwm.unPwmDisableBit.bit.SoftInvOCP = 1;
				stInvPwm.uwPwmDisableDelayCnt = 1;
				stInvPwm.uwSoftInvOCPTimes++;
#endif
			}
		}
	}
	else
	{
		if(uwInvCurrMaxTmp <= (stSysCfg.wInvCurrFaultMax-128))//RST current protect
		{
			sudOverCurrRecoverCnt++;
			if(sudOverCurrRecoverCnt > ISR_CLR_ERROR_TIME)	// 5S
			{
				sudOverCurrRecoverCnt = 0;
				stSysFaultReg.unOverCurrFault.bit.INVSoftWare = 0;
			}
		}
		else
		{
			sudOverCurrRecoverCnt = 0;
		}
	}

	/***************Check the inverter current during the waiting state open loop*****/
	if((cWaitStatus == eInverterStatus)&&(1==stWaitStatus.stRelayCheck.Flag.InvPwmOn))
	{
		if(uwInvCurrMaxTmp>=stMachineCfg.wInvCurrRlyChkMax)		// max_curr > max_protect
		{
			suwInvCurrRelayCnt++;
		}
		else if(uwInvCurrMaxTmp<=(stMachineCfg.wInvCurrRlyChkMax-64))	// max_curr < (max_protect-2A)
		{
			if(suwInvCurrRelayCnt>0)
			{
				suwInvCurrRelayCnt --;
			}
		}
		if(suwInvCurrRelayCnt>=5)		// 5*52us = 260us
		{
			suwInvCurrRelayCnt = 0;
			stWaitStatus.stRelayCheck.Flag.InvCurrOver = 1;
            stWaitStatus.uwExitInvStatusCnt++;
			//stDebug.ReadData.uwDebug44++;
		}
	}
	else
	{
		suwInvCurrRelayCnt = 0;
	}

/*****************************BusSoftOverVoltProtect************************************/
	
	if(0 ==  stSysFaultReg.unFaultBit.bit.BusVolt)
	{
		sudBusErrorClrCnt = 0;
		/*Bus Voltage Over Protect Value */
		if((stAdcPool.PBusVolt.wArithVal >= BUS_OCP_HALF_VOLT_MAX)
		 ||(stAdcPool.NBusVolt.wArithVal >= BUS_OCP_HALF_VOLT_MAX)
		 ||(stAdcPool.BusVolt.wArithVal >= BUS_OCP_VOLT_MAX))
		{
			//sudBusVoltOCPCnt ++;
			//if(sudBusVoltOCPCnt>=10)		// 10*52us = 5.2us
			//{
				//sudBusVoltOCPCnt = 0;
				if(cInverterStatus == eInverterStatus)
				{
					stInvPwm.unPwmDisableBit.bit.SoftBusOCP = 1;
					stInvPwm.uwPwmDisableDelayCnt = 1;
					stInvPwm.uwSoftBusOCPTimes++;
				}
		}								
		if((stAdcPool.PBusVolt.wArithVal >= BUS_HALF_VOLT_MAX)
		 ||(stAdcPool.NBusVolt.wArithVal >= BUS_HALF_VOLT_MAX)
		 ||(stAdcPool.BusVolt.wArithVal >= BUS_VOLT_MAX))
		{
			stSysFaultReg.unFaultBit.bit.BusVolt = 1;
			stSysFaultReg.unBusVoltFault.bit.OverVolt = 1;
		}
		else if(abs(stAdcPool.PBusVolt.wArithVal-stAdcPool.NBusVolt.wArithVal) > BUS_HALF_DELTA)
		{
			stSysFaultReg.unFaultBit.bit.BusVolt = 1;
			stSysFaultReg.unBusVoltFault.bit.UnbalanceVolt = 1;
		}
		else if(cInverterStatus == eInverterStatus)
		{
			/*Bus Voltage Under Protect Value */
			if((stAdcPool.PBusVolt.wArithVal <= BUS_HALF_VOLT_MIN)
			|| (stAdcPool.NBusVolt.wArithVal <= BUS_HALF_VOLT_MIN)
			|| (stAdcPool.BusVolt.wArithVal <= BUS_VOLT_MIN))
			{
				stSysFaultReg.unFaultBit.bit.BusVolt = 1;
				stSysFaultReg.unBusVoltFault.bit.UnderVolt = 1;
			}
		}
	}
	else
	{
		if((stAdcPool.PBusVolt.wArithVal <= BUS_HALF_VOLT_RECOVERY) && (stAdcPool.NBusVolt.wArithVal <= BUS_HALF_VOLT_RECOVERY))
		{
			sudBusErrorClrCnt++;
			if(sudBusErrorClrCnt > ISR_CLR_ERROR_TIME) 	// 10S Recovery Bus Abnormal time
			{
				sudBusErrorClrCnt = 0;
				stSysFaultReg.unFaultBit.bit.BusVolt = 0;
				stSysFaultReg.unBusVoltFault.bit.OverVolt = 0;
				stSysFaultReg.unBusVoltFault.bit.UnderVolt = 0;
				stSysFaultReg.unBusVoltFault.bit.UnbalanceVolt = 0;
			}
		}
		else
		{
			sudBusErrorClrCnt = 0;
		}
	}

 /*****************************ACSoftOverVoltProtect************************************/

	// Calculate the sqrt sum of three phase voltage
	udACVoltSqrtSumTmp = ((((int32)stAdcPool.RGridVolt.wArithVal * stAdcPool.RGridVolt.wArithVal)
						 + ((int32)stAdcPool.SGridVolt.wArithVal * stAdcPool.SGridVolt.wArithVal)
						 + ((int32)stAdcPool.TGridVolt.wArithVal * stAdcPool.TGridVolt.wArithVal))>>14);

	// Calculate the maximum of three phase voltage
	uwACVoltMaxTmp = MAX3(abs(stAdcPool.RGridVolt.wArithVal),abs(stAdcPool.SGridVolt.wArithVal),abs(stAdcPool.TGridVolt.wArithVal));

	/***************Check the inverter voltage during the waiting state open loop*****/
	//if((cWaitStatus == eInverterStatus)&&(1==stWaitStatus.stRelayCheck.Flag.InvPwmOn))
	if((cWaitStatus == eInverterStatus)&&((1==stWaitStatus.stRelayCheck.Flag.InvPwmOn)||(cRelayCheck == stWaitStatus.eStandbyStep))) 
	{
		suwInvOpenLoopCheckTimers =10;	// 10Timers
	}
	else
	{
		suwInvOpenLoopCheckTimers = 3;
	}
	

	/************************************Grid Voltage Over Range Protect***********************************************************/
	if(0 == stSysFaultReg.unGridVoltFault.bit.OverVoltIsr)
	{
		if((udACVoltSqrtSumTmp>=stSysCfg.dACVoltSqrtSumFaultMax)
	      || (uwACVoltMaxTmp>=stSysCfg.wACVoltFaultMax)
	      || (stInvPara.wVolt_d>stSysCfg.wACVoltFaultMax))
		{
			suwInvOpenLoopCheckCnt++;
			if(suwInvOpenLoopCheckCnt>=suwInvOpenLoopCheckTimers)	//10 Timers
			{
				suwInvOpenLoopCheckCnt = 0;
				stSysFaultReg.unGridVoltFault.bit.OverVoltIsr = 1;
				stSysFaultReg.unFaultBit.bit.GridVolt = 1;
			}
		}
		else
		{
			suwInvOpenLoopCheckCnt = 0;
		}
	}
	else
	{
		if((udACVoltSqrtSumTmp < (stSysCfg.dACVoltSqrtSumFaultMax -SQRT_SUM_5V))
	      && (uwACVoltMaxTmp < (stSysCfg.wACVoltFaultMax-PHASE_5V))
	      && (stInvPara.wVolt_d < (stSysCfg.wACVoltFaultMax-PHASE_5V)))
		{
			sudAcOverVoltClrCnt++;
			if(sudAcOverVoltClrCnt >= ISR_CLR_ERROR_TIME)
			{
				sudAcOverVoltClrCnt = 0;
				stSysFaultReg.unGridVoltFault.bit.OverVoltIsr = 0;
			}
		}
		else
		{
			sudAcOverVoltClrCnt = 0;
		}
	}

	/************************************Grid Voltage Over Range Protect***********************************************************/
	if(0 == stSysFaultReg.unGridVoltFault.bit.UnderVoltIsr)
	{
		if((cInverterStatus == eInverterStatus)
		||((cWaitStatus == eInverterStatus)&&(1==stWaitStatus.stRelayCheck.Flag.InvPwmOn)))
		{
			if((udACVoltSqrtSumTmp<=stSysCfg.dACVoltSqrtSumFaultMin)
			|| (stInvPara.wVolt_d<=stSysCfg.wACVoltFaultMin))
			{
				sudAcUnderVoltSetCnt ++;
				if(sudAcUnderVoltSetCnt>=10)		// 10*52us = 5.2us
				{
					sudAcUnderVoltSetCnt = 0;
#if FAULT_SHUTDOWN
					stSysFaultReg.unGridVoltFault.bit.UnderVoltIsr = 1;
					stSysFaultReg.unFaultBit.bit.GridVolt = 1;
#else
					if(cInverterStatus == eInverterStatus)
					{
						if(STRANDARD_TAIWAI != stDspReceData.unSafetyOdm.bit.SafetyStandard)
						{
							stInvPwm.unPwmDisableBit.bit.SoftUnderVolt = 1;
							stInvPwm.uwPwmDisableDelayCnt = 1;
						}
					}
					else
					{
						stSysFaultReg.unGridVoltFault.bit.UnderVoltIsr = 1;
						stSysFaultReg.unFaultBit.bit.GridVolt = 1;
					}
#endif
				}
			}
			else
			{
				sudAcUnderVoltSetCnt = 0;
			}
		}
		sudAcUnderVoltClrCnt = 0;
	}
	else
	{
		sudAcUnderVoltSetCnt = 0;
		if((udACVoltSqrtSumTmp>=(stSysCfg.dACVoltSqrtSumFaultMin+SQRT_SUM_5V))
		|| ((stInvPara.wVolt_d>=stSysCfg.wACVoltFaultMin+PHASE_5V)))
		{
			sudAcUnderVoltClrCnt++;
			if(sudAcUnderVoltClrCnt >= ISR_CLR_ERROR_TIME)
			{
				sudAcUnderVoltClrCnt = 0;
				stSysFaultReg.unGridVoltFault.bit.UnderVoltIsr = 0;
			}
		}
		else
		{
			sudAcUnderVoltClrCnt = 0;
		}
	}

	/************************************island protect***********************************************************/
	if(0 == stSysFaultReg.unGridVoltFault.bit.IslandIsr)
	{
		if((cInverterStatus == eInverterStatus)
		||((cWaitStatus == eInverterStatus)&&(1==stWaitStatus.stRelayCheck.Flag.InvPwmOn)))
		{
			if((stInvPara.wVolt_q>=ISLAND_MAX_PROTECT) || (stInvPara.wVolt_q<=(-ISLAND_MAX_PROTECT)))		// +/-130 V
			{
				sudIslandErrorSetCnt++;
				if(sudIslandErrorSetCnt>=5)		// 5*52us = 260us
				{
					sudIslandErrorSetCnt = 0;
					#if FAULT_SHUTDOWN
						stSysFaultReg.unFaultBit.bit.GridVolt = 1;
						stSysFaultReg.unGridVoltFault.bit.IslandIsr = 1;
					#else
						if(cInverterStatus == eInverterStatus)
						{
							stInvPwm.unPwmDisableBit.bit.SoftVoltq = 1;
							stInvPwm.uwPwmDisableDelayCnt = 1;
						}
						else
						{
							stSysFaultReg.unFaultBit.bit.GridVolt = 1;
							stSysFaultReg.unGridVoltFault.bit.IslandIsr = 1;
						}
					#endif
				}
			}
			else if((stInvPara.wVolt_q>=(ISLAND_MAX_PROTECT-960)) && (stInvPara.wVolt_q <= -(ISLAND_MAX_PROTECT-960)))	// +/-100 V
			{
				if(sudIslandErrorSetCnt>0)
				{
					sudIslandErrorSetCnt--;
				}
			}
		}

		sudIslandErrorClrCnt = 0;
	}
	else
	{
		sudIslandErrorClrCnt++;
		if(sudIslandErrorClrCnt >= ISR_CLR_ERROR_TIME)
		{
			sudIslandErrorClrCnt = 0;
			stSysFaultReg.unGridVoltFault.bit.IslandIsr = 0;
		}
	}

}



