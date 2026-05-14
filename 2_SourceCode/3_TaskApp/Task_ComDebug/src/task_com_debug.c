
/*
 * task_com_debug.c
 *
 *  Created on: 2020年12月26日
 *      Author: Administrator
 */
#include "dsp_user_include.h"
#include "modbus_protocol.h"

static void ModbusAnalysis(Uint16* puwRxBuf, Uint16 uwRxBufLen);
static Uint16 uwHoldRegAnalysis(Uint16* uwRxDataBuf,	Uint16* uwTxDataBuf);
static Uint16 uwSetRegAnalysis(Uint16* uwRxDataBuf,	Uint16* puwTxBuf);

static void FaultWaveRecordTime(void);

#pragma DATA_SECTION(stRs485Uart4, "COMBuffer")
ST_RS485_UART4	stRs485Uart4;


#pragma DATA_SECTION(stWaveRecord, "WaveBuffer")
ST_DEBUG stDebug;
ST_WAVERECORD stWaveRecord;



#pragma CODE_SECTION(RecordingWave,"ramfuncs");



#if SAMPLE_DEBUG
static int16 GetSin0Tmp(void)
{
    return(suwSin0Tmp);
}

static int16 GetSin120Tmp(void)
{
	return(suwSin120Tmp);
}
static int16 GetSin240Tmp(void)
{
	return(suwSin240Tmp);
}
#endif

static int16 swReturnNull(void)
{
    return(77);
}

static int16 swGFCICurr(void)
{
    return(stAdcPool.GFCI.wArithVal);
}


static int16 GetPBusVolt(void)
{
	return(stAdcPool.PBusVolt.wArithVal);
}
static int16 GetNBusVolt(void)
{
	return(stAdcPool.NBusVolt.wArithVal);
}
static int16 GetBusVolt(void)
{
	return(stAdcPool.BusVolt.wArithVal);
}

static int16 GetBusVolt1ms(void)
{
	return(stDCSample.wBusVolt1ms);
}

static int16 GetBusVoltRef(void)
{
	return(stMpptTskCtrl.uwBusRef);
}

static int16 GetPVVoltRefMax(void)
{
	return(stMpptDisturb[stMpptTskCtrl.uwPVVoltRefMaxID].wPVVoltRef);
}

static int16 GetBTADuty(void)
{
	return(stBTCtrl[PVA].uwBtPwmDuty);
}

static int16 GetBTBDuty(void)
{
	return(stBTCtrl[PVB].uwBtPwmDuty);
}

static int16 GetBTCDuty(void)
{
	return(stBTCtrl[PVC].uwBtPwmDuty);
}

static int16 GetBTDDuty(void)
{
	return(stBTCtrl[PVD].uwBtPwmDuty);
}

static int16 GetRVolt(void)
{
    return(stAdcPool.RGridVolt.wArithVal);
}
static int16 GetSVolt(void)
{
    return(stAdcPool.SGridVolt.wArithVal);
}
static int16 GetTVolt(void)
{
    return(stAdcPool.TGridVolt.wArithVal);
}

static int16 GetRInvVolt(void)
{
    return(stAdcPool.RInvVolt.wArithVal);
}
static int16 GetSInvVolt(void)
{
    return(stAdcPool.SInvVolt.wArithVal);
}
static int16 GetTInvVolt(void)
{
    return(stAdcPool.TInvVolt.wArithVal);//
}
static int16 GetRCurr(void)
{
    return(stAdcPool.RInvCurr.wArithVal);
}
static int16 GetSCurr(void)
{
    return(stAdcPool.SInvCurr.wArithVal);
}
static int16 GetTCurr(void)
{
    return(stAdcPool.TInvCurr.wArithVal);
}
static int16 GetPVAVolt(void)
{
    return(stAdcPool.PVVolt[PVA].wArithVal);
}
static int16 GetPVBVolt(void)
{
    return(stAdcPool.PVVolt[PVB].wArithVal);
}
static int16 GetPVCVolt(void)
{
    return(stAdcPool.PVVolt[PVC].wArithVal);
}
static int16 GetPVDVolt(void)
{
    return(stAdcPool.PVVolt[PVD].wArithVal);
}
static int16 GetPVACurr(void)
{
    return(stAdcPool.BTCurr[PVA].wArithVal);
}
static int16 GetPVBCurr(void)
{
    return(stAdcPool.BTCurr[PVB].wArithVal);
}
static int16 GetPVCCurr(void)
{
    return(stAdcPool.BTCurr[PVC].wArithVal);
}
static int16 GetPVDCurr(void)
{
    return(stAdcPool.BTCurr[PVD].wArithVal);
}

static int16 GetPVACtrlCurr(void)
{
    //return(stAdcPool.BTCurrCtrl[PVA].wArithVal);
	return(55);
}
static int16 GetPVBCtrlCurr(void)
{
    //return(stAdcPool.BTCurrCtrl[PVB].wArithVal);
	return(55);
}
static int16 GetPVCCtrlCurr(void)
{
    //return(stAdcPool.BTCurrCtrl[PVC].wArithVal);
	return(55);
}
static int16 GetPVDCtrlCurr(void)
{
    //return(stAdcPool.BTCurrCtrl[PVD].wArithVal);
	return(55);
}

static int16 GetPVAVoltRef(void)
{
    return(stMpptDisturb[PVA].wMpptVoltRef);
}
static int16 GetPVBVoltRef(void)
{
	return(stMpptDisturb[PVB].wMpptVoltRef);
}
static int16 GetPVCVoltRef(void)
{
	return(stMpptDisturb[PVC].wMpptVoltRef);
}
static int16 GetPVDVoltRef(void)
{
	return(stMpptDisturb[PVD].wMpptVoltRef);
}

static int16 GetPVACurrRef(void)
{
    return(stBTCurrCtrl[PVA].stIn.dRef);
}
static int16 GetPVBCurrRef(void)
{
	 return(stBTCurrCtrl[PVB].stIn.dRef);
}
static int16 GetPVCCurrRef(void)
{
	 return(stBTCurrCtrl[PVC].stIn.dRef);
}
static int16 GetPVDCurrRef(void)
{
	 return(stBTCurrCtrl[PVD].stIn.dRef);
}

static int16 GetPVACurrReal(void)
{
    //return(stBTCurrCtrl[PVA].stIn.dReal);
    return(stAdcPool.BTCurr[PVA].wPrdVal);
}
static int16 GetPVBCurrReal(void)
{
	//return(stBTCurrCtrl[PVB].stIn.dReal);
	//stAdcPool.BTCurr[PVA].wArithVal = (int16)((int32)(stAdcPool.BTCurr[PVA].wZeroVal + stAdcPool.BTCurr[PVA].wPrdVal)*stSysCfg.wBTCurrRatio1>>10)-stAdcPool.BTCurr[PVA].wOffset;	// 1A-->32 DC_I=Vsample*221/2^9
	return(stAdcPool.BTCurr[PVA].wZeroVal);
}
static int16 GetPVCCurrReal(void)
{
	//return(stBTCurrCtrl[PVC].stIn.dReal);
	 return(stAdcPool.BTCurr[PVB].wPrdVal);
}
static int16 GetPVDCurrReal(void)
{
	//return(stBTCurrCtrl[PVD].stIn.dReal);
	return(stAdcPool.BTCurr[PVB].wZeroVal);
}

static int16 GetGridPrdCnt(void)
{
    return(stPllPara.stOut.uwGridPrdCnt*10);
}

static int16 GetGridPrdTim(void)
{
    return(stPllPara.stOut.uwGridPrdTim*10);
}

static int16 GetInvTemp(void)
{
	return(stAdcPool.InvTemp.wArithVal);
}
static int16 GetActGridPrdCnt(void)
{
    return(stPllPara.stOut.uwActGridPrdCnt*10);
}

static int16 GetSinwt(void)
{
    return(stPllPara.stOut.wSinwt);
}

static int16 GetSinwt240(void)
{
	return(stPllPara.stOut.wSinwt_240);
}

static int16 GetSinwt120(void)
{
    return(stPllPara.stOut.wSinwt_120);
}

static int16 GetCoswt(void)
{
    return(stPllPara.stOut.wCoswt);
	
}

static int16 GetCoswt240(void)
{
	return(stPllPara.stOut.wCoswt_240);
}

static int16 GetCoswt120(void)
{
    return(stPllPara.stOut.wCoswt_120);
}

static int16 GetActSinwt(void)
{
    return(stPllPara.stOut.wActSinwt);
}

static int16 GetActSinwt240(void)
{
	return(stPllPara.stOut.wActSinwt_240);
}

static int16 GetActSinwt120(void)
{
    return(stPllPara.stOut.wActSinwt_120);
}

static int16 GetActCoswt(void)
{
    //return(stPllPara.stOut.wActCoswt);
    return(stWaitStatus.stRelayCheck.uwCheckCnt);
}

static int16 GetActCoswt240(void)
{
	//return(stPllPara.stOut.wActCoswt_240);
	return(stWaitStatus.stFanInvtOpenCheck.uwInvtPwmOpenCnt);
	
}

static int16 GetActCoswt120(void)
{
    return(stPllPara.stOut.wActCoswt_120);
}

static int16 GetVolt_d(void)
{
    return(stInvPara.wVolt_d);
}

static int16 GetVolt_q(void)
{
    return(stInvPara.wVolt_q);
}

static int16 GetCurr_d(void)
{
    return(stInvPara.wCurr_d);
}

static int16 GetCurr_q(void)
{
    return(stInvPara.wCurr_q);
}

static int16 GetVoltAlpha(void)
{
    return(stPhaseSequence.stIn.wVoltAlpha);
}

static int16 GetVoltBeta(void)
{
    return(stPhaseSequence.stIn.wVoltBeta);
}



static int16 GetExtrFAN1(void)
{
    return(FAN1_EXTR_LEVEL);
}

static int16 GetExtrFAN2(void)
{
    return(FAN2_EXTR_LEVEL);
}

static int16 GetExtrFAN3(void)
{
    return(FAN3_EXTR_LEVEL);
}

static int16 GetIntrFAN4(void)
{
    return(FAN4_INTR_LEVEL);
}

static int16 GetIntGFCI(void)
{
    return(stAdcPool.GFCI.wArithVal);
}


static int16 GetInvCurrdRef(void)
{
    return(stInvCurrCtrl.stIn.wCurr_d);
}

static int16 GetInvCurrdReal(void)
{
    return(stInvPara.wCurr_d);
}

static int16 GetBTACurrCtrl(void)
{
    return(stBTCurrCtrl[PVA].stIn.uwCtrlLoopEnable);
}
static int16 GetBTBCurrCtrl(void)
{
    return(stBTCurrCtrl[PVB].stIn.uwCtrlLoopEnable);
}
////////////////
static int16 GetOutCurr_d(void)
{
     return(stInvCurrCtrl.stOut.wOutCurr_d);
}
static int16 GetUout_d(void)
{	
	return(stPwmPara.stIn.wUout_d );
}
static int16 GetForwardVolt_d(void)
{
  return( stInvVoltCtrl.stOut.wForwardVolt_d);
    
}
static int16 GetOutRInvCtrl(void)
{
  return( stPwmPara.stOut.wRInvCtrl);
    
}
static int16 GetInRInvCtrl(void)
{
  return( stPwmCalc.stIn.wRInvOut);
    
}
static int16 GetEPWM1CMPA(void)
{
    return( EPwm1Regs.CMPA.bit.CMPA);
}
static int16 GetEPWM2CMPA(void)
{
    return( EPwm2Regs.CMPA.bit.CMPA);
}

static int16 GetRPWMDutyP(void)
{
	return(stPwmCalc.stOut.uwRPWMDutyP);
}
static int16 GetRPWMDutyN(void)
{
   // return( stDebug.ReadData.wDebug8);
   return( stPwmCalc.stOut.uwRPWMDutyN );
}
static int16 GetwOutCurr_d(void)
{
    return( stInvCurrCtrl.stOut.wOutCurr_d);
}
static int16 GetwOutCurr_q(void)
{
    return( stInvCurrCtrl.stOut.wOutCurr_q);
}
static int16 GetEPWM1TBPRD(void)
{
    return( EPwm1Regs.TBPRD );
}

int16 NULLArithVal = 0;
const Uint32 *pArithValArray[CHANNELNUMMAX]=
{
    //0-3
    &NULLArithVal,
    &(stAdcPool.RGridVolt.wArithVal),
    &(stAdcPool.SGridVolt.wArithVal),
    &(stAdcPool.TGridVolt.wArithVal),

    //4-6
    &(stAdcPool.RSGridVolt.wArithVal),
    &(stAdcPool.STGridVolt.wArithVal),
    &(stAdcPool.TRGridVolt.wArithVal),

    //7-9
    &(stAdcPool.RInvCurr.wArithVal),
    &(stAdcPool.SInvCurr.wArithVal),
    &(stAdcPool.TInvCurr.wArithVal),

    //10-12
    &(stAdcPool.RHardDCI.wArithVal),
    &(stAdcPool.SHardDCI.wArithVal),
    &NULLArithVal,

    //13-15
    &(stAdcPool.PBusVolt.wArithVal),
    &(stAdcPool.NBusVolt.wArithVal),
    &(stAdcPool.BusVolt.wArithVal),

    //16-18
    &(stAdcPool.PVVolt[PVA].wArithVal),
    &(stAdcPool.PVVolt[PVB].wArithVal),
    &(stAdcPool.PVVolt[PVC].wArithVal),

    //19-21
    &(stAdcPool.PVVolt[PVD].wArithVal),
    &NULLArithVal,
    &NULLArithVal,

    //22-24
    &NULLArithVal,
    &NULLArithVal,
    &NULLArithVal,

    //25-27
    &NULLArithVal,
    &NULLArithVal,
    &NULLArithVal,

    //28-30
    &(stAdcPool.BTCurr[PVA].wArithVal),
    &(stAdcPool.BTCurr[PVB].wArithVal),
    &(stAdcPool.BTCurr[PVC].wArithVal),

    //31-33
    &(stAdcPool.BTCurr[PVD].wArithVal),
    &NULLArithVal,
    &NULLArithVal,

    //34-36
    &NULLArithVal,
    &NULLArithVal,
    &NULLArithVal,

    //37-39
    &NULLArithVal,
    &NULLArithVal,
    &NULLArithVal,

};

typedef		int16  (*pFunc1)(void);
const pFunc1	GetWaveDataSubArray[100] =
{
    swReturnNull,

	GetBusVolt1ms,		GetBusVoltRef,		GetBTADuty,		//3
	GetBTBDuty,			GetPVACurr,			GetPVBCurr,		//6
	GetPVCCurr,			GetPVDCurr,			GetEPWM1TBPRD,	//9

	GetPBusVolt,			GetNBusVolt,			GetBusVolt,		//12
	GetPVACurrReal,		GetPVBCurrReal,		GetPVCCurrReal,		//15
	GetPVDCurrReal,		GetPVACurrRef,		GetPVBCurrRef,		//18

	GetInvCurrdRef,		GetInvCurrdReal,	GetTInvVolt,	//21
	GetBTACurrCtrl,		GetRVolt,			GetTVolt,		//24
	GetVolt_d,			GetVolt_q,	    		GetTCurr,		//27

	GetPVBVolt,	   		 GetRPWMDutyP,    	GetRPWMDutyN,		//30
	GetGridPrdCnt,		GetGridPrdTim,		GetActGridPrdCnt,		//33
	GetSinwt,			GetSinwt240,		GetSinwt120,		//36
	GetCoswt,	   	 	GetCoswt240,     		GetCoswt120,		//39
	GetActSinwt,			GetActSinwt240,		GetActSinwt120,		//42
	GetActCoswt,	   	GetActCoswt240,    	 GetRInvVolt,		//45
#if SAMPLE_DEBUG
	GetSin0Tmp,			GetSin120Tmp,		GetSin240Tmp,
#else
	GetPVAVoltRef,		GetPVBVoltRef,		GetPVCVoltRef,		//48
#endif
	GetPVDVoltRef,		GetPVACurrRef,		GetPVBCurrRef,		//51
	GetPVCCurrRef,		GetPVDCurrRef,	    	GetTVolt,		//54

	GetInvTemp,	    	GetNBusVolt,	    	GetTVolt,		//57
	GetRPWMDutyN,	    	GetEPWM1CMPA,		GetEPWM2CMPA,		//60
	GetInRInvCtrl,	    	GetOutRInvCtrl,     	GetForwardVolt_d,		//63
	GetUout_d,	    		GetOutCurr_d,	    	GetRVolt,		//66
	GetInvCurrdRef,		GetCurr_d,		GetCurr_d,		//69
	GetIntrFAN4,       	GetIntGFCI,	    	GetTVolt,  	//72
	swReturnNull, 	    	GetNBusVolt,	    	GetTVolt,		//75
	GetCurr_d,			GetCurr_q,		GetTVolt,		//78
	swReturnNull,		GetNBusVolt,		GetTVolt,		//81
	swReturnNull,	   	 GetNBusVolt,	    GetTVolt,		//84
	swReturnNull,		GetNBusVolt,		GetTVolt,		//87
	swReturnNull,		GetNBusVolt,		GetTVolt,		//90
	swReturnNull,		GetNBusVolt,		GetTVolt,		//93
	swReturnNull, 		GetNBusVolt,		GetTVolt,		//96
	swReturnNull, 		GetNBusVolt,		GetTVolt		//99
};

void InitDebugData(void)
{
	stDebug.ReadData.uwWaitStatus = 0;
	stDebug.ReadData.uwWaitError = 0;

	stDebug.ReadData.uwWaitStartMsgStatus = 0;
	stDebug.ReadData.uwWaitStartError = 0;
	stDebug.ReadData.uwRelayCheckStep = 0;
	stDebug.ReadData.uwRelayCheckStatus = 0;

	stDebug.ReadData.uwInvStatus = 0;
	stDebug.ReadData.uwInvError = 0;
	stDebug.ReadData.uwFaultStatus = 0;
	stDebug.ReadData.uwDebug2 = 0;
	stDebug.ReadData.uwDebug44 = 0;
	stDebug.ReadData.uwDebug45 = 0;
	stDebug.ReadData.uwDebug46 = 0;
	stDebug.ReadData.uwInvOverCurr = 0;
	stDebug.ReadData.uwBoostOverCurr = 0;
	stDebug.ReadData.uwPowerFail = 0;
	stDebug.ReadData.uwBusOverVolt = 0;
	stDebug.ReadData.uwPllFailCnt = 0;
	stDebug.ReadData.uwPllSuccessedCnt = 0;
	stDebug.ReadData.wACVoltPeakValue = 0;

	stDebug.ReadData.wVolt_dErr = 0;
	stDebug.ReadData.wVolt_qErr = 0;
	stDebug.ReadData.wVolt_dFstDiff = 0;
	stDebug.ReadData.wVolt_qFstDiff = 0;
	stDebug.ReadData.uwIsoCheckCnt = 0;
    stDebug.ReadData.wDebug1 = 0;
    stDebug.ReadData.wDebug2 = 0;
    stDebug.ReadData.wDebug3 = 0;
	stDebug.ReadData.wDebug8 = 0;

	stDebug.SetData.unSetReg.bit.TurnOnOff = OFF;
	stDebug.SetData.unSetReg.bit.NLineEnable = DISABLE;
	stDebug.SetData.unSetReg.bit.InvPwmLoop = OFF;
	stDebug.SetData.unSetReg.bit.BoostPwmLoop = OFF;
	stDebug.SetData.unSetReg.bit.PWMMode = 0;
	stDebug.SetData.unSetReg.bit.OpenLoopUnlock = DISABLE;
	stDebug.SetData.unSetReg.bit.OpenLoopEnable = DISABLE;
	
	stDebug.SetData.uwActivePercent = 10;
	stDebug.SetData.wReacitvePercent = 0;
	stDebug.SetData.uwReactiveModel = FIXED_PF_MODE;
	stDebug.SetData.wReacitvePF = 1000;
	stDebug.SetData.uwDutyOffset = 20;
	stDebug.SetData.uwAngleOffset = 4;
	
	stDebug.SetData.uwIdRef = 16;			// 5/32 = 0.15625A
	stDebug.SetData.wIqRef = 0;				

	stDebug.SetData.uwBusRefMin = 5500;		// 550v
	stDebug.SetData.uwBusRef = 8000;		// 800v

	//Openloop debug para
	stDebug.SetData.uwBtOpenLoopDuty = 1520;

	stDebug.SetData.wBTCurrRef[PVA] = 32;
	stDebug.SetData.wBTCurrRef[PVB] = 32;
	stDebug.SetData.wBTCurrRef[PVC] = 32;
	stDebug.SetData.wBTCurrRef[PVD] = 32;

	stDebug.SetData.wMpptVoltRef[PVA] = 2000;
	stDebug.SetData.wMpptVoltRef[PVB] = 2000;
	stDebug.SetData.wMpptVoltRef[PVC] = 2000;
	stDebug.SetData.wMpptVoltRef[PVD] = 2000;

	stWaveRecord.uwWaveSamRatio = 1;
	stWaveRecord.uwWaveID[0] = 0;
	stWaveRecord.uwWaveID[1] = 1;
	stWaveRecord.uwWaveID[2] = 2;
	stWaveRecord.uwWaveID[3] = 3;
	/************** DCI ***************/
	stDebug.ReadData.wDebug7[0] = 1;
	stDebug.ReadData.wDebug7[1] = 4;
	stDebug.ReadData.wDebug7[2] = 800;
	stDebug.ReadData.wDebug7[3] = 1200;
	/************** DCI ***************/
	stDebug.ReadData.wDebug7[5] = 1;
	stDebug.ReadData.wDebug7[6] = 0;
}

void ComDebugTask(void)
{
	Uint16  uwRxData = 0;
	Uint16 uwCRC16=0;
	Uint16 uwRS485ReadByteMax = 0;
	static Uint16 uwRS485ReadByte=0;
	static Uint16 uwRS485ReadOutTime = 0;
	static Uint16 uwRS485RxIndex;
	static Uint16 uwRS485RxDataLen;

	UartReceDriverInit(RS485_UART4,RS485_RX_SIZE, stRs485Uart4.bRxBufBak);

	while(1)
	{
		uwRS485ReadByte=0;
		while(SCI_RX_READY == uwSciRead(RS485_UART4, &uwRxData))
		{
			switch(uwRS485RxIndex)
			{
				case 0:
					if(uwRxData==DEBUG_ADDRESS)
					{
						stRs485Uart4.bRxBuf[uwRS485RxIndex] = uwRxData;
						uwRS485RxIndex++;
					}
					else
					{
						uwRS485RxIndex = 0;
					}
				break;
				case 1:
					if((uwRxData == 0x03)|| (uwRxData == 0x06) || (uwRxData == 0x10) || (uwRxData == 0x0C))	// Only support modbus 0x03, 0x10
					{
						stRs485Uart4.bRxBuf[uwRS485RxIndex] = uwRxData;
						uwRS485RxIndex++;
					}
					else
					{
						uwRS485RxIndex = 0;
					}
				break;
				default:
					if(stRs485Uart4.bRxBuf[0] == DEBUG_ADDRESS && ((stRs485Uart4.bRxBuf[1] == 0x03)||(stRs485Uart4.bRxBuf[1] == 0x06) || (stRs485Uart4.bRxBuf[1] == 0x10)))	// Only support modbus 0x03, 0x10
					{
						if(stRs485Uart4.bRxBuf[1] == 0x10)
						{
							if(uwRS485RxDataLen>=6)
							{
								uwRS485RxDataLen = (stRs485Uart4.bRxBuf[6]+9);
							}
							else
							{
								uwRS485RxDataLen = 8;
							}
						}
						else
						{
							uwRS485RxDataLen = 8;
						}
						if((uwRS485RxIndex<uwRS485RxDataLen)&&(uwRS485RxDataLen<RS485_RX_SIZE))
						{
							stRs485Uart4.bRxBuf[uwRS485RxIndex] = uwRxData;
							uwRS485RxIndex++;
						}
						else
						{
							uwRS485RxIndex = 0;
						}
						if((uwRS485RxIndex == uwRS485RxDataLen) && (uwRS485RxDataLen > 2))		// avoid bRx485DataLenth = 0, crc calculation over
						{
							uwCRC16 = uwCalcCRC16(stRs485Uart4.bRxBuf, (uwRS485RxDataLen - 2));
							if(((uwCRC16 & 0XFF) == stRs485Uart4.bRxBuf[uwRS485RxDataLen - 2])
							&&((uwCRC16 >> 8) == stRs485Uart4.bRxBuf[uwRS485RxDataLen - 1]))
							{
								uwRS485RxIndex = 0;
								ModbusAnalysis(stRs485Uart4.bRxBuf, uwRS485RxDataLen);
							}
							uwRS485RxIndex = 0;
						}
					}
					else 
					{						
						uwRS485RxIndex = 0;
						UartReceDriverInit(RS485_UART4,RS485_RX_SIZE, stRs485Uart4.bRxBufBak);						
							
					}
				break;
			}
			uwRS485ReadOutTime = 0;
			uwRS485ReadByte++;
			if(stRs485Uart4.bRxBuf[1] == FUNCTION_CODE_10)
			{
				uwRS485ReadByteMax = 60;
			}
			else
			{
				uwRS485ReadByteMax = 12;
			}
			if(uwRS485ReadByte >= uwRS485ReadByteMax)
			{
				uwRS485ReadByte=0;
				OSEventPend();
			}
		}
		if(uwRS485ReadOutTime >= 6)			// 50*6=300ms  no data,clear receive index
		{
			uwRS485RxIndex = 0;
			uwRS485ReadOutTime = 0;
		}
		else
		{
			uwRS485ReadOutTime++;
		}
		FaultWaveRecordTime();
		OSEventPend();
	}
}

/****************************************************************************
*
*	FunctionName:	UART4_RS485Task
*	Input:	none
*	Output:	none
*	Description:	UART4 for RS485
*     modbus 03
*     RX : COM		CMD		ADDR_H		ADDR_L		DATA_H		DATA_L		CRC_H		CRC_L
*     TX : COM		CMD		ADDR_H		DATA_LEN	DATA1_H		DATA1_L		DATA2_H		DATA2_L	 	......		CRC_H		CRC_L
*     modbus 10
*     RX : COM		CMD		ADDR_H		ADDR_L		REG_LEN_H	REG_LEN_L	DATA_BYTE	DATA_H		DATA_L		CRC_H		CRC_L
*     TX : COM		CMD		ADDR_H		ADDR_L		DATA1_H		DATA1_L		DATA2_H		DATA2_L	 	......		CRC_H		CRC_L
*	Note:	none.
***************************************************************/
static void ModbusAnalysis(Uint16* puwRxBuf, Uint16 uwRxBufLen)
{
	static Uint16 suwModbusNotRespon=0,suwModbusErr;
	static Uint16 suwTxDataLenth;

	suwModbusNotRespon = 0;

	stRs485Uart4.bTxBuf[0] = DEBUG_ADDRESS;

	stRs485Uart4.bTxBuf[1] = *(puwRxBuf + 1);

	switch(*(puwRxBuf+1))		// function code
	{
		case 0x03:
			suwModbusErr = uwHoldRegAnalysis(puwRxBuf,stRs485Uart4.bTxBuf);
		break;

		case 0x06:
		case 0x10:
			suwModbusErr = uwSetRegAnalysis(puwRxBuf,stRs485Uart4.bTxBuf);
		break;


		default:
			suwModbusNotRespon = 1;
		break;
	}

	if(suwModbusNotRespon==1)
	{
		suwModbusNotRespon = 0;
		return;
	}
	if(stRs485Uart4.bTxBuf[1]==0x10)
	{
		suwTxDataLenth = 8;
	}	
	else if(stRs485Uart4.bTxBuf[1]==0x0C)			// 0x0C
	{
		suwTxDataLenth = puwRxBuf[16];
		suwTxDataLenth = (suwTxDataLenth<<8) + puwRxBuf[17] ;
		suwTxDataLenth = (suwTxDataLenth*2) + 12+6;
	}
	else
	{
		suwTxDataLenth = stRs485Uart4.bTxBuf[2] + 5;
	}
	if(suwModbusErr  == 0)
	{
		suwTxDataLenth = 5;
		suwModbusErr = 0;
	}

	uwSciWrite(RS485_UART4, stRs485Uart4.bTxBufBak, stRs485Uart4.bTxBuf, suwTxDataLenth);
}


static Uint16 uwHoldRegAnalysis(Uint16* uwRxDataBuf,	Uint16* uwTxDataBuf)
{
	Uint16 uwStartAddress,uwNumberOfReg,uwCRC1Tmp;

	uwStartAddress = *(uwRxDataBuf+2);
	uwStartAddress = (uwStartAddress<<8)|*(uwRxDataBuf+3);
	uwNumberOfReg  = *(uwRxDataBuf+4);
	uwNumberOfReg  = (uwNumberOfReg<<8)|*(uwRxDataBuf+5);

	if(uwNumberOfReg>125)
	{
		uwTxDataBuf[1]=(0x03|0x80);
		uwTxDataBuf[2]=0x02;					// Illegal data address
		uwCRC1Tmp = uwCalcCRC16(uwTxDataBuf,3);
		uwTxDataBuf[3] = (uwCRC1Tmp&0x00FF);
		uwTxDataBuf[4] = ((uwCRC1Tmp&0xFF00)>>8);
		return 0;
	}
	else
	{
		uwReadHoldingReg(uwStartAddress,uwNumberOfReg,uwTxDataBuf);
		return 1;
	}
}

static Uint16 uwSetRegAnalysis(Uint16* uwRxDataBuf,	Uint16* puwTxBuf)
{
	Uint16 uwRegStartAddrTmp=0,uwRegNumberTmp=1;
	Uint16 uwCRC16Tmp=0, uwSetRegResultTmp=0;
	Uint16 uwTxBufIndexTmp=0,uwModbusResultTmp=0;

	uwRegStartAddrTmp = ((uwRxDataBuf[2]*256)|uwRxDataBuf[3]);		// start address
	if(0x10==uwRxDataBuf[1])
	{
		uwRegNumberTmp = ((uwRxDataBuf[4]*256)|uwRxDataBuf[5]);		// regrster number
	}
	else
	{
		uwRegNumberTmp = 1;
	}
	uwTxBufIndexTmp = 1;

	// set regeister maxnumber is 32
	if(((uwRegNumberTmp>32)|| (uwRegStartAddrTmp<0x3000))	// max set register number : 32  regiseter_num*2 = byte_number
	|| ((0x10==uwRxDataBuf[1]) && ((uwRegNumberTmp*2) != uwRxDataBuf[6])))
	{
		puwTxBuf[uwTxBufIndexTmp++] = (0x80 | 0x10);
		puwTxBuf[uwTxBufIndexTmp++] = 0x02;									// Illegal data address
		uwModbusResultTmp = 0;
	}
	else
	{
		uwSetRegResultTmp = uwSetMultipleReg(uwRegStartAddrTmp,uwRegNumberTmp,uwRxDataBuf);
		if(1==uwSetRegResultTmp)
		{
			puwTxBuf[uwTxBufIndexTmp++] = uwRxDataBuf[1];
			puwTxBuf[uwTxBufIndexTmp++] = uwRxDataBuf[2];
			puwTxBuf[uwTxBufIndexTmp++] = uwRxDataBuf[3];
			puwTxBuf[uwTxBufIndexTmp++] = uwRxDataBuf[4];
			puwTxBuf[uwTxBufIndexTmp++] = uwRxDataBuf[5];
			uwModbusResultTmp = 1;
		}
		else
		{
			puwTxBuf[1] = (0x80 | 0x10);
			puwTxBuf[2] = 0x0A;						// Illegal data
			uwModbusResultTmp = 0;
		}
	}
	uwCRC16Tmp = uwCalcCRC16(puwTxBuf,uwTxBufIndexTmp);
	puwTxBuf[uwTxBufIndexTmp++] = (uwCRC16Tmp&0x00FF);
	puwTxBuf[uwTxBufIndexTmp++] = ((uwCRC16Tmp&0xFF00)>>8);
	return uwModbusResultTmp;
}

Uint16 ReadWaveData(Uint16 uwReadIndex_s,Uint16 uwWavePointCnt_s)
{
	Uint16 uwWavePointTmp;

	uwWavePointTmp = stWaveRecord.uwReadWavePoint + uwWavePointCnt_s;
	if(uwWavePointTmp >= WAVE_POINTNUM_MAX)
	{
		uwWavePointTmp = uwWavePointTmp-WAVE_POINTNUM_MAX;
	}
	else if(uwWavePointTmp >= WAVE_POINTNUM_MAX*2)
	{
		uwWavePointTmp = uwWavePointTmp-(WAVE_POINTNUM_MAX*2);
	}
	return(stWaveRecord.uwWaveDataBuffer[uwReadIndex_s][uwWavePointTmp]);
}

int16 ChangeWaveDataRate(Uint16 uwChannelNum, int16 wVaule)
{
    int32 wRateValue = (int32)wVaule * 10;

    //uwChannelNum : 0-39
    switch(uwChannelNum)
    {
        case 0:
            wRateValue = 0;
            break;

        //R S T Phase Voltage
        case 1:
        case 2:
        case 3:

        //Line Voltage
        case 4:
        case 5:
        case 6:

        //Phase Current
        case 7:
        case 8:
        case 9:
            wRateValue = wRateValue / 32;
            break;

        //Hard DCI
        case 10:
        case 11:
        case 12:
            wRateValue = wRateValue;
            break;

        //BUS Volt
        case 13:
        case 14:
        case 15:
            wRateValue = wRateValue /10;
            break;

        //PV Volt
        case 16:
        case 17:
        case 18:
        case 19:
            wRateValue = wRateValue / 10;
            break;

        //PV Current
        case 28:
        case 29:
        case 30:
        case 31:
            wRateValue = wRateValue / 32;
            break;

        default:
            wRateValue = wRateValue /32;
            break;
    }
}

Uint16 GetChannelNum(Uint16 uwWaveBuffIndex)
{
    Uint16 ChannelNum = 0;
    switch(uwWaveBuffIndex)
    {
        case 0:
            ChannelNum = stF107Data.uwFaultWaveChannelNum1;
            break;
        case 1:
            ChannelNum = stF107Data.uwFaultWaveChannelNum2;
            break;
        case 2:
            ChannelNum = stF107Data.uwFaultWaveChannelNum3;
            break;
        case 3:
            ChannelNum = stF107Data.uwFaultWaveChannelNum4;
            break;
        case 4:
            ChannelNum = stF107Data.uwFaultWaveChannelNum5;
            break;
        case 5:
            ChannelNum = stF107Data.uwFaultWaveChannelNum6;
            break;
        case 6:
            ChannelNum = stF107Data.uwFaultWaveChannelNum7;
            break;
        case 7:
            ChannelNum = stF107Data.uwFaultWaveChannelNum8;
            break;

        default:
            ChannelNum = stF107Data.uwFaultWaveChannelNum1;
            break;
    }

    return ChannelNum;
}

void RecordingWave(void)
{
	static Uint16 suwFaultWaveRecordCnt=0;
    Uint16 uwPoint = 0;
    // if(0 == stF107Data.uwRecordFaultEnable)
    // {
    //     return;
    // }
	if(DISABLE == stWaveRecord.uwRecordTrigger)
	{
		if(stSysFaultReg.uwFaultCode != 0)
		{
		    if(stSysFaultReg.uwFaultCode != 37)
            {
                if(stWaveRecord.uwFaultCodeBkp != stSysFaultReg.uwFaultCode)
                {
                    stWaveRecord.uwRecordTrigger = ENABLE;
                    suwFaultWaveRecordCnt = 0;
                    stWaveRecord.uwRecordSaveDelayCnt = 0;
                    stWaveRecord.uwWaveFrame = 0xB1;
                    stWaveRecord.uwSendPointIndex = 0;
                    stWaveRecord.uwWaveChannelIndex = 1;
                }
            }

		}
        stWaveRecord.uwFaultCodeBkp = stSysFaultReg.uwFaultCode;

        if(stF107Data.uwRecordHandEnable == 1)
        {
            stWaveRecord.uwRecordTrigger = ENABLE;
            suwFaultWaveRecordCnt = 0;
            stWaveRecord.uwRecordSaveDelayCnt = 0;
            stWaveRecord.uwWaveFrame = 0xB1;
            stWaveRecord.uwSendPointIndex = 0;
            stWaveRecord.uwWaveChannelIndex = 1;
        }
	}
	else
	{
	    #if(IPMOS_DEBUG == 1)
		if((stWaveRecord.uwFaultRecordWaveReCnt >= FAULT_RECORD_RECOVER_TIME)
		&&(0 == stWaveRecord.unFaultRecordReasonBak.all))
		{
			stWaveRecord.uwRecordTrigger = DISABLE;
		}
        #endif
	}

	if(ENABLE == stWaveRecord.uwRecordTrigger)
	{
		suwFaultWaveRecordCnt++;
		if(suwFaultWaveRecordCnt >= stWaveRecord.uwWaveSamRatio)
		{
			suwFaultWaveRecordCnt = 0;
			stWaveRecord.uwRecordSaveDelayCnt++;
			if(stWaveRecord.uwRecordSaveDelayCnt>=WAVE_RECORD_DELAY_POINT_NUM)
			{
				stWaveRecord.uwRecordSaveDelayCnt=WAVE_RECORD_DELAY_POINT_NUM;
                stWaveRecord.uwSendStart = ENABLE;
				stWaveRecord.uwWaveRecordStopFlag = ENABLE;
			}
		}
	}
    #if(IPMOS_DEBUG == 1)
	else if(ENABLE == stWaveRecord.uwReadWaveTrigger)		// 收到实时录波指令
	{
		if(0 == stWaveRecord.uwReadWaveWaitCnt)				// 需要读取的点数
		{
			stWaveRecord.uwReadWaveWaitCnt = 0;
			stWaveRecord.uwWaveRecordStopFlag = ENABLE;		// 读取的点数已经足够
		}
		else
		{
			stWaveRecord.uwReadWaveWaitCnt--;
		}
	}
    #endif
	else
	{
	    #if(IPMOS_DEBUG == 1)
		suwFaultWaveRecordCnt = 0;
		stWaveRecord.uwRecordSaveDelayCnt = 0;
        #endif
		stWaveRecord.uwWaveRecordStopFlag = DISABLE;
	}


	if(DISABLE == stWaveRecord.uwWaveRecordStopFlag)
	{
	    #if(IPMOS_DEBUG == 1)
		stWaveRecord.uwWaveRecordCnt++;
		if(stWaveRecord.uwWaveRecordCnt >= stWaveRecord.uwWaveSamRatio)
        #endif
		{
			stWaveRecord.uwWavePoint++;
			if(stWaveRecord.uwWavePoint >= WAVE_POINTNUM_MAX)
			{
				stWaveRecord.uwWavePoint = 0;
			}
            uwPoint = stWaveRecord.uwWavePoint;

			#if(IPMOS_DEBUG == 0)
            stWaveRecord.uwWaveDataBuffer[0][uwPoint] = *(stWaveRecord.pChannelAddress1);
            stWaveRecord.uwWaveDataBuffer[1][uwPoint] = *(stWaveRecord.pChannelAddress2);
            stWaveRecord.uwWaveDataBuffer[2][uwPoint] = *(stWaveRecord.pChannelAddress3);
            stWaveRecord.uwWaveDataBuffer[3][uwPoint] = *(stWaveRecord.pChannelAddress4);

            stWaveRecord.uwWaveDataBuffer[4][uwPoint] = *(stWaveRecord.pChannelAddress5);
            stWaveRecord.uwWaveDataBuffer[5][uwPoint] = *(stWaveRecord.pChannelAddress6);
            stWaveRecord.uwWaveDataBuffer[6][uwPoint] = *(stWaveRecord.pChannelAddress7);
            stWaveRecord.uwWaveDataBuffer[7][uwPoint] = *(stWaveRecord.pChannelAddress8);

            #else
			stWaveRecord.uwWaveRecordCnt = 0;
			stWaveRecord.uwWaveDataBuffer[0][stWaveRecord.uwWavePoint] = (Uint16)GetWaveDataSubArray[stWaveRecord.uwWaveID[0]]();
			stWaveRecord.uwWaveDataBuffer[1][stWaveRecord.uwWavePoint] = (Uint16)GetWaveDataSubArray[stWaveRecord.uwWaveID[1]]();
			stWaveRecord.uwWaveDataBuffer[2][stWaveRecord.uwWavePoint] = (Uint16)GetWaveDataSubArray[stWaveRecord.uwWaveID[2]]();
			stWaveRecord.uwWaveDataBuffer[3][stWaveRecord.uwWavePoint] = (Uint16)GetWaveDataSubArray[stWaveRecord.uwWaveID[3]]();
            #endif
		}
	}
	else
	{
		stWaveRecord.uwReadWavePoint = stWaveRecord.uwWavePoint+1;
		if(stWaveRecord.uwReadWavePoint >= WAVE_POINTNUM_MAX)
		{
			stWaveRecord.uwReadWavePoint = 0;
		}
	}
}

static void FaultWaveRecordTime(void)
{
	if(ENABLE == stWaveRecord.uwRecordTrigger)
	{
		stWaveRecord.uwFaultRecordWaveReCnt++;
		if(stWaveRecord.uwFaultRecordWaveReCnt >= FAULT_RECORD_RECOVER_TIME)	//1 minutes
		{
			stWaveRecord.uwFaultRecordWaveReCnt = FAULT_RECORD_RECOVER_TIME;
		}
	}
	else
	{
		stWaveRecord.uwFaultRecordWaveReCnt = 0;
	}
}

