/*-------------------------------------------------------
* Solar_InvCtrl.c
*
*	Product:			three phase inveter
*	Filename:			Solar_InvCtrl.c
*	Purpose:			AC-DC Control loop
*	Date				Version		Author			Note
*   20/12/11			V1.01		chensx			none
*--------------------------------------------------------*/
#include "dsp_user_include.h"

ST_CTRL_LOOP 		stInvBusVoltCtrl;
ST_INV_PARA			stInvPara;
ST_INV_VOLT_CTRL	stInvVoltCtrl;
ST_INV_CURR_CTRL	stInvCurrCtrl;


/****************************************************************************
*	FunctionName:	InvCtrlParaInit
*	Input:	none
*	Output:	none
*	Description:	Inverter control parameter initialization
*	Note:	none.
***************************************************************/
void InvCtrlParaInit(void)
{
	Uint16 i;
	memset((void*)&stInvPara,0,sizeof(stInvPara));
	memset((void*)&stInvVoltCtrl,0,sizeof(stInvVoltCtrl));
	memset((void*)&stInvCurrCtrl,0,sizeof(stInvCurrCtrl));
	memset((void*)&stInvPwm,0,sizeof(stInvPwm));

	stInvVoltCtrl.stIn.dVoltHighR_Kd = 720;		// 240/1024 = 0.5859375
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = 240;		// 240/1024 = 0.234375

	stInvCurrCtrl.stIn.dCurr_Kd = 1016;			// 256/1024 = 0.25
	stInvCurrCtrl.stIn.dCurr_Kq = 173;			// 173/1024 = 0.169
	stInvCurrCtrl.stIn.dCurr_Kpi = 256;			// 256/1024 = 0.25

	stInvBusVoltCtrl.stPID.stIn.dKp = 700;		// 700/4096
	stInvBusVoltCtrl.stPID.stIn.dKi = 42;		// 642/4096

	stInvCurrCtrl.stIn.dReCtrl_KPR= 666;			// 666/1024 = 0.65
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = 51;			// 51/1024 = 0.05
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = 972;		// 972/1024 = 0.95

		//Repeat control parameter reset
	for(i=0;i<stAdcPool.uwSumCnt;i++)
	{
		stInvCurrCtrl.stIn.ReCtrl_Id[i] = 0;
		stInvCurrCtrl.stIn.ReCtrl_Iq[i] = 0;
	}
	stInvCurrCtrl.stIn.ReCtrl_Pos = 0;
	stInvCurrCtrl.stIn.ReCtrl_Cur = 0;
	stInvCurrCtrl.stIn.ReCtrl_Val_Id = 0;
	stInvCurrCtrl.stIn.ReCtrl_Val_Iq = 0;
	stInvCurrCtrl.stIn.ReCtrl_Enable = 0;	
	stInvCurrCtrl.stIn.Rectrl_Points = 4;
}

/****************************************************************************
*	FunctionName:	InvVoltLoopCtrl
*	Input:	none
*	Output:	none
*	Description:	Inverter voltage loop control,First order and second order filtering algorithm for high impedance and voltage loop
*	Note:	none.
***************************************************************/
#pragma CODE_SECTION(InvVoltLoopCtrl,"ramfuncs");
void InvVoltLoopCtrl(void)
{
	static int32	sdVolt_dSum=0,	sdVolt_qSum=0;
	static int16	swVolt_dAvg=0,	swVolt_qAvg=0;
	static int16	swVolt_dOld=0,	swVolt_qOld=0;
	static int16	swVoltFstDiff_d=0,	swVoltFstDiff_q=0;

	int16 wVolt_dErrTmp=0,wVolt_qErrTmp=0;	
	//static int16	swVolt_dOlder=0,	swVolt_qOlder=0;
	//static int16	swVoltSecDiff_d=0,	swVoltSecDiff_q=0;
	
	static Uint16 suwPeriodCnt=0; 
	/************************************* Park transform abc to dq calculate*******************************/
	stInvPara.wVolt_d = ((((int32)stAdcPool.RGridVolt.wArithVal * stPllPara.stOut.wSinwt>>14)
					    + ((int32)stAdcPool.SGridVolt.wArithVal * stPllPara.stOut.wSinwt_240>>14)
					    + ((int32)stAdcPool.TGridVolt.wArithVal * stPllPara.stOut.wSinwt_120>>14))*683>>10);	// 2/3 = 683/1024 = 0.6667

	stInvPara.wVolt_q = ((((int32)stAdcPool.RGridVolt.wArithVal * stPllPara.stOut.wCoswt>>14)
					     + ((int32)stAdcPool.SGridVolt.wArithVal * stPllPara.stOut.wCoswt_240>>14)
					     + ((int32)stAdcPool.TGridVolt.wArithVal * stPllPara.stOut.wCoswt_120>>14))*683>>10);

	stInvPara.wCurr_d = ((((int32)stAdcPool.RInvCurr.wArithVal * stPllPara.stOut.wSinwt>>14)
					      + ((int32)stAdcPool.SInvCurr.wArithVal * stPllPara.stOut.wSinwt_240>>14)
					      + ((int32)stAdcPool.TInvCurr.wArithVal * stPllPara.stOut.wSinwt_120>>14))*683>>10);

	stInvPara.wCurr_q = ((((int32)stAdcPool.RInvCurr.wArithVal * stPllPara.stOut.wCoswt>>14)
					      + ((int32)stAdcPool.SInvCurr.wArithVal * stPllPara.stOut.wCoswt_240>>14)
					      + ((int32)stAdcPool.TInvCurr.wArithVal * stPllPara.stOut.wCoswt_120>>14))*683>>10);

	stInvPara.wVolt_dFilter = (int)(((long)stInvPara.wVolt_dFilter*15316+(long)stInvPara.wVolt_d*1068)>>14);	 //200Hz 
	stInvPara.wVolt_qFilter = (int)(((long)stInvPara.wVolt_qFilter*15316+(long)stInvPara.wVolt_q*1068)>>14);
	
	/************************************* High impedance algorithm**********************************/
	sdVolt_dSum += (int32)stInvPara.wVolt_d;
	sdVolt_qSum += (int32)stInvPara.wVolt_q;
	suwPeriodCnt++;
	if(suwPeriodCnt>=(stPllPara.stOut.uwGridPrdTimFlt>>1))
	{
		suwPeriodCnt = 0;
		
		swVolt_dAvg = ((int64)sdVolt_dSum*stACSample.d1DivGridTimFlt)>>23;
		swVolt_qAvg = ((int64)sdVolt_qSum*stACSample.d1DivGridTimFlt)>>23;
		
		sdVolt_dSum = 0;
		sdVolt_qSum = 0;
	}
	
	/*******************Voltage first order differential, second order differential calculation******************/
	swVoltFstDiff_d = stInvPara.wVolt_d -swVolt_dOld;
	swVoltFstDiff_q = stInvPara.wVolt_q -swVolt_qOld;

	swVolt_dOld = stInvPara.wVolt_d;
	swVolt_qOld = stInvPara.wVolt_q;

	wVolt_dErrTmp = (swVolt_dAvg-stInvPara.wVolt_d);
	wVolt_qErrTmp = (swVolt_qAvg-stInvPara.wVolt_q);

	UPDNLMT(wVolt_dErrTmp,960,-960);		// 30V *32 = 960
	UPDNLMT(wVolt_qErrTmp,960,-960);

	UPDNLMT(swVoltFstDiff_d,960,-960);			// 30V *32 = 960
	UPDNLMT(swVoltFstDiff_q,960,-960);
	
	stDebug.ReadData.wVolt_dErr = wVolt_dErrTmp;
	stDebug.ReadData.wVolt_qErr = wVolt_qErrTmp;

	stDebug.ReadData.wVolt_dFstDiff = swVoltFstDiff_d;
	stDebug.ReadData.wVolt_qFstDiff = swVoltFstDiff_q;
	

	stInvVoltCtrl.stOut.wForwardVolt_d = stInvPara.wVolt_d+(stInvVoltCtrl.stIn.dVoltHighR_Kd*wVolt_dErrTmp>>10)
													   +(stInvVoltCtrl.stIn.dVoltFstDiff_Kd*swVoltFstDiff_d>>10);
	stInvVoltCtrl.stOut.wForwardVolt_q = stInvPara.wVolt_q+(stInvVoltCtrl.stIn.dVoltHighR_Kd*wVolt_qErrTmp>>10)
													   +(stInvVoltCtrl.stIn.dVoltFstDiff_Kd*swVoltFstDiff_q>>10);

//		stInvVoltCtrl.stOut.wForwardVolt_d = stInvPara.wVolt_dFilter+(stInvVoltCtrl.stIn.dVoltHighR_Kd*wVolt_dErrTmp>>10)
//														   +(stInvVoltCtrl.stIn.dVoltFstDiff_Kd*swVoltFstDiff_d>>10);
//		stInvVoltCtrl.stOut.wForwardVolt_q = stInvPara.wVolt_qFilter+(stInvVoltCtrl.stIn.dVoltHighR_Kd*wVolt_qErrTmp>>10)
//														   +(stInvVoltCtrl.stIn.dVoltFstDiff_Kd*swVoltFstDiff_q>>10);

}

/****************************************************************************
*	FunctionName:	InvCurrLoopCtrl
*	Input:	none
*	Output:	none
*	Description:	Inverter voltage loop control,First order and second order filtering algorithm for high impedance and voltage loop
*	Note:	none.
***************************************************************/
#pragma CODE_SECTION(InvBusVoltLoopCtrl,"ramfuncs");
void InvBusVoltLoopCtrl(void)
{
	static INT16U uwBusCntlCnt = 0;
	static INT32S dwBus1msSum = 0;
	static INT16S wBus1msVolt = 0;
	int32	dBusVoltErrorTmp;
	
	uwBusCntlCnt++;
	dwBus1msSum += stAdcPool.BusVolt.wArithVal;
	if(uwBusCntlCnt >= 16)
	{
		wBus1msVolt = dwBus1msSum>>4; 

		dwBus1msSum = 0;
		uwBusCntlCnt = 0;

		stDCSample.wBusVolt1ms =wBus1msVolt;
		stDCSample.w1DivBus = ((int32)2621440/wBus1msVolt);
		stDCSample.w1DivBusFlt = ((int32)((int32)stDCSample.w1DivBusFlt*672+(int32)stDCSample.w1DivBus*352)>>10);		// 0.8*BusFlt + 0.2*Bus

	}	
	
	if((ENABLE == unSysFlag.bit.InvPwmEnable)&& (cInverterStatus == eInverterStatus))
	{
		stInvBusVoltCtrl.stIn.uwCtrlLoopEnable = NEGATIVE_PID;

		// Bus Voltage Reference
		stInvBusVoltCtrl.stIn.dRef = stMpptTskCtrl.uwBusRef;
		stInvBusVoltCtrl.stIn.dReal = stAdcPool.BusVolt.wArithVal;
		if(stInvBusVoltCtrl.stIn.dRef < stAdcPool.BusVolt.wArithVal - 200)
		{
			stInvBusVoltCtrl.stIn.dRef = (stAdcPool.BusVolt.wArithVal - 200);
		}
		/*******************Inverter current limiting without boost***********************/
		if( ((DISABLE == stBTCtrl[PVA].uwBtPwmEnable)&&(DISABLE == stBTCtrl[PVB].uwBtPwmEnable)
		&&   (DISABLE == stBTCtrl[PVC].uwBtPwmEnable)&&(DISABLE == stBTCtrl[PVD].uwBtPwmEnable))
		||  ((stBTCtrl[PVA].uwBtPwmDuty<=50)&&(stBTCtrl[PVB].uwBtPwmDuty<=50)
		&&   (stBTCtrl[PVC].uwBtPwmDuty<=50)&&(stBTCtrl[PVD].uwBtPwmDuty<=50)))	// 
		{
			stInvBusVoltCtrl.stPID.stIn.dPIMax = stLoadLimit.wVoltActiveCurrLimit;		// rated Current 13A
			stInvBusVoltCtrl.stPID.stIn.dPIMin = -64;			// 0A
		}
		else
		{
			stInvBusVoltCtrl.stPID.stIn.dPIMax = stSysCfg.wInvCurrMaxLimit;		// rated Current 13A
			stInvBusVoltCtrl.stPID.stIn.dPIMin = -64;			// 0A
		}

		dBusVoltErrorTmp =(stInvBusVoltCtrl.stIn.dReal - stInvBusVoltCtrl.stIn.dRef);

		if(stSysCfg.uwFastDCMode == 1)
		{
			stInvBusVoltCtrl.stPID.stIn.dKp = TAIWAN_IVBUSVOLT_CNTL_Kp_H_15_25K;//TAIWAN_IVBUSVOLT_CNTL_Kp_H_15_25K;
			stInvBusVoltCtrl.stPID.stIn.dKi = TAIWAN_IVBUSVOLT_CNTL_Ki_H_15_25K;//TAIWAN_IVBUSVOLT_CNTL_Ki_H_15_25K;
		}
		else if((dBusVoltErrorTmp <  VDC5V) && (dBusVoltErrorTmp > -VDC5V))
		{
			stInvBusVoltCtrl.stPID.stIn.dKp = stInvBusVoltCtrl.stIn.dKp_L;		
			stInvBusVoltCtrl.stPID.stIn.dKi = stInvBusVoltCtrl.stIn.dKi_L;		
		}
		else
		{
			stInvBusVoltCtrl.stPID.stIn.dKp = stInvBusVoltCtrl.stIn.dKp_H;		
			stInvBusVoltCtrl.stPID.stIn.dKi = stInvBusVoltCtrl.stIn.dKi_H;		
		}
	}
	else
	{
		stInvBusVoltCtrl.stPID.stIn.dPIMin = 0;
		stInvBusVoltCtrl.stPID.stIn.dPIMax = 0;
		stInvBusVoltCtrl.stIn.uwCtrlLoopEnable = PID_DISABLE;
	}
	IncrementalPID(&stInvBusVoltCtrl);
}

void InvCurrCtrlParmUpdate(void)
{
	float32 dCurr_Kd_Tmp =0;
	float32 dCurr_Kpi_Tmp =0;
	
	if(MACHINE_ID_Plus_30_36KW ==  stSysCfg.uwMachineType)
	{

		// if(stACSample.dApparentPowerAvg > ((stLoadLimit.dActivePower * 7168) >>10)) // 70%
		//if(stDspReceData.udOutputApparentPower > 300000)	// 30kw
		if(stACSample.dApparentPowerAvg  > 250000)	// 30kw
		{
			stInvCurrCtrl.stIn.dCurr_Kd = 2500;					
			stInvCurrCtrl.stIn.dCurr_Kpi = 33000;	
		}
		//else  if(stACSample.dApparentPowerAvg > ((stLoadLimit.dActivePower *3072) >>10))//stLoadLimit.dActivePower*3/10;		// 30%
		else
		{
			//stInvCurrCtrl.stIn.dCurr_Kd = 3500;					
			//stInvCurrCtrl.stIn.dCurr_Kpi = 39000;
			dCurr_Kd_Tmp =4166 - (float32)stACSample.dApparentPowerAvg /150;
			stInvCurrCtrl.stIn.dCurr_Kd = (int32)(dCurr_Kd_Tmp);
			UPDNLMT(stInvCurrCtrl.stIn.dCurr_Kd ,3500,2500);
			
			dCurr_Kpi_Tmp =43000 - (float32)stACSample.dApparentPowerAvg /150*6;
			stInvCurrCtrl.stIn.dCurr_Kpi =(int32)(dCurr_Kpi_Tmp);
			UPDNLMT(stInvCurrCtrl.stIn.dCurr_Kpi ,39000,33000);
			
			
		}
		//else
		//{
			//stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_25_40K;						
			//stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_25_40K;	
		//}
		 //if(stACSample.dApparentPowerAvg > ((stLoadLimit.dActivePower * 4096) >>10)) // 40%
		// if(stACSample.dApparentPowerAvg > 140000)	// 30kw
		//{
			//stInvCurrCtrl.stIn.ReCtrl_Enable =0;	
		//}
		// else
		// {
			//stInvCurrCtrl.stIn.ReCtrl_Enable =1;
		 //}

	}
	else if(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)
	{

	}

	if(MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType)
	{
		if(stSysCfg.uwFastDCMode == 1)
		{
			stActivePowerCtrl.stPID.stIn.dKp = FAST_ACTPOWR_CNTL_Kp_15_25K;
			stActivePowerCtrl.stPID.stIn.dKi = FAST_ACTPOWR_CNTL_Ki_15_25K;
			
			stBTCurrCtrl[PVA].stPID.stIn.dKi = FAST_BTCURR_CNTL_Ki_15_25K;
			stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
			stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
			stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
		}
		else
		{
			stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_15_25K;
			stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_15_25K;
			
			stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_15_25K;
			stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
			stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
			stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
		}
	}
}
/****************************************************************************
*	FunctionName:	InvCurrLoopCtrl
*	Input:	none
*	Output:	none
*	Description:	Inverter voltage loop control,First order and second order filtering algorithm for high impedance and voltage loop
*	Note:	none.
***************************************************************/
#pragma CODE_SECTION(InvCurrLoopCtrl,"ramfuncs");
void InvCurrLoopCtrl(void)
{
	int16 wCurr_dErrTmp=0,wCurr_qErrTmp=0;
	static int16 swCurr_dPI=0,swCurr_dPIRes=0;
	static int16 swCurr_qPI=0,swCurr_qPIRes=0;
	int32 dCurrLoopOutTmp=0;
		


	if((ENABLE == unSysFlag.bit.InvPwmEnable)&& (cInverterStatus == eInverterStatus))
	{
	/***********************************Inv Curr_d Control Loop**********************************************/
		stInvCurrCtrl.stIn.wCurr_d = stInvBusVoltCtrl.stOut.dCtrlLoopOutput;	// stDebug.SetData.uwIdRef
		//stInvCurrCtrl.stIn.wCurr_d = stDebug.SetData.uwIdRef;	// stDebug.SetData.uwIdRef
		UPDNLMT(stInvCurrCtrl.stIn.wCurr_d,stSysCfg.wInvCurrMaxLimit,-64);							// 320/32 = 10.0A
				
		wCurr_dErrTmp = stInvCurrCtrl.stIn.wCurr_d-stInvPara.wCurr_d;
		
		dCurrLoopOutTmp=((int32)swCurr_dPI<<12)+(stInvCurrCtrl.stIn.dCurr_Kpi*wCurr_dErrTmp>>10) + (int32)swCurr_dPIRes;
//		dCurrLoopOutTmp=((int32)swCurr_dPI<<12)+(stInvCurrCtrl.stIn.dCurr_Kpi*wCurr_dErrTmp>>3) + (int32)swCurr_dPIRes;
		swCurr_dPI =(int16)(dCurrLoopOutTmp>>12);
		swCurr_dPIRes = (int16)(dCurrLoopOutTmp&0x0FFF);

		UPDNLMT(swCurr_dPI,3200,-3200);			// 3200/32 = 100.0A

		//////////////////////////////
		//------------------Repeat control deal----------
		stInvCurrCtrl.stIn.ReCtrl_Pos = stInvCurrCtrl.stIn.ReCtrl_Cur + stAdcPool.uwSumCnt - stInvCurrCtrl.stIn.Rectrl_Points;
		if(stInvCurrCtrl.stIn.ReCtrl_Pos >= stAdcPool.uwSumCnt)
		{
			stInvCurrCtrl.stIn.ReCtrl_Pos -= stAdcPool.uwSumCnt;
		}
		if(stInvCurrCtrl.stIn.ReCtrl_Enable == 1)
		{
			stInvCurrCtrl.stIn.ReCtrl_Id[stInvCurrCtrl.stIn.ReCtrl_Pos] = (long)stInvCurrCtrl.stIn.dReCtrl_KPR* (stInvCurrCtrl.stIn.ReCtrl_Id[stInvCurrCtrl.stIn.ReCtrl_Pos] +stInvCurrCtrl.stOut.wOutCurr_d)>>10;
			stInvCurrCtrl.stIn.ReCtrl_Val_Id =  (int16)(((long)stInvCurrCtrl.stIn.dReCtrl_KPR1 * stInvCurrCtrl.stIn.ReCtrl_Val_Id>>10) +  ((long)(1024 - stInvCurrCtrl.stIn.dReCtrl_KPR1) * stInvCurrCtrl.stIn.ReCtrl_Id[stInvCurrCtrl.stIn.ReCtrl_Cur]>>10));
			//stDebug.ReadData.wDebug2=stInvCurrCtrl.stIn.ReCtrl_Val_Id;
			UPDNLMT(stInvCurrCtrl.stIn.ReCtrl_Val_Id,200,-200);		       	     	        
		}
		else
		{
			stInvCurrCtrl.stIn.ReCtrl_Id[stInvCurrCtrl.stIn.ReCtrl_Pos] = stInvCurrCtrl.stIn.ReCtrl_Id[stInvCurrCtrl.stIn.ReCtrl_Pos] >>1;
			stInvCurrCtrl.stIn.ReCtrl_Val_Id = 0;
		}
		stInvCurrCtrl.stIn.ReCtrl_Val_Id =   (int16)((long)stInvCurrCtrl.stIn.dReCtrl_KPR2 * stInvCurrCtrl.stIn.ReCtrl_Val_Id >>10);
		// stDebug.ReadData.wDebug3 = wCurr_dErrTmp;
		// stDebug.ReadData.wDebug8= stInvCurrCtrl.stIn.ReCtrl_Val_Id ;
		 //stDebug.ReadData.wDebug1= stInvCurrCtrl.stIn.ReCtrl_Id[stInvCurrCtrl.stIn.ReCtrl_Pos];
		 
	//------------------Repeat control end----------

		/////////////////////////////

		stInvCurrCtrl.stOut.wOutCurr_d =	(((stInvCurrCtrl.stIn.dCurr_Kd*wCurr_dErrTmp)>>10)
									        -((stInvCurrCtrl.stIn.dCurr_Kq*stInvPara.wCurr_q)>>10)
									         + swCurr_dPI + stInvCurrCtrl.stIn.ReCtrl_Val_Id);
		
	/***********************************Inv Curr_q Control Loop**********************************************/
		stInvCurrCtrl.stIn.wCurr_q = (stLoadLimit.wVoltReactiveCurrLimit+stPllPara.wIslandDisturb+stSysCfg.Rated_CapCurrPeak);
		UPDNLMT(stInvCurrCtrl.stIn.wCurr_q,stSysCfg.wReactiveCurrMaxLimit,-stSysCfg.wReactiveCurrMaxLimit);							// 320/32 = 10.0A
		wCurr_qErrTmp = stInvCurrCtrl.stIn.wCurr_q-stInvPara.wCurr_q;
	
		dCurrLoopOutTmp=((int32)swCurr_qPI<<12)+(stInvCurrCtrl.stIn.dCurr_Kpi*wCurr_qErrTmp>>10) + (int32)swCurr_qPIRes;
//		dCurrLoopOutTmp=((int32)swCurr_qPI<<12)+(stInvCurrCtrl.stIn.dCurr_Kpi*wCurr_qErrTmp>>3) + (int32)swCurr_qPIRes;
		swCurr_qPI =(int16)(dCurrLoopOutTmp>>12);
		swCurr_qPIRes = (int16)(dCurrLoopOutTmp&0x0FFF);

		UPDNLMT(swCurr_qPI,3200,-3200);			// 3200/32 = 100.0A

		//------------------Repeat control deal----------
		if(stInvCurrCtrl.stIn.ReCtrl_Enable == 1)
		{			        
			stInvCurrCtrl.stIn.ReCtrl_Iq[stInvCurrCtrl.stIn.ReCtrl_Pos] = (long)stInvCurrCtrl.stIn.dReCtrl_KPR* (stInvCurrCtrl.stIn.ReCtrl_Iq[stInvCurrCtrl.stIn.ReCtrl_Pos] + stInvCurrCtrl.stOut.wOutCurr_q)>>10;
			stInvCurrCtrl.stIn.ReCtrl_Val_Iq = (int16)(((long)stInvCurrCtrl.stIn.dReCtrl_KPR1 * stInvCurrCtrl.stIn.ReCtrl_Val_Iq)>>10 + ((long)(1024-stInvCurrCtrl.stIn.dReCtrl_KPR1)* stInvCurrCtrl.stIn.ReCtrl_Iq[stInvCurrCtrl.stIn.ReCtrl_Cur])>>10);
			UPDNLMT(stInvCurrCtrl.stIn.ReCtrl_Val_Iq,200,-200);		        
		}
		else
		{
			stInvCurrCtrl.stIn.ReCtrl_Iq[stInvCurrCtrl.stIn.ReCtrl_Pos] = stInvCurrCtrl.stIn.ReCtrl_Iq[stInvCurrCtrl.stIn.ReCtrl_Pos] >>1;
			stInvCurrCtrl.stIn.ReCtrl_Val_Iq = 0;
		}
		stInvCurrCtrl.stIn.ReCtrl_Val_Iq  =  (int16)((long)stInvCurrCtrl.stIn.dReCtrl_KPR2 *  stInvCurrCtrl.stIn.ReCtrl_Val_Iq>>10);
		 
	   	stInvCurrCtrl.stIn.ReCtrl_Cur++;
	    if(stInvCurrCtrl.stIn.ReCtrl_Cur >= stAdcPool.uwSumCnt)
	    {
	        stInvCurrCtrl.stIn.ReCtrl_Cur = 0;
	    }
		//------------------Repeat control end----------

		stInvCurrCtrl.stOut.wOutCurr_q =   (((stInvCurrCtrl.stIn.dCurr_Kd*wCurr_qErrTmp)>>10)
									       +((stInvCurrCtrl.stIn.dCurr_Kq*stInvPara.wCurr_d)>>10)
									       + swCurr_qPI + stInvCurrCtrl.stIn.ReCtrl_Val_Iq);
	}
	else
	{
		swCurr_dPI = 0;
		swCurr_qPI = 0;
		swCurr_dPIRes = 0;
		swCurr_qPIRes = 0;
		stInvCurrCtrl.stOut.wOutCurr_d = 0;
		stInvCurrCtrl.stOut.wOutCurr_q = 0;
		
	}
		//stDebug.ReadData.wDebug1 = stInvCurrCtrl.stOut.wOutCurr_d;
		//stDebug.ReadData.wDebug2 =  stInvCurrCtrl.stIn.ReCtrl_Val_Id;
		//stDebug.ReadData.wDebug3 = stInvCurrCtrl.stIn.dReCtrl_KPR2;
		//stDebug.ReadData.wDebug8 = stInvCurrCtrl.stIn.dReCtrl_KPR1  ;

}
/****************************************************************************
*	FunctionName:	InvVoltLoopCtrl
*	Input:	none
*	Output:	none
*	Description:	Inverter voltage loop control,First order and second order filtering algorithm for high impedance and voltage loop
*	Note:	none.
***************************************************************/
#pragma CODE_SECTION(LowVoltRideThrough,"ramfuncs");
void LowVoltRideThrough(void)
{
	static Uint16	suwLowVoltThroughSetCnt=0, suwLowVoltThroughClrCnt=0;
	static Uint16	suwLowVoltFlagClrCnt=0;

	if(stF107Data.unEnable.bit.LVRT == 0)
	{
		return;
	}

	if(1 == stInvPwm.unPwmDisableBit.bit.LowVoltThroughOld)
	{
		suwLowVoltFlagClrCnt++;
		if(suwLowVoltFlagClrCnt>=10)
		{
			suwLowVoltFlagClrCnt = 0;
			stInvPwm.unPwmDisableBit.bit.LowVoltThroughOld = 0;
		}
	}

	if(((stInvPara.wVolt_d<=stSysCfg.wACVoltFaultMin)
	|| (stACSample.PhaseVoltRms.wRN <= stF107Data.wACVoltLowThrough)		//CQC--0.86*230=197.8v // 0.826*230 = 190V	*277=228.8
	|| (stACSample.PhaseVoltRms.wSN <= stF107Data.wACVoltLowThrough)
	|| (stACSample.PhaseVoltRms.wTN <= stF107Data.wACVoltLowThrough)
	|| (stACSample.PhaseVoltRms.wRN >= stF107Data.wACVoltHighThrough)		// 1.17*230 = 270V	*277=324.1
	|| (stACSample.PhaseVoltRms.wSN >= stF107Data.wACVoltHighThrough)
	|| (stACSample.PhaseVoltRms.wTN >= stF107Data.wACVoltHighThrough))
    && (cInverterStatus == eInverterStatus))
	{
		suwLowVoltThroughClrCnt = 0;
		suwLowVoltThroughSetCnt++;
		if(suwLowVoltThroughSetCnt>=50)	// 52us*50*2=5.2ms
		{
			suwLowVoltThroughSetCnt = 0;
			stInvPwm.unPwmDisableBit.bit.LowVoltThrough = 1;
			if(STRNDARD_BRAZIL != stDspReceData.unSafetyOdm.bit.SafetyStandard)
			{
			    stInvPwm.uwPwmDisableDelayCnt = 1;
			}
		}
	}
	else if((stInvPara.wVolt_d>=stSysCfg.wACVoltFaultMin)
	&& (stACSample.PhaseVoltRms.wRN >= stF107Data.wACVoltLowThroughRecover)		//	0.913*230 = 210
	&& (stACSample.PhaseVoltRms.wSN >= stF107Data.wACVoltLowThroughRecover)
	&& (stACSample.PhaseVoltRms.wTN >= stF107Data.wACVoltLowThroughRecover)
	&& (stACSample.PhaseVoltRms.wRN <= stF107Data.wACVoltHighThroughRecover)		//	1.065*230 = 245
	&& (stACSample.PhaseVoltRms.wSN <= stF107Data.wACVoltHighThroughRecover)
	&& (stACSample.PhaseVoltRms.wTN <= stF107Data.wACVoltHighThroughRecover))
	{
		suwLowVoltThroughSetCnt = 0;
		suwLowVoltThroughClrCnt++;
		if(suwLowVoltThroughClrCnt>=50)	// 52us*50*2=5.2ms
		{
			suwLowVoltThroughClrCnt = 0;
			if(stInvPwm.unPwmDisableBit.bit.LowVoltThrough==1)
			{
				stInvPwm.unPwmDisableBit.bit.LowVoltThrough = 0;
				stInvPwm.unPwmDisableBit.bit.LowVoltThroughOld = 1;
			}
		}
	}

}
//end of file

