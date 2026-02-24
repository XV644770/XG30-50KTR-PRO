/*
 * calc_pv_sample.c
 *
 *  Created on: 2020年11月26日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

ST_DC_SAMPLE stDCSample;

/*********(120℃+40℃)/5℃=32****************/
#define	TEMP_RANGE_MAX				33

const	Uint16 cuwDegree[TEMP_RANGE_MAX]=
{
		0,		50,		100,	150,	200,	250,	300,	350,	400,	450,
		500,	550,	600,	650,	700,	750,	800,	850,	900,	950,
		1000,	1050,	1100,	1150,	1200,	1250,	1300,	1350,	1400,	1450,
		1500, 	1550,	1600
};
/**********************************Amb NTC*******************************/
Uint32	udAmbResistanceTable[TEMP_RANGE_MAX]=
{	// - 40		
	762243,	522108,	363242,	256442,	183553,	133097,	97697,	72545,	54460,	41307,			// 5	
	31638,	24458,	19074,	15000,	11890,	9496,	7639,	6187,	5044,	4138,			// 55
	3415,	2834,	2365,	1984,	1672,	1416,	1205,	1030,	884,		762,				//105
	650,		564,		499																		// 120
};
/**********************************Amb NTC*******************************/
Uint32	udAmbResistanceTable36Plus[TEMP_RANGE_MAX]=
{	// - 40		
	762243,	522108,	363242,	256442,	183553,	133097,	97697,	72545,	54460,	41307,			// 5	
	31638,	24458,	19074,	15000,	9512,	7668,	6493,	5362,	4439,	3687,			// 55
	3073,	2572,	2162,	1825,	1547,	1316,	1124,	1030,	884,		762,				//105
	650,		564,		499																		// 120
};
/**********************************50-70K NTC*******************************/
Uint32	ud50_70kwInvNTCResistanceTable[TEMP_RANGE_MAX]=
{
		/**************************-40℃ ~ 120℃************/
		875831,	612908,	435257,	313391,	228593,	168790,	126077,	95203,	72633,	55955,
		43506,	34123,	26986,	21511,	17275,	13973,	11378,	9326,	7691,	6379,
		5322,	4463,	3762,	3187,	2713,	2319,	1991,	1717,	1486,	1292,
		1126,	986,	866
};

/**********************************25-40K NTC*******************************/
Uint32	ud25_40kwInvNTCResistanceTable[TEMP_RANGE_MAX]=			// Resistance pull-down	22K
{	// - 40		
	735546,	531294,	387874,	286085,	213093,	160228,	121572,	93044,	71804,	55854,			// 5	
	43780,	34567,	27484,	22000,	17723,	14366,	11714,	9605,	7919,	6564,			// 55
	5467,	4576,	3848,	3250,	2757,	2349,	2008,	1724,	1486,	1285,			//105
	1115,	970,	847																		// 120
};

Uint32	ud25_40kwBoostResistanceTable[TEMP_RANGE_MAX]=			// Resistance pull-down	15K
{
		/**************************-40℃ ~ 120℃************/
	626582,	434086,	306915,	220800,	161241,	119294,	89282,	67513,	51530,	39667,			// 5	
	30850,	24056,	18933,	15000,	11958,	9591,	7737,	6277,	5128,	4199,			// 55
	3461,	2867,	2387,	1997,	1679,	1418,	1202,	1024,	877,	753,				//105
	650,	563,	490																		// 120
};

static int16 wNTCTempNewCalc(Uint16 uwAdcTempResult,Uint16 wBranchResistance,Uint32 *cNTCResistanceTable);
/****************************************************************************
*
*	FunctionName:	CalcPVVoltCurrAvg
*	Input:	none
*	Output:	none
*	Description:	Calculate pv volt averge and boost current averge
*	Note:	none.
***************************************************************/
void CalcPVVoltCurrAvg(void)
{

	static int32 sdPVVoltSum[PV_MAX_NUM];
	static Uint16 suwPVVoltCurrCalcCnt=0;
	int i;
	int stri;

	static Uint16	uwAmTempRisistorTmp,uwInvTempRisistorTmp,uwBoostTempRisistorTmp;
	static Uint32  *pudAmTempRisistorTableTmp, *pudInvTempRisistorTableTmp, *pudBoostTempRisistorTableTmp;
	int16	wPVVoltTmp[PV_MAX_NUM];
	int16	wBTCurrTmp[PV_MAX_NUM];
	int16	wBTCurrLimitTmp[PV_MAX_NUM];
	int32	dPVPowerSumTmp=0;
	static int32	sdPVPowerSum=0;
	
	/**********************Inverter PV Voltage/Current Calculate&Precision************************************/
	wPVVoltTmp[PVA] = (int16)((int32)(stAdcPool.PVVolt[PVA].dAddSum/stAdcPool.uwMux2Ch0SumCnt));    //*10
	wPVVoltTmp[PVB] = (int16)((int32)(stAdcPool.PVVolt[PVB].dAddSum/stAdcPool.uwMux2Ch1SumCnt));    //*10
	wPVVoltTmp[PVC] = (int16)((int32)(stAdcPool.PVVolt[PVC].dAddSum/stAdcPool.uwMux2Ch0SumCnt));    //*10
	wPVVoltTmp[PVD] = (int16)((int32)(stAdcPool.PVVolt[PVD].dAddSum/stAdcPool.uwMux2Ch1SumCnt));    //*10

	wBTCurrTmp[PVA] = ((stAdcPool.BTCurr[PVA].dAddSum/stAdcPool.uwMux2Ch0SumCnt)*100)>>5;	//*100
	wBTCurrTmp[PVB] = ((stAdcPool.BTCurr[PVB].dAddSum/stAdcPool.uwMux2Ch1SumCnt)*100)>>5;	//*100
	wBTCurrTmp[PVC] = ((stAdcPool.BTCurr[PVC].dAddSum/stAdcPool.uwMux2Ch0SumCnt)*100)>>5;	//*100
	wBTCurrTmp[PVD] = ((stAdcPool.BTCurr[PVD].dAddSum/stAdcPool.uwMux2Ch1SumCnt)*100)>>5;	//*100

	dPVPowerSumTmp = 0;
	for(i=0;	i<PV_MAX_NUM;	i++)
	{
		/************************PV Voltage Adjust****************/
		stDCSample.wPVVolt[i] = wPVVoltTmp[i];		// *10s
		stDCSample.wPVVoltAdj[i] = (int16)((int32)wPVVoltTmp[i]*stF107Data.wPVVoltAdjRatio[i]/10000);    //*10
		stDCSample.wMPPTCurr[i] = wBTCurrTmp[i];

		/************************BT Current Adjust****************/
		 if((MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)||(MACHINE_ID_Plus_30_36KW ==  stSysCfg.uwMachineType))
		 {
			wBTCurrLimitTmp[i] = (int16)(((int32)(stSysCfg.wBTCurrLimitMax[PVA])*80)>>6);        // Curr/32/2(half)*100
		 }
		 else
		 {
			if(MACHINE_ID_25KW == stSysCfg.uwMachineType)
			{
			    wBTCurrLimitTmp[i] = (int16)(((int32)(stSysCfg.wBTCurrLimitMax[PVA])*100)>>6);        // Curr/32/2(half)*100
			}
			else
			{
			    wBTCurrLimitTmp[i] = (int16)(((int32)(stSysCfg.wBTCurrLimitMax[i])*100)>>6);        // Curr/32/2(half)*100
			}
		 }


		/****************PV Current 30%**************/
		if(stIVCurve.unIVCurveBits.bit.StartEnable == 1)
	 	{
	 		stDCSample.wBTCurr[i] = (int16)((int32)wBTCurrTmp[i]*stF107Data.wBTCurr30PctAdjRatio[i]/10000); //*100
	 	}
		else if(wBTCurrTmp[i] < wBTCurrLimitTmp[i])
		{
			stDCSample.wBTCurr[i] = (int16)((int32)wBTCurrTmp[i]*stF107Data.wBTCurr30PctAdjRatio[i]/10000); //*100
		}
		else		/****************PV Current 70%**************/
		{
			stDCSample.wBTCurr[i] = (int16)((int32)wBTCurrTmp[i]*stF107Data.wBTCurr70PctAdjRatio[i]/10000); //*100
		}


		if(TRUE == stMpptTskCtrl.uwPVConnect[i])
		{
			// Calculate the power of each PV 		P = V*I
			dPVPowerSumTmp += ((int32)stMpptPara[i].wPVVolt*stDCSample.wMPPTCurr[i]/100);	//unit 0.1w
		}
	}
	/**********************Inverter Bus Voltage Calculate************************************/
	stDCSample.wPBusVolt = (stAdcPool.PBusVolt.dAddSum/stAdcPool.uwMux4Ch3SumCnt);	//*10
	stDCSample.wNBusVolt = (stAdcPool.NBusVolt.dAddSum/stAdcPool.uwMux4Ch3SumCnt);	//*10
	stDCSample.wBusVolt = (stDCSample.wPBusVolt + stDCSample.wNBusVolt);
	/**********************ISO Voltage/Current Calculate************************************/
	stDCSample.wISOVolt = stAdcPool.ISOVolt.dAddSum/stAdcPool.uwMux8Ch3SumCnt;	//*10

//	stDCSample.w1DivBus = ((int32)2621440/stDCSample.wBusVolt);
//	stDCSample.w1DivBusFlt = ((int32)((int32)stDCSample.w1DivBusFlt*820+(int32)stDCSample.w1DivBus*204)>>10);		// 0.8*BusFlt + 0.2*Bus
	
	/**********************Inverter Inv/Boost/Am Temperature Calculate************************************/
	if(MACHINE_ID_50KW ==  stSysCfg.uwMachineType || MACHINE_ID_60_70KW ==  stSysCfg.uwMachineType )
	{
		uwAmTempRisistorTmp =  4020;		// 4.02K
		uwInvTempRisistorTmp = 3900;		// 3.9K
		pudInvTempRisistorTableTmp = ud50_70kwInvNTCResistanceTable;
	}
	else if((MACHINE_ID_15_22KW ==  stSysCfg.uwMachineType) 
			||(MACHINE_ID_25KW ==  stSysCfg.uwMachineType)
			||(MACHINE_ID_PARALLEL_TUBE_15_22KW ==  stSysCfg.uwMachineType)
			||(MACHINE_ID_PARALLEL_TUBE_25KW ==  stSysCfg.uwMachineType)
			||(MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType))
	{
	    uwAmTempRisistorTmp = 3900;         // 3.9k
	    uwInvTempRisistorTmp = 10000;        // 10K
	    pudInvTempRisistorTableTmp = ud25_40kwBoostResistanceTable;
	}
	else if((MACHINE_ID_25_33KW ==  stSysCfg.uwMachineType )||( MACHINE_ID_36_40KW ==  stSysCfg.uwMachineType) 
		||(MACHINE_ID_Plus_30_36KW ==  stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)
		||(MACHINE_ID_PRO_60KW == stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
		||(MACHINE_ID_PRO_33_40KW ==  stSysCfg.uwMachineType))
	{
		uwAmTempRisistorTmp = 3900;			// 3.9k
		uwInvTempRisistorTmp = 10000;		// 4.2K
		//pudInvTempRisistorTableTmp = ud25_40kwInvNTCResistanceTable;//
		pudInvTempRisistorTableTmp = ud25_40kwBoostResistanceTable;
	}
	else
	{
		uwAmTempRisistorTmp = 10000;			// 10K
		uwInvTempRisistorTmp = 10000;		// 10K
		pudInvTempRisistorTableTmp = ud25_40kwBoostResistanceTable;
	}
	
	//uwAmTempRisistorTmp =  3900;		// 3.9K
	if((MACHINE_ID_Plus_30_36KW ==  stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)
		||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
		||(MACHINE_ID_PRO_33_40KW ==  stSysCfg.uwMachineType)) 
	{
		pudAmTempRisistorTableTmp = udAmbResistanceTable36Plus;
	}
	else
	{
		pudAmTempRisistorTableTmp = udAmbResistanceTable;
	}
	

	uwBoostTempRisistorTmp = 10000;		// 10K
	pudBoostTempRisistorTableTmp = ud25_40kwBoostResistanceTable;

	stDCSample.wInvTemp = wNTCTempNewCalc((Uint16)(stAdcPool.InvTemp.wCalVal ),uwInvTempRisistorTmp,pudInvTempRisistorTableTmp);
	stDCSample.wBoostTemp = wNTCTempNewCalc((Uint16)(stAdcPool.BoostTemp.wCalVal ),uwBoostTempRisistorTmp,pudBoostTempRisistorTableTmp);
	stDCSample.wAmTemp = wNTCTempNewCalc((Uint16)(stAdcPool.AMTemp.wCalVal ),uwAmTempRisistorTmp,pudAmTempRisistorTableTmp);

	/**************************PV Voltage & Current Average Vaule Calculate*********************************/
	sdPVVoltSum[PVA] += stDCSample.wPVVolt[PVA];
	sdPVVoltSum[PVB] += stDCSample.wPVVolt[PVB];
	sdPVVoltSum[PVC] += stDCSample.wPVVolt[PVC];
	sdPVVoltSum[PVD] += stDCSample.wPVVolt[PVD];

	sdPVPowerSum += dPVPowerSumTmp;

	suwPVVoltCurrCalcCnt++;
	if(suwPVVoltCurrCalcCnt >= 32)		// 20ms * 32 = 640ms
	{
		suwPVVoltCurrCalcCnt = 0;

		stDCSample.wPVVoltAvg[PVA] = (sdPVVoltSum[PVA]>>5);
		stDCSample.wPVVoltAvg[PVB] = (sdPVVoltSum[PVB]>>5);
		stDCSample.wPVVoltAvg[PVC] = (sdPVVoltSum[PVC]>>5);
		stDCSample.wPVVoltAvg[PVD] = (sdPVVoltSum[PVD]>>5);

		stDCSample.dPVPowerTotal = (sdPVPowerSum>>5);

		sdPVVoltSum[PVA] = 0;
		sdPVVoltSum[PVB] = 0;
		sdPVVoltSum[PVC] = 0;
		sdPVVoltSum[PVD] = 0;
		sdPVPowerSum = 0;
	}
	
	/**********************Inverter String Voltage/Current Calculate************************************/
	if(DISABLE == stDspReceData.unFuncEnable.bit.StringMonitor)		// string current sample : disable
	{
		for(i=0;	i<STR_NUM_MAX;	i++)
		{
			stDCSample.wBTStrCurr[i] = 0;
		}
	}
	else															// string current sample : enable
	{
		if(stSysCfg.uwMachineType < MACHINE_ID_10_13KW)
		{
			stDCSample.wBTStrCurr[STR0] = stDCSample.wBTCurr[PVA];                                          //*100
			stDCSample.wBTStrCurr[STR1] = stDCSample.wBTCurr[PVB];
			for(stri=0; stri<2; stri++)
			{
				if(stDCSample.wBTStrCurr[stri] < 50)
				{
					stDCSample.wBTStrCurr[stri] = 0;
				}
			}
			stDCSample.wBTStrCurr[STR2] = 0;
			stDCSample.wBTStrCurr[STR3] = 0;
			stDCSample.wBTStrCurr[STR4] = 0;
			stDCSample.wBTStrCurr[STR5] = 0;
			stDCSample.wBTStrCurr[STR6] = 0;
			stDCSample.wBTStrCurr[STR7] = 0;
			stDCSample.wBTStrCurr[STR8] = 0;
			stDCSample.wBTStrCurr[STR9] = 0;
			stDCSample.wBTStrCurr[STR10] = 0;
			stDCSample.wBTStrCurr[STR11] = 0;
		}
		else if((MACHINE_ID_15_22KW == stSysCfg.uwMachineType)
				||(MACHINE_ID_25KW == stSysCfg.uwMachineType)
				||(MACHINE_ID_PARALLEL_TUBE_15_22KW == stSysCfg.uwMachineType)
				||(MACHINE_ID_PARALLEL_TUBE_25KW == stSysCfg.uwMachineType)
				||(MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType))
		{
		    if(MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType)
		    	{
				stDCSample.wBTStrCurr[STR0] = (int16)(stAdcPool.BTStrCurr[STR0].dAddSum /stAdcPool.uwMux8Ch0SumCnt);  //*100 
				stDCSample.wBTStrCurr[STR1] = stDCSample.wBTCurr[PVA] - stDCSample.wBTStrCurr[STR0];
				stDCSample.wBTStrCurr[STR2] = (int16)(stAdcPool.BTStrCurr[STR2].dAddSum /stAdcPool.uwMux8Ch1SumCnt);
			}
			else
			{				 
				stDCSample.wBTStrCurr[STR0] = (int16)(-(stAdcPool.BTStrCurr[STR0].dAddSum /stAdcPool.uwMux8Ch0SumCnt));  //*100
				stDCSample.wBTStrCurr[STR2] = (int16)(-(stAdcPool.BTStrCurr[STR2].dAddSum /stAdcPool.uwMux8Ch1SumCnt));
			}
		    
		    if((MACHINE_ID_15_22KW == stSysCfg.uwMachineType)||(MACHINE_ID_PARALLEL_TUBE_15_22KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_25_30KW ==  stSysCfg.uwMachineType))	//XG15-22K,2String+2String
		    {
		        stDCSample.wBTStrCurr[STR3] = stDCSample.wBTCurr[PVB] - stDCSample.wBTStrCurr[STR2];
		    }
		    else        // XG25K,2String+3String
		    {
		        stDCSample.wBTStrCurr[STR3] = (int16)(-(stAdcPool.BTStrCurr[STR3].dAddSum /stAdcPool.uwMux8Ch2SumCnt));
		        stDCSample.wBTStrCurr[STR4] = stDCSample.wBTCurr[PVB] - stDCSample.wBTStrCurr[STR2]-stDCSample.wBTStrCurr[STR3];
		    }
			for(stri=0; stri<5; stri++)
			{
				if(stDCSample.wBTStrCurr[stri] < 50)
				{
					stDCSample.wBTStrCurr[stri] = 0;
				}
			}
			
		    stDCSample.wBTStrCurr[STR5] = 0;
		    stDCSample.wBTStrCurr[STR6] = 0;
		    stDCSample.wBTStrCurr[STR7] = 0;
		    stDCSample.wBTStrCurr[STR8] = 0;
		    stDCSample.wBTStrCurr[STR9] = 0;
		    stDCSample.wBTStrCurr[STR10] = 0;
		    stDCSample.wBTStrCurr[STR11] = 0;
		}
		else if((MACHINE_ID_36_40KW == stSysCfg.uwMachineType)||(MACHINE_ID_25_33KW == stSysCfg.uwMachineType)
			||(MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW == stSysCfg.uwMachineType)\
			||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
			||(MACHINE_ID_PRO_33_40KW ==  stSysCfg.uwMachineType))
		{
			if((MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW == stSysCfg.uwMachineType)\
				||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
				||(MACHINE_ID_PRO_33_40KW ==  stSysCfg.uwMachineType))
			{
				stDCSample.wBTStrCurr[STR0] = (int16)((stAdcPool.BTStrCurr[STR0].dAddSum /stAdcPool.uwMux8Ch0SumCnt - stAdcPool.BTStrCurr[STR0].wOffset));  //*100
				stDCSample.wBTStrCurr[STR1] = stDCSample.wBTCurr[PVA] - stDCSample.wBTStrCurr[STR0];
				stDCSample.wBTStrCurr[STR2] = (int16)((stAdcPool.BTStrCurr[STR2].dAddSum /stAdcPool.uwMux8Ch2SumCnt - stAdcPool.BTStrCurr[STR2].wOffset));
				stDCSample.wBTStrCurr[STR3] = stDCSample.wBTCurr[PVB] - stDCSample.wBTStrCurr[STR2];
				stDCSample.wBTStrCurr[STR4] = (int16)((stAdcPool.BTStrCurr[STR4].dAddSum /stAdcPool.uwMux8Ch4SumCnt - stAdcPool.BTStrCurr[STR4].wOffset));
				stDCSample.wBTStrCurr[STR5] = stDCSample.wBTCurr[PVC] - stDCSample.wBTStrCurr[STR4];
				if(MACHINE_ID_PRO_33_40KW ==  stSysCfg.uwMachineType)
				{
					stDCSample.wBTStrCurr[STR6] = 0;
					stDCSample.wBTStrCurr[STR7] = 0;
				}
				else
				{
					stDCSample.wBTStrCurr[STR6] = (int16)((stAdcPool.BTStrCurr[STR6].dAddSum /stAdcPool.uwMux8Ch6SumCnt - stAdcPool.BTStrCurr[STR6].wOffset));
					stDCSample.wBTStrCurr[STR7] = stDCSample.wBTCurr[PVD] - stDCSample.wBTStrCurr[STR6];
				}
			}
			else
			{
				stDCSample.wBTStrCurr[STR0] = (int16)(-(stAdcPool.BTStrCurr[STR0].dAddSum /stAdcPool.uwMux8Ch0SumCnt - stAdcPool.BTStrCurr[STR0].wOffset));  //*100
				stDCSample.wBTStrCurr[STR1] = stDCSample.wBTCurr[PVA] - stDCSample.wBTStrCurr[STR0];
				stDCSample.wBTStrCurr[STR2] = (int16)(-(stAdcPool.BTStrCurr[STR2].dAddSum /stAdcPool.uwMux8Ch2SumCnt - stAdcPool.BTStrCurr[STR2].wOffset));
				stDCSample.wBTStrCurr[STR3] = stDCSample.wBTCurr[PVB] - stDCSample.wBTStrCurr[STR2];
				stDCSample.wBTStrCurr[STR4] = (int16)(-(stAdcPool.BTStrCurr[STR4].dAddSum /stAdcPool.uwMux8Ch4SumCnt - stAdcPool.BTStrCurr[STR4].wOffset));
				stDCSample.wBTStrCurr[STR5] = stDCSample.wBTCurr[PVC] - stDCSample.wBTStrCurr[STR4];
				if(MACHINE_ID_36_40KW == stSysCfg.uwMachineType)	// 36_40KW string6 & string7
				{
					stDCSample.wBTStrCurr[STR6] = (int16)(-(stAdcPool.BTStrCurr[STR6].dAddSum /stAdcPool.uwMux8Ch6SumCnt - stAdcPool.BTStrCurr[STR6].wOffset));
					stDCSample.wBTStrCurr[STR7] = stDCSample.wBTCurr[PVD] - stDCSample.wBTStrCurr[STR6];
				}
				else													// 25_33KW string6 & string7
				{
					stDCSample.wBTStrCurr[STR6] = 0;
					stDCSample.wBTStrCurr[STR7] = 0;
				}
			}
			for(stri=0; stri<8; stri++)
			{
				if(stDCSample.wBTStrCurr[stri] < 50)
				{
					stDCSample.wBTStrCurr[stri] = 0;
				}
			}

			stDCSample.wBTStrCurr[STR8] = 0;
			stDCSample.wBTStrCurr[STR9] = 0;
			stDCSample.wBTStrCurr[STR10] = 0;
			stDCSample.wBTStrCurr[STR11] = 0;
		}
		
		else					// XG50-70KW String Current
		{
			stDCSample.wBTStrCurr[STR0] = (int16)(stAdcPool.BTStrCurr[STR0].dAddSum /stAdcPool.uwMux8Ch0SumCnt);  //*100
			stDCSample.wBTStrCurr[STR1] = (int16)(stAdcPool.BTStrCurr[STR1].dAddSum /stAdcPool.uwMux8Ch1SumCnt);
			stDCSample.wBTStrCurr[STR2] = stDCSample.wBTCurr[PVA] - stDCSample.wBTStrCurr[STR0] - stDCSample.wBTStrCurr[STR1];
			stDCSample.wBTStrCurr[STR3] = (int16)(stAdcPool.BTStrCurr[STR2].dAddSum /stAdcPool.uwMux8Ch2SumCnt);
			stDCSample.wBTStrCurr[STR4] = (int16)(stAdcPool.BTStrCurr[STR3].dAddSum /stAdcPool.uwMux8Ch3SumCnt);
			stDCSample.wBTStrCurr[STR5] = stDCSample.wBTCurr[PVB] - stDCSample.wBTStrCurr[STR3] - stDCSample.wBTStrCurr[STR4];
			stDCSample.wBTStrCurr[STR6] = (int16)(stAdcPool.BTStrCurr[STR4].dAddSum /stAdcPool.uwMux8Ch4SumCnt);
			stDCSample.wBTStrCurr[STR7] = (int16)(stAdcPool.BTStrCurr[STR5].dAddSum /stAdcPool.uwMux8Ch5SumCnt);
			stDCSample.wBTStrCurr[STR8] = stDCSample.wBTCurr[PVC] - stDCSample.wBTStrCurr[STR6] - stDCSample.wBTStrCurr[STR7];
			stDCSample.wBTStrCurr[STR9] = (int16)(stAdcPool.BTStrCurr[STR6].dAddSum /stAdcPool.uwMux8Ch6SumCnt);
			stDCSample.wBTStrCurr[STR10] = (int16)(stAdcPool.BTStrCurr[STR7].dAddSum /stAdcPool.uwMux8Ch7SumCnt);
			stDCSample.wBTStrCurr[STR11] = stDCSample.wBTCurr[PVD] - stDCSample.wBTStrCurr[STR9] - stDCSample.wBTStrCurr[STR10];
		}
	}
}

/***********************Inv Model Temperature Sample***************************/
static int16 wNTCTempNewCalc(Uint16 uwAdcTempResult,Uint16 wBranchResistance,Uint32 *cNTCResistanceTable)
{
	Uint16	uwLeftIndexTmp = 0, uwRightIndexTmp = (TEMP_RANGE_MAX-1), uwMediumIndexTmp = 0;
	Uint32	udRegistanceRightValueTmp,udRegistanceLeftValueTmp;
	Uint16	uwResistanceScalePercentTmp,uwResistanceScalePercent2Tmp;
	int16 	wThermalDegreeTmp;
	Uint32	udResistanceTempTmp;

//	udResistanceTempTmp = ((INT32U)uwAdcTempResult * wBranchResistance) / (4095 - uwAdcTempResult);
	udResistanceTempTmp = ((Uint32)wBranchResistance * (4095 - uwAdcTempResult)) /  uwAdcTempResult;
	if(udResistanceTempTmp >= cNTCResistanceTable[0])
		udResistanceTempTmp = cNTCResistanceTable[0];
	if(udResistanceTempTmp <= cNTCResistanceTable[TEMP_RANGE_MAX-1])
		udResistanceTempTmp = cNTCResistanceTable[TEMP_RANGE_MAX-1];

	//Binary search to find NTC Resistance range
	while(uwLeftIndexTmp <= uwRightIndexTmp)
	{
		uwMediumIndexTmp = (uwLeftIndexTmp + uwRightIndexTmp) / 2;
		if(cNTCResistanceTable[uwMediumIndexTmp] > udResistanceTempTmp)
			uwLeftIndexTmp = uwMediumIndexTmp + 1;
		else
		if(cNTCResistanceTable[uwMediumIndexTmp] < udResistanceTempTmp)
			uwRightIndexTmp = uwMediumIndexTmp - 1;
		else
		{
			uwRightIndexTmp = uwMediumIndexTmp;
			uwLeftIndexTmp = uwMediumIndexTmp;
			break;
		}
	}

	if(uwRightIndexTmp != uwLeftIndexTmp)
	{
		if(uwLeftIndexTmp > (TEMP_RANGE_MAX-1))
			uwLeftIndexTmp = (TEMP_RANGE_MAX-1);
		if(uwRightIndexTmp > (TEMP_RANGE_MAX-2))
			uwRightIndexTmp = (TEMP_RANGE_MAX-2);

		udRegistanceRightValueTmp = cNTCResistanceTable[uwLeftIndexTmp];
		udRegistanceLeftValueTmp = cNTCResistanceTable[uwRightIndexTmp];

		uwResistanceScalePercentTmp = (Uint16)((Uint32)(udResistanceTempTmp - udRegistanceRightValueTmp) * 1000
					/ (udRegistanceLeftValueTmp - udRegistanceRightValueTmp));
		uwResistanceScalePercent2Tmp = (1000 - uwResistanceScalePercentTmp);
		wThermalDegreeTmp = ((Uint32)cuwDegree[uwRightIndexTmp] * uwResistanceScalePercentTmp
						+ (Uint32)cuwDegree[uwRightIndexTmp + 1] * uwResistanceScalePercent2Tmp) / 1000;
	}
	else
	{
		if(uwRightIndexTmp > (TEMP_RANGE_MAX-1))
			uwRightIndexTmp = (TEMP_RANGE_MAX-1);
		wThermalDegreeTmp = (INT16S)cuwDegree[uwRightIndexTmp];
	}
	wThermalDegreeTmp = wThermalDegreeTmp - 400;

	return wThermalDegreeTmp;

}


static void SysConfigModel3_6K(Uint16 uwMachineType)
{
	stSysCfg.uwMachineType = MACHINE_ID_3_6KW;
	stSysCfg.uwMachineMin = CONFIG_1K5W;
	stSysCfg.uwMachineMax = CONFIG_7K2W;	
	stSysCfg.uwPVNumber = 2;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;

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
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_3_6K;
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
	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;

	stMachineCfg.wHardInvOCPCap = OCP_CAP_3_6K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_3_6K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_3_6K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_3_6K;
	stMachineCfg.wInvCurrRlyChkMax = CURR_RLYMAX_3_6K;	
	stMachineCfg.wInvTempOver = TEMP_OVER_INV_3_6K;
	stMachineCfg.wBoostTempOver = TEMP_OVER_BST_3_6K;
	stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_3_6K;
	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_3_6K;
	stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_3_6K;
	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_3_6K;
	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_3_6K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_3_6K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_3_6K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_3_6K;
	stMachineCfg.wISOInternalRes = 400;
	stMachineCfg.wISOInternalRes_N = 400;

	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_25_40K;

	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_3_6K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_3_6K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_3_6K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_3_6K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_3_6K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_3_6K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_3_6K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_3_6K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_3_6K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_3_6K;		

	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_3_6K;			
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_3_6K;			
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_3_6K;			

	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_3_6K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_3_6K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_3_6K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_3_6K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_3_6K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_3_6K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_3_6K;	

}
static void SysConfigModel7_9K(Uint16 uwMachineType)
{
	stSysCfg.uwMachineType = MACHINE_ID_7_9KW;
	stSysCfg.uwMachineMin = CONFIG_3K5W;
	stSysCfg.uwMachineMax = CONFIG_10K8W;	
	stSysCfg.uwPVNumber = 2;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;
	

	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_7_9K;
	stSysCfg.wACCurrRatio = AC_CURR_RATIO_7_9K;
	stSysCfg.wDCIRatio = DCI_RATIO_7_9K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_7_9K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_7_9K;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_7_9K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_7_9K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_7_9K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_7_9K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_7_9K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_7_9K;
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_7_9K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_7_9K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_7_9K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_7_9K;
	stSysCfg.wBTCurrLimitMax[PVC] = 0;	
	stSysCfg.wBTCurrLimitMax[PVD] = 0;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_7_9K;
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;        // 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;

	stMachineCfg.wHardInvOCPCap = OCP_CAP_7_9K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_7_9K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_7_9K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_7_9K;
	stMachineCfg.wInvCurrRlyChkMax = CURR_RLYMAX_7_9K;	
	stMachineCfg.wInvTempOver = TEMP_OVER_INV_7_9K;
	stMachineCfg.wBoostTempOver = TEMP_OVER_BST_7_9K;
	stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_7_9K;
	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_7_9K;
	stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_7_9K;
	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_7_9K;
	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_7_9K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_7_9K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_7_9K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_7_9K;
	stMachineCfg.wISOInternalRes = 400;		// Machine Internal Register:400K
	stMachineCfg.wISOInternalRes_N = 400;	// Machine Internal Register(Disconnect N Line):400K

	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_25_40K;

	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_7_9K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_7_9K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_7_9K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_7_9K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_7_9K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_7_9K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_7_9K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_7_9K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_7_9K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_7_9K;		

	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_7_9K;			
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_7_9K;			
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_7_9K;			

	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_7_9K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_7_9K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_7_9K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_7_9K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_7_9K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_7_9K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_7_9K;	


	
}
static void SysConfigModel10_13K(Uint16 uwMachineType)
{
	stSysCfg.uwMachineType = MACHINE_ID_10_13KW;
	stSysCfg.uwMachineMin = CONFIG_5KW;
	stSysCfg.uwMachineMax = CONFIG_15K6W;
	stSysCfg.uwPVNumber = 2;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;

	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_10_13K;
	stSysCfg.wACCurrRatio = AC_CURR_RATIO_10_13K;
	stSysCfg.wDCIRatio = DCI_RATIO_10_13K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_10_13K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_10_13K;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_10_13K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_10_13K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_10_13K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_10_13K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_10_13K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_10_13K;
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_10_13K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_10_13K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_10_13K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_10_13K;
	stSysCfg.wBTCurrLimitMax[PVC] = 0;	
	stSysCfg.wBTCurrLimitMax[PVD] = 0;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_10_13K;
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;        // 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	
	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;
				
	stMachineCfg.wHardInvOCPCap = OCP_CAP_10_13K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_10_13K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_10_13K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_10_13K;
	stMachineCfg.wInvCurrRlyChkMax =  CURR_RLYMAX_10_13K;	
	stMachineCfg.wInvTempOver = TEMP_OVER_INV_10_13K;
	stMachineCfg.wBoostTempOver = TEMP_OVER_BST_10_13K;
	stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_10_13K;
	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_10_13K;
	stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_10_13K;
	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_10_13K;
	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_10_13K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_10_13K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_10_13K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_10_13K;
	stMachineCfg.wISOInternalRes = 400;		// Machine Internal Register:400K
	stMachineCfg.wISOInternalRes_N = 400;	// Machine Internal Register(Disconnect N Line):400K

	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_25_40K;

	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_10_13K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_10_13K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_10_13K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_10_13K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_10_13K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_10_13K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_10_13K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_10_13K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_10_13K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_10_13K;		

	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_10_13K;			
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_10_13K;			
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_10_13K;			

	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_10_13K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_10_13K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_10_13K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_10_13K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_10_13K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_10_13K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_10_13K;	
}

static void SysConfigModel15_25K(Uint16 uwMachineType)
{
	if((MACHINE_ID_15_22KW == uwMachineType)||(MACHINE_ID_PARALLEL_TUBE_15_22KW == uwMachineType))
	{
		if(MACHINE_ID_15_22KW == uwMachineType)
		{
			stSysCfg.uwMachineType = MACHINE_ID_15_22KW;
			stMachineCfg.wEPWMDeadBand = PWM_DBTIME_15_25K;
		}
		else
		{
			stSysCfg.uwMachineType = MACHINE_ID_PARALLEL_TUBE_15_22KW;
			stMachineCfg.wEPWMDeadBand = PWM_DBTIME_PARALLEL_TUBE_15_25K;
		}

	    stSysCfg.uwMachineMin = CONFIG_7K5W;
	    stSysCfg.uwMachineMax = CONFIG_25KW;
	    stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_15_22K;
	    stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_15_22K;
	    stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_15_22K;
	    stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_15_22K;
	    stSysCfg.wBTCurrLimitMax[PVC] = 0;
	    stSysCfg.wBTCurrLimitMax[PVD] = 0;
	    stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_15_22K;
	    // Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	    // Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	    stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;     // 46.8A,1.8*RateCurr
	    stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	    stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;        // 1.8*RateCurr
	    stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	}
	else
	{
		if(MACHINE_ID_25KW == uwMachineType)
		{
			stSysCfg.uwMachineType = MACHINE_ID_25KW;
			stMachineCfg.wEPWMDeadBand = PWM_DBTIME_15_25K;
			stSysCfg.uwMachineMin = CONFIG_12K5W;
	   		stSysCfg.uwMachineMax = CONFIG_30KW;
		    	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_15_22K;
		    	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_25K;
		    	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_15_22K;
		    	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_25K;
		   	 stSysCfg.wBTCurrLimitMax[PVC] = 0;
		    	stSysCfg.wBTCurrLimitMax[PVD] = 0;
		    	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_25K;
		}
		else if(MACHINE_ID_Plus_25_30KW == uwMachineType)
		{
			stSysCfg.uwMachineType = MACHINE_ID_Plus_25_30KW;
			stMachineCfg.wEPWMDeadBand = PWM_DBTIME_15_25K;
			stSysCfg.uwMachineMin = CONFIG_12K5W;
	    		stSysCfg.uwMachineMax = CONFIG_36KW;
			stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_25_30K;
		    	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_25_30K;
		    	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_25_30K;
		    	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_25_30K;
		   	 stSysCfg.wBTCurrLimitMax[PVC] = 0;
		    	stSysCfg.wBTCurrLimitMax[PVD] = 0;
		    	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_25_30K;
			
		}
		else
		{
			stSysCfg.uwMachineType = MACHINE_ID_PARALLEL_TUBE_25KW;
			stMachineCfg.wEPWMDeadBand = PWM_DBTIME_PARALLEL_TUBE_15_25K;
			 stSysCfg.uwMachineMin = CONFIG_7K5W;
	   		 stSysCfg.uwMachineMax = CONFIG_30KW;
			 stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_15_22K;
		    	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_25K;
		    	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_15_22K;
		    	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_25K;
		   	 stSysCfg.wBTCurrLimitMax[PVC] = 0;
		    	stSysCfg.wBTCurrLimitMax[PVD] = 0;
		    	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_25K;
		}

	
	    // Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	    // Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	    stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVB]*1843)>>10;     // 75.6A,1.8*RateCurr
	    stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVB];

	    stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;    // 50.4,1.8*RateCurr
	    stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	}

	stSysCfg.uwPVNumber = 2;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;

	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_15_25K;

	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_15_25K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_15_25K;

	stSysCfg.wNPEVoltRatio = NPE_RATIO_25_40K;//NPE_RATIO_15_25K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_15_25K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_15_25K;
	 if(MACHINE_ID_Plus_25_30KW == uwMachineType)
	 {
		stSysCfg.wStrCurrRatio = STR_CURR_RATIO_25_30K;
		stSysCfg.wStrCurrBias = STR_CURR_BIAS_25_30K;	
		stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX_30k;
		stSysCfg.wACCurrRatio = AC_CURR_RATIO_25_30K;
		stSysCfg.wDCIRatio = DCI_RATIO_25_30K;
		stMachineCfg.wInvCurrRlyChkMax = CURR_RLYMAX_25_30K;
		stSysCfg.wISOVoltRatio = ISO_RATIO_25_40K;
		stMachineCfg.wISOInternalRes = 2564;		// Machine Internal Register:400K
		stMachineCfg.wISOInternalRes_N = 2564;	// Machine Internal Register(Disconnect N Line):400K
		stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_25_30K_PLUS;		
		stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_25_30K_PLUS;

		stMachineCfg.wInvTempOver = TEMP_OVER_INV_25_30K;
		stMachineCfg.wBoostTempOver = TEMP_OVER_BST_25_30K;
		stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_25_30K;
	 	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_25_30K;
	 	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_25_30K;
		stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_25_30K;
		
	 }
	 else
	 {
		stSysCfg.wStrCurrRatio = STR_CURR_RATIO_15_25K;
		stSysCfg.wStrCurrBias = STR_CURR_BIAS_15_25K;
		stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;
		stSysCfg.wACCurrRatio = AC_CURR_RATIO_15_25K;
		stSysCfg.wDCIRatio = DCI_RATIO_15_25K;
		stMachineCfg.wInvCurrRlyChkMax = CURR_RLYMAX_15_25K;	
		stSysCfg.wISOVoltRatio = ISO_RATIO_15_25K;
		stMachineCfg.wISOInternalRes = 2564;		// Machine Internal Register:400K
		stMachineCfg.wISOInternalRes_N = 2564;	// Machine Internal Register(Disconnect N Line):400K
		stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_15_25K;		
		stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_15_25K;

		stMachineCfg.wInvTempOver = TEMP_OVER_INV_15_25K;
		stMachineCfg.wBoostTempOver = TEMP_OVER_BST_15_25K;
		stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_15_25K;
		stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_15_25K;
		stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_15_25K;
		stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_15_25K;
	 }
	
	stMachineCfg.wHardInvOCPCap = OCP_CAP_15_25K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_15_25K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_15_25K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_15_25K;

	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_15_25K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_15_25K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_15_25K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_15_25K;
	//stMachineCfg.wISOInternalRes = 2564;		// Machine Internal Register:400K
	//stMachineCfg.wISOInternalRes_N = 2564;	// Machine Internal Register(Disconnect N Line):400K

	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_15_25K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_15_25K;

	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_15_25K;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_15_25K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_15_25K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_15_25K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_15_25K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_15_25K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_15_25K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_15_25K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_15_25K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_15_25K;

	//stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_15_25K;		
	//stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_15_25K;		

	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_15_25K;			
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_15_25K;			
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_15_25K;			

	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_15_25K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_15_25K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_15_25K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_15_25K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_15_25K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_15_25K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_15_25K;	

}
static void SysConfigModel25_40K(Uint16 uwMachineType)
{
	if(uwMachineType == MACHINE_ID_25_33KW)
	{		
		stSysCfg.uwMachineMin = CONFIG_12K5W;
		stSysCfg.uwMachineMax = CONFIG_39K6W;
		stSysCfg.uwMachineType = MACHINE_ID_25_33KW;	
		stSysCfg.uwPVNumber = 3;
		stMachineCfg.wHardInvOCPCap = OCP_CAP_25_40K;
		stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
		stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;
		stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
		stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;
	}
	else if(uwMachineType == MACHINE_ID_Plus_30_36KW)
	{		
		stSysCfg.uwMachineType = MACHINE_ID_Plus_30_36KW;	
		stSysCfg.uwMachineMin = CONFIG_15KW;
		stSysCfg.uwMachineMax = CONFIG_43K2W;
		stSysCfg.uwPVNumber = 3;
		stMachineCfg.wHardInvOCPCap = OCP_CAP_PLUS_25_40K;
		stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K_Plus;
		stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K_Plus;
		stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX_PLUS;
		stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD_PLUS;
	}
	else if(uwMachineType == MACHINE_ID_Plus_50KW)
	{		
		stSysCfg.uwMachineType = MACHINE_ID_Plus_50KW;	
		stSysCfg.uwMachineMin = CONFIG_25KW;
		stSysCfg.uwMachineMax = CONFIG_60KW;
		stSysCfg.uwPVNumber = 4;
		stMachineCfg.wHardInvOCPCap = OCP_CAP_PLUS_50K_PLUS;
		stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K_Plus;
		stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K_Plus;
	      stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX_PLUS;
		  stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD_PLUS;
	}
	else
	{		
		stSysCfg.uwMachineType = MACHINE_ID_36_40KW;	
		stSysCfg.uwMachineMin = CONFIG_18KW;
		stSysCfg.uwMachineMax = CONFIG_48KW;
		stSysCfg.uwPVNumber = 4;
		stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
		stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;
		stMachineCfg.wHardInvOCPCap = OCP_CAP_25_40K;
		stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
		stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;
		
	}
	

	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_25_40K;
	stSysCfg.wACCurrRatio = AC_CURR_RATIO_25_40K;
	stSysCfg.wDCIRatio = DCI_RATIO_25_40K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_25_40K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_25_40K;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_25_40K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_25_40K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_25_40K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_25_40K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_25_40K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_25_40K;
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_25_40K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_25_40K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_25_40K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_25_40K;
	stSysCfg.wBTCurrLimitMax[PVC] = BTCURR_LIMIT_25_40K;	
	stSysCfg.wBTCurrLimitMax[PVD] = BTCURR_LIMIT_25_40K;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_25_40K;
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;     //46.8,26*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;
	
	//stMachineCfg.wHardInvOCPCap = OCP_CAP_25_40K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_25_40K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_25_40K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_25_40K;
	stMachineCfg.wInvCurrRlyChkMax =  CURR_RLYMAX_25_40K;

	 if(uwMachineType == MACHINE_ID_Plus_50KW)
	 {
	 	stMachineCfg.wInvTempOver = TEMP_OVER_INV_50K_PLUS;
		stMachineCfg.wBoostTempOver = TEMP_OVER_BST_50K_PLUS;
		stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_50K_PLUS;
		stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_50K_PLUS;
		stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_50K_PLUS;
		stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_50K_PLUS;
	 }
	 else
	 {
	 	stMachineCfg.wInvTempOver = TEMP_OVER_INV_25_40K;
		stMachineCfg.wBoostTempOver = TEMP_OVER_BST_25_40K;
		stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_25_40K;
	 	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_25_40K;
	 	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_25_40K;
		stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_25_40K;
	 }

	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_25_40K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_25_40K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_25_40K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_25_40K;
	stMachineCfg.wISOInternalRes = 3304;		// Machine Internal Register:3304K
	stMachineCfg.wISOInternalRes_N = 3304;	// Machine Internal Register(Disconnect N Line):3304K
	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_25_40K;

	//stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
	//stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_25_40K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_25_40K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_25_40K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_25_40K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_25_40K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_25_40K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_25_40K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_25_40K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_25_40K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_25_40K;		


	if(uwMachineType == MACHINE_ID_Plus_50KW)
	 {
		stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_50K_PLUS;			
		stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_50K_PLUS;			
		stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_50K_PLUS;	
	 }
	 else
	 {
		stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_25_40K;			
		stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_25_40K;			
		stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_25_40K;	
	 }
	
	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_25_40K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_25_40K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_25_40K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_25_40K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_25_40K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_25_40K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_25_40K;
}
static void SysConfigModel50_70K(Uint16 uwMachineType)
{
	stSysCfg.uwMachineType = MACHINE_ID_60_70KW;
	stSysCfg.uwMachineMin = CONFIG_25KW;
	stSysCfg.uwMachineMax = CONFIG_70KW;

	stSysCfg.wACCurrRatio = AC_CURR_RATIO_60_70K;
	
	stSysCfg.uwPVNumber = 4;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD;

	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_50_70K;
	stSysCfg.wDCIRatio = DCI_RATIO_50_70K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_50_70K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_50_70;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_50_70K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_50_70K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_50_70K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_50_70K;
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_50_70K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_60_70K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_60_70K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_50_70K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_50_70K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_50_70K;
	stSysCfg.wBTCurrLimitMax[PVC] = BTCURR_LIMIT_50_70K;	
	stSysCfg.wBTCurrLimitMax[PVD] = BTCURR_LIMIT_50_70K;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_50_70K;
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10; //46.8,26*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;
	
	stMachineCfg.wHardInvOCPCap = OCP_CAP_50_70K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_50_70K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_50_70K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_50_70K;
	stMachineCfg.wInvCurrRlyChkMax = CURR_RLYMAX_50_70K;	
	stMachineCfg.wInvTempOver = TEMP_OVER_INV_50_70K;
	stMachineCfg.wBoostTempOver = TEMP_OVER_BST_50_70K;
	stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_50_70K;
	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_50_70K;
	stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_50_70K;
	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_50_70K;
	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_50_70K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_50_70K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_50_70K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_50_70K;
	stMachineCfg.wISOInternalRes = 346;		// Machine Internal Register:346K
	stMachineCfg.wISOInternalRes_N = 346;	// Machine Internal Register(Disconnect N Line):346K

	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_50_70K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_50_70K;

	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_50_70K;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_50_70K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_50_70K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_50_70K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_50_70K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_50_70K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_50_70K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_50_70K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_50_70K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_50_70K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_50_70K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_50_70K;		

	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_50_70K;			
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_50_70K;			
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_50_70K;			

	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_50_70K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_50_70K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_50_70K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_50_70K;

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_50_70K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_50_70K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_50_70K;
}

static void SysConfigModel_Pro_33_50K(Uint16 uwMachineType)
{
	if(uwMachineType == MACHINE_ID_PRO_33_40KW)
	{
		stSysCfg.uwMachineType = MACHINE_ID_PRO_33_40KW;	
		stSysCfg.uwMachineMin = CONFIG_16K5W;
		stSysCfg.uwMachineMax = CONFIG_40KW;
		stSysCfg.uwPVNumber = 3;
		stMachineCfg.wHardInvOCPCap = OCP_CAP_PRO_33_40K_PRO;
	}
	else if(uwMachineType == MACHINE_ID_PRO_50KW || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2))
	{
		stSysCfg.uwMachineType = MACHINE_ID_PRO_50KW;	
		stSysCfg.uwMachineMin = CONFIG_20KW;
		stSysCfg.uwMachineMax = CONFIG_50KW;
		stSysCfg.uwPVNumber = 4;
		stMachineCfg.wHardInvOCPCap = OCP_CAP_PRO_50K_PRO;
	}
    else if(MACHINE_ID_PRO_50KW_2 == stSysCfg.uwMachineType)
    {
        stSysCfg.uwMachineType = MACHINE_ID_PRO_50KW_2;
        stSysCfg.uwMachineMin = CONFIG_40KW;
        stSysCfg.uwMachineMax = CONFIG_50KW;
        stSysCfg.uwPVNumber = 4;
        stMachineCfg.wHardInvOCPCap = OCP_CAP_PRO_50K_PRO;
    }
	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K_Plus;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K_Plus;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX_PRO;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD_PLUS;
	
	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_33_50K;
	stSysCfg.wACCurrRatio = AC_CURR_RATIO_33_50K;
	stSysCfg.wDCIRatio = DCI_RATIO_33_50K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_33_50K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_33_50K;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_33_50K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_33_50K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_33_50K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_33_50K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_33_50K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_33_50K;
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_33_50K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_33_50K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_33_50K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_33_50K;
	stSysCfg.wBTCurrLimitMax[PVC] = BTCURR_LIMIT_33_50K;	
	stSysCfg.wBTCurrLimitMax[PVD] = BTCURR_LIMIT_33_50K;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_33_50K;
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;     //46.8,26*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;
	
	//stMachineCfg.wHardInvOCPCap = OCP_CAP_25_40K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_33_50K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_33_50K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_33_50K;
	stMachineCfg.wInvCurrRlyChkMax = CURR_RLYMAX_33_50K;

	stMachineCfg.wInvTempOver = TEMP_OVER_INV_33_50K_PRO;
	stMachineCfg.wBoostTempOver = TEMP_OVER_BST_33_50K_PRO;
	stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_33_50K_PRO;
	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_33_50K_PRO;
	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_33_50K_PRO;
	stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_33_50K_PRO;

	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_33_50K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_33_50K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_33_50K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_33_50K;
	stMachineCfg.wISOInternalRes = 3304;		// Machine Internal Register:3304K
	stMachineCfg.wISOInternalRes_N = 3304;	// Machine Internal Register(Disconnect N Line):3304K
	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_25_40K;

	//stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
	//stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_33_50K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_33_50K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_33_50K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_33_50K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_33_50K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_33_50K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_33_50K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_33_50K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_33_50K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_33_50K;		


	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_33_50K_PRO;
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_33_50K_PRO;
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_33_50K_PRO;
	
	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_33_50K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_33_50K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_33_50K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_33_50K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_33_50K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_33_50K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_33_50K;
}

static void SysConfigModel_Pro_60K(Uint16 uwMachineType)
{
	stSysCfg.uwMachineType = MACHINE_ID_PRO_60KW;	
	stSysCfg.uwMachineMin = CONFIG_30KW;
	stSysCfg.uwMachineMax = CONFIG_60KW;
	stSysCfg.uwPVNumber = 4;
	stMachineCfg.wHardInvOCPCap = OCP_CAP_PRO_60K_PRO;
	stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K_Plus;
	stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K_Plus;
	stSysCfg.uwPWMBoostDutyMax = PWM_BOOST_DUTY_MAX_PLUS;
	stSysCfg.uwPWMBoostPeriod = PWM_BOOST_PERIOD_PLUS;
	
	stSysCfg.wACVoltRatio = AC_VOLT_RATIO_60K;
	stSysCfg.wACCurrRatio = AC_CURR_RATIO_60K;
	stSysCfg.wDCIRatio = DCI_RATIO_60K;
	stSysCfg.wGFCIRatio = GFCI_CURR_RATIO_60K;
	stSysCfg.wGFCIBias = GFCI_CURR_BIAS_60K;
	stSysCfg.wNPEVoltRatio = NPE_RATIO_60K;

	stSysCfg.wBusVoltRatio = BUS_VOLT_RATIO_60K;
	stSysCfg.wPVVoltRatio = PV_VOLT_RATIO_60K;
	stSysCfg.wBTCurrRatio1 = PV_CURR_RATIO_60K;
	stSysCfg.wBTCurrRatio2 = PV_CURR_RATIO_60K;
	stSysCfg.wStrCurrRatio = STR_CURR_RATIO_60K;
	stSysCfg.wStrCurrBias = STR_CURR_BIAS_60K;
	stSysCfg.wISOVoltRatio = ISO_RATIO_60K;

	stSysCfg.wBTCurrLimitMax[PVA] = BTCURR_LIMIT_60K;		
	stSysCfg.wBTCurrLimitMax[PVB] = BTCURR_LIMIT_60K;
	stSysCfg.wBTCurrLimitMax[PVC] = BTCURR_LIMIT_60K;	
	stSysCfg.wBTCurrLimitMax[PVD] = BTCURR_LIMIT_60K;	
	stSysCfg.wBTCurrRefLimitMax = BTCURR_LIMIT_60K;
	// Hardware_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.4)
	// Software_BoostOverCurr = (BoostCurrent * 1.5(ripple) * 1.2)
	stSysCfg.wBTOverCurrFaultMax = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;		// 1.8*RateCurr
	stSysCfg.wBTOverCurrFaultAvg = stSysCfg.wBTCurrLimitMax[PVA];

	stSysCfg.wBTOverCurrFaultMax1 = ((int32)stSysCfg.wBTCurrLimitMax[PVA]*1843)>>10;     //46.8,26*RateCurr
	stSysCfg.wBTOverCurrFaultAvg1 = stSysCfg.wBTCurrLimitMax[PVA];
	stSysCfg.wPVVoltLimitMax =PV_VOLT_LIMIT_MAX;
	
	//stMachineCfg.wHardInvOCPCap = OCP_CAP_25_40K;
	stMachineCfg.wEPWMDeadBand = PWM_DBTIME_60K;
	stMachineCfg.wRelayPwmCap = RELAY_PWM_60K;
	stMachineCfg.wRelayM0Duty = RELAY_M0_Duty_60K;
	stMachineCfg.wInvCurrRlyChkMax =  CURR_RLYMAX_60K;

	stMachineCfg.wInvTempOver = TEMP_OVER_INV_60K_PRO;
	stMachineCfg.wBoostTempOver = TEMP_OVER_BST_60K_PRO;
	stMachineCfg.wAmbTempOver = TEMP_OVER_AMB_60K_PRO;
	stMachineCfg.wAmbTempDerate = TEMP_DERAT_AMB_60K_PRO;
	stMachineCfg.wInvTempDerate = TEMP_DERAT_INV_60K_PRO;
	stMachineCfg.wBoostTempDerate = TEMP_DERAT_BST_60K_PRO;

	stMachineCfg.wInvBstDerateStep = DRAT_IVBST_STEP_60K;
	stMachineCfg.wInvBstDerateBackStep = DRAT_IVBST_BKSTEP_60K;
	stMachineCfg.wAmbDerateStep = DRAT_AMB_STEP_60K;
	stMachineCfg.wAmbDerateBackStep = DRAT_AMB_BKSTEP_60K;
	stMachineCfg.wISOInternalRes = 3304;		// Machine Internal Register:3304K
	stMachineCfg.wISOInternalRes_N = 3304;	// Machine Internal Register(Disconnect N Line):3304K
	stMachineCfg.wIntrFan4SetFault = FAN4_INTR_PERIOD_FAULT_25_40K;
	stMachineCfg.wIntrFan4ClrFault = FAN4_INTR_PERIOD_RECORVE_25_40K;

	//stMachineCfg.wExtrFan123SetFault = FAN123_EXTR_PERIOD_FAULT_25_40K;
	//stMachineCfg.wExtrFan123ClrFault = FAN123_EXTR_PREIOD_RECORVE_25_40K;

	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp = BUSSOFT_CNTL_Kp_60K;
	stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi = BUSSOFT_CNTL_Ki_60K;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVB].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVC].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKp = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKp;
	stBusVoltSoftStartCtrl[PVD].stPID.stIn.dKi = stBusVoltSoftStartCtrl[PVA].stPID.stIn.dKi;
	
	stPVVoltCtrl[PVA].stPID.stIn.dKp = PVVOLT_CNTL_Kp_60K;
	stPVVoltCtrl[PVA].stPID.stIn.dKi = PVVOLT_CNTL_Ki_60K;
	stPVVoltCtrl[PVB].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVB].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVC].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVC].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	stPVVoltCtrl[PVD].stPID.stIn.dKp = stPVVoltCtrl[PVA].stPID.stIn.dKp;
	stPVVoltCtrl[PVD].stPID.stIn.dKi = stPVVoltCtrl[PVA].stPID.stIn.dKi ;
	
	stBTCurrCtrl[PVA].stPID.stIn.dKp = BTCURR_CNTL_Kp_60K;
	stBTCurrCtrl[PVA].stPID.stIn.dKi = BTCURR_CNTL_Ki_60K;
	stBTCurrCtrl[PVB].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVB].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVC].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVC].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	stBTCurrCtrl[PVD].stPID.stIn.dKp = stBTCurrCtrl[PVA].stPID.stIn.dKp;
	stBTCurrCtrl[PVD].stPID.stIn.dKi = stBTCurrCtrl[PVA].stPID.stIn.dKi;
	
	stActivePowerCtrl.stPID.stIn.dKp = ACTPOWR_CNTL_Kp_60K;
	stActivePowerCtrl.stPID.stIn.dKi = ACTPOWR_CNTL_Ki_60K;

	stInvVoltCtrl.stIn.dVoltHighR_Kd = IVVOLT_CNTL_HiRKd_60K;		
	stInvVoltCtrl.stIn.dVoltFstDiff_Kd = IVVOLT_CNTL_FstDKd_60K;		


	stInvCurrCtrl.stIn.dCurr_Kd = INVCURR_CNTL_Kd_60K_PRO;
	stInvCurrCtrl.stIn.dCurr_Kq = INVCURR_CNTL_Kq_60K_PRO;
	stInvCurrCtrl.stIn.dCurr_Kpi = INVCURR_CNTL_Kpi_60K_PRO;
	
	stInvBusVoltCtrl.stIn.dKp_H = IVBUSVOLT_CNTL_Kp_H_60K;		
	stInvBusVoltCtrl.stIn.dKi_H = IVBUSVOLT_CNTL_Ki_H_60K;
	stInvBusVoltCtrl.stIn.dKp_L = IVBUSVOLT_CNTL_Kp_L_60K;		
	stInvBusVoltCtrl.stIn.dKi_L = IVBUSVOLT_CNTL_Ki_L_60K;	

	stInvCurrCtrl.stIn.dReCtrl_KPR= INVCURR_CNTL_KPR_60K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR1 = INVCURR_CNTL_KPR1_60K;			
	stInvCurrCtrl.stIn.dReCtrl_KPR2 = INVCURR_CNTL_KPR2_60K;
}


void ReadMachineModel(void)
{
	static Uint16 suwMachine1CheckCnt=0,uwModelTempOld=0;
	Uint16 uwModelTemp;
	static Uint16	suwMachineCheckFailed=0;
	/**************Inverter Machine Model Read*****************************/

	if(FALSE==unSysFlag.bit.MachineCheckEnd)
	{
		uwModelTemp = MACHINE_TYPE;
//		uwModelTemp = MACHINE_ID_25_33KW;
        if(uwModelTemp == MACHINE_ID_PRO_50KW_2)
        {
            uwModelTemp = MACHINE_ID_PRO_50KW;
        }
		if(uwModelTemp == uwModelTempOld)
			suwMachine1CheckCnt++;
		else
			suwMachine1CheckCnt = 0;
		uwModelTempOld = uwModelTemp;

		if(suwMachine1CheckCnt >= 100)
		{
			suwMachine1CheckCnt = 0;
			suwMachineCheckFailed = 0;
			switch(uwModelTemp)
			{
				case MACHINE_ID_3_6KW:
					SysConfigModel3_6K(uwModelTemp);
					break;
				case MACHINE_ID_7_9KW:
					SysConfigModel7_9K(uwModelTemp);
					break;
				case MACHINE_ID_10_13KW:
					SysConfigModel10_13K(uwModelTemp);
					break;
				case MACHINE_ID_15_22KW:
				case MACHINE_ID_PARALLEL_TUBE_15_22KW:
				case MACHINE_ID_25KW:
				case MACHINE_ID_PARALLEL_TUBE_25KW:
				case MACHINE_ID_Plus_25_30KW:
					SysConfigModel15_25K(uwModelTemp);
					break;
				case MACHINE_ID_25_33KW:					
				case MACHINE_ID_36_40KW:
				case MACHINE_ID_Plus_30_36KW:
				case MACHINE_ID_Plus_50KW:
					SysConfigModel25_40K(uwModelTemp);
					break;
				case MACHINE_ID_50KW:
				case MACHINE_ID_60_70KW:
					SysConfigModel50_70K(uwModelTemp);
					break;
				case MACHINE_ID_PRO_33_40KW:
				case MACHINE_ID_PRO_50KW:
				case MACHINE_ID_PRO_50KW_2:
					SysConfigModel_Pro_33_50K(uwModelTemp);
					break;
				case MACHINE_ID_PRO_60KW:
					SysConfigModel_Pro_60K(uwModelTemp);
					break;
				default:
					SysConfigModel3_6K(uwModelTemp);
					suwMachineCheckFailed = 1;
					break;
			}
			
			/**************If the model identification fails, continue to identify*************/
			if(1==suwMachineCheckFailed)
			{
				unSysFlag.bit.MachineCheckEnd = 0;
			}
			else
			{
				CfgInvPwmDB();
				INV_OCP(stMachineCfg.wHardInvOCPCap);
				CfgBoostPwmInterlace(stSysCfg.uwPVNumber);
				unSysFlag.bit.MachineCheckEnd = 1;
			}
		}
	}
}


void ControlParaAdjust(void)
{
	if(MACHINE_ID_60_70KW==stSysCfg.uwMachineType)
	{
			
	}
}




