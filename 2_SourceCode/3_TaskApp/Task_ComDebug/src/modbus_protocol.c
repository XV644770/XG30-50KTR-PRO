/*
 * modbus_protocol.c
 *
 *  Created on: 2020年12月28日
 *      Author: Administrator
 */
#include "dsp_user_include.h"
							//  XG      AA     60    KW
Uint16	uwDspSerialNumber[4]={0x5847,0x4141,0x3630,0x4B57};

/* Function Name:    ReadHoldingReg
* Input:            uwRegAddr:Register start address;
*					 uwRegNum:Read register number
* Output:           puwTxBuf:Output buffer
* Returns:      	 bTxLen:Output byte number
* Description:      The function code 03H reads the holding register
*/
Uint16 uwReadHoldingReg(Uint16 uwRegAddr,Uint16 uwRegNum,Uint16* puwTxBuf)
{
	Uint16 uwTxIndexTmp,uwTxLenTmp=1;
	Uint16 uwRegOffset;
	Uint16 uwTransmitData,uwCRC1Tmp;

	static Uint16 uwHoldingRegCnt=0;
	static Uint16 uwSettingRegCnt=0;

	uwRegOffset = uwRegAddr;

	uwTxLenTmp  = 1;
	puwTxBuf[uwTxLenTmp++] = 0x03;
	puwTxBuf[uwTxLenTmp++] = 2*uwRegNum;

	uwSettingRegCnt++;

	for(uwTxIndexTmp=uwRegNum;uwTxIndexTmp>0;uwTxIndexTmp--)
	{
		switch(uwRegOffset)
		{
			/*****************Read --- Parameters Holding Register****************************/

			case 0x3001:		uwTransmitData = uwDspSerialNumber[0];				break;
			case 0x3002:		uwTransmitData = uwDspSerialNumber[1];				break;
			case 0x3003:		uwTransmitData = uwDspSerialNumber[2];				break;
			case 0x3004:		uwTransmitData = uwDspSerialNumber[3];				break;
			case 0x3016:		uwTransmitData = MACHINE_TYPE;						break;
			case 0x3017:		uwTransmitData = stSysFaultReg.unHardwareFault.all;	break;
			case 0x3018:
				if(1==stDebug.SetData.unSetReg.bit.TurnOnOff)
				{
					uwTransmitData = 0x5555; 					/* inverter turn on/off */
				}
				else
				{
					uwTransmitData = 0xAAAA;
				}
			break;
			case 0x3019:		uwTransmitData = stDebug.SetData.uwActivePercent;	break;
			case 0x301A:		uwTransmitData = stDebug.SetData.uwBusRefMin;		break;
			case 0x301B:		uwTransmitData = stDebug.SetData.wReacitvePF;			break;
			case 0x301C:		uwTransmitData = stDebug.SetData.wReacitvePercent;	break;
			case 0x301E:		uwTransmitData = stDspReceData.unSafetyOdm.all;				break;
			case 0x301F:		uwTransmitData = (stDspReceData.stProtectEnvrmt.unProtectBit.all*256+stDspReceData.stProtectEnvrmt.unEnvrmtBit.all);			break;
			case 0x3020:		uwTransmitData = stDspReceData.unFuncEnable.all;				break;
			case 0x3021:		uwTransmitData = stDspReceData.uwSettingPower;		break;
			case 0x3022: 		uwTransmitData = stSysCfg.eMpptMode;				break;

						/***********************Inv Curr Ref Para****************/
			case 0x302B: 		uwTransmitData = stDebug.SetData.uwIdRef;				break;
			case 0x302C: 		uwTransmitData = stDebug.SetData.wIqRef;				break;
			case 0x302D: 		uwTransmitData = stDebug.SetData.uwAngleOffset;			break;
			case 0x302E:		uwTransmitData = stMachineCfg.wInvTempDerate;	break;
			case 0x302F:		uwTransmitData = stMachineCfg.wBoostTempDerate;	break;
									/***********************PV Volt Ctrl Loop Para****************/
			case 0x3030:		uwTransmitData = stPVVoltCtrl[PVA].stPID.stIn.dKp;		break;
			case 0x3031:		uwTransmitData = stPVVoltCtrl[PVA].stPID.stIn.dKi;		break;
			case 0x3032:		uwTransmitData = stBTCurrCtrl[PVA].stPID.stIn.dKp;		break;
			case 0x3033:		uwTransmitData = stBTCurrCtrl[PVA].stPID.stIn.dKi;		break;
			case 0x3034:		uwTransmitData = stActivePowerCtrl.stPID.stIn.dKp;		break;
			case 0x3035:		uwTransmitData = stActivePowerCtrl.stPID.stIn.dKi;		break;
			case 0x3036:		uwTransmitData = stDebug.SetData.uwBTSetDuty;			break;//mgs add for test OCP
					/***********************Inv Voltage Ctrl Loop Para****************/
			case 0x3040:		uwTransmitData = stInvVoltCtrl.stIn.dVoltHighR_Kd;		break;
			case 0x3041:		uwTransmitData = stInvVoltCtrl.stIn.dVoltFstDiff_Kd;	break;
			case 0x3042:		uwTransmitData = stInvVoltCtrl.stIn.dVoltSecDiff_Kd;	break;
			case 0x3043:		uwTransmitData = stInvCurrCtrl.stIn.dCurr_Kd;			break;
			case 0x3044:		uwTransmitData = stInvCurrCtrl.stIn.dCurr_Kq;			break;
			case 0x3045:		uwTransmitData = stInvCurrCtrl.stIn.dCurr_Kpi;			break;
			case 0x3046:		uwTransmitData = stInvBusVoltCtrl.stPID.stIn.dKp;		break;
			case 0x3047:		uwTransmitData = stInvBusVoltCtrl.stPID.stIn.dKi;		break;
			case 0x3048:		uwTransmitData = stPllPara.stIn.wIslandDisturbKp;		break;

			case 0x3049:	uwTransmitData = stMachineCfg.wAmbTempDerate;				break;
			case 0x304A:	uwTransmitData = stMachineCfg.wInvBstDerateStep;			break;
			case 0x304B:	uwTransmitData = stMachineCfg.wInvBstDerateBackStep;		break;
			case 0x304C:	uwTransmitData = stMachineCfg.wAmbDerateStep;				break;
			case 0x304D:	uwTransmitData = stMachineCfg.wAmbDerateBackStep;			break;

			case 0x304E:		uwTransmitData = stInvCurrCtrl.stIn.dReCtrl_KPR;			break;
			case 0x304F:		uwTransmitData = stInvCurrCtrl.stIn.dReCtrl_KPR1;			break;
			case 0x3050:		uwTransmitData = stInvCurrCtrl.stIn.dReCtrl_KPR2;			break;
			case 0x3051:		uwTransmitData = stInvCurrCtrl.stIn.ReCtrl_Enable;			break; 
			case 0x3052:		uwTransmitData = stDebug.SetData.unSetReg.bit.PWMMode;	    break;

			/*****************Read --- Fan Test Message****************************/
			case 0x3053:		uwTransmitData = stFan.unFlagBits[FAN1_EXTR].bit.OnOff;		break;
			case 0x3054:		uwTransmitData = stFan.unFlagBits[FAN1_EXTR].bit.FanTest;		break;
			case 0x3055:			
				if(stFan.uwFanPeriod[FAN1_EXTR] > 0)
				{
				
					uwTransmitData = (16384/stFan.uwFanPeriod[FAN1_EXTR])>>4;	
					if(stFan.unFlagBits[FAN1_EXTR].bit.OnOff > FAN_SPEED_60)
					{
						 uwTransmitData  = uwTransmitData -20;
					}
			      }
				else
				{
					uwTransmitData = 0;
				}
			break;
			case 0x3056:		
				if(stFan.uwFanPeriod[FAN2_EXTR] > 0)
				{
				
					uwTransmitData = (16384/stFan.uwFanPeriod[FAN2_EXTR])>>4;	
					if(stFan.unFlagBits[FAN2_EXTR].bit.OnOff > FAN_SPEED_60)
					{
						 uwTransmitData  = uwTransmitData -20;
					}
			      }
				else
				{
					uwTransmitData = 0;
				}
			break;
			case 0x3057:		
				if(stFan.uwFanPeriod[FAN3_EXTR] > 0)
				{
				
					uwTransmitData = (16384/stFan.uwFanPeriod[FAN3_EXTR])>>4;
					if(stFan.unFlagBits[FAN3_EXTR].bit.OnOff > FAN_SPEED_60)
					{
						 uwTransmitData  = uwTransmitData -20;
					}
			      }
				else
				{
					uwTransmitData = 0;
				}
				break;
			case 0x3058:		
				if(stFan.uwFanPeriod[FAN4_INTR] > 0)
				{
				
					uwTransmitData = (16384/stFan.uwFanPeriod[FAN4_INTR])>>4;
					if(uwTransmitData > 30)
					{
						 uwTransmitData  = uwTransmitData -30;
					}

			      }
				else
				{
					uwTransmitData = 0;
				}		
				break;
			case 0x3059:		uwTransmitData = stInvCurrCtrl.stIn.Rectrl_Points;		break;
			case 0x305A:		uwTransmitData = stF107Data.uwArcFaultLimit;			break;
			case 0x305B:		uwTransmitData = stF107Data.uwAfciSetCmd;				break;
			case 0x3065:		uwTransmitData = wDCI_PI_SET;							break;
			case 0x3066:		uwTransmitData = stMachineCfg.wHardInvOCPCap;			break;
			case 0x3067:		uwTransmitData = wDCI_Injection_Enable;					break;
			
			case 0x3069:	uwTransmitData =stMachineCfg.wInvTempOver;					break;
			case 0x306A:	uwTransmitData =stMachineCfg.wAmbTempOver;					break;
			case 0x306B:	uwTransmitData =stMachineCfg.wBoostTempOver;				break;
			case 0x306C:	uwTransmitData = stDebug.SetData.unSetReg.bit.OpenLoopUnlock;		break;
			case 0x306D:	uwTransmitData = stDebug.SetData.unSetReg.bit.OpenLoopEnable;		break;

			/*****************Read --- Parameters Inputing Register****************************/
			case 0x3800:		uwTransmitData = SOFTWARE_FORMAL*100+SOFTWARE_TEST;		break;
			case 0x3801:		uwTransmitData = stF042Para.uwSoftWareVersion;			break;
			case 0x3802:		uwTransmitData = stTaskTimer.uwStartDelayTime;			break;
			case 0x3803:		uwTransmitData = unSysFlag.all;							break;
			case 0x3806:		uwTransmitData = eInverterStatus;						break;
			case 0x3807:		uwTransmitData = stSysFaultReg.uwWarnCode;				break;
			case 0x3808:		uwTransmitData = stSysFaultReg.uwWarnSubCode;			break;
			case 0x3809:		uwTransmitData = stSysFaultReg.uwFaultCode;				break;
			case 0x380A:		uwTransmitData = stSysFaultReg.uwFaultSubCode;			break;
			case 0x380B:		uwTransmitData = stDCSample.wPVVolt[PVA];				break;
			case 0x380C:		uwTransmitData = stDCSample.wBTCurr[PVA];				break;
			case 0x380D:		uwTransmitData = stDCSample.wPVVolt[PVB];				break;
			case 0x380E: 		uwTransmitData = stDCSample.wBTCurr[PVB];				break;
			case 0x380F: 		uwTransmitData = stDCSample.wPVVolt[PVC];				break;
			case 0x3810: 		uwTransmitData = stDCSample.wBTCurr[PVC];				break;
			case 0x3811:		uwTransmitData = stDCSample.wPVVolt[PVD];				break;
			case 0x3812:		uwTransmitData = stDCSample.wBTCurr[PVD];				break;
			case 0x3813: 		uwTransmitData = stDCSample.wPBusVolt;					break;
			case 0x3814: 		uwTransmitData = stDCSample.wNBusVolt;					break;
			case 0x3815: 		uwTransmitData = stMpptTskCtrl.uwBusRef;				break;
			case 0x3816: 		uwTransmitData = stDCSample.wBusVolt;					break;

			case 0x3817: 		uwTransmitData = stACSample.PhaseVoltRms.wRN;			break;
			case 0x3818:		uwTransmitData = stACSample.PhaseCurrRms.wRN;			break;
			case 0x3819: 		uwTransmitData = stACSample.PhaseVoltRms.wSN;			break;
			case 0x381A: 		uwTransmitData = stACSample.PhaseCurrRms.wSN;			break;
			case 0x381B: 		uwTransmitData = stACSample.PhaseVoltRms.wTN;			break;
			case 0x381C: 		uwTransmitData = stACSample.PhaseCurrRms.wTN;			break;
			case 0x381D: 		uwTransmitData = stACSample.InvVoltRms.wRN;				break;
			case 0x381E: 		uwTransmitData = stACSample.RlyVoltRms.wRN;				break;
			case 0x381F: 		uwTransmitData = stACSample.InvVoltRms.wSN;				break;
			case 0x3820: 		uwTransmitData = stACSample.RlyVoltRms.wSN;				break;
			case 0x3821: 		uwTransmitData = stACSample.InvVoltRms.wTN;				break;
			case 0x3822: 		uwTransmitData = stACSample.RlyVoltRms.wTN;				break;


			case 0x3823: 		uwTransmitData = (stACSample.dActivePowerAvg>>16);				break;
			case 0x3824: 		uwTransmitData = (stACSample.dActivePowerAvg&0xffff);			break;
			case 0x3825: 		uwTransmitData = (stACSample.dReactivePower>>16);				break;
			case 0x3826: 		uwTransmitData = (stACSample.dReactivePower&0xffff);			break;
			case 0x3827: 		uwTransmitData = (stACSample.dApparentPowerAvg>>16);			break;
			case 0x3828: 		uwTransmitData = (stACSample.dApparentPowerAvg&0xffff);			break;
			case 0x3829: 		uwTransmitData = stACSample.wPowerFactor;						break;
			case 0x382A: 		uwTransmitData = (stLoadLimit.dActivePowerActual>>16);			break;
			case 0x382B: 		uwTransmitData = (stLoadLimit.dActivePowerActual&0xffff);		break;
			case 0x382C: 		uwTransmitData = (stLoadLimit.dApparPowerActual>>16);			break;
			case 0x382D: 		uwTransmitData = (stLoadLimit.dApparPowerActual&0xffff);		break;
			case 0x382E: 		uwTransmitData = stLoadLimit.uwDeratingMode;					break;
			case 0x382F: 		uwTransmitData = (stLoadLimit.dActPowerLimitOutput>>16);		break;
			case 0x3830: 		uwTransmitData = (stLoadLimit.dActPowerLimitOutput&0xffff);		break;
			case 0x3831: 		uwTransmitData = stDCSample.wInvTemp;							break;
			case 0x3832: 		uwTransmitData = stDCSample.wBoostTemp;							break;
			case 0x3833: 		uwTransmitData = stDCSample.wAmTemp;							break;
			case 0x3834: 		uwTransmitData = stFreqRang.uwFreqFlag;							break;
			case 0x3835:		uwTransmitData = stACSample.wGridFreqReal;						break;
			case 0x3836:		uwTransmitData = stACSample.wGridFreqAvg;						break;
			case 0x3837:		uwTransmitData = stPllPara.stOut.uwPllOk;						break;
			/*****************Read --- PV Control Message****************************/
			case 0x3838:		uwTransmitData = stBTCtrl[PVA].uwBtPwmEnable;					break;
			case 0x3839:		uwTransmitData = stBTCtrl[PVA].uwBtPwmDuty;						break;
			case 0x383A:		uwTransmitData = stBTCtrl[PVB].uwBtPwmEnable;					break;
			case 0x383B:		uwTransmitData = stBTCtrl[PVB].uwBtPwmDuty;						break;
			case 0x383C:		uwTransmitData = stBTCtrl[PVC].uwBtPwmEnable;					break;
			case 0x383D:		uwTransmitData = stBTCtrl[PVC].uwBtPwmDuty;						break;
			case 0x383E:		uwTransmitData = stBTCtrl[PVD].uwBtPwmEnable;					break;
			case 0x383F:		uwTransmitData = stBTCtrl[PVD].uwBtPwmDuty;						break;
			/*****************Read --- INV Control Message****************************/
			case 0x3840:		uwTransmitData = stInvPara.wVolt_d;						break;
			case 0x3841:		uwTransmitData = stInvPara.wVolt_q;						break;
			case 0x3842:		uwTransmitData = stInvCurrCtrl.stIn.wCurr_d;			break;
			case 0x3843:		uwTransmitData = stInvPara.wCurr_d;						break;
			case 0x3844:		uwTransmitData = stInvCurrCtrl.stIn.wCurr_q;			break;
			case 0x3845:		uwTransmitData = stInvPara.wCurr_q;						break;
			/*****************Read --- BOOST Control Message****************************/
			case 0x3846:		uwTransmitData = stBTCurrCtrl[PVA].stIn.dRef;			break;
			case 0x3847:		uwTransmitData = stBTCurrCtrl[PVA].stIn.dReal;			break;
			case 0x3848:		uwTransmitData = stBTCurrCtrl[PVB].stIn.dRef;			break;
			case 0x3849:		uwTransmitData = stBTCurrCtrl[PVB].stIn.dReal;			break;
			case 0x384A:		uwTransmitData = stBTCurrCtrl[PVC].stIn.dRef;			break;
			case 0x384B:		uwTransmitData = stBTCurrCtrl[PVC].stIn.dReal;			break;
			case 0x384C:		uwTransmitData = stBTCurrCtrl[PVD].stIn.dRef;			break;
			case 0x384D:		uwTransmitData = stBTCurrCtrl[PVD].stIn.dReal;			break;
			case 0x384E:		uwTransmitData = stF042Para.uwFrameIndex;				break;
			case 0x384F:		uwTransmitData = stInvPwm.uwPwmDisableDelayCnt;			break;
			case 0x3850:		uwTransmitData = stInvPwm.unPwmDisableBit.all;			break;
			case 0x3851:		uwTransmitData = stInvPwm.unPwmDisableBitOld.all;		break;
			/*****************Read --- Debug Message****************************/
			case 0x3852:		uwTransmitData = stDebug.ReadData.uwWaitStatus;			break;
			case 0x3853:		uwTransmitData = stDebug.ReadData.uwWaitError;			break;
			case 0x3854:		uwTransmitData = stDebug.ReadData.uwInvStatus;			break;
			case 0x3855:		uwTransmitData = stDebug.ReadData.uwInvError;			break;
			case 0x3856:		uwTransmitData = stDebug.ReadData.uwRelayCheckStatus;	break;
			case 0x3857:		uwTransmitData = (stSysFaultReg.unFaultBit.all>>16);	break;
			case 0x3858:		uwTransmitData = (stSysFaultReg.unFaultBit.all&0xffff);	break;
			case 0x3859:		uwTransmitData = stDCSample.uwISOResistance;			break;
			case 0x385A:		uwTransmitData = stF107Data.wBTCurr70PctAdjRatio[PVA]/100;		break;
			case 0x385B:		uwTransmitData = stF107Data.wBTCurr70PctAdjRatio[PVA];		break;
			case 0x385C:		uwTransmitData = stACSample.wGridFreqAvg;		break;
			case 0x385D:		uwTransmitData =stACSample.wGridFreqReal;		break;
			case 0x385E:		uwTransmitData =stFreqRang.uwFreqFlag;				break;
			case 0x385F:		uwTransmitData = 111;			break;
			case 0x3860:		uwTransmitData = stPllPara.stOut.uwPllOk;			break;

			case 0x3861:		uwTransmitData = EPwm1Regs.CMPA.bit.CMPA;			break;
			case 0x3862:		uwTransmitData = EPwm1Regs.AQCSFRC.all;			break;
			case 0x3863:		uwTransmitData = EPwm8Regs.CMPA.bit.CMPA;			break;
			case 0x3864:		uwTransmitData = EPwm8Regs.AQCSFRC.all;			break;
			case 0x3865:		uwTransmitData = stAfciPara1.uwCurrCH1;		break;
			case 0x3866:		uwTransmitData = stAfciPara1.uwCurrCH2;			break;
			case 0x3867:		uwTransmitData = stAfciPara1.uwCurrCH3;		break;
			case 0x3868:		uwTransmitData = stAfciPara1.uwCurrCH4;		break;
			case 0x3869:		uwTransmitData = stAfciPara1.uwArcRecordCH1;		break;
			case 0x386A:		uwTransmitData = stAfciPara1.uwArcRecordCH2;		break;
			case 0x386B:		uwTransmitData = stAfciPara1.uwArcRecordCH3;		break;

			case 0x386C:		uwTransmitData = stAfciPara1.uwArcRecordCH4;		break;
			case 0x386D:		uwTransmitData = stAfciPara1.uwSelfCheckResult;			break;
			case 0x386E:		uwTransmitData =stAfciPara1.uwArcFaultLimit;	break;
			case 0x386F:		uwTransmitData = 666;			break;

			case 0x3870:		uwTransmitData = stMachineCfg.wHardInvOCPCap;	break;
			case 0x3871:		uwTransmitData = stWaitStatus.stRelayCheck.Flag.Success;	break;
			case 0x3872:		uwTransmitData = stWaitStatus.stRelayCheck.Flag.Fail;		break;
			case 0x3873:		uwTransmitData = stWaitStatus.stRelayCheck.Flag.InvPwmOn;				break;
			case 0x3874:		uwTransmitData = stAfciPara2.uwArcFaultBit;		break;
			case 0x3875:		uwTransmitData = stWaitStatus.stRelayCheck.Flag.InvCurrOver;		break;
			case 0x3876:		uwTransmitData = stInvPwm.uwHardInvOCPTimes;			break;
			case 0x3877:		uwTransmitData = stInvPwm.uwHardBoostOCPTimes;			break;
			case 0x3878:		uwTransmitData = stInvPwm.uwSoftUnderVoltTimes;			break;
			case 0x3879:		uwTransmitData = stAfciPara2.uwArcFaultLimit;	break;
			case 0x387A:		uwTransmitData = stInvPwm.uwSoftBoostOCPTimes;		break;
			case 0x387B:		uwTransmitData = stInvPwm.uwSoftInvOCPTimes;		break;
			case 0x387C:		uwTransmitData = stWaitStatus.stRelayCheck.Flag.InvPwmOn;		break;
			case 0x387D:		uwTransmitData = stWaitStatus.stRelayCheck.Flag.InvCurrOver;		break;
			/*****************Read --- PVA--PVD String Curr****************************/
#if 0
			case 0x387E:        uwTransmitData = stDCSample.wBTStrCurr[STR0];  break;          //20210901Revised
			case 0x387F:        uwTransmitData = stDCSample.wBTStrCurr[STR1];  break;          //20210901Revised
			case 0x3880:        uwTransmitData = stDCSample.wBTStrCurr[STR2];  break;          //20210901Revised
			case 0x3881:        uwTransmitData = stDCSample.wBTStrCurr[STR3];  break;          //20210901Revised
			case 0x3882:        uwTransmitData = stDCSample.wBTStrCurr[STR4];  break;          //20210901Revised
			case 0x3883:        uwTransmitData = stDCSample.wBTStrCurr[STR5];  break;          //20210901Revised
			case 0x3884:        uwTransmitData = stDCSample.wBTStrCurr[STR6];  break;          //20210901Revised
			case 0x3885:        uwTransmitData = stDCSample.wBTStrCurr[STR7];  break;          //20210901Revised
			case 0x3886:        uwTransmitData = stDCSample.wBTStrCurr[STR8];  break;          //20210902Revised
			case 0x3887:        uwTransmitData = stDCSample.wBTStrCurr[STR9];  break;          //20210902Revised
			case 0x3888:        uwTransmitData = stDCSample.wBTStrCurr[STR10];  break;         //20210902Revised
			case 0x3889:        uwTransmitData = stDCSample.wBTStrCurr[STR11];  break;         //20210902Revised
#else
			case 0x387E:        uwTransmitData = stF107Data.wRSVoltAdjRatio;  break;          //20210901Revised
			case 0x387F:        uwTransmitData = stF107Data.wSTVoltAdjRatio;  break;          //20210901Revised
			case 0x3880:        uwTransmitData = stF107Data.wTRVoltAdjRatio;  break;          //20210901Revised
			case 0x3881:        uwTransmitData = stF107Data.wRNCurr30AdjRatio;  break;        //20210901Revised
			case 0x3882:        uwTransmitData = stF107Data.wSNCurr30AdjRatio;  break;         //20210901Revised
			case 0x3883:        uwTransmitData = stF107Data.wTNCurr30AdjRatio;  break;          //20210901Revised
			case 0x3884:        uwTransmitData = stF107Data.wRNCurr70AdjRatio;  break;          //20210901Revised
			case 0x3885:        uwTransmitData = stF107Data.wSNCurr70AdjRatio;  break;          //20210901Revised
			case 0x3886:        uwTransmitData = stF107Data.wTNCurr70AdjRatio;  break;          //20210902Revised
			case 0x3887:        uwTransmitData = 444;  break;          //20210902Revised
			case 0x3888:        uwTransmitData = 555;  break;         //20210902Revised
			case 0x3889:        uwTransmitData = 666;  break;         //20210902Revised

			case 0x388A:        uwTransmitData = stF107Data.wPVVoltAdjRatio[PVA];  break;         //20210902Revised
			case 0x388B:        uwTransmitData = stF107Data.wPVVoltAdjRatio[PVB];  break;         //20210902Revised
			case 0x388C:        uwTransmitData = stF107Data.wPVVoltAdjRatio[PVC];  break;         //20210902Revised
			case 0x388D:        uwTransmitData = stF107Data.wPVVoltAdjRatio[PVD];  break;         //20210902Revised
			case 0x388E:        uwTransmitData = stF107Data.wBTCurr30PctAdjRatio[PVA];  break;         //20210902Revised
			case 0x388F:        uwTransmitData = stF107Data.wBTCurr30PctAdjRatio[PVB];  break;         //20210902Revised
			case 0x3890:        uwTransmitData = stF107Data.wBTCurr30PctAdjRatio[PVC];  break;         //20210902Revised
			case 0x3891:        uwTransmitData = stF107Data.wBTCurr30PctAdjRatio[PVD];  break;         //20210902Revised
			case 0x3892:        uwTransmitData = stF107Data.wBTCurr70PctAdjRatio[PVA];  break;         //20210902Revised
			case 0x3893:        uwTransmitData = stF107Data.wBTCurr70PctAdjRatio[PVB];  break;         //20210902Revised
			case 0x3894:        uwTransmitData = stF107Data.wBTCurr70PctAdjRatio[PVC];  break;         //20210902Revised
			case 0x3895:        uwTransmitData = stF107Data.wBTCurr70PctAdjRatio[PVD];  break;         //20210902Revised
			case 0x3896:        uwTransmitData = stF107Data.wActPower30AdjRatio;  break;         //20210902Revised
			case 0x3897:        uwTransmitData = stF107Data.wActPower70AdjRatio;  break;         //20210902Revised
#endif
			case 0x3901:		uwTransmitData = stGridProtect.uwACStartOverVolt;		break;
			case 0x3902:		uwTransmitData = stGridProtect.uwACStartUnderVolt;		break;
			case 0x3903:		uwTransmitData = stGridProtect.uwACStartOverFreq;		break;
			case 0x3904:		uwTransmitData = stGridProtect.uwACStartUnderFreq;		break;

			case 0x3905:		uwTransmitData = stGridProtect.uwACUnderVolt1;		break;
			case 0x3906:		uwTransmitData = stGridProtect.uwACOverVolt1;		break;
			case 0x3907:		uwTransmitData = stGridProtect.uwACUnderVolt1Time;		break;
			case 0x3908:		uwTransmitData = stGridProtect.uwACOverVolt1Time;		break;

			case 0x3909:		uwTransmitData = stGridProtect.uwACUnderVolt2;		break;
			case 0x390A:		uwTransmitData = stGridProtect.uwACUnderVolt2Time;		break;
			case 0x390B:		uwTransmitData = stGridProtect.uwACOverVolt2;		break;
			case 0x390C:		uwTransmitData = stGridProtect.uwACOverVolt2Time;		break;
			case 0x390D:		uwTransmitData = stGridProtect.uwAC10MinOverVolt;		break;
							/*****************F107 TX Frame3*******************/
			case 0x390E:		uwTransmitData = stGridProtect.uwACUnderFreq1;		break;
			case 0x390F:		uwTransmitData = stGridProtect.uwACUnderFreq2;		break;
			case 0x3910:		uwTransmitData = stGridProtect.uwACUnderFreq1Time;		break;
			case 0x3911:		uwTransmitData = stGridProtect.uwACUnderFreq2Time;		break;

			case 0x3912:		uwTransmitData = stGridProtect.uwACOverFreq1;		break;
			case 0x3913:		uwTransmitData = stGridProtect.uwACOverFreq2;		break;
			case 0x3914:		uwTransmitData = stGridProtect.uwACOverFreq1Time;		break;
			case 0x3915:		uwTransmitData = stGridProtect.uwACOverFreq2Time;		break;

			case 0x5FFC:		uwTransmitData = 0x150A;		break;				// year-month
			case 0x5FFD:		uwTransmitData = 0x1213;		break;				// day-hour
			case 0x5FFE:		uwTransmitData = 0x1F01;		break;				// minute-second
			case 0x5FFF:		uwTransmitData = 0x0001;		break;				// fault-code

			case 0x7FFC:		uwTransmitData = 0x150A;		break;				// year-month
			case 0x7FFD:		uwTransmitData = 0x1213;		break;				// day-hour
			case 0x7FFE:		uwTransmitData = 0x1F01;		break;				// minute-second
			case 0x7FFF:		uwTransmitData = 0x0001;		break;				// fault-code


			default:
				if((uwRegOffset>=0x6000)&&(uwRegOffset<=0x61F3))
				{
					uwTransmitData = ReadWaveData(0,uwRegOffset-0x6000);
				}
				else if((uwRegOffset>=0x6400)&&(uwRegOffset<=0x65F3))
				{
					uwTransmitData = ReadWaveData(1,uwRegOffset-0x6400);
				}
				else if((uwRegOffset>=0x6800)&&(uwRegOffset<=0x69F3))
				{
					uwTransmitData = ReadWaveData(2,uwRegOffset-0x6800);
				}
				else if((uwRegOffset>=0x6C00)&&(uwRegOffset<=0x6DF3))
				{
					uwTransmitData = ReadWaveData(3,uwRegOffset-0x6C00);
				}
				else if((uwRegOffset>=0x8000)&&(uwRegOffset<=0x81F3))
				{
					uwTransmitData = ReadWaveData(0,uwRegOffset-0x8000);
				}
				else if((uwRegOffset>=0x8400)&&(uwRegOffset<=0x85F3))
				{
					uwTransmitData = ReadWaveData(1,uwRegOffset-0x8400);
				}
				else if((uwRegOffset>=0x8800)&&(uwRegOffset<=0x89F3))
				{
					uwTransmitData = ReadWaveData(2,uwRegOffset-0x8800);
				}
				else if((uwRegOffset>=0x8C00)&&(uwRegOffset<=0x8DF3))
				{
					uwTransmitData = ReadWaveData(3,uwRegOffset-0x8C00);
				}
				else
				{
					uwTransmitData = 0;
				}
			break;
		}
		puwTxBuf[uwTxLenTmp++] = ((uwTransmitData&0xFF00)>>8);
		puwTxBuf[uwTxLenTmp++] = (uwTransmitData&0x00FF);
		uwRegOffset++;		//Next register address
	}

	if((uwRegOffset>0x6DF2)&&(uwRegOffset<=0x6DF4))
	{
		stWaveRecord.uwReadWaveTrigger = DISABLE;
	}

	uwCRC1Tmp = uwCalcCRC16(puwTxBuf, uwTxLenTmp);
	puwTxBuf[uwTxLenTmp++] = (uwCRC1Tmp&0x00FF);
	puwTxBuf[uwTxLenTmp++] = ((uwCRC1Tmp&0xFF00)>>8);
	return uwTxLenTmp;
}

/* Function Name:    uwSetMultipleReg
* Input:            uwRegAddr:Register start address;
*					 uwRegNum:Read register number
* Output:           puwTxBuf:Output buffer
* Returns:      	 bTxLen:Output byte number
* Description:      The function code 10H set the holding register
*/
Uint16 uwSetMultipleReg(Uint16 uwRegAddr,Uint16 uwRegNum,Uint16* puwRxBuf)
{
	Uint16 uwSetValueTmp=0,uwSetResultTmp;
	Uint16 uwBufIndex=4,uwRegOffsetAddrTmp=0;
	Uint16 uwPvId;

	uwSetResultTmp = 0;		// The default setting is correct
	if(0x10==puwRxBuf[1])
	{
		uwBufIndex = 7;			// 01 10(FunctionCode) 00 00(StartAddr) 00 00(SetRegNum) 20(length) 00 00 (DataH+DataL) ......
	}
	else
	{
		uwBufIndex = 4;			// 01 06(FunctionCode) 00 00(StartAddr) 00 00 (DataH+DataL) .....
	}
	for(uwRegOffsetAddrTmp=0;uwRegOffsetAddrTmp<uwRegNum;uwRegOffsetAddrTmp++)
	{
		uwSetValueTmp = puwRxBuf[uwBufIndex++];
   	 	uwSetValueTmp = (uwSetValueTmp<<8)|(puwRxBuf[uwBufIndex++]);

		switch(uwRegAddr)
		{
			case 0x3018:
				if(0xAAAA==uwSetValueTmp)
				{
					stDebug.SetData.unSetReg.bit.TurnOnOff = 0;		// turn off
				}
				else if(0x5555==uwSetValueTmp)
				{
					stDebug.SetData.unSetReg.bit.TurnOnOff = 1;		// turn on
				}
			break;

			case 0x3019:
				if(uwSetValueTmp<=1000)
				{
					stDebug.SetData.uwActivePercent = uwSetValueTmp;
				}
				else
				{
					uwSetResultTmp = 1;		// set failed !
				}
			break;

			case 0x301A:
				if(uwSetValueTmp<=9000)		// 950V
				{
					stDebug.SetData.uwBusRefMin = uwSetValueTmp;
				}
				else
				{
					uwSetResultTmp = 1;		// set failed !
				}
			break;

			case 0x301B:
				if((uwSetValueTmp<=1000)&&(uwSetValueTmp>=800)	// +0.8 ~ 1.0
				|| (uwSetValueTmp<=64735)&&(uwSetValueTmp>=64535))	// -0.8 ~ -1.0
				{
					stDebug.SetData.wReacitvePF = uwSetValueTmp;
					stDebug.SetData.uwReactiveModel = FIXED_PF_MODE;
				}
				else
				{
					uwSetResultTmp = 1;		// set failed !
				}
			break;

			case 0x301C:
				if((uwSetValueTmp<=600)||(uwSetValueTmp>=64936))		// +60% ~ -60%
				{
					stDebug.SetData.wReacitvePercent = uwSetValueTmp;
					stDebug.SetData.uwReactiveModel = REACTIVE_PERCENT_MODE;
				}
				else
				{
					uwSetResultTmp = 1;		// set failed !
				}
			break;

			/***********************Inv Curr Ref Para****************/
			case 0x302B:
				if(uwSetValueTmp<=3200)		// 3200/32 = 100A
				{
					stDebug.SetData.uwIdRef = uwSetValueTmp;
				}
				else
				{
					uwSetResultTmp = 1;		// set failed !
				}
			break;

			case 0x302C:
				if((uwSetValueTmp<=320)||(uwSetValueTmp>=65216))		// 320/32 = 10A
				{
					stDebug.SetData.wIqRef = (int16)uwSetValueTmp;
				}
				else
				{
					uwSetResultTmp = 1;		// set failed !
				}
			break;

			case 0x302D:
				if(uwSetValueTmp <= 50)
				{
					stDebug.SetData.uwAngleOffset = uwSetValueTmp;
				}
			break;

			case 0x302E:
					stMachineCfg.wInvTempDerate = uwSetValueTmp;
			break;
			case 0x302F:
					stMachineCfg.wBoostTempDerate = uwSetValueTmp;
			break;

			/***********************PV Volt Ctrl Loop Para****************/
			case 0x3030:
				for(uwPvId=0;	uwPvId<PV_MAX_NUM;	uwPvId++)
				{
					stPVVoltCtrl[uwPvId].stPID.stIn.dKp = uwSetValueTmp;
				}
			break;

			case 0x3031:
				for(uwPvId=0;	uwPvId<PV_MAX_NUM;	uwPvId++)
				{
					stPVVoltCtrl[uwPvId].stPID.stIn.dKi = uwSetValueTmp;
				}
			break;

			/***********************Boost Curr Ctrl Loop Para****************/
			case 0x3032:
				for(uwPvId=0;	uwPvId<PV_MAX_NUM;	uwPvId++)
				{
					stBTCurrCtrl[uwPvId].stPID.stIn.dKp = uwSetValueTmp;
				}
			break;

			case 0x3033:
				for(uwPvId=0;	uwPvId<PV_MAX_NUM;	uwPvId++)
				{
					stBTCurrCtrl[uwPvId].stPID.stIn.dKi = uwSetValueTmp;
				}
			break;

			case 0x3034:	stActivePowerCtrl.stPID.stIn.dKp = uwSetValueTmp;				break;
			case 0x3035:	stActivePowerCtrl.stPID.stIn.dKi = uwSetValueTmp;				break;

			case 0x3036:	stDebug.SetData.uwBTSetDuty = uwSetValueTmp;					break;//mgs add for test OCP

				/***********************Inv Voltage Ctrl Loop Para****************/
			case 0x3040:
				stInvVoltCtrl.stIn.dVoltHighR_Kd = uwSetValueTmp;
			break;

			case 0x3041:
				stInvVoltCtrl.stIn.dVoltFstDiff_Kd = uwSetValueTmp;
			break;

			case 0x3042:
				stInvVoltCtrl.stIn.dVoltSecDiff_Kd = uwSetValueTmp;
			break;

			case 0x3043:
				stInvCurrCtrl.stIn.dCurr_Kd = uwSetValueTmp;
			break;

			case 0x3044:
				 stInvCurrCtrl.stIn.dCurr_Kq = uwSetValueTmp;
			break;
			case 0x3045:
				stInvCurrCtrl.stIn.dCurr_Kpi = uwSetValueTmp;
			break;

			case 0x3046:
				stInvBusVoltCtrl.stPID.stIn.dKp = uwSetValueTmp;
			break;
			case 0x3047:
				stInvBusVoltCtrl.stPID.stIn.dKi = uwSetValueTmp;
			break;
			
			case 0x3048:
				stPllPara.stIn.wIslandDisturbKp = uwSetValueTmp;
			break;

			case 0x3049:	stMachineCfg.wAmbTempDerate = uwSetValueTmp;			break;
			case 0x304A:	stMachineCfg.wInvBstDerateStep = uwSetValueTmp;		break;
			case 0x304B:	stMachineCfg.wInvBstDerateBackStep = uwSetValueTmp;	break;
			case 0x304C:	stMachineCfg.wAmbDerateStep = uwSetValueTmp;		break;
			case 0x304D:	stMachineCfg.wAmbDerateBackStep = uwSetValueTmp;	break;

			case 0x304E:	 stInvCurrCtrl.stIn.dReCtrl_KPR= uwSetValueTmp;			break;
			case 0x304F: stInvCurrCtrl.stIn.dReCtrl_KPR1= uwSetValueTmp;			break;
			case 0x3050:	 stInvCurrCtrl.stIn.dReCtrl_KPR2= uwSetValueTmp;			break;
			case 0x3051:	 stInvCurrCtrl.stIn.ReCtrl_Enable = uwSetValueTmp;		break; 
			case 0x3052:	stDebug.SetData.unSetReg.bit.PWMMode= uwSetValueTmp;		break;
			/*****************Read --- Fan Test Message****************************/
			case 0x3053:	 stFan.unFlagBits[FAN1_EXTR].bit.OnOff= uwSetValueTmp;		break;
			case 0x3054:	 stFan.unFlagBits[FAN1_EXTR].bit.FanTest = uwSetValueTmp;		break;

			case 0x3059:	stInvCurrCtrl.stIn.Rectrl_Points =uwSetValueTmp;		break;
			case 0x305A:	stF107Data.uwArcFaultLimit = uwSetValueTmp;		break;
			case 0x305B:	stF107Data.uwAfciSetCmd = uwSetValueTmp;		break;
			/************************************Fault/Real Wave Record *********/
			//case 0x305B:		break;
			case 0x305C:	stWaveRecord.uwWaveSamRatio = uwSetValueTmp;	break;
			case 0x305D:	stWaveRecord.uwReadWaveWaitCnt = uwSetValueTmp;		break;
			case 0x305E:	stWaveRecord.uwWaveID[0] = uwSetValueTmp;	break;
			case 0x305F:	stWaveRecord.uwWaveID[1] = uwSetValueTmp;		break;
			case 0x3060:	stWaveRecord.uwWaveID[2] = uwSetValueTmp;	break;
			case 0x3061:
			{
				stWaveRecord.uwWaveID[3] = uwSetValueTmp;
				stWaveRecord.uwReadWaveTrigger = ENABLE;
				break;
			}

			case 0x3065:	wDCI_PI_SET= uwSetValueTmp;	break;
			case 0x3066:	stMachineCfg.wHardInvOCPCap= uwSetValueTmp;
			INV_OCP(stMachineCfg.wHardInvOCPCap);
			break;

			case 0x3067:	wDCI_Injection_Enable= uwSetValueTmp;	break;
			
			case 0x3069:	stMachineCfg.wInvTempOver= uwSetValueTmp;	break;
			case 0x306A:	stMachineCfg.wAmbTempOver= uwSetValueTmp;	break;
			case 0x306B:	stMachineCfg.wBoostTempOver= uwSetValueTmp;	break;
			case 0x306C:
				if((0xAA55 == uwSetValueTmp) && (cInverterStatus != eInverterStatus))
				{
					stDebug.SetData.unSetReg.bit.OpenLoopUnlock = ENABLE;
				}
				else
				{
					stDebug.SetData.unSetReg.bit.OpenLoopUnlock = DISABLE;
					stDebug.SetData.unSetReg.bit.OpenLoopEnable = DISABLE;
				}
			break;
			case 0x306D:
				if((ENABLE == stDebug.SetData.unSetReg.bit.OpenLoopUnlock) && (ENABLE == uwSetValueTmp))
				{
					stDebug.SetData.unSetReg.bit.OpenLoopEnable = uwSetValueTmp;
				}
				else
				{
					stDebug.SetData.unSetReg.bit.OpenLoopUnlock = DISABLE;
					stDebug.SetData.unSetReg.bit.OpenLoopEnable = DISABLE;
				}
			break;

			default:
			{
				uwSetResultTmp = 0;
				break;
			}
		}
		
		if(uwSetResultTmp != 0)		// fault
		{
			break;
		}
		else
		{
			uwRegAddr++;
		}
	}

	if(0==uwSetResultTmp)			// set successed
	{
		return 1;
	}
	else							// set failed
	{
		return 0;
	}
}

