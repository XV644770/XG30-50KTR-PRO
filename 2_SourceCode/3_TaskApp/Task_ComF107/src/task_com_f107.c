/*
 * task_sci_stm32.c
 *
 *  Created on: 2020年11月26日
 *      Author: Administrator
 */
#include "dsp_user_include.h"

#pragma DATA_SECTION(stDspUart, "COMBuffer")
ST_DSP_UART stDspUart;

ST_F107_DATA stF107Data;

int16	wAdjRatioDecide(Uint16	uwF107AdjRatio)
{
	int16	wAdjRatioTmp;

	if(uwF107AdjRatio < ADJ_RATIO_MIN)		// adjust ratio < 0.75
	{
		wAdjRatioTmp = ADJ_RATIO_MIN;		// adjust ratio = 0.75
	}
	else if(uwF107AdjRatio > ADJ_RATIO_MAX)	// adjust ratio > 1.25
	{
		wAdjRatioTmp = ADJ_RATIO_MAX;		// adjust ratio = 1.25
	}
	else
	{
		wAdjRatioTmp = uwF107AdjRatio;
	}

	return wAdjRatioTmp;
}

static void GridProtectValueCheck(void)
{
	Uint16 uwFreqPointDerateTmp=0,uwFreqTimeDerateTmp=0;
	Uint16 uwVoltPointDerateTmp=0,uwVoltTimeDerateTmp=0;

	if(FREQ_NULL == stFreqRang.uwFreqFlag)
	{
		return;
	}
	/******************check frequency range protect value  ********/
	if(STRANDARD_TAIWAI != stDspReceData.unSafetyOdm.bit.SafetyStandard)
	{
		if(FREQ_60HZ == stFreqRang.uwFreqFlag)		// 60Hz System
		{
			if((abs(stF107Data.uwACOverFreq1-6000)>500) || (abs(stF107Data.uwACOverFreq2-6000)>500))
			{
				stF107Data.uwACOverFreq1 = 6250;
				stF107Data.uwACOverFreq2 = 6250;
				stF107Data.uwACStartOverFreq = 6250;
			}

			if((abs(stF107Data.uwACUnderFreq1-6000)>500) || (abs(stF107Data.uwACUnderFreq2-6000)>500))
			{
				stF107Data.uwACUnderFreq1 = 5750;
				stF107Data.uwACUnderFreq2 = 5750;
				stF107Data.uwACStartUnderFreq = 5750;
			}
		}
		else	// 50Hz
		{
			if((abs(stF107Data.uwACOverFreq1-5000)>500) || (abs(stF107Data.uwACOverFreq2-5000)>500))
			{
				stF107Data.uwACOverFreq1 = 5250;
				stF107Data.uwACOverFreq2 = 5250;
				stF107Data.uwACStartOverFreq = 5250;
			}

			if((abs(stF107Data.uwACUnderFreq1-5000)>500) || (abs(stF107Data.uwACUnderFreq2-5000)>500))
			{
				stF107Data.uwACUnderFreq1 = 4750;
				stF107Data.uwACUnderFreq2 = 4750;
				stF107Data.uwACStartUnderFreq = 4750;
			}
		}
	}

	/******************init frequency range protect value  ********/
	if(STRANDARD_CQC_2018==stDspReceData.unSafetyOdm.bit.SafetyStandard)
	{
		uwFreqPointDerateTmp = 3;	// 0.03Hz
		uwFreqTimeDerateTmp = 2;	// 2 Grid Period
	}
	else
	{
		uwFreqPointDerateTmp = 2;	// 0.03Hz
		uwFreqTimeDerateTmp = 2;	// 2 Grid Period
	}

	stGridProtect.uwACUnderFreq1 = stF107Data.uwACUnderFreq1+uwFreqPointDerateTmp;
	stGridProtect.uwACUnderFreq2 = stF107Data.uwACUnderFreq2+uwFreqPointDerateTmp;
	stGridProtect.uwACStartUnderFreq = stF107Data.uwACStartUnderFreq+uwFreqPointDerateTmp;
	stGridProtect.uwACOverFreq1 = stF107Data.uwACOverFreq1-uwFreqPointDerateTmp;
	stGridProtect.uwACOverFreq2 = stF107Data.uwACOverFreq2-uwFreqPointDerateTmp;
	stGridProtect.uwACStartOverFreq = stF107Data.uwACStartOverFreq-uwFreqPointDerateTmp;

	stGridProtect.uwACUnderFreq1Time = GRID_PROTECT_LIMIT(stF107Data.uwACUnderFreq1Time,uwFreqTimeDerateTmp);
	stGridProtect.uwACUnderFreq2Time = GRID_PROTECT_LIMIT(stF107Data.uwACUnderFreq2Time,uwFreqTimeDerateTmp);
	stGridProtect.uwACOverFreq1Time = GRID_PROTECT_LIMIT(stF107Data.uwACOverFreq1Time,uwFreqTimeDerateTmp);
	stGridProtect.uwACOverFreq2Time = GRID_PROTECT_LIMIT(stF107Data.uwACOverFreq2Time,uwFreqTimeDerateTmp);

	/******************init voltage range protect value  ********/
	if(STRANDARD_CQC_2018==stDspReceData.unSafetyOdm.bit.SafetyStandard)
	{
		uwVoltPointDerateTmp = 30;	// 3V
		uwVoltTimeDerateTmp = 2;	// 2 Grid Period
	}
    else if(STRANDARD_TAIWAI==stDspReceData.unSafetyOdm.bit.SafetyStandard)
    {
        uwVoltPointDerateTmp = 30;   // 4V
        uwVoltTimeDerateTmp = 2;    // 2 Grid Period
    }
	else
	{
		uwVoltPointDerateTmp = 30;	// 3V
		uwVoltTimeDerateTmp = 2;	// 2 Grid Period
	}

	stGridProtect.uwACStartUnderVolt = stF107Data.uwACStartUnderVolt+uwVoltPointDerateTmp;
	stGridProtect.uwACUnderVolt1 = stF107Data.uwACUnderVolt1+uwVoltPointDerateTmp;
	stGridProtect.uwACUnderVolt2 = stF107Data.uwACUnderVolt2+uwVoltPointDerateTmp;
    stGridProtect.uwACUnderVolt3 = stF107Data.uwACUnderVolt3;

	stGridProtect.uwACStartOverVolt = GRID_PROTECT_LIMIT(stF107Data.uwACStartOverVolt,uwVoltPointDerateTmp);
	if(STRANDARD_TAIWAI==stDspReceData.unSafetyOdm.bit.SafetyStandard)
    {
	    stGridProtect.uwACOverVolt1 = stF107Data.uwACOverVolt1 + uwVoltPointDerateTmp;
        stGridProtect.uwACOverVolt2 = stF107Data.uwACOverVolt2;
    }
	else
	{
	    stGridProtect.uwACOverVolt1 = GRID_PROTECT_LIMIT(stF107Data.uwACOverVolt1,uwVoltPointDerateTmp);
	    stGridProtect.uwACOverVolt2 = GRID_PROTECT_LIMIT(stF107Data.uwACOverVolt2,uwVoltPointDerateTmp);
	}

	stGridProtect.uwAC10MinOverVolt = GRID_PROTECT_LIMIT(stF107Data.uwAC10MinOverVolt,uwVoltPointDerateTmp);

	stGridProtect.uwACUnderVolt1Time = GRID_PROTECT_LIMIT(stF107Data.uwACUnderVolt1Time,uwVoltTimeDerateTmp);
	stGridProtect.uwACOverVolt1Time = GRID_PROTECT_LIMIT(stF107Data.uwACOverVolt1Time,uwVoltTimeDerateTmp);
	stGridProtect.uwACUnderVolt2Time = GRID_PROTECT_LIMIT(stF107Data.uwACUnderVolt2Time,uwVoltTimeDerateTmp);
	stGridProtect.uwACOverVolt2Time = GRID_PROTECT_LIMIT(stF107Data.uwACOverVolt2Time,uwVoltTimeDerateTmp);
    stGridProtect.uwACUnderVolt3Time = GRID_PROTECT_LIMIT(stF107Data.uwACUnderVolt3Time,uwVoltTimeDerateTmp);
}

/****************************************************************************
*
*	FunctionName:	ComF107Task
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate
*	Note:	none.
***************************************************************/
void ComF107ParaInit(void)
{
    memset((void*)&stDspUart,0,sizeof(stDspUart));
    memset((void*)&stF107Data,0,sizeof(stF107Data));

    /*****************F107 TX Frame0*******************/
    stF107Data.uwPFCtrlMode = 0;
    stF107Data.wCosPhi = 1000;			// PF = 1
    stF107Data.uwStartLoadSpeed = 500;

	stF107Data.unEnable.bit.LoadingSlope = 1;			// Loading Slope Enable
	stF107Data.unEnable.bit.ACHighVoltDerating = 1;		// AC Voltage High->Derating
	stF107Data.unEnable.bit.NEVoltCheck = 0;			// NE Voltage Check
	stF107Data.unEnable.bit.LVRT = 0;					// low voltage through

	stF107Data.unEnable.bit.ISLAND = 1;					// island protect enable
	stF107Data.unEnable.bit.Freq50_60Hz = 0;			// 0:50Hz 1:60Hz
	stF107Data.unEnable.bit.OverFreqDerate = 0;			// Over Freqency derating
	stF107Data.unEnable.bit.UnderFreqLoad = 0;			// Under Freqency loading
	stF107Data.unEnable.bit.OverLoad = 0 ;

	stF107Data.unSet.bit.TrunOnOff = 0;
	stF107Data.unSet.bit.FlashReady = 0;
	stF107Data.unSet.bit.FanSelfCheckCmd = 0;
	stF107Data.uwRestartLoadSpeed = 500;
	stF107Data.uwShutDownSpeed = 0;

	/*****************F107 TX Frame1**********20210904Revised*********/
	stF107Data.wRSVoltAdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wSTVoltAdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wTRVoltAdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wRNCurr30AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wSNCurr30AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wTNCurr30AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wRNCurr70AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wSNCurr70AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wTNCurr70AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wActPower30AdjRatio = ADJ_RATIO_DEFAULT;
	stF107Data.wActPower70AdjRatio = ADJ_RATIO_DEFAULT;

	/*****************F107 TX Frame2*******************/
    stF107Data.unSafetyOdm.bit.SafetyStandard = 0;
    stF107Data.unSafetyOdm.bit.Manufacturer = 0;

    stF107Data.stProtectEnvrmt.unProtectBit.all = 0x0f;
    stF107Data.stProtectEnvrmt.unEnvrmtBit.all = 0x01;	// 0:220V	1:400V	2:480V

    stF107Data.unFuncEnable.bit.ExportLimit = 0;
    stF107Data.unFuncEnable.bit.StringMonitor = 0;
    stF107Data.unFuncEnable.bit.PID = 0;
    stF107Data.unFuncEnable.bit.DC_SPD = 0;
	stF107Data.unFuncEnable.bit.AC_SPD = 0;
	stF107Data.unFuncEnable.bit.MultiExportLimit = 0;

	stF107Data.uwSettingPower = 600;			// 600*0.1kw = 60kw
	stF107Data.uwMpptModeSet = INDEPENDENT;				// PV Input:MPPT Mode
	stF107Data.uwAdjustMode = 0;		// Adjust mode is 0

	stF107Data.uwPVStartUnderVolt = 2500;
	stF107Data.uwACStartOverFreq = 5350;
	stF107Data.uwACStartUnderFreq = 4750;
	stF107Data.uwStartDelayTime = 60;
	stF107Data.uwReStartDelayTime = 60;

	stF107Data.wACVoltLowThrough = 1930;//193V
	stF107Data.wACVoltHighThrough = 2620;//262V
	stF107Data.wACVoltLowThroughRecover = 2100;//210V
	stF107Data.wACVoltHighThroughRecover = 2450;//245V
    stDspReceData.wGFCIRmsFaultDerate = 0;

    stF107Data.swP_PFLineVoltPercentStart = 1040;//193V
    stF107Data.swP_PFLineVoltPercentRecover = 1030;//262V
    stF107Data.swP_PFActPowerPercentStart = 500;//210V

	stF107Data.uwACUnderVolt1Time = 1;
	stF107Data.uwACOverVolt1Time = 1;
	stF107Data.uwACUnderVolt2Time = 1;
	stF107Data.uwACOverVolt2Time = 1;
    stF107Data.uwACUnderVolt3Time = 1;

	/*****************F107 TX Frame3*******************/
	stF107Data.uwACUnderFreq1 = 4750;
	stF107Data.uwACUnderFreq2 = 4750;
	stF107Data.uwACUnderFreq1Time = 1;
	stF107Data.uwACUnderFreq2Time = 1;

	stF107Data.uwACOverFreq1 = 5350;
	stF107Data.uwACOverFreq2 = 5350;
	stF107Data.uwACOverFreq1Time = 1;
	stF107Data.uwACOverFreq2Time = 1;

	stF107Data.uwDCIOverCurrTime = 50;  //20ms * 50 = 1000ms

	//AFCI
    stF107Data.uwArcFaultLimit = 120;
    stF107Data.uwAfciSetCmd = 0;
    stF107Data.uwAFCI_StartFreq = 30;
    stF107Data.uwAFCI_EndFreq = 60;
    stF107Data.uwAFCI_MagRatio = 23;
    stF107Data.uwAFCI_FreqRatio = 40;
    stF107Data.uwAFCI_BinRatio = 8;
    stF107Data.uwAFCI_MagThld_PowerOn_H = 8;

    stF107Data.uwAFCICMD = CLR_MODEL_Auto;
    stF107Data.usAFCI_AuthenFlag = 0;
    stF107Data.uwAFCI_Set_FaultCntTime_S = 30;
    stF107Data.uwSelfCheckEnable_New = 0;
    stF107Data.uwAFCI_ManualResetFlag = 0;

    stF107Data.unF107FaultBit.all = 0;
    stF107Data.unF107WarnBit.all = 0;

    stF107Data.uwActiveRate = 5;
    stF107Data.wReactiveRate = 0;
    stF107Data.wAntiFlowMeterPower = 0;
    stF107Data.wMultiAntiFlowRate = 0;

    stF107Data.uwACStartOverVolt = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;	// 1126/1024 = 1.1
	stF107Data.uwACStartUnderVolt = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;	// 922/1024 = 0.9

	stF107Data.uwACOverVolt1 = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;
	stF107Data.uwACOverVolt2 = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;

	stF107Data.uwACUnderVolt1 = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;
	stF107Data.uwACUnderVolt2 = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;

	stF107Data.uwAC10MinOverVolt = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;

	stF107Data.uwQvHighVoltEndPoint = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;	// 1126/1024 = 1.1
	stF107Data.uwQvHighVoltStartPoint = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;	// 1126/1024 = 1.1
	stF107Data.uwQvHighVoltReactivePrt = 500;
	stF107Data.uwQvLowVoltStartPoint=((Uint32)stDspReceData.uwACNormalVolt*922)>>10;	// 922/1024 = 0.9
	stF107Data.uwQvLowVoltEndPoint=((Uint32)stDspReceData.uwACNormalVolt*922)>>10;	// 922/1024 = 0.9
	stF107Data.uwQvLowVoltReactivePrt = 500;
	stF107Data.wISOValue = 37;

	/*****************F107 TX Frame4**********20210904Revised*********/
	stF107Data.wPVVoltAdjRatio[PVA] = ADJ_RATIO_DEFAULT;
	stF107Data.wPVVoltAdjRatio[PVB] = ADJ_RATIO_DEFAULT;
	stF107Data.wPVVoltAdjRatio[PVC] = ADJ_RATIO_DEFAULT;
	stF107Data.wPVVoltAdjRatio[PVD] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr30PctAdjRatio[PVA] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr30PctAdjRatio[PVB] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr30PctAdjRatio[PVC] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr30PctAdjRatio[PVD] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr70PctAdjRatio[PVA] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr70PctAdjRatio[PVB] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr70PctAdjRatio[PVC] = ADJ_RATIO_DEFAULT;
	stF107Data.wBTCurr70PctAdjRatio[PVD] = ADJ_RATIO_DEFAULT;
}

/****************************************************************************
*
*	FunctionName:	uwTxF107NormalData
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate,DSP Send F107 data
*	Note:	none.
***************************************************************/
static Uint16 uwTxF107NormalData(Uint16 *puwTxBuf)
{
	Uint16 uwTxDataLenTmp=0;
	static Uint16 suwTxDataIndex=0;
	Uint32 udUartTxDataTmp=0;
	static Uint16    swActivePowerPercentAdj107 = 0;
	static Uint32    dReactivePowerAdj107 = 0;
	static Uint32    dwReactivePower = 0;
	static Uint32    dwReactivePowerSum = 0;
	static Uint32    dwReactivePowerAvg = 0;
	static Uint32    dwReactivePowerCnt = 0;
	
	puwTxBuf[0]=0x55;
	puwTxBuf[1]=suwTxDataIndex;
	puwTxBuf[2] = 0;
	puwTxBuf[3] = 58;

	if(0==suwTxDataIndex)
	{
		puwTxBuf[4] = SOFTWARE_FORMAL;
		puwTxBuf[5] = SOFTWARE_TEST;
		puwTxBuf[6] = stPllPara.stOut.uwPllOk;		// 1:PLL Successed	0:PLL Failed
		puwTxBuf[7] = stFreqRang.uwFreqFlag;		// 1:60Hz	0:50Hz

		udUartTxDataTmp = (Uint32)stACSample.dActivePowerAvg;   //0x381D
		puwTxBuf[8] = ((udUartTxDataTmp&0xFF000000)>>24);
		puwTxBuf[9] = ((udUartTxDataTmp&0x00FF0000)>>16);
		puwTxBuf[10] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[11] = (udUartTxDataTmp&0x000000FF);

		swActivePowerPercentAdj107 = (int16)(stACSample.dActivePowerAvg*1000/stDspReceData.udOutputActivePower);

	    if(swActivePowerPercentAdj107 < 500 && swActivePowerPercentAdj107 > 50)
	    {
	        dReactivePowerAdj107 = 25 * swActivePowerPercentAdj107 + 5000;
	    }
	    else if(swActivePowerPercentAdj107 >= 500)
	    {
	        dReactivePowerAdj107 = 4500;
	    }
	    else
	    {
            dReactivePowerAdj107 = 6000;
	    }

	    if(stACSample.dReactivePower >= dReactivePowerAdj107)
	    {
	        dwReactivePower = (Uint32)stACSample.dReactivePower-dReactivePowerAdj107;    //0x381F
	    }
	    else
	    {
	        dwReactivePower = (Uint32)stACSample.dReactivePower;    //0x381F
	    }

	    dwReactivePowerCnt++;
	    dwReactivePowerSum += dwReactivePower;
	    if(dwReactivePowerCnt == 10)
	    {
	        dwReactivePowerAvg = dwReactivePowerSum/10;
	        dwReactivePowerSum = 0;
	        dwReactivePowerCnt = 0;
	    }
	    if(swActivePowerPercentAdj107 <= 50)
	    {
	        udUartTxDataTmp = dwReactivePower;
	    }
	    else
	    {
	        udUartTxDataTmp = dwReactivePowerAvg;
	    }

		puwTxBuf[12] = ((udUartTxDataTmp&0xFF000000)>>24);
		puwTxBuf[13] = ((udUartTxDataTmp&0x00FF0000)>>16);
		puwTxBuf[14] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[15] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.wPowerFactor;      //0x3821
		puwTxBuf[16] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[17] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stDCSample.uwISOResistance;//ISO Value    //0x3835
		puwTxBuf[18] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[19] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.LineVoltRms.wRS;   //0x3811
		puwTxBuf[20] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[21] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.LineVoltRms.wST;   //0x3812
		puwTxBuf[22] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[23] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.LineVoltRms.wTR;   //0x3813
		puwTxBuf[24] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[25] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.PhaseCurrRms.wRN;  //0x3814
		puwTxBuf[26] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[27] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.PhaseCurrRms.wSN;  //0x3815
		puwTxBuf[28] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[29] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.PhaseCurrRms.wTN;  //0x3816
		puwTxBuf[30] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[31] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wPBusVolt;     //0x3810F
		puwTxBuf[32] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[33] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wNBusVolt;     //0x3810
		puwTxBuf[34] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[35] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.HardDCI.wRN;
		puwTxBuf[36] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[37] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.HardDCI.wSN;
		puwTxBuf[38] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[39] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.HardDCI.wTN;
		puwTxBuf[40] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[41] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.wGfciRms;
		puwTxBuf[42] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[43] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.wGridFreqAvg;
		puwTxBuf[44] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[45] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stSysFaultReg.unHardwareFault.all;
		puwTxBuf[46] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[47] = (udUartTxDataTmp&0x000000FF);

	}
	else if(1==suwTxDataIndex)
	{
		udUartTxDataTmp = stInvPwm.unPwmDisableBit.all;//debug1
		puwTxBuf[4] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[5] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stDebug.ReadData.uwRelayCheckStatus;//debug2
		puwTxBuf[6] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[7] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stInvPwm.uwHardInvOCPTimes;//debug3
		puwTxBuf[8] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[9] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stInvPwm.uwHardBoostOCPTimes;//debug4
		puwTxBuf[10] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[11] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stInvPwm.uwSoftUnderVoltTimes;//debug5
		puwTxBuf[12] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[13] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stInvPwm.uwSoftBoostOCPTimes;//debug6
		puwTxBuf[14] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[15] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stInvPwm.uwSoftInvOCPTimes;//debug7
		puwTxBuf[16] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[17] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stInvPwm.uwSoftVoltqTimes;//debug8
		puwTxBuf[18] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[19] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stFan.unFlagBits[FAN1_EXTR].bit.OnOff;//debug9
		puwTxBuf[20] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[21] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stMachineCfg.wExtrFan123SetFault;//debug10
		puwTxBuf[22] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[23] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stFan.unFlagBits[FAN1_EXTR].bit.OnOff;//debug11
		puwTxBuf[24] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[25] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stF107Data.unEnable.bit.OverLoad;//debug12
		puwTxBuf[26] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[27] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stACSample.wVoltNPERms;//debug13
		puwTxBuf[28] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[29] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = 0;//debug14
		puwTxBuf[30] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[31] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = 0;//debug15
		puwTxBuf[32] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[33] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = 0;//debug16
		puwTxBuf[34] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[35] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stTaskTimer.uwStartDelayTime;
		puwTxBuf[36] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[37] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stDCSample.wInvTemp;
		puwTxBuf[38] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[39] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stDCSample.wAmTemp;
		puwTxBuf[40] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[41] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stDCSample.wBoostTemp;
		puwTxBuf[42] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[43] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stLoadLimit.uwDeratingMode;
		puwTxBuf[44] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[45] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = 0;
		puwTxBuf[46] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[47] = (udUartTxDataTmp&0x000000FF);
	}
	else if(2==suwTxDataIndex)
	{
		udUartTxDataTmp = (Uint16)stACSample.PhaseVoltRms.wRN;  //0x3814
		puwTxBuf[4] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[5] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.PhaseVoltRms.wSN;  //0x3815
		puwTxBuf[6] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[7] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stACSample.PhaseVoltRms.wTN;  //0x3816
		puwTxBuf[8] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[9] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wPVVoltAdj[PVA];      //0x3836
		puwTxBuf[10] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[11] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wPVVoltAdj[PVB];      //0x383A
		puwTxBuf[12] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[13] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wPVVoltAdj[PVC];      //0x383C
		puwTxBuf[14] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[15] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wPVVoltAdj[PVD];      //0x383E
		puwTxBuf[16] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[17] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wBTCurr[PVA];      //0x3837
		puwTxBuf[18] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[19] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wBTCurr[PVB];      //0x3839
		puwTxBuf[20] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[21] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wBTCurr[PVC];      //0x383B
		puwTxBuf[22] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[23] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint16)stDCSample.wBTCurr[PVD];      //0x383D
		puwTxBuf[24] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[25] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint32)stLoadLimit.dActPowerLimitOutput; //0x382A
		puwTxBuf[26] = ((udUartTxDataTmp&0xFF000000)>>24);
		puwTxBuf[27] = ((udUartTxDataTmp&0x00FF0000)>>16);
		puwTxBuf[28] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[29] = (udUartTxDataTmp&0x000000FF);
		puwTxBuf[30] = PLATFORM_CHAR;
		puwTxBuf[31] = DSPSOFTWARE_CHAR;
		puwTxBuf[32] = stF042Para.uwSoftWareChar;
		puwTxBuf[33] = 0x00;		// reserve

		udUartTxDataTmp = stF042Para.uwSoftWareVersion;
		puwTxBuf[34] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[35] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stSysCfg.uwMachineMax;
		puwTxBuf[36] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[37] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stSysCfg.uwMachineMin;
		puwTxBuf[38] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[39] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stDspReceData.uwSettingPower;
		puwTxBuf[40] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[41] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint32)stACSample.dActivePowerAvg;			// Active Power Real Value
		puwTxBuf[42] = ((udUartTxDataTmp&0xFF000000)>>24);
		puwTxBuf[43] = ((udUartTxDataTmp&0x00FF0000)>>16);
		puwTxBuf[44] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[45] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = stSysCfg.uwPVNumber;
		puwTxBuf[46] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[47] = (udUartTxDataTmp&0x000000FF);
	}
	else if(3==suwTxDataIndex)
	{
		udUartTxDataTmp = (Uint32)stSysFaultReg.unFaultBit.all;
		puwTxBuf[4] = ((udUartTxDataTmp&0xFF000000)>>24);
		puwTxBuf[5] = ((udUartTxDataTmp&0x00FF0000)>>16);
		puwTxBuf[6] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[7] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (Uint32)stSysFaultReg.unWarnBit.all;
		puwTxBuf[8] = ((udUartTxDataTmp&0xFF000000)>>24);
		puwTxBuf[9] = ((udUartTxDataTmp&0x00FF0000)>>16);
		puwTxBuf[10] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[11] = (udUartTxDataTmp&0x000000FF);

		/**************DSP Send To Arm MPPT_Num&String_Num******************************/
		puwTxBuf[12] = stSysCfg.uwPVNumber;
		puwTxBuf[13] = STR_NUM_MAX;

		/*******************25-70kW PV-PVD String Current STR1~STR12*******************/
		if(cInverterStatus == eInverterStatus)
		{
			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR0];
			puwTxBuf[14] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[15] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR1];
			puwTxBuf[16] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[17] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR2];
			puwTxBuf[18] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[19] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR3];
			puwTxBuf[20] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[21] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR4];
			puwTxBuf[22] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[23] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR5];
			puwTxBuf[24] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[25] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR6];
			puwTxBuf[26] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[27] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR7];
			puwTxBuf[28] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[29] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR8];
			puwTxBuf[30] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[31] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR9];
			puwTxBuf[32] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[33] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR10];
			puwTxBuf[34] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[35] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = (int16)stDCSample.wBTStrCurr[STR11];
			puwTxBuf[36] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[37] = (udUartTxDataTmp&0x000000FF);
		}
		else 
		{
			udUartTxDataTmp = 0;
			puwTxBuf[14] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[15] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[16] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[17] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[18] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[19] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[20] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[21] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[22] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[23] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp =0;
			puwTxBuf[24] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[25] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[26] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[27] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[28] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[29] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[30] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[31] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[32] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[33] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[34] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[35] = (udUartTxDataTmp&0x000000FF);

			udUartTxDataTmp = 0;
			puwTxBuf[36] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[37] = (udUartTxDataTmp&0x000000FF);
		}

		udUartTxDataTmp = (int16)stACSample.wEfficiency;
		puwTxBuf[38] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[39] = (udUartTxDataTmp&0x000000FF);
		puwTxBuf[40] = 0;   //reserve
		puwTxBuf[41] = 0;   //reserve
		puwTxBuf[42] = 0;   //reserve
		puwTxBuf[43] = 0;   //reserve
		puwTxBuf[44] = 0;   //reserve
		puwTxBuf[45] = 0;   //reserve
		puwTxBuf[46] = 0;   //reserve
		puwTxBuf[47] = 0;   //reserve
	}
	else if(4==suwTxDataIndex)
	{
		/**********************AFCI***************************/
		if(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD)
	    {
			//PV1					
			puwTxBuf[4] = stAfciPara.uwArcCH1;   //Str1 Arc
			puwTxBuf[5] = stAfciPara.uwArcCH1;   //Str2 Arc
			//PV2
			puwTxBuf[6] = stAfciPara.uwArcCH2;   //Str3 Arc
			puwTxBuf[7] = stAfciPara.uwArcCH2;   //Str4 Arc
			//PV3
			puwTxBuf[8] = stAfciPara.uwArcCH3;   //Str5 Arc
			puwTxBuf[9] = stAfciPara.uwArcCH3;   //Str6 Arc
			//PV4
			puwTxBuf[10] = stAfciPara.uwArcCH4;   //Str7 Arc
			puwTxBuf[11] = stAfciPara.uwArcCH4;   //Str8 Arc
			
			/******************PV ArcFault******************/
			puwTxBuf[20] = stAfciPara.uwArcFaultBit;     	 	//---10H
			puwTxBuf[21] = stAfciPara.uwSelfCheckResult;   		//---10L
			
			/******************PV Arc Record****************/
			puwTxBuf[28] = stAfciPara.uwArcRecordCH1;   //Str1 Arc Record
			puwTxBuf[29] = stAfciPara.uwArcRecordCH2;   //Str2 Arc Record
			puwTxBuf[30] = stAfciPara.uwArcRecordCH3;   //Str3 Arc Record
			puwTxBuf[31] = stAfciPara.uwArcRecordCH4;   //Str4 Arc Record
		}
		else
		{
			/******************PV1 Arc*********************/
			puwTxBuf[4] = stAfciPara1.uwArcCH1;   //Str1 Arc
			puwTxBuf[5] = stAfciPara1.uwArcCH2;   //Str2 Arc
			puwTxBuf[6] = stAfciPara1.uwArcCH3;   //Str3 Arc
			puwTxBuf[7] = stAfciPara1.uwArcCH4;   //Str4 Arc
			/******************PV2 Arc*********************/
			puwTxBuf[8] = stAfciPara2.uwArcCH1;   //Str5 Arc
			puwTxBuf[9] = stAfciPara2.uwArcCH2;   //Str6 Arc
			puwTxBuf[10] = stAfciPara2.uwArcCH3;   //Str7 Arc
			puwTxBuf[11] = stAfciPara2.uwArcCH4;   //Str8 Arc
			/******************PV3 Arc*********************/
			puwTxBuf[12] = stAfciPara3.uwArcCH1;   //Str9 Arc
			puwTxBuf[13] = stAfciPara3.uwArcCH2;   //Str10 Arc
			puwTxBuf[14] = stAfciPara3.uwArcCH3;   //Str11 Arc
			puwTxBuf[15] = stAfciPara3.uwArcCH4;   //Str12 Arc
			/******************PV4 Arc*********************/
			puwTxBuf[16] = stAfciPara4.uwArcCH1;   //Str13 Arc
			puwTxBuf[17] = stAfciPara4.uwArcCH2;   //Str14 Arc
			puwTxBuf[18] = stAfciPara4.uwArcCH3;   //Str15 Arc
			puwTxBuf[19] = stAfciPara4.uwArcCH4;   //Str16 Arc

			/******************PV1 ArcFault******************/
			puwTxBuf[20] = stAfciPara1.uwArcFaultBit;     	 	//---10H
			puwTxBuf[21] = stAfciPara1.uwSelfCheckResult;   		//---10L
			/******************PV2 ArcFault******************/
			puwTxBuf[22] = stAfciPara2.uwArcFaultBit;                 //---11H
			puwTxBuf[23] = stAfciPara2.uwSelfCheckResult;   		//---11L
			/******************PV3 ArcFault******************/
			puwTxBuf[24] = stAfciPara3.uwArcFaultBit;   			//---12H
			puwTxBuf[25] = stAfciPara3.uwSelfCheckResult;   		//---12L
			/******************PV4 ArcFault******************/
			puwTxBuf[26] = stAfciPara4.uwArcFaultBit;   			//---13H
			puwTxBuf[27] = stAfciPara4.uwSelfCheckResult;   		//---13L
			/******************PV1 Arc Record****************/
			puwTxBuf[28] = stAfciPara1.uwArcRecordCH1;   //Str1 Arc Record
			puwTxBuf[29] = stAfciPara1.uwArcRecordCH2;   //Str2 Arc Record
			puwTxBuf[30] = stAfciPara1.uwArcRecordCH3;   //Str3 Arc Record
			puwTxBuf[31] = stAfciPara1.uwArcRecordCH4;   //Str4 Arc Record
			/******************PV2 Arc Record****************/
			puwTxBuf[32] = stAfciPara2.uwArcRecordCH1;   //Str5 Arc Record
			puwTxBuf[33] = stAfciPara2.uwArcRecordCH2;   //Str6 Arc Record
			puwTxBuf[34] = stAfciPara2.uwArcRecordCH3;   //Str7 Arc Record
			puwTxBuf[35] = stAfciPara2.uwArcRecordCH4;   //Str8 Arc Record
			/******************PV3 Arc Record****************/
			puwTxBuf[36] = stAfciPara3.uwArcRecordCH1;   //Str9 Arc Record
			puwTxBuf[37] = stAfciPara3.uwArcRecordCH2;   //Str10 Arc Record
			puwTxBuf[38] = stAfciPara3.uwArcRecordCH3;   //Str11 Arc Record
			puwTxBuf[39] = stAfciPara3.uwArcRecordCH4;   //Str12 Arc Record
			/******************PV4 Arc Record****************/
			puwTxBuf[40] = stAfciPara4.uwArcRecordCH1;   //Str13 Arc Record
			puwTxBuf[41] = stAfciPara4.uwArcRecordCH2;   //Str14 Arc Record
			puwTxBuf[42] = stAfciPara4.uwArcRecordCH3;   //Str15 Arc Record
			puwTxBuf[43] = stAfciPara4.uwArcRecordCH4;   //Str16 Arc Record
		}

	}

	else if(5==suwTxDataIndex)
	{
							
		udUartTxDataTmp = (int16)stautotest.uwProcess;
		puwTxBuf[4] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[5] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (int16)stautotest.uwStep;
		puwTxBuf[6] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[7] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (int16)stautotest.uwVFDefaultValue;//Set:LCD 显示设定阈值
		puwTxBuf[8] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[9] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (int16)stautotest.uwDefaultValue;
		puwTxBuf[10] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[11] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (int16)stautotest.uwFaultValue;//Tes:LCD 显示实际保护值
		puwTxBuf[12] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[13] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (int16)(stautotest.uwBreakTime+stautotest.uwBreakTimeAdj);
		puwTxBuf[14] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[15] = (udUartTxDataTmp&0x000000FF);
	

		udUartTxDataTmp = (int16)stautotest.uwStatus;
		puwTxBuf[16] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[17] = (udUartTxDataTmp&0x000000FF);

		udUartTxDataTmp = (int16)stautotest.uwVFValue;//Cut:LCD显示测试值
		puwTxBuf[18] = ((udUartTxDataTmp&0x0000FF00)>>8);
		puwTxBuf[19] = (udUartTxDataTmp&0x000000FF);
		
        udUartTxDataTmp = (int16)stAfciPara.uwAFCI_SelfCheckComp_Flag;
        puwTxBuf[20] = ((udUartTxDataTmp&0x0000FF00)>>8);
        puwTxBuf[21] = (udUartTxDataTmp&0x000000FF);

        udUartTxDataTmp = (int16)stAfciPara.CommFault_ReSetCmdComp_Flag;
        puwTxBuf[22] = ((udUartTxDataTmp&0x0000FF00)>>8);
        puwTxBuf[23] = (udUartTxDataTmp&0x000000FF);
		
		        udUartTxDataTmp = (Uint16)stAfciPara.uwAFCI_FaultCntFlag;
        puwTxBuf[24] = ((udUartTxDataTmp&0x0000FF00)>>8);
        puwTxBuf[25] = (udUartTxDataTmp&0x000000FF);

	}
	
	suwTxDataIndex++;
	if(suwTxDataIndex>=6)
	{
		suwTxDataIndex = 0;
	}
	puwTxBuf[48]=eInverterStatus;
	puwTxBuf[49]=stACSample.uwPowerEnergySum;
	puwTxBuf[50]=stSysFaultReg.uwFaultCode;
	puwTxBuf[51]=stSysFaultReg.uwFaultSubCode;
	puwTxBuf[52]=stSysFaultReg.uwWarnCode;
	puwTxBuf[53]=stSysFaultReg.uwWarnSubCode;
	puwTxBuf[54]=0;	
	puwTxBuf[55]=stIVCurve.CurveAnalyState ;
	puwTxBuf[56]=0;		// rsv
	puwTxBuf[57]=0;		// rsv
	puwTxBuf[58]=0;		// rsv
	puwTxBuf[59]=0;		// rsv
	puwTxBuf[60]=0;		// rsv
	puwTxBuf[61]=0;		// rsv
	uwTxDataLenTmp = 62;

	return uwTxDataLenTmp;
}

void DownloadF107Data(Uint16 *puwRxBuf)
{
	Uint16 uwRxBufIndexTmp;
	Uint16 uwDownload107DataTmp=0;

	uwRxBufIndexTmp = puwRxBuf[1];
	if(0==uwRxBufIndexTmp)
	{
		stF107Data.uwPFCtrlMode = (Uint16)(puwRxBuf[4]*256+puwRxBuf[5]);
		stF107Data.wCosPhi = (Uint16)(puwRxBuf[6]*256+puwRxBuf[7]);
		stF107Data.uwStartLoadSpeed = (Uint16)(puwRxBuf[8]*256+puwRxBuf[9]);
		stF107Data.uwQvHighVoltEndPoint = (Uint16)(puwRxBuf[10]*256+puwRxBuf[11]);
		stF107Data.uwQvHighVoltStartPoint = (Uint16)(puwRxBuf[12]*256+puwRxBuf[13]);
		stF107Data.uwQvHighVoltReactivePrt = (Uint16)(puwRxBuf[14]*256+puwRxBuf[15]);
		stF107Data.uwQvLowVoltStartPoint = (Uint16)(puwRxBuf[16]*256+puwRxBuf[17]);
		stF107Data.uwQvLowVoltEndPoint = (Uint16)(puwRxBuf[18]*256+puwRxBuf[19]);
		stF107Data.uwQvLowVoltReactivePrt = (Uint16)(puwRxBuf[20]*256+puwRxBuf[21]);
		stF107Data.unEnable.all = (Uint16)(puwRxBuf[22]*256+puwRxBuf[23]);
		stF107Data.uwOverFreqDeratePoint = (Uint16)(puwRxBuf[24]*256+puwRxBuf[25]);
		stF107Data.uwFreqPowerChangeRate = (Uint16)(puwRxBuf[26]*256+puwRxBuf[27]);
		stF107Data.uwHighVoltDerateStart = (Uint16)(puwRxBuf[28]*256+puwRxBuf[29]);
		stF107Data.uwHighVoltDerateEnd = (Uint16)(puwRxBuf[30]*256+puwRxBuf[31]);
		stF107Data.uwLowVoltDerateStart = (Uint16)(puwRxBuf[32]*256+puwRxBuf[33]);
		stF107Data.uwLowVoltDerateEnd = (Uint16)(puwRxBuf[34]*256+puwRxBuf[35]);
		stF107Data.uwRestartLoadSpeed = (Uint16)(puwRxBuf[36]*256+puwRxBuf[37]);
		if(STRANDARD_CQC_2018==stDspReceData.unSafetyOdm.bit.SafetyStandard)
		{
			stF107Data.uwShutDownSpeed = (Uint16)(puwRxBuf[38]*256+puwRxBuf[39]);
		}
		else
		{
			stF107Data.uwShutDownSpeed = 0;
		}
		stF107Data.unSet.bit.FanSelfCheckCmd = (puwRxBuf[40]&0x01);
		stF107Data.unSet.bit.ACVoltLevel = (puwRxBuf[41]&0x03);

		stF107Data.uwArcFaultLimit = puwRxBuf[44];
		stF107Data.uwAfciSetCmd = puwRxBuf[45];
		
		stDspUart.uwComReceFrameEnd |= 0x01;
		stautotest.uwEnableFlag = (Uint16)(puwRxBuf[46]*256+puwRxBuf[47]);
	}
	else if(1==uwRxBufIndexTmp)
	{
	    /*****************Add AC Adjust Ratio to advance Sample precision***************/
	    uwDownload107DataTmp = (Uint16)(puwRxBuf[4]*256+puwRxBuf[5]);
	    stF107Data.wRSVoltAdjRatio = wAdjRatioDecide(uwDownload107DataTmp);
	    uwDownload107DataTmp = (Uint16)(puwRxBuf[6]*256+puwRxBuf[7]);
	    stF107Data.wSTVoltAdjRatio = wAdjRatioDecide(uwDownload107DataTmp);


	    uwDownload107DataTmp = (Uint16)(puwRxBuf[8]*256+puwRxBuf[9]);
	    stF107Data.wTRVoltAdjRatio = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[10]*256+puwRxBuf[11]);
	    stF107Data.wRNCurr30AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[12]*256+puwRxBuf[13]);
	    stF107Data.wSNCurr30AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);


	    uwDownload107DataTmp = (Uint16)(puwRxBuf[14]*256+puwRxBuf[15]);
	    stF107Data.wTNCurr30AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);


	    uwDownload107DataTmp = (Uint16)(puwRxBuf[16]*256+puwRxBuf[17]);
	    stF107Data.wRNCurr70AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[18]*256+puwRxBuf[19]);
	    stF107Data.wSNCurr70AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[20]*256+puwRxBuf[21]);
	    stF107Data.wTNCurr70AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[22]*256+puwRxBuf[23]);
	    stF107Data.wActPower30AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);
		stF107Data.wActPower30AdjRatio = ((int32)stF107Data.wActPower30AdjRatio<<11)/10000;

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[24]*256+puwRxBuf[25]);
	    stF107Data.wActPower70AdjRatio = wAdjRatioDecide(uwDownload107DataTmp);
	    stF107Data.wActPower70AdjRatio = ((int32)stF107Data.wActPower70AdjRatio<<11)/10000;
	    stDspUart.uwComReceFrameEnd |= 0x02;
	   if(0 == stIVCurve.unIVCurveBits.bit.StartEnable )
	   {
		 stIVCurve.unIVCurveBits.bit.StartEnable = (puwRxBuf[31]&0x01);
	   }
		stIVCurve.CurveAnalyDataID= puwRxBuf[32];
		stIVCurve.CurveAnalyDataSuccess= puwRxBuf[33];	      	

		stF107Data.uwSoftWareDebug1 = (Uint16)(puwRxBuf[40]*256+puwRxBuf[41]);
		stF107Data.uwSoftWareDebug2 = (Uint16)(puwRxBuf[42]*256+puwRxBuf[43]);
	}
	else if(2==uwRxBufIndexTmp)
	{
		stF107Data.unSafetyOdm.all = (Uint16)(puwRxBuf[4]*256+puwRxBuf[5]);
		stF107Data.stProtectEnvrmt.unProtectBit.all = puwRxBuf[6];
		stF107Data.stProtectEnvrmt.unEnvrmtBit.all = puwRxBuf[7];
		stF107Data.unFuncEnable.all = (Uint16)(puwRxBuf[8]*256+puwRxBuf[9]);
		stF107Data.uwSettingPower = (Uint16)(puwRxBuf[10]*256+puwRxBuf[11]);
		// rsv-puwRxBuf[12]
		stF107Data.uwMpptModeSet = (puwRxBuf[13]);
		if((eInverterStatus != cInverterStatus)&&(stF107Data.uwMpptModeSet<=DC_SOURCE))
		{
			stSysCfg.eMpptMode = (ENUM_MPPT_MODE)stF107Data.uwMpptModeSet;
		}

		// rsv-puwRxBuf[14]
		stF107Data.uwAdjustMode = (puwRxBuf[15]&0x03);

		stF107Data.uwPVStartUnderVolt = (Uint16)(puwRxBuf[16]*256+puwRxBuf[17]);
		stF107Data.uwACStartOverVolt = (Uint16)(puwRxBuf[18]*256+puwRxBuf[19]);
		stF107Data.uwACStartUnderVolt = (Uint16)(puwRxBuf[20]*256+puwRxBuf[21]);
		stF107Data.uwACStartOverFreq = (Uint16)(puwRxBuf[22]*256+puwRxBuf[23]);
		stF107Data.uwACStartUnderFreq = (Uint16)(puwRxBuf[24]*256+puwRxBuf[25]);
		stF107Data.uwStartDelayTime = (Uint16)(puwRxBuf[26]*256+puwRxBuf[27]);
		stF107Data.uwReStartDelayTime = (Uint16)(puwRxBuf[28]*256+puwRxBuf[29]);

		stF107Data.uwACUnderVolt1 = (Uint16)(puwRxBuf[30]*256+puwRxBuf[31]);
		stF107Data.uwACOverVolt1 = (Uint16)(puwRxBuf[32]*256+puwRxBuf[33]);
		stF107Data.uwACUnderVolt1Time = (Uint16)(puwRxBuf[34]*256+puwRxBuf[35]);
		stF107Data.uwACOverVolt1Time = (Uint16)(puwRxBuf[36]*256+puwRxBuf[37]);

		stF107Data.uwACUnderVolt2 = (Uint16)(puwRxBuf[38]*256+puwRxBuf[39]);
		stF107Data.uwACOverVolt2 = (Uint16)(puwRxBuf[40]*256+puwRxBuf[41]);
		stF107Data.uwACUnderVolt2Time = (Uint16)(puwRxBuf[42]*256+puwRxBuf[43]);
		stF107Data.uwACOverVolt2Time = (Uint16)(puwRxBuf[44]*256+puwRxBuf[45]);
		stF107Data.uwAC10MinOverVolt = (Uint16)(puwRxBuf[46]*256+puwRxBuf[47]);

		stDspUart.uwComReceFrameEnd |= 0x04;
	}
	else if(3==uwRxBufIndexTmp)
	{
		stF107Data.uwACUnderFreq1 = (Uint16)(puwRxBuf[4]*256+puwRxBuf[5]);
		stF107Data.uwACOverFreq1 = (Uint16)(puwRxBuf[6]*256+puwRxBuf[7]);
		stF107Data.uwACUnderFreq1Time = (Uint16)(puwRxBuf[8]*256+puwRxBuf[9]);
		stF107Data.uwACOverFreq1Time = (Uint16)(puwRxBuf[10]*256+puwRxBuf[11]);

		stF107Data.uwACUnderFreq2 = (Uint16)(puwRxBuf[12]*256+puwRxBuf[13]);
		stF107Data.uwACOverFreq2 = (Uint16)(puwRxBuf[14]*256+puwRxBuf[15]);
		stF107Data.uwACUnderFreq2Time = (Uint16)(puwRxBuf[16]*256+puwRxBuf[17]);
		stF107Data.uwACOverFreq2Time = (Uint16)(puwRxBuf[18]*256+puwRxBuf[19]);
        stF107Data.uwACUnderVolt3Time = (Uint16)(puwRxBuf[22]*256+puwRxBuf[23]);

		stF107Data.uwQvPFPvHighVoltPrt = (Uint16)(puwRxBuf[24]*256+puwRxBuf[25]);
		stF107Data.uwQvPFAutoAdjustPower = (Uint16)(puwRxBuf[26]*256+puwRxBuf[27]);
		stF107Data.uwQvPFAutoAdjustPF = (Uint16)(puwRxBuf[28]*256+puwRxBuf[29]);
		stF107Data.wISOValue = (Uint16)(puwRxBuf[30]*256+puwRxBuf[31]);
		stF107Data.uwDeratPFVoltPrt = (Uint16)(puwRxBuf[32]*256+puwRxBuf[33]);
        stF107Data.uwACUnderVolt3 = (Uint16)(puwRxBuf[34]*256+puwRxBuf[35]);

        //AFCI
        stF107Data.uwAFCICMD = (Uint16)(puwRxBuf[40]*256+puwRxBuf[41]);
        stF107Data.uwAFCI_Set_FaultCntTime_S = (Uint16)((puwRxBuf[42]*256+puwRxBuf[43]) * 10);
        stF107Data.uwAFCI_StartFreq = (Uint16)(puwRxBuf[44]);
        stF107Data.uwAFCI_EndFreq = (Uint16)(puwRxBuf[45]);
        stF107Data.uwAFCI_MagRatio = (Uint16)(puwRxBuf[46]);
        stF107Data.uwAFCI_FreqRatio = (Uint16)(puwRxBuf[47]);
	
		stDspUart.uwComReceFrameEnd |= 0x08;
	}
	else if(4==uwRxBufIndexTmp)
	{
	    /************PV Volt&Curr Adjust Ratio **********************/
	    uwDownload107DataTmp = (Uint16)(puwRxBuf[4]*256+puwRxBuf[5]);
	    stF107Data.wPVVoltAdjRatio[PVA] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[6]*256+puwRxBuf[7]);
	    stF107Data.wPVVoltAdjRatio[PVB] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[8]*256+puwRxBuf[9]);
	    stF107Data.wPVVoltAdjRatio[PVC] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[10]*256+puwRxBuf[11]);
	    stF107Data.wPVVoltAdjRatio[PVD] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[12]*256+puwRxBuf[13]);
	    stF107Data.wBTCurr30PctAdjRatio[PVA] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[14]*256+puwRxBuf[15]);
	    stF107Data.wBTCurr30PctAdjRatio[PVB] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[16]*256+puwRxBuf[17]);
	    stF107Data.wBTCurr30PctAdjRatio[PVC] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[18]*256+puwRxBuf[19]);
	    stF107Data.wBTCurr30PctAdjRatio[PVD] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[20]*256+puwRxBuf[21]);
	    stF107Data.wBTCurr70PctAdjRatio[PVA] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[22]*256+puwRxBuf[23]);
	    stF107Data.wBTCurr70PctAdjRatio[PVB] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[24]*256+puwRxBuf[25]);
	    stF107Data.wBTCurr70PctAdjRatio[PVC] = wAdjRatioDecide(uwDownload107DataTmp);

	    uwDownload107DataTmp = (Uint16)(puwRxBuf[26]*256+puwRxBuf[27]);
	    stF107Data.wBTCurr70PctAdjRatio[PVD] = wAdjRatioDecide(uwDownload107DataTmp);

        stF107Data.uwWaveDealFrame = puwRxBuf[28];
        stF107Data.uwWaveDealState = puwRxBuf[29];

        stF107Data.uwFaultWaveChannelNum1 = puwRxBuf[30];
        stF107Data.uwFaultWaveChannelNum2 = puwRxBuf[31];
        stF107Data.uwFaultWaveChannelNum3 = puwRxBuf[32];
        stF107Data.uwFaultWaveChannelNum4 = puwRxBuf[33];

        stF107Data.uwFaultWaveChannelNum5 = puwRxBuf[34];
        stF107Data.uwFaultWaveChannelNum6 = puwRxBuf[35];
        stF107Data.uwFaultWaveChannelNum7 = puwRxBuf[36];
        stF107Data.uwFaultWaveChannelNum8 = puwRxBuf[37];

        stF107Data.uwRecordFaultEnable = puwRxBuf[38];
        stF107Data.uwRecordHandEnable = puwRxBuf[39];
		
		stF107Data.uwAFCI_BinRatio = (Uint16)(puwRxBuf[40]);
        stF107Data.uwAFCI_MagThld_PowerOn_H = (Uint16)(puwRxBuf[41]);
	    stF107Data.usAFCI_AuthenFlag = (Uint16)(puwRxBuf[42]*256+puwRxBuf[43]);
        stF107Data.uwSelfCheckEnable_New = (Uint16)(puwRxBuf[44]*256+puwRxBuf[45]);
        stF107Data.uwAFCI_ManualResetFlag = (Uint16)(puwRxBuf[46]*256+puwRxBuf[47]);

        stDspUart.uwComReceFrameEnd |= 0x10;    //20210904Revised
	}
    else if(5==uwRxBufIndexTmp)
    {
        /************HighLowThrough & GFCI **********************/
        stF107Data.wACVoltLowThrough = (Uint16)(puwRxBuf[4]*256+puwRxBuf[5]);
        stF107Data.wACVoltHighThrough = (Uint16)(puwRxBuf[6]*256+puwRxBuf[7]);
        stDspReceData.wGFCIRmsFaultDerate = puwRxBuf[8]*256+puwRxBuf[9];
        stF107Data.wACVoltLowThroughRecover = (Uint16)(puwRxBuf[10]*256+puwRxBuf[11]);
        stF107Data.wACVoltHighThroughRecover = (Uint16)(puwRxBuf[12]*256+puwRxBuf[13]);
        stF107Data.swP_PFLineVoltPercentStart = (Uint16)(puwRxBuf[14]*256+puwRxBuf[15]);
        stF107Data.swP_PFLineVoltPercentRecover = (Uint16)(puwRxBuf[16]*256+puwRxBuf[17]);
        stF107Data.swP_PFActPowerPercentStart = (Uint16)(puwRxBuf[18]*256+puwRxBuf[19]);
        stF107Data.uwFreqDeratEndPoint = (Uint16)(puwRxBuf[20]*256+puwRxBuf[21]);

        stDspUart.uwComReceFrameEnd |= 0x20;    //20250725Revised
    }
	stF107Data.unSet.bit.FlashReady = puwRxBuf[48];
	stF107Data.unSet.bit.TrunOnOff = puwRxBuf[49];

	if(OFF == stF107Data.unSet.bit.TrunOnOff)
	{
		// CQC2018  Receive Shutdown command
		if((cInverterStatus==eInverterStatus)&&(stF107Data.uwShutDownSpeed!=0))
		{
			if(stLoadLimit.dActPowerLoadSpeedLimit<=2000)		// acitve power low : 200w
			{
				stSysFaultReg.unFaultBit.bit.TurnOff = 1;
			}
		}
		else
		{
			stSysFaultReg.unFaultBit.bit.TurnOff = 1;
		}
	}
	else
	{
		stSysFaultReg.unFaultBit.bit.TurnOff = 0;
	}
	stF107Data.uwActiveRate = (Uint16)(puwRxBuf[50]*256+puwRxBuf[51]);
	stF107Data.wAntiFlowMeterPower = (int16)(puwRxBuf[52]*256+puwRxBuf[53]);
	stF107Data.wReactiveRate = (Uint16)(puwRxBuf[54]*256+puwRxBuf[55]);
	stF107Data.unF107FaultBit.all = puwRxBuf[56];
	if(1==stF107Data.unF107FaultBit.bit.F107RxDspSci)
	{
		stSysFaultReg.unFaultBit.bit.CommunicateFault = 1;
		stSysFaultReg.unCommunicateFault.bit.F107RxDspSci = 1;
	}
	else
	{
		stSysFaultReg.unCommunicateFault.bit.F107RxDspSci = 0;
	}

	stF107Data.unF107WarnBit.all = puwRxBuf[57];
	stF107Data.wMultiAntiFlowRate = (Uint16)(puwRxBuf[58]*256+puwRxBuf[59]);

	DealComPowerData();
	GridProtectValueCheck();
}
void CurveAnalyStateSwitch(void)
{
	if((1 == stIVCurve.unIVCurveBits.bit.StartEnable)&&(cCurveAnalyStateOff == stIVCurve.CurveAnalyState))
	{
		stIVCurve.CurveAnalyState = cCurveAnalyStateOn;
		stIVCurve.CurveDataFlag = 0;
	}
	else if((0 == stIVCurve.unIVCurveBits.bit.StartEnable)&&(cCurveAnalyStateOn == stIVCurve.CurveAnalyState))
	{
		stIVCurve.CurveAnalyState = cCurveAnalyStateDataSend;
		stDspUart.wSciDVICommand = 0xF1;
	}
}

void CurveAnalyDataIDSwitch(void)
{
	static Uint16 suwFaultCnt = 0;

	switch(stDspUart.wSciDVICommand )
	{
		case 0xF1:  //发送PV1的扫描数据
		{
			if((0xF1 == stIVCurve.CurveAnalyDataID)&&(1 == stIVCurve.CurveAnalyDataSuccess))
			{
				stDspUart.wSciDVICommand  = 0xF2;
				suwFaultCnt = 0;
			}
			else
			{
				suwFaultCnt++;
				if(suwFaultCnt > 10)
				{
					stIVCurve.CurveAnalyState = cCurveAnalyStateOff;
					suwFaultCnt = 0;
				}
			}
			break;
		}
		case 0xF2:  //发送PV2的扫描数据
		{
			if((0xF2 ==stIVCurve.CurveAnalyDataID)&&(1 == stIVCurve.CurveAnalyDataSuccess))
			{
				stDspUart.wSciDVICommand  = 0xF3;
				suwFaultCnt = 0;
			}
			else
			{
				suwFaultCnt++;
				if(suwFaultCnt > 10)
				{
					suwFaultCnt = 0;
					stIVCurve.CurveAnalyState = cCurveAnalyStateOff;
				}
			}
			break;
		}
		case 0xF3:  //发送PV3的扫描数据
		{
			if((0xF3 == stIVCurve.CurveAnalyDataID)&&(1 == stIVCurve.CurveAnalyDataSuccess))
			{
				stDspUart.wSciDVICommand  = 0xF4;
				suwFaultCnt = 0;
			}
			else
			{
				suwFaultCnt++;
				if(suwFaultCnt > 10)
				{
					suwFaultCnt = 0;
					stIVCurve.CurveAnalyState = cCurveAnalyStateOff;
				}
			}
			break;
		}
		case 0xF4:  //发送PV4的扫描数据
		{
			if((0xF4 == stIVCurve.CurveAnalyDataID)&&(1 == stIVCurve.CurveAnalyDataSuccess))
			{
				stDspUart.wSciDVICommand  = 0xF5;
				suwFaultCnt = 0;
			}
			else
			{
				suwFaultCnt++;
				if(suwFaultCnt > 10)
				{
					suwFaultCnt = 0;
					stIVCurve.CurveAnalyState = cCurveAnalyStateOff;
				}
			}
			break;
		}
		default:
		{
			stIVCurve.CurveAnalyState = cCurveAnalyStateOff;
			break;
		}
	}
}
/****************************************************************************
*
*	FunctionName:	uwTxF107NormalData
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate,DSP Send F107 data
*	Note:	none.
***************************************************************/
static Uint16 uwTxF107IVData(Uint16 *puwTxBuf)
{
	//Uint16 uwTxDataLenTmp=0;
	//static Uint16 suwTxDataIndex=0xF1;
	Uint32 udUartTxDataTmp=0;
	Uint16 uwCrcRxTmp = 0;
	static Uint16  suwIndex = 0;
	static Uint16  suwIndex_20 = 0;
	Uint16 uwBuffIndexTmp = 4;
	Uint32 udUartTxDataTmp1=0;
	
	puwTxBuf[0]=0x55;
	puwTxBuf[1]=stDspUart.wSciDVICommand;
	puwTxBuf[2] = 0x01;
	puwTxBuf[3] = 0xE0;

	if(0xF1==stDspUart.wSciDVICommand)//PV1 IV扫描数据
	{
		for(suwIndex_20 = 0;suwIndex_20 < 20;suwIndex_20++)
		{
			udUartTxDataTmp1 =udUartTxDataTmp1 + cBus12V5;
			if(udUartTxDataTmp1 > cBus250V)
			{
				udUartTxDataTmp1= cBus250V;
			}
			udUartTxDataTmp = udUartTxDataTmp1;
			
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVA].stOut.uwPvCurrCurve[0];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}


		for(suwIndex = 0;suwIndex < 100;suwIndex++)
		{
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVA].stOut.uwPvVoltCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVA].stOut.uwPvCurrCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}

	}
	else if(0xF2==stDspUart.wSciDVICommand)//PV2 IV扫描数据
	{
		for(suwIndex_20 = 0;suwIndex_20 < 20;suwIndex_20++)
		{
			udUartTxDataTmp1 =udUartTxDataTmp1 + cBus12V5;
			if(udUartTxDataTmp1 > cBus250V)
			{
				udUartTxDataTmp1= cBus250V;
			}
			udUartTxDataTmp = udUartTxDataTmp1;
			
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVB].stOut.uwPvCurrCurve[0];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;

		}


		for(suwIndex = 0;suwIndex < 100;suwIndex++)
		{
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVB].stOut.uwPvVoltCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVB].stOut.uwPvCurrCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}

	}
	else if(0xF3==stDspUart.wSciDVICommand)//PV3 IV扫描数据
	{
		for(suwIndex_20 = 0;suwIndex_20 < 20;suwIndex_20++)
		{
			udUartTxDataTmp1 =udUartTxDataTmp1 + cBus12V5;
			if(udUartTxDataTmp1 > cBus250V)
			{
				udUartTxDataTmp1= cBus250V;
			}
			udUartTxDataTmp = udUartTxDataTmp1;
			
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVC].stOut.uwPvCurrCurve[0];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}


		for(suwIndex = 0;suwIndex < 100;suwIndex++)
		{
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVC].stOut.uwPvVoltCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVC].stOut.uwPvCurrCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}

	}
	else if(0xF4==stDspUart.wSciDVICommand)//PV4 IV扫描数据
	{
		for(suwIndex_20 = 0;suwIndex_20 < 20;suwIndex_20++)
		{
			udUartTxDataTmp1 =udUartTxDataTmp1 + cBus12V5;
			if(udUartTxDataTmp1 > cBus250V)
			{
				udUartTxDataTmp1= cBus250V;
			}
			udUartTxDataTmp = udUartTxDataTmp1;
			
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVD].stOut.uwPvCurrCurve[0];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}


		for(suwIndex = 0;suwIndex < 100;suwIndex++)
		{
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVD].stOut.uwPvVoltCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
			udUartTxDataTmp = stPvCurveAnalyCtrl[PVD].stOut.uwPvCurrCurve[suwIndex];
			puwTxBuf[uwBuffIndexTmp] = ((udUartTxDataTmp&0x0000FF00)>>8);
			puwTxBuf[uwBuffIndexTmp+1] = (udUartTxDataTmp&0x000000FF);
			uwBuffIndexTmp += 2;
		}		

	}
	return uwBuffIndexTmp;
}

/****************************************************************************
*
*	FunctionName:	ComF107Task
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate
*	Note:	none.
***************************************************************/
static Uint16 uwTxF107FaultWaveData(Uint16 *puwTxBuf)
{
    int16 uwTxDataTemp = 0;
    Uint16 uwDataSendIndex = 0;
    Uint16 *pData;
    Uint16 uwWaveBuffIndex = 0;

	puwTxBuf[0]=0x55;
	puwTxBuf[1]=stWaveRecord.uwWaveFrame;

    if(stF107Data.uwWaveDealFrame == stWaveRecord.uwWaveFrame \
        && stF107Data.uwWaveDealState == SAVESUCCESS)
    {
        stWaveRecord.uwSendTimeCnt = 0;
        stWaveRecord.uwWaveFrame++;
        stWaveRecord.uwSendPointIndex += stWaveRecord.uwDataLen;
        if(stWaveRecord.uwSendPointIndex >= WAVE_POINTNUM_MAX)
        {
            stWaveRecord.uwSendPointIndex = 0;
            stWaveRecord.uwWaveChannelIndex++;
        }

        //all channel send success
        if(stWaveRecord.uwWaveChannelIndex > WAVE_CHANNELNUM_MAX)
        {
            stWaveRecord.uwSendStart = DISABLE;
            stWaveRecord.uwRecordTrigger = DISABLE;
        }
    }
    else
    {
        stWaveRecord.uwSendTimeCnt++;
    }

    if(stWaveRecord.uwSendTimeCnt >= 15)
    {
        stWaveRecord.uwSendTimeCnt = 0;
        stWaveRecord.uwSendStart = DISABLE;
        stWaveRecord.uwRecordTrigger = DISABLE;
    }

    pData = &puwTxBuf[8];
    uwWaveBuffIndex = stWaveRecord.uwWaveChannelIndex - 1;
    for(uwDataSendIndex = 0;uwDataSendIndex < WAVE_SEND_UINT16_MAX;uwDataSendIndex++)
    {
        uwTxDataTemp = ReadWaveData(uwWaveBuffIndex, stWaveRecord.uwSendPointIndex + uwDataSendIndex);
        uwTxDataTemp = ChangeWaveDataRate(GetChannelNum(uwWaveBuffIndex),uwTxDataTemp);
        *pData++ = uwTxDataTemp>>8;
        *pData++ = uwTxDataTemp;

        if((uwDataSendIndex + stWaveRecord.uwSendPointIndex) >= WAVE_POINTNUM_MAX)
        {
            break;
        }
    }
    stWaveRecord.uwDataLen = uwDataSendIndex;

	puwTxBuf[2] = (stWaveRecord.uwDataLen * 2 + 4) >> 8;
	puwTxBuf[3] = (stWaveRecord.uwDataLen * 2 + 4);

	puwTxBuf[4] = WAVE_CHANNELNUM_MAX;
	puwTxBuf[5] = stWaveRecord.uwWaveChannelIndex;

	puwTxBuf[6] = stSysFaultReg.uwFaultCode;
	puwTxBuf[7] = stSysFaultReg.uwFaultSubCode;

    return (stWaveRecord.uwDataLen * 2 + 8);
}

/****************************************************************************
*
*	FunctionName:	ComF107Task
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate
*	Note:	none.
***************************************************************/
void ComF107Task(void)
{
	static TASK_EVENT event = 0;
	Uint16 uwCrcRxTmp = 0;
	Uint16 uwRxLenTmp=0, uwTxLenTmp=0;
	static Uint16 suwComFaultSetCnt=0, suwComFaultClrCnt=0;

	while(1)
	{
		event = OSEventPend();

		//receive stm32f107 data
		if(event&(1<<COM_107_RX_ENDED))
		{
			uwRxLenTmp = stDspUart.uwRxBuf[2];
			uwRxLenTmp = uwRxLenTmp*256+stDspUart.uwRxBuf[3]+6;

			uwCrcRxTmp = uwCalcCRC16(stDspUart.uwRxBuf, (uwRxLenTmp - 2));

			if(((uwCrcRxTmp&0xFF)==stDspUart.uwRxBuf[uwRxLenTmp-2])
			 &&((uwCrcRxTmp>>8)==stDspUart.uwRxBuf[uwRxLenTmp-1]))
			{
				DownloadF107Data(stDspUart.uwRxBuf);

				suwComFaultSetCnt = 0;
				if((++suwComFaultClrCnt) > CLR_F107_COM_FAULT_CNT)
				{
					suwComFaultClrCnt = CLR_F107_COM_FAULT_CNT;
#if  F107_BOARD
					stSysFaultReg.unCommunicateFault.bit.DspRxF107Sci = 0;
#endif
				}
			}
		}
		else if(event&(1<<COM_107_TIMER))
		{
			suwComFaultSetCnt++;
			if(suwComFaultSetCnt > SET_F107_COM_FAULT_CNT)		//10S
			{
				suwComFaultSetCnt = 0;
				suwComFaultClrCnt = 0;
#if  F107_BOARD
				stSysFaultReg.unFaultBit.bit.CommunicateFault = 1;
				stSysFaultReg.unCommunicateFault.bit.DspRxF107Sci = 1;
#endif
			}
		}

		// DSP send message to stm32
		if(event&(1<<COM_107_TIMER))
		{
			if(cInitialStatus != eInverterStatus)
			{
				CurveAnalyStateSwitch();
				
				if(cCurveAnalyStateDataSend == stIVCurve.CurveAnalyState) 
				{
					CurveAnalyDataIDSwitch();			
					uwTxLenTmp = uwTxF107IVData(stDspUart.uwTxBuf);
					uwCrcRxTmp = uwCalcCRC16(stDspUart.uwTxBuf,uwTxLenTmp);
					stDspUart.uwTxBuf[uwTxLenTmp++] = (uwCrcRxTmp&0x00FF);
					stDspUart.uwTxBuf[uwTxLenTmp++] = ((uwCrcRxTmp&0xFF00)>>8);
					uwSciWrite(DSP_F107_SCID,stDspUart.uwTxBufBak,stDspUart.uwTxBuf,uwTxLenTmp);
				}
                else if(stWaveRecord.uwSendStart == ENABLE)
                {
					uwTxLenTmp = uwTxF107FaultWaveData(stDspUart.uwTxBuf);
					uwCrcRxTmp = uwCalcCRC16(stDspUart.uwTxBuf,uwTxLenTmp);
					stDspUart.uwTxBuf[uwTxLenTmp++] = (uwCrcRxTmp&0x00FF);
					stDspUart.uwTxBuf[uwTxLenTmp++] = ((uwCrcRxTmp&0xFF00)>>8);
					uwSciWrite(DSP_F107_SCID,stDspUart.uwTxBufBak,stDspUart.uwTxBuf,uwTxLenTmp);
                }
				else if(cCurveAnalyStateDataSend != stIVCurve.CurveAnalyState)
				{
					uwTxLenTmp = uwTxF107NormalData(stDspUart.uwTxBuf);
					uwCrcRxTmp = uwCalcCRC16(stDspUart.uwTxBuf,uwTxLenTmp);
					stDspUart.uwTxBuf[uwTxLenTmp++] = (uwCrcRxTmp&0x00FF);
					stDspUart.uwTxBuf[uwTxLenTmp++] = ((uwCrcRxTmp&0xFF00)>>8);
					uwSciWrite(DSP_F107_SCID,stDspUart.uwTxBufBak,stDspUart.uwTxBuf,uwTxLenTmp);
				}
				
			}

		}
	}
}
