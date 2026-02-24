/*
 * safty_fan_ctrl.c
 *
 *  Created on: 2021年3月11日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

static void FanStartLogicCtrl(void);
static void FanFaultCehck(void);

ST_FAN	stFan;

void FanApplication(void)
{
	FanStartLogicCtrl();
	FanFaultCehck();
}

/****20ms Task Period************/
void FanOnOffCtrl(void)
{
	static Uint16	suwExternalFanSpeed=6249;

	if(ON==stFan.unFlagBits[FAN4_INTR].bit.OnOff)
	{
		FAN4_INTR_ON;
	}
	else
	{
		FAN4_INTR_OFF;
	}
	// 25-40k,Fan1&Fan2 always stay on. Fan speed controlled by FAN_EXTR_PWM.
	FAN1_EXTR_ON;
	FAN2_EXTR_ON;


	if(0 == stFan.unFlagBits[FAN1_EXTR].bit.FanTest)
	{
		if(FAN_SPEED_100==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>50)
			{
				suwExternalFanSpeed -= 25;		// 6250/25 = 250*20ms = 5s
			}
			else
			{
				suwExternalFanSpeed = 1;
			}
		}
		else if(FAN_SPEED_90==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>675)
			{
				suwExternalFanSpeed -= 25;		// 5625/25 = 225*20ms = 4.5s
			}
			else
			{
				suwExternalFanSpeed = 625;
			}
		}
		else if(FAN_SPEED_80==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>1300)
			{
				suwExternalFanSpeed -= 25;		// 5000/25 = 200*20ms = 4s
			}
			else
			{
				suwExternalFanSpeed = 1250;
			}
		}
		else if(FAN_SPEED_70==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>1925)
			{
				suwExternalFanSpeed -= 25;		// 4375/25 = 175*20ms = 3.5s
			}
			else
			{
				suwExternalFanSpeed = 1875;
			}
		}
		else if(FAN_SPEED_60==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>2550)
			{
				suwExternalFanSpeed -= 25;		// 3750/25 = 150*20ms = 3s
			}
			else
			{
				suwExternalFanSpeed = 2500;
			}
		}
		else if(FAN_SPEED_50==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>3175)
			{
				suwExternalFanSpeed -= 25;		// 3125/25 = 125*20ms = 2.5s
			}
			else
			{
				suwExternalFanSpeed = 3125;
			}
		}
		else if(FAN_SPEED_40==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>3800)
			{
				suwExternalFanSpeed -= 25;		// 2500/25 = 100*20ms = 2s
			}
			else
			{
				suwExternalFanSpeed = 3750;
			}
		}
		else if(FAN_SPEED_30==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>4425)
			{
				suwExternalFanSpeed -= 25;		// 1875/25 = 75*20ms = 1.5s
			}
			else
			{
				suwExternalFanSpeed = 4375;
			}
		}
		else if(FAN_SPEED_20==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>5050)
			{
				suwExternalFanSpeed -= 25;		// 1250/25 = 50*20ms = 1s
			}
			else
			{
				suwExternalFanSpeed = 5000;
			}
		}
		else if(FAN_SPEED_10==stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		{
			if(suwExternalFanSpeed>5675)
			{
				suwExternalFanSpeed -= 25;		// 625/25 = 25*20ms = 0.5s
			}
			else
			{
				suwExternalFanSpeed = 5625;
			}
		}
		else
		{
			suwExternalFanSpeed = 6249;			// Fan Close
		}
		FAN_EXTR_PWM(suwExternalFanSpeed);	
		
	}
	else
	{
		suwExternalFanSpeed = 6250 -((long) (639897 * stFan.unFlagBits[FAN1_EXTR].bit.FanTest) >>10);	
		FAN_EXTR_PWM(suwExternalFanSpeed);	
		
	}
	//stDebug.ReadData.wDebug2=suwExternalFanSpeed;

}

/****500ms Task Period************/
static void FanStartLogicCtrl(void)
{
	int32	dActivePower25Pct,dActivePower50Pct,dActivePower100Pct,dActivePower6Pct,dActivePower75Pct,dActivePower20Pct;
	static Uint16	suwFanStartDelay=10;
	static Uint16	suwExternalFanOnOff=0,suwInternalFanOnOff=0;
	static Uint16	suwFanPowerOnCheckCnt=0;

	static Uint16	suwExtrFanFullOn=0,suwExtrFanHalfOn=0;
	static Uint16	suwExtrFanSpeed=0;
	
	dActivePower100Pct = stDspReceData.uwSettingPower*1000;		// 0.1w
	dActivePower75Pct = ((stLoadLimit.dActivePower*3)>>2);		// 3/4 = 0.75
	dActivePower50Pct = (stLoadLimit.dActivePower>>1);		// 1/2 = 0.5
	dActivePower25Pct = (stLoadLimit.dActivePower>>2);		// 1/4 = 0.25
	dActivePower20Pct = (stLoadLimit.dActivePower*0.2);		// 1/4 = 0.25
	dActivePower6Pct = (stLoadLimit.dActivePower>>4);		// 1/16 = 0.0625

	if(cInitialStatus == eInverterStatus)
	{
		suwExternalFanOnOff = OFF;
		suwInternalFanOnOff = OFF;
		suwFanPowerOnCheckCnt = 0;
		suwExtrFanFullOn = 0;
		suwExtrFanHalfOn = 0;
		suwExtrFanSpeed = FAN_SPEED_0;
	}
	/*else if(suwFanPowerOnCheckCnt<20)		// 20*500ms = 10s
	{
		suwFanPowerOnCheckCnt++;
		suwExternalFanOnOff = FAN_SPEED_100;
		suwInternalFanOnOff = ON;
	}*/
	else if(1==stF107Data.unSet.bit.FanSelfCheckCmd)
	{
		suwExternalFanOnOff = FAN_SPEED_100;
		suwInternalFanOnOff = ON;
		suwExtrFanSpeed = FAN_SPEED_100;
	}
	else
	{
		/************Internal Fan Open or Close Logic*******/
		if((MACHINE_ID_Plus_50KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)
			||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
			||(MACHINE_ID_PRO_33_40KW == stSysCfg.uwMachineType))
		{
			if((stDCSample.wAmTemp > 750)||(stACSample.dActivePowerAvg > dActivePower25Pct))
			{
				suwInternalFanOnOff = ON;
			}
			else if((stDCSample.wAmTemp <= 500)&&(stACSample.dActivePowerAvg <= dActivePower25Pct))
			{
				suwInternalFanOnOff = OFF;
			}
		}
		else
		{
			if((cInverterStatus == eInverterStatus)
				||((cWaitStatus == eInverterStatus)&&(stWaitStatus.eStandbyStep >= cBusSoftStart))
				||(stDCSample.wAmTemp > 700))
			{
				suwInternalFanOnOff = ON;
			}
			else
			{
				suwInternalFanOnOff = OFF;
			}
		}

		if((cInverterStatus == eInverterStatus)
			&& ((MACHINE_ID_25_33KW == stSysCfg.uwMachineType)||(MACHINE_ID_36_40KW == stSysCfg.uwMachineType)
			||(MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)||(MACHINE_ID_PRO_33_40KW == stSysCfg.uwMachineType)))
		{
			if((stACSample.dApparentPowerAvg > ((stLoadLimit.dActivePower * 1126) >>10))||(stDCSample.wAmTemp > 790)
				||(stDCSample.wBoostTemp > 840)||(stDCSample.wInvTemp > 950))
			{
				suwExtrFanSpeed = FAN_SPEED_100;
			}		
			else if((stDCSample.wAmTemp > 785)||(stDCSample.wBoostTemp > 837)||(stDCSample.wInvTemp > 942))  
			{
				suwExtrFanSpeed = FAN_SPEED_90;
			}
			else if((stDCSample.wAmTemp > 780)||(stDCSample.wBoostTemp > 834)||(stDCSample.wInvTemp > 934)) 
			{
				suwExtrFanSpeed = FAN_SPEED_80;
			}
			else if((stDCSample.wAmTemp > 775)||(stDCSample.wBoostTemp > 831)||(stDCSample.wInvTemp > 926))
			{
				suwExtrFanSpeed = FAN_SPEED_70;
			}
			else if((stDCSample.wAmTemp > 770)||(stDCSample.wBoostTemp > 828)||(stDCSample.wInvTemp > 918))
			{
				suwExtrFanSpeed = FAN_SPEED_60;
			}
			else if((stDCSample.wAmTemp > 765)||(stDCSample.wBoostTemp > 825)||(stDCSample.wInvTemp > 910))
			{
				suwExtrFanSpeed = FAN_SPEED_50;
			}
			else if((stDCSample.wAmTemp > 760)||(stDCSample.wBoostTemp > 822)||(stDCSample.wInvTemp > 902))
			{
				suwExtrFanSpeed = FAN_SPEED_40;
			}
			else if((stACSample.dActivePowerAvg > dActivePower25Pct)||(stDCSample.wAmTemp > 755)||(stDCSample.wBoostTemp > 819)||(stDCSample.wInvTemp > 894))
			{
				suwExtrFanSpeed = FAN_SPEED_40;
			}
			else if((stACSample.dActivePowerAvg <= dActivePower25Pct)&&(stDCSample.wAmTemp <= 705)&&(stDCSample.wBoostTemp <= 769)&&(stDCSample.wInvTemp <= 844))
			{
				suwExtrFanSpeed = FAN_SPEED_0;
				suwExternalFanOnOff = OFF;
			}
		}
		else if((cInverterStatus == eInverterStatus)&& (MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType))
		{
			if((stACSample.dApparentPowerAvg > ((stLoadLimit.dActivePower * 1126) >>10))||(stDCSample.wAmTemp > 775)
				||(stDCSample.wBoostTemp > 875)||(stDCSample.wInvTemp > 865))
			{
				suwExtrFanSpeed = FAN_SPEED_100;
			}		
			else if((stDCSample.wAmTemp > 770)||(stDCSample.wBoostTemp > 868)||(stDCSample.wInvTemp > 858))  
			{
				suwExtrFanSpeed = FAN_SPEED_90;
			}
			else if((stDCSample.wAmTemp > 765)||(stDCSample.wBoostTemp > 861)||(stDCSample.wInvTemp > 851)) 
			{
				suwExtrFanSpeed = FAN_SPEED_80;
			}
			else if((stDCSample.wAmTemp > 760)||(stDCSample.wBoostTemp > 854)||(stDCSample.wInvTemp > 844))
			{
				suwExtrFanSpeed = FAN_SPEED_70;
			}
			else if((stDCSample.wAmTemp > 755)||(stDCSample.wBoostTemp > 847)||(stDCSample.wInvTemp > 837))
			{
				suwExtrFanSpeed = FAN_SPEED_60;
			}
			else if((stDCSample.wAmTemp > 750)||(stDCSample.wBoostTemp > 840)||(stDCSample.wInvTemp > 830))
			{
				suwExtrFanSpeed = FAN_SPEED_50;
			}
			else if((stDCSample.wAmTemp > 745)||(stDCSample.wBoostTemp > 830)||(stDCSample.wInvTemp > 820))
			{
				suwExtrFanSpeed = FAN_SPEED_40;
			}
			else if((stDCSample.wAmTemp > 744)||(stDCSample.wBoostTemp > 828)||(stDCSample.wInvTemp > 818))
			{
				suwExtrFanSpeed = FAN_SPEED_40;
			}
			else if((stACSample.dActivePowerAvg <= dActivePower25Pct)&&(stDCSample.wAmTemp <= 700)&&(stDCSample.wBoostTemp <= 778)&&(stDCSample.wInvTemp <= 768))
			{
				suwExtrFanSpeed = FAN_SPEED_0;
				suwExternalFanOnOff = OFF;
			}
		}
		else if((cInverterStatus == eInverterStatus)&& (MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)
			||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2))
		{
			if((stACSample.dApparentPowerAvg > ((stLoadLimit.dActivePower * 1126) >>10))||(stDCSample.wAmTemp > 790)
				||(stDCSample.wBoostTemp > 840)||(stDCSample.wInvTemp > 880))
			{
				suwExtrFanSpeed = FAN_SPEED_100;
			}
			else if((stDCSample.wAmTemp > 785)||(stDCSample.wBoostTemp > 837)||(stDCSample.wInvTemp > 872))
			{
				suwExtrFanSpeed = FAN_SPEED_90;
			}
			else if((stDCSample.wAmTemp > 780)||(stDCSample.wBoostTemp > 834)||(stDCSample.wInvTemp > 864))
			{
				suwExtrFanSpeed = FAN_SPEED_80;
			}
			else if((stDCSample.wAmTemp > 775)||(stDCSample.wBoostTemp > 831)||(stDCSample.wInvTemp > 856))
			{
				suwExtrFanSpeed = FAN_SPEED_70;
			}
			else if((stDCSample.wAmTemp > 770)||(stDCSample.wBoostTemp > 828)||(stDCSample.wInvTemp > 848))
			{
				suwExtrFanSpeed = FAN_SPEED_60;
			}
			else if((stACSample.dActivePowerAvg > dActivePower75Pct)||(stDCSample.wAmTemp > 765)||(stDCSample.wBoostTemp > 790)||(stDCSample.wInvTemp > 810))
			{
				suwExtrFanSpeed = FAN_SPEED_50;
			}
			else if((stACSample.dActivePowerAvg > dActivePower50Pct)||(stDCSample.wAmTemp > 760)||(stDCSample.wBoostTemp > 750)||(stDCSample.wInvTemp > 780))
			{
				suwExtrFanSpeed = FAN_SPEED_40;
			}
			else if((stACSample.dActivePowerAvg > dActivePower25Pct)||(stDCSample.wAmTemp > 750)||(stDCSample.wBoostTemp > 650)||(stDCSample.wInvTemp > 680))
			{
				suwExtrFanSpeed = FAN_SPEED_40;
			}
			else if((stACSample.dActivePowerAvg <= dActivePower25Pct)&&(stDCSample.wAmTemp <= 550)&&(stDCSample.wBoostTemp <= 600)&&(stDCSample.wInvTemp <= 630))
			{
				suwExtrFanSpeed = FAN_SPEED_0;
				suwExternalFanOnOff = OFF;
			}
		}
		//else
		//{
			//suwExtrFanSpeed = OFF;
		//}
		else				// XG50-70KW
		{
			/************External Fan Open or Close Logic*******/
			if((stACSample.dActivePowerAvg > dActivePower50Pct) || (stDCSample.wInvTemp > 900)  || (stDCSample.wBoostTemp > 900))
			{
				suwExternalFanOnOff = FAN_SPEED_100;
			}
			else if(stACSample.dActivePowerAvg > dActivePower25Pct)
			{
				suwExternalFanOnOff = FAN_SPEED_50;
			}
			else if((stACSample.dActivePowerAvg+dActivePower6Pct) < dActivePower25Pct)
			{
				suwExternalFanOnOff = OFF;
				suwExtrFanSpeed = FAN_SPEED_0;
			}
		}

	}

	/**********Fan On Delay 5s*****/
	if(suwFanStartDelay > 0)
	{
		suwFanStartDelay--;
	}
	else
	{
		 if(suwExtrFanSpeed !=stFan.unFlagBits[FAN1_EXTR].bit.OnOff)
		 {
		 	suwFanStartDelay = 10;		// 10*500ms = 5s
			stFan.unFlagBits[FAN1_EXTR].bit.OnOff = suwExtrFanSpeed;
			stFan.unFlagBits[FAN2_EXTR].bit.OnOff = suwExtrFanSpeed;
			stFan.unFlagBits[FAN3_EXTR].bit.OnOff = suwExtrFanSpeed;
		 }


		 if((OFF==stFan.unFlagBits[FAN4_INTR].bit.OnOff)&&(ON==suwInternalFanOnOff))
		{
			stFan.unFlagBits[FAN4_INTR].bit.OnOff = ON;
		}
			 
	}
	if(OFF==suwInternalFanOnOff)
	{
		stFan.unFlagBits[FAN4_INTR].bit.OnOff = OFF;
	}
	/***XG 15-25kW and XG 25-40KW have FAN1,FAN2,FAN4**/
	if((MACHINE_ID_15_22KW == stSysCfg.uwMachineType)
	    ||(MACHINE_ID_25KW == stSysCfg.uwMachineType)
		||(MACHINE_ID_PARALLEL_TUBE_15_22KW == stSysCfg.uwMachineType)
		||(MACHINE_ID_PARALLEL_TUBE_25KW == stSysCfg.uwMachineType)
	    ||(MACHINE_ID_25_33KW == stSysCfg.uwMachineType)
	    ||(MACHINE_ID_36_40KW == stSysCfg.uwMachineType)
	     ||(MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)
	     ||(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)
	      ||(MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType)
	      ||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType)
	      || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType)
	      || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
	      || (MACHINE_ID_PRO_33_40KW == stSysCfg.uwMachineType))
	{
	    stFan.unFlagBits[FAN3_EXTR].bit.OnOff = OFF;
	}

}

static void FanFaultCehck(void)
{
	Uint16	i;
	static Uint16	suwFanCheckTem=0;
	static Uint16	suwFanFaultSetCnt[FAN_NUM_MAX]={0},suwFanFaultClrCnt[FAN_NUM_MAX]={0};

	for(i=0;	i<FAN_NUM_MAX;	i++)
	{
		if(cInverterStatus == eInverterStatus)
		{
			suwFanCheckTem++;
			if(suwFanCheckTem >= 120)
			{
				if(stFan.unFlagBits[i].bit.OnOff != OFF)
				{
					if(TRUE == stFan.unFlagBits[i].bit.FreqFault)
					{
						suwFanFaultClrCnt[i] = 0;
						suwFanFaultSetCnt[i]++;
						if(suwFanFaultSetCnt[i] >= FAN_FAULT_SET_TIME)
						{
							suwFanFaultSetCnt[i] = 0;
							stSysFaultReg.unFanWarn.all |= (1<<i);
						}
					}
					else
					{
						suwFanFaultSetCnt[i] = 0;
						suwFanFaultClrCnt[i]++;
						if(suwFanFaultClrCnt[i] >= FAN_FAULT_CLR_TIME)
						{
							suwFanFaultClrCnt[i] = 0;
							stSysFaultReg.unFanWarn.all &= ~(1<<i);
						}
					}
				}
				else
				{
					suwFanFaultClrCnt[i] = 0;
					suwFanFaultSetCnt[i] = 0;
					stSysFaultReg.unFanWarn.all &= ~(1<<i);
				}
			}
		}
		else
		{
			suwFanCheckTem = 0;
		}
	}
	if(stSysFaultReg.unFanWarn.all == 0)
	{
		stSysFaultReg.unWarnBit.bit.FanFault = 0;
	}
	else
	{
		stSysFaultReg.unWarnBit.bit.FanFault = 1;
	}
}


//#pragma CODE_SECTION(FanSpeedDetect,"ramfuncs");
void FanSpeedDetect(void)
{
	static Uint16	suwFanPulseOnCnt[FAN_NUM_MAX]={0},suwFanPulseOffCnt[FAN_NUM_MAX]={0};
	static Uint16	suwFanPulseHighCnt[FAN_NUM_MAX]={0},suwFanPulseLowCnt[FAN_NUM_MAX]={0};
	static Uint16	suwFanPulseHighFlag[FAN_NUM_MAX]={0},suwFanPulseLowFlag[FAN_NUM_MAX]={0};

	static int32 sdFanPeriodSum[FAN_NUM_MAX]={0};
	static Uint16 suwFanPeriodTemp[FAN_NUM_MAX]={0};
	static Uint16 suwFanPeriodCalcCnt=0;

	/**********Internal FAN4 ***/  // 额定频率为133Hz/7.50ms
	if(ON==FAN4_INTR_LEVEL)			// PWM si high level
	{
		if(suwFanPulseLowFlag[FAN4_INTR]==1)		// 电平由低变高
		{
			suwFanPulseLowFlag[FAN4_INTR] = 0;
			suwFanPeriodTemp[FAN4_INTR] = suwFanPulseLowCnt[FAN4_INTR]+suwFanPulseHighCnt[FAN4_INTR];
			suwFanPulseHighCnt[FAN4_INTR] = 0;
		}

		suwFanPulseHighCnt[FAN4_INTR]++;
		if(suwFanPulseHighCnt[FAN4_INTR] >= 3000)		  //3000 * 1ms=3s// 30000*104us = 3s
		{
			suwFanPulseHighCnt[FAN4_INTR] = 3000;
			suwFanPeriodTemp[FAN4_INTR] = 3000;
		}
		suwFanPulseHighFlag[FAN4_INTR] = 1;

		suwFanPulseOnCnt[FAN4_INTR]++;
		if(suwFanPulseOnCnt[FAN4_INTR] >= stMachineCfg.wIntrFan4SetFault)
		{
			suwFanPulseOnCnt[FAN4_INTR] = stMachineCfg.wIntrFan4SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN4_INTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOnCnt[FAN4_INTR] >= stMachineCfg.wIntrFan4SetFault)
			{
				stFan.unFlagBits[FAN4_INTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOffCnt[FAN4_INTR] >= 2)
			 &&(suwFanPulseOffCnt[FAN4_INTR] <= stMachineCfg.wIntrFan4ClrFault))
			{
				stFan.unFlagBits[FAN4_INTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOffCnt[FAN4_INTR]=0;
	}
	else
	{
		if(suwFanPulseHighFlag[FAN4_INTR]==1)		// 电平由低变高
		{
			suwFanPulseHighFlag[FAN4_INTR] = 0;
			suwFanPeriodTemp[FAN4_INTR] = suwFanPulseLowCnt[FAN4_INTR]+suwFanPulseHighCnt[FAN4_INTR];
			suwFanPulseLowCnt[FAN4_INTR] = 0;
		}

		suwFanPulseLowCnt[FAN4_INTR]++;
		if(suwFanPulseLowCnt[FAN4_INTR] >= 3000)		 //3000 * 1ms=3s// 30000*104us = 3s
		{
			suwFanPulseLowCnt[FAN4_INTR] = 3000;
			suwFanPeriodTemp[FAN4_INTR] = 3000;
		}
		suwFanPulseLowFlag[FAN4_INTR] = 1;


		suwFanPulseOffCnt[FAN4_INTR]++;
		if(suwFanPulseOffCnt[FAN4_INTR] >= stMachineCfg.wIntrFan4SetFault)
		{
			suwFanPulseOffCnt[FAN4_INTR] = stMachineCfg.wIntrFan4SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN4_INTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOffCnt[FAN4_INTR] >= stMachineCfg.wIntrFan4SetFault)
			{
				stFan.unFlagBits[FAN4_INTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOnCnt[FAN4_INTR] >= 2)
			 &&(suwFanPulseOnCnt[FAN4_INTR] <= stMachineCfg.wIntrFan4ClrFault))
			{
				stFan.unFlagBits[FAN4_INTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOnCnt[FAN4_INTR]=0;
	}

	/**********External FAN1 ***/
	if(ON==FAN1_EXTR_LEVEL)			// PWM si high level
	{
		if(suwFanPulseLowFlag[FAN1_EXTR]==1)		// 电平由低变高
		{
			suwFanPulseLowFlag[FAN1_EXTR] = 0;
			suwFanPeriodTemp[FAN1_EXTR] = suwFanPulseLowCnt[FAN1_EXTR]+suwFanPulseHighCnt[FAN1_EXTR];
			suwFanPulseHighCnt[FAN1_EXTR] = 0;
		}

		suwFanPulseHighCnt[FAN1_EXTR]++;
		if(suwFanPulseHighCnt[FAN1_EXTR] >= 3000)		 // 30000*104us = 3s
		{
			suwFanPulseHighCnt[FAN1_EXTR] = 3000;
			suwFanPeriodTemp[FAN1_EXTR] = 3000;
		}
		suwFanPulseHighFlag[FAN1_EXTR] = 1;


		suwFanPulseOnCnt[FAN1_EXTR]++;
		if(suwFanPulseOnCnt[FAN1_EXTR] >= stMachineCfg.wExtrFan123SetFault)
		{
			suwFanPulseOnCnt[FAN1_EXTR] = stMachineCfg.wExtrFan123SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN1_EXTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOnCnt[FAN1_EXTR] >= stMachineCfg.wExtrFan123SetFault)
			{
				stFan.unFlagBits[FAN1_EXTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOffCnt[FAN1_EXTR] >= 2)
			 &&(suwFanPulseOffCnt[FAN1_EXTR] <= stMachineCfg.wExtrFan123ClrFault))
			{
				stFan.unFlagBits[FAN1_EXTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOffCnt[FAN1_EXTR]=0;
	}
	else
	{
		if(suwFanPulseHighFlag[FAN1_EXTR]==1)		// 电平由低变高
		{
			suwFanPulseHighFlag[FAN1_EXTR] = 0;
			suwFanPeriodTemp[FAN1_EXTR] = suwFanPulseLowCnt[FAN1_EXTR]+suwFanPulseHighCnt[FAN1_EXTR];
			suwFanPulseLowCnt[FAN1_EXTR] = 0;
		}

		suwFanPulseLowCnt[FAN1_EXTR]++;
		if(suwFanPulseLowCnt[FAN1_EXTR] >= 3000)		 // 30000*104us = 3s
		{
			suwFanPulseLowCnt[FAN1_EXTR] = 3000;
			suwFanPeriodTemp[FAN1_EXTR] = 3000;
		}
		suwFanPulseLowFlag[FAN1_EXTR] = 1;

		suwFanPulseOffCnt[FAN1_EXTR]++;
		if(suwFanPulseOffCnt[FAN1_EXTR] >= stMachineCfg.wExtrFan123SetFault)
		{
			suwFanPulseOffCnt[FAN1_EXTR] = stMachineCfg.wExtrFan123SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN1_EXTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOffCnt[FAN1_EXTR] >= stMachineCfg.wExtrFan123SetFault)
			{
				stFan.unFlagBits[FAN1_EXTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOnCnt[FAN1_EXTR] >= 2)
			 &&(suwFanPulseOnCnt[FAN1_EXTR] <= stMachineCfg.wExtrFan123ClrFault))
			{
				stFan.unFlagBits[FAN1_EXTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOnCnt[FAN1_EXTR]=0;
	}
	//stDebug.ReadData.wDebug4[1]=suwFanPulseOnCnt[FAN1_EXTR];
	//stDebug.ReadData.wDebug4[2]=suwFanPulseOffCnt[FAN1_EXTR] ;
	//stDebug.ReadData.wDebug1=suwFanPulseOnCnt[FAN1_EXTR];
	//stDebug.ReadData.wDebug2=suwFanPulseOffCnt[FAN1_EXTR];
	/**********External FAN2 ***/
	if(ON==FAN2_EXTR_LEVEL)			// PWM si high level
	{

		if(suwFanPulseLowFlag[FAN2_EXTR]==1)		// 电平由低变高
		{
			suwFanPulseLowFlag[FAN2_EXTR] = 0;
			suwFanPeriodTemp[FAN2_EXTR] = suwFanPulseLowCnt[FAN2_EXTR]+suwFanPulseHighCnt[FAN2_EXTR];
			suwFanPulseHighCnt[FAN2_EXTR] = 0;
		}

		suwFanPulseHighCnt[FAN2_EXTR]++;
		if(suwFanPulseHighCnt[FAN2_EXTR] >= 3000)		 //3000 * 1ms=3s// 30000*104us = 3s
		{
			suwFanPulseHighCnt[FAN2_EXTR] = 3000;
			suwFanPeriodTemp[FAN2_EXTR] = 3000;
		}
		suwFanPulseHighFlag[FAN2_EXTR] = 1;

		suwFanPulseOnCnt[FAN2_EXTR]++;
		if(suwFanPulseOnCnt[FAN2_EXTR] >= stMachineCfg.wExtrFan123SetFault)
		{
			suwFanPulseOnCnt[FAN2_EXTR] = stMachineCfg.wExtrFan123SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN2_EXTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOnCnt[FAN2_EXTR] >= stMachineCfg.wExtrFan123SetFault)
			{
				stFan.unFlagBits[FAN2_EXTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOffCnt[FAN2_EXTR] >= 2)
			 &&(suwFanPulseOffCnt[FAN2_EXTR] <= stMachineCfg.wExtrFan123ClrFault))
			{
				stFan.unFlagBits[FAN2_EXTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOffCnt[FAN2_EXTR]=0;
	}
	else
	{
		if(suwFanPulseHighFlag[FAN2_EXTR]==1)		// 电平由低变高
		{
			suwFanPulseHighFlag[FAN2_EXTR] = 0;
			suwFanPeriodTemp[FAN2_EXTR] = suwFanPulseLowCnt[FAN2_EXTR]+suwFanPulseHighCnt[FAN2_EXTR];
			suwFanPulseLowCnt[FAN2_EXTR] = 0;
		}

		suwFanPulseLowCnt[FAN2_EXTR]++;
		if(suwFanPulseLowCnt[FAN2_EXTR] >= 3000)		 //3000 * 1ms=3s// 30000*104us = 3s
		{
			suwFanPulseLowCnt[FAN2_EXTR] = 3000;
			suwFanPeriodTemp[FAN2_EXTR] = 3000;
		}
		suwFanPulseLowFlag[FAN2_EXTR] = 1;

		suwFanPulseOffCnt[FAN2_EXTR]++;
		if(suwFanPulseOffCnt[FAN2_EXTR] >= stMachineCfg.wExtrFan123SetFault)
		{
			suwFanPulseOffCnt[FAN2_EXTR] = stMachineCfg.wExtrFan123SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN2_EXTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOffCnt[FAN2_EXTR] >= stMachineCfg.wExtrFan123SetFault)
			{
				stFan.unFlagBits[FAN2_EXTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOnCnt[FAN2_EXTR] >= 2)
			 &&(suwFanPulseOnCnt[FAN2_EXTR] <= stMachineCfg.wExtrFan123ClrFault))
			{
				stFan.unFlagBits[FAN2_EXTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOnCnt[FAN2_EXTR]=0;
	}

	/**********External FAN3 ***/
	if(ON==FAN3_EXTR_LEVEL)			// PWM si high level
	{
		if(suwFanPulseLowFlag[FAN3_EXTR]==1)		// 电平由低变高
		{
			suwFanPulseLowFlag[FAN3_EXTR] = 0;
			suwFanPeriodTemp[FAN3_EXTR] = suwFanPulseLowCnt[FAN3_EXTR]+suwFanPulseHighCnt[FAN3_EXTR];
			suwFanPulseHighCnt[FAN3_EXTR] = 0;
		}

		suwFanPulseHighCnt[FAN3_EXTR]++;
		if(suwFanPulseHighCnt[FAN3_EXTR] >= 3000)		 //3000 * 1ms=3s// 30000*104us = 3s
		{
			suwFanPulseHighCnt[FAN3_EXTR] = 3000;
			suwFanPeriodTemp[FAN3_EXTR] = 3000;
		}
		suwFanPulseHighFlag[FAN3_EXTR] = 1;


		suwFanPulseOnCnt[FAN3_EXTR]++;
		if(suwFanPulseOnCnt[FAN3_EXTR] >= stMachineCfg.wExtrFan123SetFault)
		{
			suwFanPulseOnCnt[FAN3_EXTR] = stMachineCfg.wExtrFan123SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN3_EXTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOnCnt[FAN3_EXTR] >= stMachineCfg.wExtrFan123SetFault)
			{
				stFan.unFlagBits[FAN3_EXTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOffCnt[FAN3_EXTR] >= 2)
			 &&(suwFanPulseOffCnt[FAN3_EXTR] <= stMachineCfg.wExtrFan123ClrFault))
			{
				stFan.unFlagBits[FAN3_EXTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOffCnt[FAN3_EXTR]=0;
	}
	else
	{
		
		if(suwFanPulseHighFlag[FAN3_EXTR]==1)		// 电平由低变高
		{
			suwFanPulseHighFlag[FAN3_EXTR] = 0;
			suwFanPeriodTemp[FAN3_EXTR] = suwFanPulseLowCnt[FAN3_EXTR]+suwFanPulseHighCnt[FAN3_EXTR];
			suwFanPulseLowCnt[FAN3_EXTR] = 0;
		}

		suwFanPulseLowCnt[FAN3_EXTR]++;
		if(suwFanPulseLowCnt[FAN3_EXTR] >= 3000)		 //3000 * 1ms=3s// 30000*104us = 3s
		{
			suwFanPulseLowCnt[FAN3_EXTR] = 3000;
			suwFanPeriodTemp[FAN3_EXTR] = 3000;
		}
		suwFanPulseLowFlag[FAN3_EXTR] = 1;

		suwFanPulseOffCnt[FAN3_EXTR]++;
		if(suwFanPulseOffCnt[FAN3_EXTR] >= stMachineCfg.wExtrFan123SetFault)
		{
			suwFanPulseOffCnt[FAN3_EXTR] = stMachineCfg.wExtrFan123SetFault;
		}

		if(FALSE==stFan.unFlagBits[FAN3_EXTR].bit.FreqFault)		// Fan frequency normal
		{
			if(suwFanPulseOffCnt[FAN3_EXTR] >= stMachineCfg.wExtrFan123SetFault)
			{
				stFan.unFlagBits[FAN3_EXTR].bit.FreqFault = TRUE;
			}
		}
		else							// Fan frequency Abnormal
		{
			if((suwFanPulseOnCnt[FAN3_EXTR] >= 2)
			 &&(suwFanPulseOnCnt[FAN3_EXTR] <= stMachineCfg.wExtrFan123ClrFault))
			{
				stFan.unFlagBits[FAN3_EXTR].bit.FreqFault = FALSE;
			}
		}
		suwFanPulseOnCnt[FAN3_EXTR]=0;
	}

	sdFanPeriodSum[FAN1_EXTR] += suwFanPeriodTemp[FAN1_EXTR];
	sdFanPeriodSum[FAN2_EXTR] += suwFanPeriodTemp[FAN2_EXTR];
	sdFanPeriodSum[FAN3_EXTR] += suwFanPeriodTemp[FAN3_EXTR];
	sdFanPeriodSum[FAN4_INTR] += suwFanPeriodTemp[FAN4_INTR];

	suwFanPeriodCalcCnt++;
	if(suwFanPeriodCalcCnt >= 32)	// 1ms * 128= 128ms
	{
		suwFanPeriodCalcCnt = 0;

		 stFan.uwFanPeriod[FAN1_EXTR] = (sdFanPeriodSum[FAN1_EXTR] >>5);
		 stFan.uwFanPeriod[FAN2_EXTR] = (sdFanPeriodSum[FAN2_EXTR] >>5);
		 stFan.uwFanPeriod[FAN3_EXTR] = (sdFanPeriodSum[FAN3_EXTR] >>5);
		 stFan.uwFanPeriod[FAN4_INTR] = (sdFanPeriodSum[FAN4_INTR] >>5);

		sdFanPeriodSum[FAN1_EXTR] = 0;
		sdFanPeriodSum[FAN2_EXTR] = 0;
		sdFanPeriodSum[FAN3_EXTR] =0;
		sdFanPeriodSum[FAN4_INTR] = 0;
		 
	}
	
}

