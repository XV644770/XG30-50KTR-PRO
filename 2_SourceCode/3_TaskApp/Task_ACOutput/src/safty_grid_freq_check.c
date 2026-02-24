/*
 * safty_grid_freq_check.c
 *
 *  Created on: 2021年1月9日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

ST_FREQ_RANG stFreqRang;
ST_GRID_PROTECT	stGridProtect;
ST_GRID_FREQ stGridFreq;

void GridFreqParaInit(void)
{
	memset((void*)&stFreqRang,0,sizeof(stFreqRang));
	stFreqRang.uwFreqFlag = FREQ_NULL;

	memset((void*)&stGridProtect,0,sizeof(stGridProtect));

	memset((void*)&stGridFreq,0,sizeof(stGridFreq));

	stGridProtect.uwACStartOverFreq = 6250;
	stGridProtect.uwACStartUnderFreq = 4750;

	stGridProtect.uwACUnderFreq1 = 4750;
	stGridProtect.uwACUnderFreq2 = 4750;
	stGridProtect.uwACUnderFreq1Time = 1;
	stGridProtect.uwACUnderFreq2Time = 1;

	stGridProtect.uwACOverFreq1 = 6250;
	stGridProtect.uwACOverFreq2 = 6250;
	stGridProtect.uwACOverFreq1Time = 1;
	stGridProtect.uwACOverFreq2Time = 1;


	stGridProtect.uwACStartOverVolt = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;	// 1126/1024 = 1.1
	stGridProtect.uwACStartUnderVolt = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;	// 922/1024 = 0.9

	stGridProtect.uwACOverVolt1 = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;
	stGridProtect.uwACOverVolt2 = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;

	stGridProtect.uwACUnderVolt1 = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;
	stGridProtect.uwACUnderVolt2 = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;
    stGridProtect.uwACUnderVolt3 = ((Uint32)stDspReceData.uwACNormalVolt*922)>>10;

	stGridProtect.uwAC10MinOverVolt = ((Uint32)stDspReceData.uwACNormalVolt*1126)>>10;

	stGridProtect.uwACOverVolt1Time = 1;
	stGridProtect.uwACUnderVolt1Time = 1;
	stGridProtect.uwACUnderVolt2Time = 1;
	stGridProtect.uwACOverVolt2Time = 1;
}

void GridFreqRangeCheck(void)
{
	static Uint16 suwGridFreqAutoCheck60HZCnt=0;
	static Uint16 suwGridFreqAutoCheck50HZCnt=0;
	static Uint16 suwGridFreqAutoCheckFailCnt=0;

	stFreqRang.wGridFreq = stACSample.wGridFreqAvg;
	if((1 == stSysFaultReg.unGridVoltFault.bit.UtilityLoss)
	|| (FREQ_NULL != stFreqRang.uwFreqFlag)
	|| (cInitialStatus == eInverterStatus))
	{
		return;
	}
	else
	{
		if((cInverterStatus != eInverterStatus) &&(FREQ_NULL == stFreqRang.uwFreqFlag)&& (TRUE == stPllPara.stOut.uwPllOk))
		{
			if((stFreqRang.wGridFreq <= (FREQ_60HZ_MAX-50))
			&& (stFreqRang.wGridFreq >= (FREQ_60HZ_MIN+50)))
			{
				//stFreqRang.uwFreqFlag = FREQ_60HZ;
				
				//stDebug.ReadData.uwDebug4++;
				
				suwGridFreqAutoCheck50HZCnt = 0;
				suwGridFreqAutoCheck60HZCnt++;
				if(suwGridFreqAutoCheck60HZCnt >= 10)//mgs/add for freq auto detection false 
				{
					suwGridFreqAutoCheck60HZCnt = 0;
					stFreqRang.uwFreqFlag = FREQ_60HZ;
					stSysFaultReg.unGridFreqFault.bit.FreqAutoCheckFail =0;
					suwGridFreqAutoCheckFailCnt = 0;
				}
			}
			else if( (stFreqRang.wGridFreq <= (FREQ_50HZ_MAX-50))
				     && (stFreqRang.wGridFreq >= (FREQ_50HZ_MIN+50)))
			{
				//stFreqRang.uwFreqFlag = FREQ_50HZ;

				suwGridFreqAutoCheck60HZCnt = 0;
				suwGridFreqAutoCheck50HZCnt++;
				if(suwGridFreqAutoCheck50HZCnt >= 10)
				{
					suwGridFreqAutoCheck50HZCnt = 0;
					stFreqRang.uwFreqFlag = FREQ_50HZ;
					stSysFaultReg.unGridFreqFault.bit.FreqAutoCheckFail =0;
					suwGridFreqAutoCheckFailCnt = 0;
				}
			}
			else
			{
				suwGridFreqAutoCheck50HZCnt = 0;
				suwGridFreqAutoCheck60HZCnt = 0;
				suwGridFreqAutoCheckFailCnt++;
				if(suwGridFreqAutoCheckFailCnt >= 100)//after 5s alrrm freq auto check fail 
				{
					suwGridFreqAutoCheckFailCnt = 0;
					stFreqRang.uwFreqFlag = FREQ_50HZ;
					stSysFaultReg.unGridFreqFault.bit.FreqAutoCheckFail =1;
				}
			}
		}
	}
}


static void GridFreqOverUnderProtection(void)
{
	Uint16 uwGridFreqTmp;
	Uint16 uwGridOverFreq1Tmp,uwGridOverFreq2Tmp,uwGridOverFreqRecorverTmp,uwGridOverFreqRecorverTmp2;
	Uint16 uwGridUnderFreq1Tmp,uwGridUnderFreq2Tmp,uwGridUnderFreqRecorverTmp,uwGridUnderFreqRecorverTmp2;

	static Uint16 suwGridUnderFreq2Cnt,suwGridUnderFreq1Cnt;
	static Uint16 suwGridOverFreq1Cnt,suwGridOverFreq2Cnt=0,uwGridFaultRecoverCnt;

	static Uint16 suwFreqDisturbCnt;

	static Uint16 bFreHIorBL = 1;
    	static Uint16 bFreHIorBL_2= 1;
	//uwGridFreqTmp = stACSample.wGridFreqReal;

		if((stautotest.uwStep  == AT_50P5Hz || stautotest.uwStep  == AT_49P5Hz||stautotest.uwStep  == AT_51P5Hz || stautotest.uwStep  == AT_47P5Hz)\
        	&&(cInverterStatus == eInverterStatus))
	    {
	   		 stautotest.uwFaultValue = 0;
			 stautotest.uwBreakTime = 0;//实际保护时间
			 stautotest.uwBreakTimeAdj = 0;
			if(stautotest.uwStep  == AT_50P5Hz)
			{
				uwGridOverFreq1Tmp = (Uint16)stautotest.uwVFValue;
			}
			else
			{
				uwGridOverFreq1Tmp = stGridProtect.uwACOverFreq1;
			}

			if(stautotest.uwStep  == AT_49P5Hz)
			{
				uwGridUnderFreq1Tmp = (Uint16)stautotest.uwVFValue;
			}
			else
			{
				uwGridUnderFreq1Tmp = stGridProtect.uwACUnderFreq1;
			}

			if(stautotest.uwStep  == AT_51P5Hz)
			{
				uwGridOverFreq2Tmp =(Uint16) stautotest.uwVFValue;
			}
			else
			{
				uwGridOverFreq2Tmp = stGridProtect.uwACOverFreq2;
			}

			if(stautotest.uwStep  == AT_47P5Hz)
			{
				uwGridUnderFreq2Tmp = (Uint16)stautotest.uwVFValue;
			}
			else
			{
				uwGridUnderFreq2Tmp = stGridProtect.uwACUnderFreq2;
			}
	    }
	    else
	    {
			uwGridOverFreq1Tmp = stGridProtect.uwACOverFreq1;
			uwGridOverFreq2Tmp = stGridProtect.uwACOverFreq2;
			uwGridUnderFreq1Tmp = stGridProtect.uwACUnderFreq1;
			uwGridUnderFreq2Tmp = stGridProtect.uwACUnderFreq2;
	    }

	uwGridOverFreqRecorverTmp = uwGridOverFreq1Tmp - 10;		// Over Freqency Recorver Value
	uwGridUnderFreqRecorverTmp = uwGridUnderFreq1Tmp + 10;
	
	uwGridOverFreqRecorverTmp2 = uwGridOverFreq2Tmp - 10;		// Over Freqency Recorver Value
	uwGridUnderFreqRecorverTmp2 = uwGridUnderFreq2Tmp + 10;

	  if(((stautotest.uwStep  == AT_51P5Hz) || (stautotest.uwStep  == AT_47P5Hz)||(stautotest.uwStep  == AT_50P5Hz) || (stautotest.uwStep  == AT_49P5Hz))\
	  	&&(cInverterStatus == eInverterStatus))
	    {
	        if((stautotest.uwStep ==AT_51P5Hz)||(stautotest.uwStep ==AT_47P5Hz))
	        {
	            stautotest.uwLocalCMD = 0;
	        }
	        else if((stautotest.uwStep ==AT_50P5Hz)||(stautotest.uwStep ==AT_49P5Hz))
	        {
	            stautotest.uwLocalCMD = 3;
	        }
	    }
	    else
	    {
	        if(stautotest.uwStep ==AT_49P5Hz)
	        {
	            stautotest.uwLocalCMD = 0;
	        }
	    }
		uwGridFreqTmp = stACSample.wGridFreqReal;
		
		if(STRANDARD_ITALY_CEIO16 != stDspReceData.unSafetyOdm.bit.SafetyStandard)
		 {
	            stautotest.uwLocalCMD = 0;
	        }
	
	if((1==stSysFaultReg.unGridFreqFault.bit.OverFreq1)
	 ||(1==stSysFaultReg.unGridFreqFault.bit.OverFreq2)
	 ||(1==stSysFaultReg.unGridFreqFault.bit.UnderFreq1)
	 ||(1==stSysFaultReg.unGridFreqFault.bit.UnderFreq2))
	{
		if(STRANDARD_ITALY_CEIO16 == stDspReceData.unSafetyOdm.bit.SafetyStandard)
		{
			if(((stautotest.uwLocalCMD != 3)&&(uwGridFreqTmp <= uwGridOverFreqRecorverTmp)&&(uwGridFreqTmp >= uwGridUnderFreqRecorverTmp))
			||((stautotest.uwLocalCMD == 3)&&(uwGridFreqTmp <= uwGridOverFreqRecorverTmp2)&&(uwGridFreqTmp >= uwGridUnderFreqRecorverTmp2)))
			{
				uwGridFaultRecoverCnt++;
				if(uwGridFaultRecoverCnt > CLR_FAULT_TIME)	// 20*100ms = 2s
				{
					uwGridFaultRecoverCnt=0;
					stSysFaultReg.unGridFreqFault.bit.OverFreq1 = 0;
					stSysFaultReg.unGridFreqFault.bit.OverFreq2 = 0;
					stSysFaultReg.unGridFreqFault.bit.UnderFreq1 = 0;
					stSysFaultReg.unGridFreqFault.bit.UnderFreq2 = 0;
				}
			}
			else
			{
				if(uwGridFaultRecoverCnt > 0)
					uwGridFaultRecoverCnt--;
			}

			bFreHIorBL = 1;
        		bFreHIorBL_2 = 1;
       		 if((stautotest.uwStep == AT_51P5Hz) || (stautotest.uwStep == AT_47P5Hz)||(stautotest.uwStep == AT_50P5Hz) || (stautotest.uwStep == AT_49P5Hz))
       		 {
           		 	stautotest.wBreakCnt = 0;
       		 }
		
		}
		else
		{
			if((uwGridFreqTmp <= uwGridOverFreqRecorverTmp)
			&& (uwGridFreqTmp >= uwGridUnderFreqRecorverTmp))
			{
				uwGridFaultRecoverCnt++;
				if(uwGridFaultRecoverCnt > CLR_FAULT_TIME)	// 20*100ms = 2s
				{
					uwGridFaultRecoverCnt=0;
					stSysFaultReg.unGridFreqFault.bit.OverFreq1 = 0;
					stSysFaultReg.unGridFreqFault.bit.OverFreq2 = 0;
					stSysFaultReg.unGridFreqFault.bit.UnderFreq1 = 0;
					stSysFaultReg.unGridFreqFault.bit.UnderFreq2 = 0;
				}
			}
			else
			{
				if(uwGridFaultRecoverCnt > 0)
					uwGridFaultRecoverCnt--;
			}
		}

	}
	else
	{
		if((STRANDARD_ITALY_CEIO16 == stDspReceData.unSafetyOdm.bit.SafetyStandard)&&stautotest.uwEnableFlag == ENABLE&&(cInverterStatus == eInverterStatus))
		{
			 if((uwGridFreqTmp >= uwGridOverFreq2Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
			{
				suwGridOverFreq2Cnt++;

			        if(bFreHIorBL==2) 
			        {
					suwGridOverFreq2Cnt = 0;
				}					
			           bFreHIorBL = 0;

				if(suwGridOverFreq2Cnt >= stGridProtect.uwACOverFreq2Time)
				{
					suwGridUnderFreq2Cnt=0;
					suwGridUnderFreq1Cnt=0;
					suwGridOverFreq1Cnt=0;
					suwGridOverFreq2Cnt=0;
				   	stautotest.uwFaultValue = stautotest.uwVFValue;
	                stautotest.uwTripValue =  stautotest.uwFaultValue;
	                stautotest.uwBreakTime =  stautotest.wBreakCnt*20;
					stautotest.uwBreakTimeAdj = 40;
					stSysFaultReg.unFaultBit.bit.GridFreq = 1;
					stSysFaultReg.unGridFreqFault.bit.OverFreq2 = 1;
						
				}
			 }
			 else if((uwGridFreqTmp <= uwGridUnderFreq2Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
			 {
				if(stautotest.uwStep != AT_49P5Hz)
				{
					suwGridUnderFreq2Cnt++;
	                if(bFreHIorBL==0)
	                {
	                    suwGridUnderFreq2Cnt = 0;
	                }
	                bFreHIorBL = 2;
					if(suwGridUnderFreq2Cnt >= stGridProtect.uwACUnderFreq2Time)
					{
						suwGridUnderFreq2Cnt=0;
						suwGridUnderFreq1Cnt=0;
						suwGridOverFreq1Cnt=0;
						suwGridOverFreq2Cnt=0;
				        stautotest.uwFaultValue = stautotest.uwVFValue;
	                    stautotest.uwTripValue = stautotest.uwFaultValue;
	                    stautotest.uwBreakTime = stautotest.wBreakCnt*20;
					    stautotest.uwBreakTimeAdj = 40;
						stSysFaultReg.unFaultBit.bit.GridFreq = 1;
						stSysFaultReg.unGridFreqFault.bit.UnderFreq2 = 1;
					}
			 	}
			}
			 else
		        {
		        	suwGridOverFreq2Cnt = 0;
				suwGridUnderFreq2Cnt=0;
		             bFreHIorBL = 1;
		            if( stautotest.uwStep == AT_51P5Hz ||  stautotest.uwStep == AT_47P5Hz)
		            	{
		                 	stautotest.wBreakCnt = 0;
		            	}
		        }

			if(stautotest.uwLocalCMD == 3)		
			{
				
				 if((uwGridFreqTmp >= uwGridOverFreq1Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
				{
					
			              if(suwGridOverFreq1Cnt < 65535)
			              {
			  			suwGridOverFreq1Cnt++;
					}
		                  
		                	if(bFreHIorBL_2==2)
		                	{
						suwGridOverFreq1Cnt = 0;
					}
		              	 bFreHIorBL_2 = 0;
					if(suwGridOverFreq1Cnt >= stGridProtect.uwACOverFreq1Time)
					{
						suwGridUnderFreq2Cnt=0;
						suwGridUnderFreq1Cnt=0;
						suwGridOverFreq1Cnt=0;
						suwGridOverFreq2Cnt=0;
					       stautotest.uwFaultValue =stautotest.uwVFValue;
		                            stautotest.uwTripValue = stautotest.uwFaultValue;
		                            stautotest.uwBreakTime = stautotest.wBreakCnt*20;
						stautotest.uwBreakTimeAdj = 40;
						stSysFaultReg.unFaultBit.bit.GridFreq = 1;
						stSysFaultReg.unGridFreqFault.bit.OverFreq1 = 1;
					}
				}
				 else if((uwGridFreqTmp <= uwGridUnderFreq1Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
				 {
					if(suwGridUnderFreq1Cnt < 65535)
			              {
			  			suwGridUnderFreq1Cnt++;
					}
		                  
		                	if(bFreHIorBL_2==0)
		                	{
						suwGridUnderFreq1Cnt = 0;
					}
		              	 bFreHIorBL_2 = 2;
					if(suwGridUnderFreq1Cnt >= stGridProtect.uwACUnderFreq1Time)
					{
						suwGridUnderFreq2Cnt=0;
						suwGridUnderFreq1Cnt=0;
						suwGridOverFreq1Cnt=0;
						suwGridOverFreq2Cnt=0;
                        stautotest.uwFaultValue = stautotest.uwVFValue;
                        stautotest.uwTripValue = stautotest.uwFaultValue;
                        stautotest.uwBreakTime = stautotest.wBreakCnt*20;
						stautotest.uwBreakTimeAdj = 40;
						stSysFaultReg.unFaultBit.bit.GridFreq = 1;
						stSysFaultReg.unGridFreqFault.bit.UnderFreq1 = 1;
					}
				}
				 else
			        {
		                	 suwGridUnderFreq2Cnt=0;
					 suwGridUnderFreq1Cnt=0;
					 suwGridOverFreq1Cnt=0;
					 suwGridOverFreq2Cnt=0;
			                bFreHIorBL_2 = 1;
			                if(stautotest.uwStep == AT_50P5Hz || stautotest.uwStep == AT_49P5Hz)
			                {
			                    stautotest.wBreakCnt = 0;
			                }
			         }
			}

		}
		else
		{
			if((uwGridFreqTmp <= uwGridUnderFreq2Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
			{
				suwGridUnderFreq2Cnt++;
				suwGridUnderFreq1Cnt++;
				suwFreqDisturbCnt = 0;

			if(suwGridUnderFreq2Cnt >= stGridProtect.uwACUnderFreq2Time)
			{
				stSysFaultReg.unFaultBit.bit.GridFreq = 1;
				stSysFaultReg.unGridFreqFault.bit.UnderFreq2 = 1;

				suwGridUnderFreq2Cnt=0;
				suwGridUnderFreq1Cnt=0;
				suwGridOverFreq1Cnt=0;
				suwGridOverFreq2Cnt=0;
			}
		}
		else if((uwGridFreqTmp <= uwGridUnderFreq1Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
		{
			suwGridUnderFreq1Cnt++;
			suwFreqDisturbCnt = 0;

			if(suwGridUnderFreq1Cnt >= stGridProtect.uwACUnderFreq1Time)
			{
				stSysFaultReg.unFaultBit.bit.GridFreq = 1;
				stSysFaultReg.unGridFreqFault.bit.UnderFreq1 = 1;

				suwGridUnderFreq2Cnt=0;
				suwGridUnderFreq1Cnt=0;
				suwGridOverFreq1Cnt=0;
				suwGridOverFreq2Cnt=0;
			}
		}
		else if((uwGridFreqTmp >= uwGridOverFreq2Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
		{
			suwGridOverFreq2Cnt++;
			suwGridOverFreq1Cnt++;
			suwFreqDisturbCnt = 0;

			if(suwGridOverFreq2Cnt >= stGridProtect.uwACOverFreq2Time)
			{
				stSysFaultReg.unFaultBit.bit.GridFreq = 1;
				stSysFaultReg.unGridFreqFault.bit.OverFreq2 = 1;

				suwGridUnderFreq2Cnt=0;
				suwGridUnderFreq1Cnt=0;
				suwGridOverFreq1Cnt=0;
				suwGridOverFreq2Cnt=0;
			}
		}
		else if((uwGridFreqTmp >= uwGridOverFreq1Tmp)&&(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough))
		{
			suwGridOverFreq1Cnt++;
			suwFreqDisturbCnt = 0;

			if(suwGridOverFreq1Cnt >= stGridProtect.uwACOverFreq1Time)
			{
				stSysFaultReg.unFaultBit.bit.GridFreq = 1;
				stSysFaultReg.unGridFreqFault.bit.OverFreq1 = 1;

				suwGridUnderFreq2Cnt=0;
				suwGridUnderFreq1Cnt=0;
				suwGridOverFreq1Cnt=0;
				suwGridOverFreq2Cnt=0;
			}
		}
		else
		{
			suwFreqDisturbCnt++;
			if(suwFreqDisturbCnt>=5)		// 5*20ms = 100ms
			{
				suwFreqDisturbCnt = 5;

					suwGridUnderFreq2Cnt=0;
					suwGridUnderFreq1Cnt=0;
					suwGridOverFreq1Cnt=0;
					suwGridOverFreq2Cnt=0;
				}
			}
		}
	}
}

static void GridFreqStartCheck(void)
{
	Uint16 uwGridFreqTmp;
	Uint16 uwGridStartOverFreqTmp,uwGridStartOverFreqRecorverTmp;
	Uint16 uwGridStartUnderFreqTmp,uwGridStartUnderFreqRecorverTmp;

	static Uint16 suwGridStartUnderFreqCnt;
	static Uint16 suwGridStartOverFreqCnt=0,uwGridStartFreqFaultRecoverCnt;

	uwGridFreqTmp = stACSample.wGridFreqReal;

	uwGridStartOverFreqTmp = stGridProtect.uwACStartOverFreq;
	uwGridStartUnderFreqTmp = stGridProtect.uwACStartUnderFreq;

	uwGridStartOverFreqRecorverTmp = uwGridStartOverFreqTmp - 10;		// 0.1Hz
	uwGridStartUnderFreqRecorverTmp = uwGridStartUnderFreqTmp + 10;

	if((1==stSysFaultReg.unGridFreqFault.bit.OverStartFreq)
	||(1==stSysFaultReg.unGridFreqFault.bit.UnderStartFreq))
	{
		if((uwGridFreqTmp <= uwGridStartOverFreqRecorverTmp)
		&& (uwGridFreqTmp >= uwGridStartUnderFreqRecorverTmp))
		{
			uwGridStartFreqFaultRecoverCnt++;
			if(uwGridStartFreqFaultRecoverCnt > CLR_FAULT_TIME)	// 20*100ms = 2s
			{
				uwGridStartFreqFaultRecoverCnt=0;
				stSysFaultReg.unGridFreqFault.bit.OverStartFreq = 0;
				stSysFaultReg.unGridFreqFault.bit.UnderStartFreq = 0;
			}
		}
		else
		{
			uwGridStartFreqFaultRecoverCnt = 0;
		}
	}
	else
	{
		if(uwGridFreqTmp <= uwGridStartUnderFreqTmp)
		{
			suwGridStartUnderFreqCnt++;
			suwGridStartOverFreqCnt=0;

			if(suwGridStartUnderFreqCnt >= SET_AC_START_FAULT_TIME)
			{
				stSysFaultReg.unFaultBit.bit.GridFreq = 1;
				stSysFaultReg.unGridFreqFault.bit.UnderStartFreq = 1;
				suwGridStartUnderFreqCnt = 0;
			}
		}
		
		else if(uwGridFreqTmp >= uwGridStartOverFreqTmp)
		{
			suwGridStartOverFreqCnt++;
			suwGridStartUnderFreqCnt=0;

			if(suwGridStartOverFreqCnt >= SET_AC_START_FAULT_TIME)
			{
				stSysFaultReg.unFaultBit.bit.GridFreq = 1;
				stSysFaultReg.unGridFreqFault.bit.OverStartFreq = 1;
				suwGridStartOverFreqCnt = 0;
			}
		}
		else
		{
			suwGridStartUnderFreqCnt = 0;
			suwGridStartOverFreqCnt = 0;
		}
	}
}

void GridFreqProtection(void)
{
	if((cInitialStatus == eInverterStatus)
	|| (1 == stSysFaultReg.unGridVoltFault.bit.UtilityLoss)
	|| (FREQ_NULL == stFreqRang.uwFreqFlag))
	{
		return;
	}
	else
	{
		GridFreqOverUnderProtection();
		if(cInverterStatus != eInverterStatus)
		{
			GridFreqStartCheck();
		}
	}
}

/************************************************************************************
*Function name:	sRGridFreqCal														*
*global vars:	wOutputPeriodNew:	output period,1.0us/cnt							*
*Ouput:			wOutputFreqNew:		0.01HZ per cnt									*
*Description:																		*
*************************************************************************************/

void	RGridFreqCal(Uint16 uwRGridPeriodNew)
{	
	if(uwRGridPeriodNew != 0)
	{
		stGridFreq.uwRGridFreqNew = 100000000 /uwRGridPeriodNew;
		
		if(stGridFreq.uwRGridFreqNew >= Freq80Hz)	
			stGridFreq.uwRGridFreqNew = stGridFreq.uwRGridFreqOld;
		else
		    stGridFreq.uwRGridFreqNew = stGridFreq.uwRGridFreqNew;
//			stGridFreq.uwRGridFreqNew = (stGridFreq.uwRGridFreqNew + stGridFreq.uwRGridFreqOld) >> 1;
	}
	else
	{
		stGridFreq.uwRGridFreqNew = 0;
	}	
		
	stGridFreq.uwRGridFreqOld = stGridFreq.uwRGridFreqNew;	
}

void	SGridFreqCal(Uint16 uwSGridPeriodNew)
{	
	if(uwSGridPeriodNew != 0)
	{
		stGridFreq.uwSGridFreqNew = 100000000 /uwSGridPeriodNew;
		
		if(stGridFreq.uwSGridFreqNew >= Freq80Hz)	
			stGridFreq.uwSGridFreqNew = stGridFreq.uwSGridFreqOld;
		else
		    stGridFreq.uwSGridFreqNew = stGridFreq.uwSGridFreqNew;
//			stGridFreq.uwSGridFreqNew = (stGridFreq.uwSGridFreqNew + stGridFreq.uwSGridFreqOld) >> 1;
	}
	else
	{
		stGridFreq.uwSGridFreqNew = 0;
	}	
		
	stGridFreq.uwSGridFreqOld = stGridFreq.uwSGridFreqNew;	
	
}

void	TGridFreqCal(Uint16 uwTGridPeriodNew)
{	
	if(uwTGridPeriodNew != 0)
	{
		stGridFreq.uwTGridFreqNew = 100000000 /uwTGridPeriodNew;
		
		if(stGridFreq.uwTGridFreqNew >= Freq80Hz)	
			stGridFreq.uwTGridFreqNew = stGridFreq.uwTGridFreqOld;
		else
		    stGridFreq.uwTGridFreqNew = stGridFreq.uwTGridFreqNew;
//			stGridFreq.uwTGridFreqNew = (stGridFreq.uwTGridFreqNew + stGridFreq.uwTGridFreqOld) >> 1;
	}
	else
	{
		stGridFreq.uwTGridFreqNew = 0;
	}	
		
	stGridFreq.uwTGridFreqOld = stGridFreq.uwTGridFreqNew;	
}

void ZeroLossChk(void)  
{
    stGridFreq.uwRLossCnt++;

	if(stGridFreq.uwRLossCnt > 3)
	{		
		//stACSample.PhaseVoltRms.wRN=0;		
		//stACSample.LineVoltRms.wRS = 0;
		stGridFreq.uwRGridFreqNew = 0;
		stGridFreq.uwRLossCnt = 3;
	}
	

	//-----------------------------//
	stGridFreq.uwSLossCnt++;
	
	if(stGridFreq.uwSLossCnt > 3)
	{
		//stACSample.PhaseVoltRms.wSN=0;		
		//stACSample.LineVoltRms.wST = 0;		
		stGridFreq.uwSGridFreqNew = 0;
		stGridFreq.uwSLossCnt = 3;
	}
	
    //-----------------------------//
	stGridFreq.uwTLossCnt++;
	
	if(stGridFreq.uwTLossCnt > 3)
	{
		//stACSample.PhaseVoltRms.wTN=0;		
		//stACSample.LineVoltRms.wTR = 0;		
		stGridFreq.uwTGridFreqNew = 0;
		stGridFreq.uwTLossCnt = 3;
	}
	
	
}

