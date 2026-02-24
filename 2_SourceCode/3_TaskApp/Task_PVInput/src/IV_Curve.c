#include "dsp_user_include.h"

 IV_CURVE stIVCurve;
ST_PV_CURVE_ANALY_CTRL stPvCurveAnalyCtrl[PV_MAX_NUM];
void ClearPVCurveAnalyCtrlPara(Uint16 uwMpptID_s);
void m_PVVoltRecover(void);
void IVCurve(Uint16 uwPVIndex);

/****************************************************************************
*
*	FunctionName:	IVCurve()
*	Input:	none
*	Output:	none
*	Description:	IV Curve
*	Note:	none.
***************************************************************/
void IVCurve(Uint16 uwPVIndexTmp)
{
	static Uint16 swIntialFlag = 1;
	//Uint16 uwPVIndexTmp = 0;
	Uint16 uwCurveAnalyRunEndFlag = 0;
	
	if(stSysCfg.uwPVNumber < 3)
	{
		if (PARALLEL == stSysCfg.eMpptMode)//parallel mppt
		{
			stPvCurveAnalyCtrl[PVA].stOut.eIVState = START;
			stPvCurveAnalyCtrl[PVB].stOut.eIVState = START;
		}
		else
		{
			if(0 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)
			{
				stPvCurveAnalyCtrl[PVA].stOut.eIVState = START;
				stPvCurveAnalyCtrl[PVB].stOut.eIVState = STOP;
			}
			else if(1 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)
			{
				stPvCurveAnalyCtrl[PVA].stOut.eIVState = STOP;
				stPvCurveAnalyCtrl[PVB].stOut.eIVState = START;
			}
		}
		
		if((1 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(1 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd))
		{
			uwCurveAnalyRunEndFlag =1;	
		}
		else
		{
			uwCurveAnalyRunEndFlag =0;	
		}
	}
	else if(stSysCfg.uwPVNumber < 4)
	{
		if (PARALLEL == stSysCfg.eMpptMode)//parallel mppt
		{
			stPvCurveAnalyCtrl[PVA].stOut.eIVState = START;
			stPvCurveAnalyCtrl[PVB].stOut.eIVState = START;
			stPvCurveAnalyCtrl[PVC].stOut.eIVState = START;
		}
		else
		{
			if((0 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(0 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd))
			{
				stPvCurveAnalyCtrl[PVA].stOut.eIVState = START;
				stPvCurveAnalyCtrl[PVB].stOut.eIVState = START;
				stPvCurveAnalyCtrl[PVC].stOut.eIVState = STOP;
			}
			else if((1 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(1 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd))
			{
				stPvCurveAnalyCtrl[PVA].stOut.eIVState = STOP;
				stPvCurveAnalyCtrl[PVB].stOut.eIVState = STOP;
				stPvCurveAnalyCtrl[PVC].stOut.eIVState = START;
			}
		}
		
		if((1 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(1 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd)
			&&(1 == stPvCurveAnalyCtrl[PVC].stOut.uwCurveAnalyRunEnd))
		{
			uwCurveAnalyRunEndFlag =1;	
		}
		else
		{
			uwCurveAnalyRunEndFlag =0;	
		}
	}
	else if(stSysCfg.uwPVNumber < 5)
	{
		if (PARALLEL == stSysCfg.eMpptMode)//parallel mppt
		{
			stPvCurveAnalyCtrl[PVA].stOut.eIVState = START;
			stPvCurveAnalyCtrl[PVB].stOut.eIVState = START;
			stPvCurveAnalyCtrl[PVC].stOut.eIVState = START;
			stPvCurveAnalyCtrl[PVD].stOut.eIVState = START;
		}
		else
		{
			if((0 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(0 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd))
			{
				stPvCurveAnalyCtrl[PVA].stOut.eIVState = START;
				stPvCurveAnalyCtrl[PVB].stOut.eIVState = START;
				stPvCurveAnalyCtrl[PVC].stOut.eIVState = STOP;
				stPvCurveAnalyCtrl[PVD].stOut.eIVState = STOP;
			}
			else if((1 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(1 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd))
			{
				stPvCurveAnalyCtrl[PVA].stOut.eIVState = STOP;
				stPvCurveAnalyCtrl[PVB].stOut.eIVState = STOP;
				stPvCurveAnalyCtrl[PVC].stOut.eIVState = START;
				stPvCurveAnalyCtrl[PVD].stOut.eIVState = START;
			}
		}
		
		if((1 == stPvCurveAnalyCtrl[PVA].stOut.uwCurveAnalyRunEnd)&&(1 == stPvCurveAnalyCtrl[PVB].stOut.uwCurveAnalyRunEnd)
			&&(1 == stPvCurveAnalyCtrl[PVC].stOut.uwCurveAnalyRunEnd)&&(1 == stPvCurveAnalyCtrl[PVD].stOut.uwCurveAnalyRunEnd))
		{
			uwCurveAnalyRunEndFlag =1;	
		}
		else
		{
			uwCurveAnalyRunEndFlag =0;	
		}
	}
	else
	{
		uwCurveAnalyRunEndFlag =0;
	}

	if((cInverterStatus != eInverterStatus)&&(1 == stIVCurve.unIVCurveBits.bit.StartEnable))//during IV curve,Inverter fault
	{
		stIVCurve.unIVCurveBits.bit.StartEnable  = 0;
		swIntialFlag = 1;
		//stIVCurve.unIVCurveBits.bit.EndFlag = 1;
		stIVCurve.CurveAnalyEndFlag = 0;
		uwCurveAnalyRunEndFlag =0;
	}
	
	if((Runing == stMpptDisturb[PVA].eTrackStatus)||(Runing == stMpptDisturb[PVB].eTrackStatus)
		||(Runing == stMpptDisturb[PVC].eTrackStatus)||(Runing == stMpptDisturb[PVD].eTrackStatus))
  	{		   
		if(stF107Data.unEnable.bit.LVRT == 0)
		{	
			if(1 ==uwCurveAnalyRunEndFlag)
			{
				m_PVVoltRecover();//PV Volt recover to Mppt volt
			}
			else
			{
				if (PARALLEL == stSysCfg.eMpptMode)//parallel mppt
				{
					for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
					{
						stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fPvVoltAvg =stDCSample.wPVVoltAvg[PVA];// wPvVolt[PVA];//PV volt
						stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fPvCurrAvg = stDCSample.wBTCurr[PVA];//PV cur
						stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwOverInput = stIVCurve.bOverInput[PVA];//overload flag
						if(stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwIntialflag == 1)
						{
							stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwIntialflag = 0;
							stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPVVoltCurveRef = stPvCurveAnalyCtrl[PVA].stIn.fPvVoltAvg;
							stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fMpptVolt = stPvCurveAnalyCtrl[PVA].stIn.fPvVoltAvg;//record pv volt
						}
						//stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.eIVState = START;
						m_PVCurveAnalyCtrl(&stPvCurveAnalyCtrl[uwPVIndexTmp]);//PV curve
						stMpptDisturb[uwPVIndexTmp].wPVVoltRef = stPvCurveAnalyCtrl[PVA].stOut.fPVVoltCurveRef;
						if(stDCSample.wPVVoltAvg[uwPVIndexTmp]< cBus220V)
						{
							stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.uwCurveAnalyRunEnd = 1;
						}
					}
				}
				else//independent mppt
				{
					//for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
					{
						//if(stMpptTskCtrl.uwBTOnOff[uwPVIndexTmp]==FALSE)//no pv ,end IV curve 	
						if(stDCSample.wPVVoltAvg[uwPVIndexTmp]< cBus220V)
						{
							stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.uwCurveAnalyRunEnd = 1;
						}
						stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fPvVoltAvg = stDCSample.wPVVoltAvg[uwPVIndexTmp];//PV volt
						stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fPvCurrAvg = stDCSample.wBTCurr[uwPVIndexTmp];//PV cur
						stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwOverInput = stIVCurve.bOverInput[uwPVIndexTmp];//overload flag
						if(stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwIntialflag == 1)
						{
							stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwIntialflag = 0;
							stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPVVoltCurveRef = stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fPvVoltAvg;
							stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fMpptVolt = stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fPvVoltAvg;//record pv volt
						}
						m_PVCurveAnalyCtrl(&stPvCurveAnalyCtrl[uwPVIndexTmp]);//PV curve
						stMpptDisturb[uwPVIndexTmp].wPVVoltRef = stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPVVoltCurveRef;
						
					}
				}
				
			}
		}				
  	}
	else
	{	
	    stIVCurve.cLvtMpptDelayCnt=0;
	}
}

void m_PVVoltRecover(void)
{
	Uint16 uwPVIndexTmp = 0;
	Uint16 uwIntiaSetFlag = 0;

	if(stSysCfg.uwPVNumber < 3)
	{
		if(((stMpptDisturb[PVA].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVA].stIn.fMpptVolt - cBus50V))//
		 &&((stMpptDisturb[PVB].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVB].stIn.fMpptVolt - cBus50V)))
		{
			uwIntiaSetFlag =1;	
		}
		else
		{
			uwIntiaSetFlag =0;	
		}
	}
	else if(stSysCfg.uwPVNumber < 4)
	{
		if(((stMpptDisturb[PVA].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVA].stIn.fMpptVolt - cBus50V))//
		 &&((stMpptDisturb[PVB].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVB].stIn.fMpptVolt - cBus50V))//
	 	&&((stMpptDisturb[PVC].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVC].stIn.fMpptVolt - cBus50V)))
		{
			uwIntiaSetFlag =1;	
		}
		else
		{
			uwIntiaSetFlag =0;	
		}
	}
	else if(stSysCfg.uwPVNumber < 5)
	{
		if(((stMpptDisturb[PVA].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVA].stIn.fMpptVolt - cBus50V))//
		 &&((stMpptDisturb[PVB].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVB].stIn.fMpptVolt - cBus50V))//
	 	&&((stMpptDisturb[PVC].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVC].stIn.fMpptVolt - cBus50V))
	 	&&((stMpptDisturb[PVD].wPVVoltRef) >= (stPvCurveAnalyCtrl[PVD].stIn.fMpptVolt - cBus50V)))
		{
			uwIntiaSetFlag =1;	
		}
		else
		{
			uwIntiaSetFlag =0;	
		}
	}
	else
	{
		uwIntiaSetFlag =0;
	}

	
	stIVCurve.CurveMpptRecover++;
	if(stIVCurve.CurveMpptRecover > 100)//  wait for MPPT recover
	{
		stIVCurve.CurveMpptRecoverCnt ++;
		stIVCurve.CurveMpptRecover = 0;
		for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
		{
			if(stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fMpptVolt > cBus220V)
			{
				stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPvVoltCurveStep = (stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.fMpptVolt - cBus220V) * 0.1;//recover mppt step
				stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPVVoltCurveRef = stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPVVoltCurveRef + stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPvVoltCurveStep;
				stMpptDisturb[uwPVIndexTmp].wPVVoltRef= stPvCurveAnalyCtrl[uwPVIndexTmp].stOut.fPVVoltCurveRef;
			}
		}
	}
	if(1 ==uwIntiaSetFlag)
	{
		for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
		{
			stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwIntialflag = 1;
		}
		//stIVCurve.CurveAnalyStartEnable = 0;
		stIVCurve.CurveAnalyEndFlag = 1;
		stIVCurve.CurveMpptRecover = 0;
		stIVCurve.unIVCurveBits.bit.StartEnable  = 0;
	}
	else if(stIVCurve.CurveMpptRecoverCnt > 10)
	{
		for(uwPVIndexTmp=0; uwPVIndexTmp<stSysCfg.uwPVNumber; uwPVIndexTmp++)
		{
			stPvCurveAnalyCtrl[uwPVIndexTmp].stIn.uwIntialflag = 1;
		}
		//stIVCurve.CurveAnalyStartEnable = 0;
		stIVCurve.CurveAnalyEndFlag = 1;
		stIVCurve.CurveMpptRecover = 0;
		stIVCurve.CurveMpptRecoverCnt = 0;
		stIVCurve.unIVCurveBits.bit.StartEnable  = 0;
	}

}
void m_PVCurveParaReset(ST_PV_CURVE_ANALY_CTRL  *pstPvCurveAnalyCtrl)
{
	pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunEnd = 0;
	pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt = 0;
	pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunCnt = 0;
	pstPvCurveAnalyCtrl->stOut.uwWaitcnt = 0;
	pstPvCurveAnalyCtrl->stOut.uwMpptcnt = 0;
	pstPvCurveAnalyCtrl->stIn.uwIntialflag = 1;
	pstPvCurveAnalyCtrl->stOut.eIVState = STOP;
}

void m_PVCurveAnalyCtrl(ST_PV_CURVE_ANALY_CTRL  *pstPvCurveAnalyCtrl)
{
	Uint16 uwIndexTmp;
	 Uint16  bOverLoad;

	 if(0 == pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunEnd)
	 {
		 if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt <= 1250)//wait for pv volt to open volt
		 {
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt++;
			 pstPvCurveAnalyCtrl->stOut.uwMpptcnt++;
			 if(pstPvCurveAnalyCtrl->stOut.uwMpptcnt > 50)//0.5S
			 {
				 pstPvCurveAnalyCtrl->stOut.uwMpptcnt = 0;
				 if(pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef < pstPvCurveAnalyCtrl->stIn.fPvVoltAvg + cBus30V)
				 {
					 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef + cBus30V;
				 }
			 }
			 if( pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef > cBus950V)//pv volt limit
			 {
				 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = cBus950V;
			 }
			 pstPvCurveAnalyCtrl->stOut.fCurveOpenVolt = pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef;
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex = 100;
		 }
		 else if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt == 1251)//reset ctrl para
		 {
			// sPvCtrlInitial();
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt++;
			// pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = pstPvCurveAnalyCtrl->stIn.fPvVoltAvg;
		 }
		 else if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt <= 1500)//wait for 2.5s
		 {
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt++;
			// pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = pstPvCurveAnalyCtrl->stIn.fPvVoltAvg;
		 }
		 else if(pstPvCurveAnalyCtrl->stOut.eIVState == STOP)
	 	{
	 		
	 	}
		 else if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt == 1501)//15s in total
		 {
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt++;
			 pstPvCurveAnalyCtrl->stOut.fPvVoltCurveStep = (pstPvCurveAnalyCtrl->stIn.fPvVoltAvg - cBus250V) * 0.01;//CurveStep
			 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef  = pstPvCurveAnalyCtrl->stIn.fPvVoltAvg - pstPvCurveAnalyCtrl->stOut.fPvVoltCurveStep;
			 pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[100-1] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvVoltAvg);
			 pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[100-1] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvCurrAvg);
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex = 100-2;
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunCnt = 0;
		 }
		 else
		 {
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt = 1502;
			 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunCnt++;

			 if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunCnt >= 100)
			 {
				 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunCnt = 0;
				 if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex >= 2)
				 {
					 if((TRUE == pstPvCurveAnalyCtrl->stIn.uwOverInput)||(bOverLoad != 0))//over load stop VI curve
					 {
						 //pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunEnd = 1;
						 for(uwIndexTmp = 0; uwIndexTmp <= pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex; uwIndexTmp++)//reset para
						 {
							 pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[uwIndexTmp] = (Uint16)(pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex + 1]);
							 pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[uwIndexTmp] = (Uint16)(pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex + 1]);
						 }
					 }
					 if(pstPvCurveAnalyCtrl->stIn.fPvVoltAvg > (pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex +1]))//error data judgement
					 {
						 pstPvCurveAnalyCtrl->stOut.uwWaitcnt++;
						 if(pstPvCurveAnalyCtrl->stOut.uwWaitcnt == 2)//use last data Instead of error data
						 {
							 pstPvCurveAnalyCtrl->stOut.uwWaitcnt = 0;
							 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef -= pstPvCurveAnalyCtrl->stOut.fPvVoltCurveStep;
							 if(pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef < cBus250V)
							 {
								 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = cBus250V;
							 }
							 pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex +1]);
							 pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex + 1]);
							 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex--;
						 }
					 }
					 else
					 {
						 pstPvCurveAnalyCtrl->stOut.uwWaitcnt = 0;
						 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef -= pstPvCurveAnalyCtrl->stOut.fPvVoltCurveStep;
						 if(pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef < cBus250V)//PV Volt Limit 250V
						 {
							 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = cBus250V;
						 }
						 pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvVoltAvg);//record PV data
						 pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvCurrAvg);
						 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex--;
					 }
				 }
				 else if(pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex == 1)
				 {
					 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = cBus250V;//volt limit 250V
					 pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvVoltAvg);
					 pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvCurrAvg);
					 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex--;
				 }
				 else
				 {
					 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunEnd = 1;
					 pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = cBus250V;
					 pstPvCurveAnalyCtrl->stOut.uwPvVoltCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvVoltAvg);
					 pstPvCurveAnalyCtrl->stOut.uwPvCurrCurve[pstPvCurveAnalyCtrl->stOut.uwCurveAnalyIndex] = (Uint16)(pstPvCurveAnalyCtrl->stIn.fPvCurrAvg);
				 }
			 }
		 }
	 }
	 else
	 {
	 	pstPvCurveAnalyCtrl->stOut.fPVVoltCurveRef = pstPvCurveAnalyCtrl->stOut.fCurveOpenVolt;
		 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyStartCnt = 0;//clear all cnt
		 pstPvCurveAnalyCtrl->stOut.uwCurveAnalyRunCnt = 0;
		 pstPvCurveAnalyCtrl->stOut.uwWaitcnt = 0;
		 pstPvCurveAnalyCtrl->stOut.uwMpptcnt = 0;
	 }

 }

void ClearPVCurveAnalyCtrlPara(Uint16 uwMpptID_s)
{
	memset((void*)&stPvCurveAnalyCtrl[uwMpptID_s].stOut,0,sizeof(stPvCurveAnalyCtrl[uwMpptID_s].stOut));
	stPvCurveAnalyCtrl[uwMpptID_s].stIn.uwIntialflag = 1;
}



