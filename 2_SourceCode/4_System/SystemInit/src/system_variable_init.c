/*
* Sys_Init_Para.c
*
*	Product:			three phase inveter
*	Filename:			Sys_Init_Para.c
*	Purpose:			Inverter running configuration parameters  initialization
*	Date				Version		Author			Note
*   20/12/11			V1.01		chensx			none
*/

//#include "system_periph_init.h"
//#include "solar_cntr.h"

#include "dsp_user_include.h"

ST_SYS_FAULT_REG stSysFaultReg;
ST_SYS_CFG stSysCfg;
UN_SYS_FLAG unSysFlag;
ST_MACHINE_CFG stMachineCfg;

void SysParaInit(void)
{
	memset((void*)&stSysCfg,0,sizeof(stSysCfg));
	memset((void*)&stSysFaultReg,0,sizeof(stSysFaultReg));
	memset((void*)&unSysFlag,0,sizeof(unSysFlag));
	memset((void*)&stMachineCfg,0,sizeof(stMachineCfg));

	stSysCfg.eMpptMode = DC_SOURCE;
	stSysCfg.uwMachineType = MACHINE_ID_3_6KW;
	stSysCfg.uwMachineMin = CONFIG_1K5W;
	stSysCfg.uwMachineMax = CONFIG_7K2W;	
	stSysCfg.uwPVNumber = 2;
	// Sample Ratio
	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_3_6K;
	stSysCfg.wACCurrRatio = AC_CURR_RATIO_3_6K;
	stSysCfg.wDCIRatio = DCI_RATIO_3_6K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_3_6K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_3_6K;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_3_6K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_3_6K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_3_6K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_3_6K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_3_6K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_3_6K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_3_6K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_3_6K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_3_6K;
	stSysCfg.wBTCurrLimitMax[PVC] = 0;	
	stSysCfg.wBTCurrLimitMax[PVD] = 0;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_3_6K;

	
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;        // 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];

	// Hardware_InvOverCurr = (InvCurrentRms * 1.414*1.15(ripple) * 1.4)
	// Software_InvOverCurr = (InvCurrentRms * 1.414*1.15(ripple) * 1.2)
	// 1154 = (1.414*1.15*1.2*32/1.732)*32
//	stSysCfg.wInvCurrFaultMax = (stDspReceData.udOutputApparentPower*1154/stDspReceData.uwACNormalVolt)>>5;		// Q32
	stSysCfg.wInvCurrFaultMax = (stDspReceData.udOutputApparentPower*1212/stDspReceData.uwACNormalVolt)>>5;
	// Id_Limit = AcitvePower/(230V-20V)/3* 1.414*32
	// 836 = (1.414*32/1.732)*32
	stSysCfg.wInvCurrMaxLimit = (stDspReceData.udOutputActivePower*836/(stDspReceData.uwACNormalVolt-346))>>5;		// Q32
	// Iq_Limit = (ApparentPower*48.43%/230V)/3*1.414*32
	// 405 = (0.4843*1.414*32/1.732)*32
	stSysCfg.wReactiveCurrMaxLimit = (stDspReceData.udOutputApparentPower*405/stDspReceData.uwACNormalVolt)>>5;		// Q32
	stSysCfg.dACVoltSqrtSumFaultMax = SQRT_SUM_400V_MAX;
	stSysCfg.wACVoltFaultMax = PHASE_400V_MAX;
	stSysCfg.dACVoltSqrtSumFaultMin = SQRT_SUM_400V_MIN;
	stSysCfg.wACVoltFaultMin = PHASE_400V_MIN;
	// stSysCfg.Rated_CapCurrPeak = (((int32)stACSample.wLineVoltRmsMax * 63)>>14) ;// 2*PI*F*C*U,U=u/sqrt3*sqrt2
	// wCapCurrCoeff = 2*PI*C*f*32768 (Q15), C~15uF; mult(Q15)=round(2*PI*C/100*32768^2)=round(0.0309*32768)=1012; default 154 at init
	stSysCfg.wCapCurrCoeff = (stACSample.wGridFreqReal > 0) ? (int16)((int32)stACSample.wGridFreqReal * 1012 >> 15) : 0;

	unSysFlag.all = 0;
}

void VariableInit(void)
{
	ComDataInit();
	ComF107ParaInit();
	UartReceDriverInit(DSP_F107_SCID, DSP_RX_SIZE, stDspUart.uwRxBufBak);

	ComF042ParaInit();
	UartReceDriverInit(DSP_F042_SCIB, DSP_RX_M0_SIZE, stDspM0Uart.uwRxBufBak);

	InitDebugData();
	UartReceDriverInit(RS485_UART4,RS485_RX_SIZE, stRs485Uart4.bRxBufBak);

	UartReceDriverInit(DSP_AFCI_SCIC, AFCI_RX_SIZE, stAfciUart.uwRxBufBak);

	StatusTaskInit();

	PVInputTaskParaInit();
	
	ACOutputTaskParaInit();

	SysParaInit();

	IsrParaInit();

	ComAfciParaInit();
}


