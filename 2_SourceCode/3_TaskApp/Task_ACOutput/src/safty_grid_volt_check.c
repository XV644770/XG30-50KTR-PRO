/*
 * safty_grid_volt_check.c
 *
 *  Created on: 2021年1月9日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

static void GridVoltOverUnderProtection(void)
{
	Uint16 uwGridRVoltTmp,uwGridSVoltTmp,uwGridTVoltTmp;
	Uint16 uwGridOverVolt1Tmp,uwGridOverVolt2Tmp,uwGridOverVoltRecorverTmp;
	Uint16 uwGridUnderVolt1Tmp,uwGridUnderVolt2Tmp,uwGridUnderVolt3Tmp,uwGridUnderVoltRecorverTmp;

	static Uint16 suwGridUnderVolt3Cnt,suwGridUnderVolt2Cnt,suwGridUnderVolt1Cnt;
	static Uint16 suwGridOverVolt1Cnt,suwGridOverVolt2Cnt=0,uwGridFaultRecoverCnt;

	static Uint16 uwOPVFaultCnt2_AT = 0;
	static Uint16 wOPVFaultCnt=0;
	
	if(DISABLE == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// actually, N wire is connected
	{
	    if((stautotest.uwStep  == AT_1P15Un_R ||stautotest.uwStep  == AT_1P15Un_S ||stautotest.uwStep  == AT_1P15Un_T ||
		  stautotest.uwStep  == AT_0P85Un_R ||stautotest.uwStep  == AT_0P85Un_S||stautotest.uwStep  == AT_0P85Un_T||
		  stautotest.uwStep  == AT_1P10Un_R ||stautotest.uwStep  == AT_1P10Un_S|| stautotest.uwStep  == AT_1P10Un_T||
		  stautotest.uwStep  == AT_0P40Un_R ||stautotest.uwStep  == AT_0P40Un_S||stautotest.uwStep  == AT_0P40Un_T) 
		  &&(cInverterStatus == eInverterStatus))
	    {
	    	 stautotest.uwFaultValue = 0;
		 stautotest.uwBreakTime = 0;//实际保护时间
		 stautotest.uwBreakTimeAdj = 0;

		  if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T))
		  {
			uwGridOverVolt2Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridOverVolt2Tmp = (Uint32)stGridProtect.uwACOverVolt2*591>>10;
		  }
		   if((stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T))
		  {
			uwGridOverVolt1Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridOverVolt1Tmp = (Uint32)stGridProtect.uwACOverVolt1*591>>10;
		  }
		    if((stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T))
		  {
			uwGridUnderVolt1Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridUnderVolt1Tmp = (Uint32)stGridProtect.uwACUnderVolt1*591>>10;
		  }
		    if((stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
		  {
			uwGridUnderVolt2Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridUnderVolt2Tmp = (Uint32)stGridProtect.uwACUnderVolt2*591>>10;
		  }
	    }
	    else
	    {
		/**************Grid Phase voltage out of range**********************/
		uwGridOverVolt1Tmp = (Uint32)stGridProtect.uwACOverVolt1*591>>10;		// 1024 / 591 = 1.732
		uwGridOverVolt2Tmp = (Uint32)stGridProtect.uwACOverVolt2*591>>10;
		uwGridUnderVolt1Tmp = (Uint32)stGridProtect.uwACUnderVolt1*591>>10;
		uwGridUnderVolt2Tmp = (Uint32)stGridProtect.uwACUnderVolt2*591>>10;
        uwGridUnderVolt3Tmp = (Uint32)stGridProtect.uwACUnderVolt3*591>>10;
	    }
		uwGridRVoltTmp = stACSample.PhaseVoltRms.wRN;
		uwGridSVoltTmp = stACSample.PhaseVoltRms.wSN;
		uwGridTVoltTmp = stACSample.PhaseVoltRms.wTN;	  

		uwGridOverVoltRecorverTmp = uwGridOverVolt1Tmp - 50;		// Over voltage Recorver Value
		uwGridUnderVoltRecorverTmp = uwGridUnderVolt1Tmp + 50;
	}
	else														// N wire is disconnected,software virtual n
	{
	    if((stautotest.uwStep  == AT_1P15Un_R ||stautotest.uwStep  == AT_1P15Un_S ||stautotest.uwStep  == AT_1P15Un_T ||
		  stautotest.uwStep  == AT_0P85Un_R ||stautotest.uwStep  == AT_0P85Un_S||stautotest.uwStep  == AT_0P85Un_T||
		  stautotest.uwStep  == AT_1P10Un_R ||stautotest.uwStep  == AT_1P10Un_S|| stautotest.uwStep  == AT_1P10Un_T||
		  stautotest.uwStep  == AT_0P40Un_R ||stautotest.uwStep  == AT_0P40Un_S||stautotest.uwStep  == AT_0P40Un_T) \
		  &&(cInverterStatus == eInverterStatus))
	    {
	    		stautotest.uwFaultValue = 0;
			stautotest.uwBreakTime = 0;//实际保护时间
			stautotest.uwBreakTimeAdj = 0;
		/**************Grid Line voltage out of range**********************/
		if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T))
		  {
			uwGridOverVolt2Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridOverVolt2Tmp = stGridProtect.uwACOverVolt2;
		  }
		    if((stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T))
		  {
			uwGridOverVolt1Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridOverVolt1Tmp = stGridProtect.uwACOverVolt1;
		  }
		     if((stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T))
		  {
			uwGridUnderVolt1Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridUnderVolt1Tmp = stGridProtect.uwACUnderVolt1;
		  }
		  if((stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
		  {
			uwGridUnderVolt2Tmp = (Uint16)stautotest.uwVFValue;	
		  }
		  else
		  {
			uwGridUnderVolt2Tmp = stGridProtect.uwACUnderVolt2;
		  }
	    }
	    else
	    {
		uwGridOverVolt1Tmp = stGridProtect.uwACOverVolt1;
		uwGridOverVolt2Tmp = stGridProtect.uwACOverVolt2;
		uwGridUnderVolt1Tmp = stGridProtect.uwACUnderVolt1;
		uwGridUnderVolt2Tmp = stGridProtect.uwACUnderVolt2;
        uwGridUnderVolt3Tmp = stGridProtect.uwACUnderVolt3;
	    }
		uwGridRVoltTmp = stACSample.LineVoltRms.wRS;
		uwGridSVoltTmp = stACSample.LineVoltRms.wST;
		uwGridTVoltTmp = stACSample.LineVoltRms.wTR;

		uwGridOverVoltRecorverTmp = uwGridOverVolt1Tmp - 87;		// Over voltage Recorver Value
		uwGridUnderVoltRecorverTmp = uwGridUnderVolt1Tmp + 87;
	}

	/**************Grid Phase voltage out of range checking**********************/
	if((1 == stSysFaultReg.unGridVoltFault.bit.OverVolt1)
	|| (1 == stSysFaultReg.unGridVoltFault.bit.OverVolt2)
	|| (1 == stSysFaultReg.unGridVoltFault.bit.UnderVolt1)
	|| (1 == stSysFaultReg.unGridVoltFault.bit.UnderVolt2)
    || (1 == stSysFaultReg.unGridVoltFault.bit.UnderVolt3))
	{
		if((uwGridRVoltTmp <= uwGridOverVoltRecorverTmp)
		&& (uwGridSVoltTmp <= uwGridOverVoltRecorverTmp)
		&& (uwGridTVoltTmp <= uwGridOverVoltRecorverTmp)
		&& (uwGridRVoltTmp >= uwGridUnderVoltRecorverTmp)
		&& (uwGridSVoltTmp >= uwGridUnderVoltRecorverTmp)
		&& (uwGridTVoltTmp >= uwGridUnderVoltRecorverTmp))
		{
			uwGridFaultRecoverCnt++;
			if(uwGridFaultRecoverCnt > CLR_FAULT_TIME)//20)
			{
				uwGridFaultRecoverCnt=0;

				stSysFaultReg.unGridVoltFault.bit.OverVolt1 = 0;
				stSysFaultReg.unGridVoltFault.bit.OverVolt2 = 0;
				stSysFaultReg.unGridVoltFault.bit.UnderVolt1 = 0;
				stSysFaultReg.unGridVoltFault.bit.UnderVolt2 = 0;
                stSysFaultReg.unGridVoltFault.bit.UnderVolt3 = 0;
			}
		}
		else
		{
			uwGridFaultRecoverCnt = 0;
		}
	       if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T)
		   	||(stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T)
		   	||(stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T)
		   	||(stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
	        {
	            stautotest.wBreakCnt = 0;
	        }
	}
	else
	{
		if((STRANDARD_ITALY_CEIO16 == stDspReceData.unSafetyOdm.bit.SafetyStandard)&&stautotest.uwEnableFlag == ENABLE&&(cInverterStatus == eInverterStatus))
		{				
	             if((uwGridRVoltTmp >= uwGridOverVolt2Tmp) ||(uwGridSVoltTmp >= uwGridOverVolt2Tmp) || (uwGridTVoltTmp >= uwGridOverVolt2Tmp))
	            {
	                if((stautotest.uwStep == AT_1P15Un_R)||(stautotest.uwStep == AT_1P15Un_S)||(stautotest.uwStep == AT_1P15Un_T))
	                {
	                    uwOPVFaultCnt2_AT++;
	                    if(uwOPVFaultCnt2_AT > 2)         // stop  adding the  suppose   wAutoTestVFVaulue
	                    {
	                        stautotest.uwStopShiftFlag = 1;
	                    }

	                    if(uwOPVFaultCnt2_AT >= stGridProtect.uwACOverVolt2Time)
	                    {
	                   	 	
	                        	stSysFaultReg.unGridVoltFault.bit.OverVolt2=1;
					stSysFaultReg.unFaultBit.bit.GridVolt = 1;
	                            stautotest.uwFaultValue = stautotest.uwVFValue;
	                            stautotest.uwTripValue = stautotest.uwGridVoltJudge;
	                            stautotest.uwBreakTime = stautotest.wBreakCnt*20;
					stautotest.uwBreakTimeAdj = 40;
	                        	uwOPVFaultCnt2_AT = 0;
					
	                    }
	                }
	                else
	                {
	                    uwOPVFaultCnt2_AT = 0;
	                    wOPVFaultCnt = 0;
	                     if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T)
			   	||(stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T)
			   	||(stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T)
			   	||(stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
	                    {
	                        stautotest.wBreakCnt = 0;
	                    }
	                }
	            }
	            else if((uwGridRVoltTmp >= uwGridOverVolt1Tmp) ||(uwGridSVoltTmp >= uwGridOverVolt1Tmp) || (uwGridTVoltTmp >= uwGridOverVolt1Tmp))
	            {
	               	 wOPVFaultCnt++;
	                    if((stautotest.uwStep == AT_1P15Un_R)||(stautotest.uwStep == AT_1P15Un_S)||(stautotest.uwStep == AT_1P15Un_T))
	                    {
	                        	if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T)
				   	||(stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T)
				   	||(stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T)
				   	||(stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
	                        {
	                            stautotest.wBreakCnt = 0;
	                        }
	                    }
	                    else if(wOPVFaultCnt >= stGridProtect.uwACOverVolt1Time)
	                    {
	                        	stSysFaultReg.unFaultBit.bit.GridVolt = 1;
					stSysFaultReg.unGridVoltFault.bit.OverVolt1 = 1;
	                            stautotest.uwFaultValue = stautotest.uwVFValue;
	                            stautotest.uwTripValue = stautotest.uwGridVoltJudge;
	                            stautotest.uwBreakTime = stautotest.wBreakCnt*20;
					stautotest.uwBreakTimeAdj = 40;
	                        	wOPVFaultCnt = 0;

	                    }

	            }
	            else if((uwGridRVoltTmp <= uwGridUnderVolt2Tmp) ||(uwGridSVoltTmp <= uwGridUnderVolt2Tmp) || (uwGridTVoltTmp <= uwGridUnderVolt2Tmp)) //two conditions shared one fault cnt,may lead to set fault immediately when wGrid_vRMS < wGridVoltLow1EE
	            {		
	                uwOPVFaultCnt2_AT++;
	               if(uwOPVFaultCnt2_AT >= stGridProtect.uwACUnderVolt2Time)
	                {
	                    	  stSysFaultReg.unGridVoltFault.bit.UnderVolt2=1;
				   stSysFaultReg.unFaultBit.bit.GridVolt = 1;
	                        stautotest.uwFaultValue = stautotest.uwVFValue;
	                        stautotest.uwTripValue = stautotest.uwGridVoltJudge;
	                        stautotest.uwBreakTime = stautotest.wBreakCnt*20;
				 stautotest.uwBreakTimeAdj = 40;
	                       uwOPVFaultCnt2_AT = 0;
	                }
	            }
	            else if((uwGridRVoltTmp <= uwGridUnderVolt1Tmp) ||(uwGridSVoltTmp <= uwGridUnderVolt1Tmp) || (uwGridTVoltTmp <= uwGridUnderVolt1Tmp))  // surppose that wGridVoltLow1EE is larger than wGridVoltLow2EE
	            {
	                if(( stautotest.uwStep != AT_0P40Un_R)||(stautotest.uwStep != AT_0P40Un_S)||( stautotest.uwStep != AT_0P40Un_T))
	                {
	                    wOPVFaultCnt++;
	                    if(wOPVFaultCnt >= stGridProtect.uwACUnderVolt1Time)
	                    {
	                        	stSysFaultReg.unGridVoltFault.bit.UnderVolt1=1;
					stSysFaultReg.unFaultBit.bit.GridVolt = 1;
	                            stautotest.uwFaultValue = stautotest.uwVFValue;
	                            stautotest.uwTripValue = stautotest.uwGridVoltJudge;
	                            stautotest.uwBreakTime = stautotest.wBreakCnt*20;
					stautotest.uwBreakTimeAdj = 40;
	                        	wOPVFaultCnt = 0;

	                    }
	                }
	                else
	                {
	                    uwOPVFaultCnt2_AT = 0;
	                    wOPVFaultCnt = 0;
	                    	 if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T)
			   	||(stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T)
			   	||(stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T)
			   	||(stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
	                    {
	                         stautotest.wBreakCnt = 0;
	                    }
	                }
	            }
	            else
	            {
	                uwOPVFaultCnt2_AT = 0;
	                wOPVFaultCnt = 0;
	               if((stautotest.uwStep  == AT_1P15Un_R)||(stautotest.uwStep  == AT_1P15Un_S)||(stautotest.uwStep  == AT_1P15Un_T)
		   	||(stautotest.uwStep  == AT_0P85Un_R)||(stautotest.uwStep  == AT_0P85Un_S)||(stautotest.uwStep  == AT_0P85Un_T)
		   	||(stautotest.uwStep  == AT_1P10Un_R)||(stautotest.uwStep  == AT_1P10Un_S)||(stautotest.uwStep  == AT_1P10Un_T)
		   	||(stautotest.uwStep  == AT_0P40Un_R)||(stautotest.uwStep  == AT_0P40Un_S)||(stautotest.uwStep  == AT_0P40Un_T))
	                {
	                     stautotest.wBreakCnt = 0;
	                }
	            }
		}
		else
		{
            if((uwGridRVoltTmp <= uwGridUnderVolt3Tmp) ||(uwGridSVoltTmp <= uwGridUnderVolt3Tmp) || (uwGridTVoltTmp <= uwGridUnderVolt3Tmp))
            {
                suwGridUnderVolt3Cnt++;
                suwGridUnderVolt2Cnt++;
                suwGridUnderVolt1Cnt++;
                suwGridOverVolt1Cnt=0;
                suwGridOverVolt2Cnt=0;
                if(suwGridUnderVolt3Cnt >= stGridProtect.uwACUnderVolt3Time)
                {
                    stSysFaultReg.unGridVoltFault.bit.UnderVolt3=1;
                    stSysFaultReg.unFaultBit.bit.GridVolt = 1;

                    suwGridUnderVolt3Cnt=0;
                    suwGridUnderVolt2Cnt=0;
                    suwGridUnderVolt1Cnt=0;
                }
            }
			
            else if((uwGridRVoltTmp <= uwGridUnderVolt2Tmp) ||(uwGridSVoltTmp <= uwGridUnderVolt2Tmp) || (uwGridTVoltTmp <= uwGridUnderVolt2Tmp))
			{
				suwGridUnderVolt2Cnt++;
				suwGridUnderVolt1Cnt++;
				suwGridOverVolt1Cnt=0;
				suwGridOverVolt2Cnt=0;
	            suwGridUnderVolt3Cnt=0;

			if(suwGridUnderVolt2Cnt >= stGridProtect.uwACUnderVolt2Time)
			{
				stSysFaultReg.unGridVoltFault.bit.UnderVolt2=1;
				stSysFaultReg.unFaultBit.bit.GridVolt = 1;
                suwGridUnderVolt3Cnt=0;
				suwGridUnderVolt2Cnt=0;
				suwGridUnderVolt1Cnt=0;
			}
		}
		else if((uwGridRVoltTmp <= uwGridUnderVolt1Tmp) ||(uwGridSVoltTmp <= uwGridUnderVolt1Tmp) || (uwGridTVoltTmp <= uwGridUnderVolt1Tmp))
		{
			suwGridUnderVolt1Cnt++;
			suwGridOverVolt1Cnt=0;
			suwGridOverVolt2Cnt=0;
			suwGridUnderVolt2Cnt=0;
            suwGridUnderVolt3Cnt=0;

			if(suwGridUnderVolt1Cnt >= stGridProtect.uwACUnderVolt1Time)
			{
				stSysFaultReg.unGridVoltFault.bit.UnderVolt1=1;
				stSysFaultReg.unFaultBit.bit.GridVolt = 1;

				suwGridUnderVolt1Cnt=0;
			}
		}
		else if((uwGridRVoltTmp >= uwGridOverVolt2Tmp) ||(uwGridSVoltTmp >= uwGridOverVolt2Tmp) || (uwGridTVoltTmp >= uwGridOverVolt2Tmp))
		{
			suwGridOverVolt2Cnt++;
			suwGridOverVolt1Cnt++;
            suwGridUnderVolt3Cnt=0;
			suwGridUnderVolt2Cnt=0;
			suwGridUnderVolt1Cnt=0;

			if(suwGridOverVolt2Cnt >= stGridProtect.uwACOverVolt2Time)
			{
				stSysFaultReg.unGridVoltFault.bit.OverVolt2=1;
				stSysFaultReg.unFaultBit.bit.GridVolt = 1;
				suwGridOverVolt1Cnt=0;
				suwGridOverVolt2Cnt=0;
			}
		}
		else if((uwGridRVoltTmp >= uwGridOverVolt1Tmp) ||(uwGridSVoltTmp >= uwGridOverVolt1Tmp) || (uwGridTVoltTmp >= uwGridOverVolt1Tmp))
		{
			suwGridOverVolt1Cnt++;
            suwGridUnderVolt3Cnt=0;
			suwGridUnderVolt2Cnt=0;
			suwGridUnderVolt1Cnt=0;
			suwGridOverVolt2Cnt=0;

				if(suwGridOverVolt1Cnt >= stGridProtect.uwACOverVolt1Time)
				{
					if((ODM_CHUANG_WEI == stDspReceData.unSafetyOdm.bit.Manufacturer)
				       &&(0==stF107Data.unSet.bit.ACVoltLevel))
					{
						stSysFaultReg.unWarnBit.bit.ACOverVolt = 1;
					}
					else
					{
						stSysFaultReg.unFaultBit.bit.GridVolt = 1;
						stSysFaultReg.unGridVoltFault.bit.OverVolt1 = 1;
					}
					suwGridOverVolt1Cnt=0;
				}
			}
			else
			{
	            suwGridUnderVolt3Cnt=0;
				suwGridUnderVolt2Cnt=0;
				suwGridUnderVolt1Cnt=0;
				suwGridOverVolt1Cnt=0;
				suwGridOverVolt2Cnt=0;
			}
		}
	}
}

static void GridVoltStartCheck(void)
{
	Uint16 uwGridRVoltTmp,uwGridSVoltTmp,uwGridTVoltTmp;
	Uint16 uwVacStartHighTmp,uwVacStartHighRecorverTmp;
	Uint16 uwVacStartLowTmp,uwVacStartLowRecorverTmp;
	static Uint16 suwVacStartHighSetCnt=0,suwVacStartLowSetCnt=0,uwVacStartRecoverCnt=0;

	if(DISABLE == stF107Data.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// actually, N wire is connected
	{
		uwGridRVoltTmp = stACSample.PhaseVoltRms.wRN;
		uwGridSVoltTmp = stACSample.PhaseVoltRms.wSN;
		uwGridTVoltTmp = stACSample.PhaseVoltRms.wTN;
		/**************start Grid Phase voltage out of range**********************/
		uwVacStartHighTmp = (Uint16)((int32)stGridProtect.uwACStartOverVolt*591>>10);
		uwVacStartLowTmp = (Uint16)((int32)stGridProtect.uwACStartUnderVolt*591>>10);

		uwVacStartHighRecorverTmp = uwVacStartHighTmp - 50;		// Over voltage Recorver Value
		uwVacStartLowRecorverTmp = uwVacStartLowTmp + 50;
	}
	else														// N wire is disconnected,software virtual n
	{
		uwGridRVoltTmp = stACSample.LineVoltRms.wRS;
		uwGridSVoltTmp = stACSample.LineVoltRms.wST;
		uwGridTVoltTmp = stACSample.LineVoltRms.wTR;
		/**************start Grid Line voltage out of range**********************/
		uwVacStartHighTmp = stGridProtect.uwACStartOverVolt;
		uwVacStartLowTmp = stGridProtect.uwACStartUnderVolt;

		uwVacStartHighRecorverTmp = uwVacStartHighTmp - 87;		// Over voltage Recorver Value
		uwVacStartLowRecorverTmp = uwVacStartLowTmp + 87;
	}

	/**************Grid Phase voltage out of range checking**********************/
	if((1 == stSysFaultReg.unGridVoltFault.bit.UnderStartVolt)
	|| (1 == stSysFaultReg.unGridVoltFault.bit.OverStartVolt))
	{
		if((uwGridRVoltTmp <= uwVacStartHighRecorverTmp)
		&& (uwGridSVoltTmp <= uwVacStartHighRecorverTmp)
		&& (uwGridTVoltTmp <= uwVacStartHighRecorverTmp)
		&& (uwGridRVoltTmp >= uwVacStartLowRecorverTmp)
		&& (uwGridSVoltTmp >= uwVacStartLowRecorverTmp)
		&& (uwGridTVoltTmp >= uwVacStartLowRecorverTmp))
		{
			uwVacStartRecoverCnt++;
			if(uwVacStartRecoverCnt > CLR_FAULT_TIME)//20)
			{
				uwVacStartRecoverCnt=0;

				stSysFaultReg.unGridVoltFault.bit.OverStartVolt = 0;
				stSysFaultReg.unGridVoltFault.bit.UnderStartVolt = 0;
			}
		}
		else
		{
			uwVacStartRecoverCnt = 0;
		}
	}
	else
	{
		if((uwGridRVoltTmp <= uwVacStartLowTmp) ||(uwGridSVoltTmp <= uwVacStartLowTmp) || (uwGridTVoltTmp <= uwVacStartLowTmp))
		{
			suwVacStartLowSetCnt++;
			suwVacStartHighSetCnt = 0;
			if(suwVacStartLowSetCnt >= SET_AC_START_FAULT_TIME)
			{
				stSysFaultReg.unGridVoltFault.bit.UnderStartVolt = 1;
				stSysFaultReg.unFaultBit.bit.GridVolt = 1;
				suwVacStartLowSetCnt = 0;
			}
		}
		else if((uwGridRVoltTmp >= uwVacStartHighTmp) ||(uwGridSVoltTmp >= uwVacStartHighTmp) || (uwGridTVoltTmp >= uwVacStartHighTmp))
		{
			suwVacStartHighSetCnt++;
			suwVacStartLowSetCnt = 0;
			if(suwVacStartHighSetCnt >= SET_AC_START_FAULT_TIME)
			{
				stSysFaultReg.unGridVoltFault.bit.OverStartVolt = 1;
				stSysFaultReg.unFaultBit.bit.GridVolt = 1;
				suwVacStartHighSetCnt = 0;
			}
		}
		else
		{
			suwVacStartHighSetCnt = 0;
			suwVacStartLowSetCnt = 0;
		}
	}
}

static void GridVoltLossProtection(void)
{
	static Uint16 suwUtilityLossRecoveryCnt,suwGridVoltLossCnt = 0,suwGridVoltLastLossCnt = 0;
	if(cInitialStatus == eInverterStatus)
		return;
	
	if(((stACSample.PhaseVoltRms.wRN < PHASE_VOLT_MIN)
	||(stACSample.PhaseVoltRms.wSN < PHASE_VOLT_MIN)
	||(stACSample.PhaseVoltRms.wTN < PHASE_VOLT_MIN)))
	{
		stSysFaultReg.unFaultBit.bit.GridVolt = 1;
		stSysFaultReg.unGridVoltFault.bit.UtilityLoss = 1;
		suwUtilityLossRecoveryCnt = 0;
		suwGridVoltLossCnt = 0;
	}
	else if(((abs(stACSample.PhaseVoltRms.wSN-stACSample.PhaseVoltRms.wTN)<100)&&(abs(stACSample.PhaseVoltRms.wTN-2*stACSample.PhaseVoltRms.wRN)<150))
	        ||((abs(stACSample.PhaseVoltRms.wRN-stACSample.PhaseVoltRms.wTN)<100)&&(abs(stACSample.PhaseVoltRms.wRN-2*stACSample.PhaseVoltRms.wSN)<150))
	        ||((abs(stACSample.PhaseVoltRms.wRN-stACSample.PhaseVoltRms.wSN)<100)&&(abs(stACSample.PhaseVoltRms.wSN-2*stACSample.PhaseVoltRms.wTN)<150)))
	{
	    suwGridVoltLossCnt++;
	    if(suwGridVoltLossCnt>=SET_FAULT_TIME)
	    {
	        suwGridVoltLossCnt = 0;
	        suwUtilityLossRecoveryCnt = 0;
	        stSysFaultReg.unFaultBit.bit.GridVolt = 1;
	        stSysFaultReg.unGridVoltFault.bit.UtilityLoss = 1;
	    }
	}
	else
	{
		suwUtilityLossRecoveryCnt++;
		if(suwUtilityLossRecoveryCnt>=CLR_FAULT_TIME)
		{
			suwUtilityLossRecoveryCnt = 0;
			suwGridVoltLossCnt = 0;
			stSysFaultReg.unGridVoltFault.bit.UtilityLoss = 0;
		}
	}
}
static void GridVoltNPECheck(void)
{
	static Uint16 suwNPEFaultCnt = 0;
	static Uint16 suwNPEFaultClrCnt = 0;
	//if(cInverterStatus == eInverterStatus)
		//return;
	if(ENABLE == stDspReceData.unFuncEnable.bit.NPEEnable)	//NPECheck
	{
		if(stACSample.wVoltNPERms  > 2560)  //1600/32=80v//232*32=7424
		{
			suwNPEFaultCnt++;
			suwNPEFaultClrCnt = 0;
			if(suwNPEFaultCnt>=100)//20ms*100=2000ms=2s
			{
				stSysFaultReg.unFaultBit.bit.NPE = 1;
				suwNPEFaultCnt = 0;
			}
		}
		else if(stACSample.wVoltNPERms  < 1600)//1600/32=50V
		{
			suwNPEFaultCnt = 0;
			suwNPEFaultClrCnt++;
			if(suwNPEFaultClrCnt>=CLR_FAULT_TIME)
			{
				suwNPEFaultClrCnt = 0;
				stSysFaultReg.unFaultBit.bit.NPE = 0;
			}
		}
	}
	else
	{
		suwNPEFaultCnt = 0;
		suwNPEFaultClrCnt = 0;
		stSysFaultReg.unFaultBit.bit.NPE = 0;
		
	}
}

void GridVoltProtection(void)
{
	GridVoltLossProtection();
	if((cInverterStatus != eInverterStatus)&&(cInitialStatus != eInverterStatus))
	{
		GridVoltNPECheck();
	}
	
	if((cInitialStatus == eInverterStatus)
	|| (1 == stSysFaultReg.unGridVoltFault.bit.UtilityLoss)
	|| (FREQ_NULL == stFreqRang.uwFreqFlag))
	{
		return;
	}
	else
	{
		GridVoltOverUnderProtection();
		if(cInverterStatus != eInverterStatus)
		{
			GridVoltStartCheck();
		}
	}

}

void TemperatureProtection(void)
{
	static Uint16 suwInvTempFaultSetCnt,suwInvTempFaultClrCnt;

	if((cInitialStatus == eInverterStatus) || (FALSE ==unSysFlag.bit.MachineCheckEnd))
		return;
	
	if((stDCSample.wInvTemp > stMachineCfg.wInvTempOver)||(stDCSample.wInvTemp < TempN35oC)
	    ||(stDCSample.wBoostTemp > stMachineCfg.wBoostTempOver)||(stDCSample.wBoostTemp < TempN35oC)
		||(stDCSample.wAmTemp > stMachineCfg.wAmbTempOver)||(stDCSample.wAmTemp < TempN35oC))		// 950
	{
		suwInvTempFaultClrCnt = 0;
		suwInvTempFaultSetCnt++;
		if(suwInvTempFaultSetCnt>=150)		// 150*20ms = 3s
		{
			suwInvTempFaultSetCnt = 0;
			stSysFaultReg.unFaultBit.bit.OverTemp = 1;
			if((stDCSample.wInvTemp > stMachineCfg.wInvTempOver)||(stDCSample.wInvTemp < TempN35oC))
			{
			    stSysFaultReg.unTempFault.bit.InvOverTemp = 1;      //6-1
			}
			else if((stDCSample.wBoostTemp > stMachineCfg.wBoostTempOver)||(stDCSample.wBoostTemp < TempN35oC))
			{
			    stSysFaultReg.unTempFault.bit.BoostOverTemp = 1;    //6-2
			}
			else
			{
			    stSysFaultReg.unTempFault.bit.AmOverTemp = 1;       //6-4
			}
		}
	}
	else
	{
		suwInvTempFaultSetCnt = 0;
		suwInvTempFaultClrCnt++;
		if(suwInvTempFaultClrCnt>=CLR_FAULT_TIME)		//250*20ms = 5s
		{
			suwInvTempFaultClrCnt = 0;
			stSysFaultReg.unFaultBit.bit.OverTemp = 0;
			stSysFaultReg.unTempFault.all = 0;          //Clr OverTemp Fault
		}
	}
}
