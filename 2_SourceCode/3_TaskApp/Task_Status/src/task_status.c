/*
 * Task_status.c
 *
 *  Created on: 2020年11月26日
 *      Author: Administrator
 */

#include "dsp_user_include.h"
#include "update_dsp.h"

ST_WAIT_STATUS stWaitStatus;
ENUM_INVERTER_STATUS eInverterStatus;

extern int16 uwCheckOffset(int16 wRelaValue);

static void LEDToggle(Uint16 uwLEDBlinkTime);
static void StatusChange(void);
static void BusSoftStart(void);
static void	InitialStatus(void);
static void WaitStatus(void);
static void InvStatus(void);
static void FlashStatus(void);
static void FaultStatus(void);
static void ExitCurrStatus(ENUM_INVERTER_STATUS SysNextINVStatus);
static void FanInvtPwmCheck(void);

TASK_EVENT event = 0;

/****************************************************************************
*
*	FunctionName:	StatusTaskInit
*	Input:	none
*	Output:	none
*	Description:	Status Task Variable init
*	Note:	none.
***************************************************************/
void StatusTaskInit(void)
{
    memset((void*)&stWaitStatus,0,sizeof(stWaitStatus));
    eInverterStatus = cInitialStatus;
    stWaitStatus.eStandbyStep = cInit;
    stWaitStatus.stRelayCheck.wPhaseVoltRmsMax = ((int32)stDspReceData.uwACNormalVolt*296)>>10;		// Rms * 0.5
    stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*197)>>10;		// Rms * 0.33
}

/****************************************************************************
*	FunctionName:	StatusTask
*	Input:	none
*	Output:	none
*	Description:	dsp
*	Note:	none.
***************************************************************/
void StatusTask(void)
{
	ExitCurrStatus(cInitialStatus);
	
	while(1)
	{
		event=OSEventPend();		// idle
		// status change check
		StatusChange();
		// Inverter state switching
		switch(eInverterStatus)
		{
			case cInitialStatus:
				InitialStatus();
			break;
			
			case cWaitStatus:
				WaitStatus();
			break;

			case cInverterStatus:
				InvStatus();
			break;

			case cFaultStatus:
				FaultStatus();
			break;

			case cFlashStatus:
				FlashStatus();
			break;

			default:
				FaultStatus();
			break;
		}
	}
}

/****************************************************************************
*	FunctionName:	StatusChange
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
static void StatusChange(void)
{
	if(event&(1<<STATUS_TO_WAIT))		// To wait status
	{
		ExitCurrStatus(cWaitStatus);
	}

	if(event&(1<<STATUS_TO_INVERTER)) // To Invert status
	{
		eInverterStatus = cInverterStatus;
	}

	if(event&(1<<STATUS_TO_FAULT))	// To fault status
	{
		ExitCurrStatus(cFaultStatus);
	}

	if(event&(1<<STATUS_TO_UPDATE))	// To fault status
	{
		ExitCurrStatus(cFlashStatus);
	}
}

/****************************************************************************
*	FunctionName:	ExitCurrStatus
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
static void ExitCurrStatus(ENUM_INVERTER_STATUS SysNextINVStatus)
{
	Uint16 uwGridCountDownTmp = 60;		// default time : 30S

	OS_ENTER_CRITICAL();
	eInverterStatus = SysNextINVStatus;
	stWaitStatus.eStandbyStep = cInit;
	stWaitStatus.stBusStart.uwStartCnt=0;
	stWaitStatus.stBusStart.Flag.Fail = 0;
	stWaitStatus.stBusStart.Flag.Success = 0;

	stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt = 0;
	stWaitStatus.stFanInvtOpenCheck.Flag.Success = 0;
	stWaitStatus.stFanInvtOpenCheck.Flag.Fail = 0;

	stWaitStatus.stRelayCheck.Flag.Success = 0;
	stWaitStatus.stRelayCheck.Flag.Fail = 0;
	stWaitStatus.stRelayCheck.Flag.InvPwmOn = 0;
	stWaitStatus.stRelayCheck.Flag.InvPwmoff = 0;
	stWaitStatus.stRelayCheck.Flag.InvCurrOver = 0;
	stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
	stWaitStatus.stRelayCheck.uwCheckCnt = 0;
	stWaitStatus.stRelayCheck.uwF042OnOff = 0;
	DSP_RELAY_LOW;
	unSysFlag.bit.bInvStart = 0;
	//unSysFlag.bit.GFCISelfCheckFinished=0;
	unSysFlag.bit.ISOCheckFinished=0;
	OS_EXIT_CRITICAL();
	
	if(stautotest.uwEnableFlag ==DISABLE)
	{
		//uwrestartcnt =4;
	}
	else
	{
		//uwrestartcnt =9;
		stWaitStatus.uwExitInvStatusCnt =0;
	}
	
	// Reconnect delay time initialization
	if((stF107Data.uwAdjustMode==ATE_ADJUST_30)||(stF107Data.uwAdjustMode==ATE_ADJUST_70))
	{
		uwGridCountDownTmp = 40;		// 20s
	}
	else if(stWaitStatus.uwExitInvStatusCnt>4)	// Within 10 minutes, it will exit connection for 5 times, Grid connection countdown 1min
	{
		uwGridCountDownTmp = 1200;		// 1200/20ms = 60s
	}
	else
	{
		if(stF107Data.uwStartDelayTime >= 10)		// 10s
		{
			if(1 == unSysFlag.bit.GoToInvOnce)
			{
				uwGridCountDownTmp = stF107Data.uwReStartDelayTime*2;
			}
			else
			{
				uwGridCountDownTmp = stF107Data.uwStartDelayTime*2;
			}
		}
		else
		{
			uwGridCountDownTmp = 60;		// 30s
		}
	}

	stTaskTimer.uwStartDelayTime = uwGridCountDownTmp;
}

/****************************************************************************
*	FunctionName:	InitialStatus
*	Input:	none
*	Output:	none
*	Description:	inverter initial status,
*	Note:	none.
***************************************************************/
static void	InitialStatus(void)
{

	static Uint16 uwCount = 0;
	static int32	sudGFCIAvgSum=0;
	static int32	sudRDCIAvgSum=0;
	static int32	sudSDCIAvgSum=0;
	static Uint16 uwADCnt = 0;
		
	if(event&(1<<STATUS_TASK_PERIOD))			// every 10ms run 1 times
	{
		LEDToggle(50);		// 1s blink
		if((++uwCount >= 300) && (TRUE == unSysFlag.bit.MachineCheckEnd))						// 3S
		{	
			uwCount = 0;
			ExitCurrStatus(cWaitStatus);
		}
		else if(uwCount >= 250 && uwCount < 280)
		{
			uwADCnt++;
			sudGFCIAvgSum += stAdcPool.GFCI.wArithVal;
			sudRDCIAvgSum += stAdcPool.RHardDCI.wArithVal;
			sudSDCIAvgSum += stAdcPool.SHardDCI.wArithVal;
			
		}
		else if(uwCount == 280)
		{
			stAdcPool.GFCI.wOffset = sudGFCIAvgSum / uwADCnt;
			//stAdcPool.GFCI.wOffset = uwCheckOffset(sudGFCIAvgSum/uwADCnt);
			if((stAdcPool.GFCI.wOffset > 1500)||(stAdcPool.GFCI.wOffset < -1500))	// +/- 100mA
			{
				stAdcPool.GFCI.wOffset = 0;
			}

			stAdcPool.RHardDCI.wOffset = sudRDCIAvgSum / uwADCnt;
			stAdcPool.SHardDCI.wOffset = sudSDCIAvgSum / uwADCnt;
			uwADCnt = 0;
		}
		
		if(0 != stF107Data.unSet.bit.FlashReady)		// Receive Update Command
		{
			stDebug.ReadData.uwFaultStatus += 10;
			OSEventSend(STATUS_PRIO, STATUS_TO_UPDATE);	//Event trigger to flash status
		}
	}

}
/****************************************************************************
*	FunctionName:	WaitStatus
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
static void WaitStatus(void)
{
	static Uint16 suwInvConnectCnt=0;
	static Uint16 suwbInvStartOld=0;
	int16	wACVoltPeakTmp;
	
	if(event&(1<<STATUS_TASK_PERIOD))			// every 10ms run 1 times
	{
		LEDToggle(50);		// 1s blink
	    if((stDebug.ReadData.wInvVoltLack == 1))// && (0 = stSysFaultReg.unFaultBit.all))
	    {
	        stDebug.ReadData.wInvVoltLackReCnt = 0;
	        stDebug.ReadData.wInvVoltLackCnt++;
	        if(stDebug.ReadData.wInvVoltLackCnt >= 12000)//12000*10ms = 2min
	        {
	            stDebug.ReadData.wInvVoltLackCnt = 12000;
	            stSysFaultReg.unFaultBit.bit.GridVolt = 1;
	            stSysFaultReg.unGridVoltFault.bit.UtilityLoss = 1;
	        }
	    }
	    else
	    {
	        stDebug.ReadData.wInvVoltLackCnt = 0;
	        stDebug.ReadData.wInvVoltLackReCnt++;
	        if(stDebug.ReadData.wInvVoltLackReCnt >= 500)//12000*10ms = 2min
	        {
	            stDebug.ReadData.wInvVoltLackReCnt = 500;
	            stSysFaultReg.unFaultBit.bit.GridVolt = 0;
	            stSysFaultReg.unGridVoltFault.bit.UtilityLoss = 0;
	        }
	        stDebug.ReadData.wInvVoltLackCnt = 0;
	    }

	}
	if(0 != stSysFaultReg.unFaultBit.all) 			// fault return true
	{
		stDebug.ReadData.uwWaitStatus = 2;
		stDebug.ReadData.uwWaitError = 1000+ GetUint64Bit(stSysFaultReg.unFaultBit.all);
		OSEventSend(STATUS_PRIO, STATUS_TO_FAULT);	//Event trigger to fault status
	}
	else if((0 == stTaskTimer.uwStartDelayTime)
		 &&(1 == unSysFlag.bit.bInvStart)
		 &&(1 == unSysFlag.bit.SampleOffsetFinished)
		 &&(1 == unSysFlag.bit.ISOCheckFinished)
		 &&(1 == unSysFlag.bit.GFCISelfCheckFinished)
		 &&(1 == unSysFlag.bit.MachineCheckEnd))
	{
		if(event&(1<<STATUS_TASK_PERIOD))			// every 10ms run 1 times
		{
			switch(stWaitStatus.eStandbyStep)
			{
				case cInit:
					stWaitStatus.eStandbyStep = cBusSoftStart;
					//stWaitStatus.eStandbyStep = cRelayCheck;
					stWaitStatus.stBusStart.uwStartCnt=0;
                    stWaitStatus.uwWaitBusRefBkp = 0;
                    stWaitStatus.uwBusLowCnt = 0;
				break;

				case cBusSoftStart:
					BusSoftStart();
					if(1 == stWaitStatus.stBusStart.Flag.Fail)		// relay check fail
					{
						stWaitStatus.stBusStart.Flag.Fail = 0;
						stWaitStatus.stBusStart.uwStartCnt=0;
						stWaitStatus.uwExitInvStatusCnt ++;
						stDebug.ReadData.uwWaitStatus = 4;
						stDebug.ReadData.uwWaitError += 20000;
						OSEventSend(STATUS_PRIO, STATUS_TO_WAIT);
					}
					else if(1 == stWaitStatus.stBusStart.Flag.Success)	// relay fail success
					{
						if(OFF == stDebug.SetData.unSetReg.bit.BoostPwmLoop)
						{
						    stWaitStatus.uwWaitBusRefBkp = stMpptTskCtrl.uwBusRef;
							stDebug.ReadData.uwWaitStatus = 11;
							//stWaitStatus.eStandbyStep = cFanInvtOpenCheck;
							stWaitStatus.eStandbyStep = cRelayCheck;
							stWaitStatus.stRelayCheck.uwCheckCnt =0;
						}
						else
						{
							stDebug.ReadData.uwWaitStatus = 10;
							stDebug.ReadData.uwWaitError += 10;
						}
					}

				break;

				/*case cFanInvtOpenCheck:
					// example:	ACVolt=230V		BusVolt = 230*1.732*1.414
					wACVoltPeakTmp  = (((int32)stACSample.wLineVoltRmsMax * 1448)>>10);

					if(stAdcPool.BusVolt.wArithVal < wACVoltPeakTmp)
					{
						if((ENABLE == stBTCtrl[PVA].uwBtPwmEnable)
						 ||(ENABLE == stBTCtrl[PVB].uwBtPwmEnable)
						 ||(ENABLE == stBTCtrl[PVC].uwBtPwmEnable)
						 ||(ENABLE == stBTCtrl[PVD].uwBtPwmEnable))
						{
							stDebug.ReadData.uwWaitStatus = 13;
							ExitCurrStatus(cWaitStatus);
							return;
						}
					}

					FanInvtPwmCheck();
					if(1 == stWaitStatus.stFanInvtOpenCheck.Flag.Fail)
					{
						stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt = 0;
						stWaitStatus.stFanInvtOpenCheck.Flag.Fail = 0;
						stWaitStatus.uwExitInvStatusCnt ++;
						OSEventSend(STATUS_PRIO, STATUS_TO_WAIT);
					}
					else if(1 == stWaitStatus.stFanInvtOpenCheck.Flag.Success)
					{
						stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt = 0;
						stWaitStatus.eStandbyStep = cRelayCheck;
					}

				break;*/

				case cRelayCheck:
					// example:	ACVolt=230V		BusVolt = 230*1.732*1.414
					wACVoltPeakTmp  = (((int32)stACSample.wLineVoltRmsMax * 1448)>>10);
					
					if(stAdcPool.BusVolt.wArithVal < wACVoltPeakTmp)
					{
						if((ENABLE == stBTCtrl[PVA].uwBtPwmEnable)
						 ||(ENABLE == stBTCtrl[PVB].uwBtPwmEnable)
						 ||(ENABLE == stBTCtrl[PVC].uwBtPwmEnable)
						 ||(ENABLE == stBTCtrl[PVD].uwBtPwmEnable))
						{
							stDebug.ReadData.uwWaitStatus = 8;
                            stWaitStatus.uwExitInvStatusCnt++;
							ExitCurrStatus(cWaitStatus);
							
							return;
						}

                        stWaitStatus.uwBusLowCnt++;
                        if(stWaitStatus.uwBusLowCnt >= 5)
                        {
                            stWaitStatus.uwBusLowCnt = 0;
                            stWaitStatus.uwExitInvStatusCnt++;
							ExitCurrStatus(cWaitStatus);
							return;
                        }
					}

                    if((stWaitStatus.uwWaitBusRefBkp -VOLT_50V) > stMpptTskCtrl.uwBusRef)
                    {
                        stWaitStatus.uwBusRefLowCnt++;
                        if(stWaitStatus.uwBusRefLowCnt >= 5)
                        {
                            stWaitStatus.uwBusRefLowCnt = 0;
                            stWaitStatus.uwExitInvStatusCnt++;
                            ExitCurrStatus(cWaitStatus);
							return;
                        }
                    }

					if(ENABLE == stDspReceData.unFuncEnable.bit.RelayCheck)
					{
						DoubleRelayCheck();
					}
					else
					{
						SingleRelayCheck();
					}

					if(1 == stWaitStatus.stRelayCheck.Flag.Fail)		// relay check fail
					{
						stWaitStatus.stRelayCheck.Flag.Fail = 0;
						stWaitStatus.stRelayCheck.uwCheckCnt =0;
						stWaitStatus.stRelayCheck.uwFaultTimes++;
						if(stWaitStatus.stRelayCheck.uwFaultTimes>=5)	// Continuous detection of relay failure for 5 times, error shutdown
						{
							stSysFaultReg.unFaultBit.bit.RelayCheck = 1;
							OSEventSend(STATUS_PRIO, STATUS_TO_FAULT);
							stWaitStatus.stRelayCheck.uwFaultTimes = 0;

							stDebug.ReadData.uwWaitStatus = 5;
							stDebug.ReadData.uwWaitError += 20000;
						}
						else
						{
							ExitCurrStatus(cWaitStatus);

							stDebug.ReadData.uwWaitStatus = 6;
							stDebug.ReadData.uwWaitError += 30000;
						}
					}
					else if(1 == stWaitStatus.stRelayCheck.Flag.Success)	// relay check success
					{
						stWaitStatus.stRelayCheck.uwFaultTimes = 0;
						stWaitStatus.eStandbyStep = cGridConnect;
						suwInvConnectCnt = 0;
					}
					
				break;

				case cGridConnect:
					suwInvConnectCnt++;
					if(suwInvConnectCnt >= 50)  // delay 50*10ms = 500ms = 1s
					{
						suwInvConnectCnt = 0;
						OSEventSend(STATUS_PRIO, STATUS_TO_INVERTER);
						stDebug.ReadData.uwWaitStatus = 7;
						stDebug.ReadData.uwWaitError += 40000;
						stWaitStatus.eStandbyStep = cInit;
					}
					break;
				default:
					stDebug.ReadData.uwWaitStatus = 9;
					stDebug.ReadData.uwWaitError += 10;
					stWaitStatus.eStandbyStep = cInit;
					break;
			}
		}
	}
	else if((1==suwbInvStartOld) && (unSysFlag.bit.bInvStart==0))
	{
		stDebug.ReadData.uwWaitStatus = 3;
		stDebug.ReadData.uwWaitError += 1;
		ExitCurrStatus(cWaitStatus);
	}
	suwbInvStartOld = unSysFlag.bit.bInvStart;
	
}

/****************************************************************************
*	FunctionName:	InvStatus
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
static void InvStatus(void)
{
//	DSP_RELAY_PWM;
	DSP_RELAY_PWM(stMachineCfg.wRelayPwmCap);
	unSysFlag.bit.GoToInvOnce = 1;
	if(event&(1<<STATUS_TASK_PERIOD))			// every 10ms run 1 times
	{
		LEDToggle(20);		// 200ms blink
	}

	if(0 != stSysFaultReg.unFaultBit.all) 	 // fault return true
	{
		stDebug.ReadData.uwInvStatus = 1;
		stDebug.ReadData.uwInvError = 1000+ GetUint64Bit(stSysFaultReg.unFaultBit.all);
		stWaitStatus.uwExitInvStatusCnt++;
		OSEventSend(STATUS_PRIO, STATUS_TO_FAULT);	//Event trigger to fault status
	}
	else if((TRUE ==unSysFlag.bit.PvVoltLow) || (TRUE ==unSysFlag.bit.PvVoltHigh))
	{
		stDebug.ReadData.uwInvStatus = 3;
		stDebug.ReadData.uwInvError += 100;
		stWaitStatus.uwExitInvStatusCnt++;
		OSEventSend(STATUS_PRIO, STATUS_TO_WAIT);
	}
#if  F107_BOARD	
#else
	else if(OFF == stDebug.SetData.unSetReg.bit.TurnOnOff)
	{
		stDebug.ReadData.uwInvStatus = 2;
		stDebug.ReadData.uwInvError += 10;
		OSEventSend(STATUS_PRIO, STATUS_TO_WAIT);
	}
#endif

}

/****************************************************************************
*	FunctionName:	FaultStatus
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
static void FaultStatus(void)
{
	static Uint16 suwTzFaultClrCnt=0;
	static Uint16 suwRelayFaultClrCnt=0;

	if(event&(1<<STATUS_TASK_PERIOD))			// every 10ms run 1 times
	{
		LEDToggle(50);		// 3s blink
		if((1==stSysFaultReg.unOverCurrFault.bit.PowerHardWare)
		|| (1==stSysFaultReg.unOverCurrFault.bit.BusOverHardWare))
		{
			suwTzFaultClrCnt++;
			if(suwTzFaultClrCnt>=CLR_FAULT_TIME)
			{
				suwTzFaultClrCnt = 0;
				OS_ENTER_CRITICAL();
				stSysFaultReg.unOverCurrFault.bit.PowerHardWare = 0;
				stSysFaultReg.unOverCurrFault.bit.BusOverHardWare = 0;
				OS_EXIT_CRITICAL();
			}
		}
		else
		{
			suwTzFaultClrCnt = 0;
		}

        if(stSysFaultReg.unFaultBit.bit.RelayCheck == 1)
        {
            suwRelayFaultClrCnt++;
            if(suwRelayFaultClrCnt >= 6000)
            {
                suwRelayFaultClrCnt = 0;
                stSysFaultReg.unFaultBit.bit.RelayCheck = 0;
            }
        }
	}

	if(0 != stF107Data.unSet.bit.FlashReady)
	{
		stDebug.ReadData.uwFaultStatus += 10;
		OSEventSend(STATUS_PRIO, STATUS_TO_UPDATE);	//Event trigger to flash status
	}
	else if(0 == stSysFaultReg.unFaultBit.all) // fault clear
	{
		stDebug.ReadData.uwFaultStatus += 100;
		OSEventSend(STATUS_PRIO, STATUS_TO_WAIT);
	}

}

/****************************************************************************
*	FunctionName:	FlashStatus
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
static void FlashStatus(void)
{
	static Uint16 suwEnterBootDelayTime = 0;

	if(event&(1<<STATUS_TASK_PERIOD))			// every 10ms run 1 times
	{
		suwEnterBootDelayTime++;

		if(suwEnterBootDelayTime>50)				// 50*10ms = 5s
		{
			suwEnterBootDelayTime = 0;

			OS_ENTER_CRITICAL();
			if(stF107Data.unSet.bit.FlashReady == 0x01)
			{
				BootLoader();
			}
			OS_EXIT_CRITICAL();
		}
	}
}

static void BusSoftStart(void)
{
	if(ON == stDebug.SetData.unSetReg.bit.BoostPwmLoop)
	{
		stWaitStatus.stBusStart.uwStartCnt = 205;
	}
	stWaitStatus.stBusStart.uwStartCnt++;
	if(stWaitStatus.stBusStart.uwStartCnt > 2000)	// 2000 * 10ms = 20s
	{
		stWaitStatus.stBusStart.uwStartCnt = 0;
		stWaitStatus.stBusStart.Flag.Fail=1;			// Bus Soft Start Failed !
	}
	else if(stWaitStatus.stBusStart.uwStartCnt > 200)	// 200 * 10ms = 2s
	{
		if((stMpptTskCtrl.uwBusRef >= stMpptTskCtrl.wBusStandardVolt)
	     &&(stDCSample.wBusVolt >= (stMpptTskCtrl.uwBusRef-200)))
		{
			stWaitStatus.stBusStart.Flag.Success=1;		// Bus Soft Start Successed !
		}
	}
}

static void FanInvtPwmCheck(void)
{
	static Uint16 suwRelayVoltNormalCnt1=0;
	stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt++;

	/***********STEP1:DSP&F042 Relay Break,Disconnect the relay and check for sticking *********/
	if(50 == stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt) 	//  50* 10ms = 0.5s	// wait Bus Voltage Stable
	{
		OS_ENTER_CRITICAL();
		DSP_RELAY_LOW;									// DSP  Relay	Break
 		stWaitStatus.stRelayCheck.uwF042OnOff = 0;			// F042 Relay	Break
		OS_EXIT_CRITICAL();
	}
	else if((stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt>= 70)&&(stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt <100))	
	{

		/***********Check Relay Voltage*********/
		if((stACSample.RlyVoltRms.wRN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wSN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wTN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax))
		{
			suwRelayVoltNormalCnt1++;
			if(suwRelayVoltNormalCnt1>=5)
			{
				suwRelayVoltNormalCnt1 = 0;
				stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt = 99;
			}
		}
		else
		{
			suwRelayVoltNormalCnt1 = 0;
			stWaitStatus.stFanInvtOpenCheck.Flag.Fail = 1;
		}
	}
	/***********STEP2:Replace 10s Fan Check Reduce Fan Noise *********/
	else if(100 == stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt)
	{
		OS_ENTER_CRITICAL();
		stWaitStatus.stRelayCheck.Flag.InvPwmOn = 1;
		OS_EXIT_CRITICAL();
	}
	/***********STEP3:Invt Open Pwm Chcek the different of Invt Volt and  Grid Volt*********/
	else if((stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt>=1095)&&(stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt<=1100))
	{
		//stDebug.ReadData.wDebug1 = stACSample.RlyVoltRms.wRN;
		//stDebug.ReadData.wDebug2 = stACSample.RlyVoltRms.wSN;
		//stDebug.ReadData.wDebug3 = stWaitStatus.stRelayCheck.wPhaseVoltRmsMax;
		//stDebug.ReadData.wDebug8 = stWaitStatus.stRelayCheck.wPhaseVoltRmsMax;
		if((stACSample.RlyVoltRms.wRN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
			&&(stACSample.RlyVoltRms.wSN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
			&&(stACSample.RlyVoltRms.wTN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax))
		{
			stWaitStatus.stFanInvtOpenCheck.Flag.Success = 1;
			OS_ENTER_CRITICAL();
			stWaitStatus.stRelayCheck.Flag.InvPwmOn = 0;
			OS_EXIT_CRITICAL();
		}
		else
		{
			stWaitStatus.stFanInvtOpenCheck.Flag.Fail = 1;
			OS_ENTER_CRITICAL();
			stWaitStatus.stRelayCheck.Flag.InvPwmOn = 0;
			OS_EXIT_CRITICAL();
		}
	}
}

static void LEDToggle(Uint16 uwLEDBlinkTime)
{
	static Uint16 suwLEDCnt = 0;
	suwLEDCnt++;
	if(suwLEDCnt >= uwLEDBlinkTime)
	{
		suwLEDCnt = 0;
		TOGGLE_PIN35;
		TOGGLE_PIN99;
	}
}
