/*
 * safty_auto_test.c
 *
 *  Created on: 2021年5月21日
 *      Author: Administrator
 */

#include "dsp_user_include.h"
 ST_AUTO_TEST  stautotest;

 void sAutoTestUpShift(void);
 void OPTask_AutoTest(void);
 void stautotestInit(void);

void stautotestInit(void)
{
	memset(&stautotest,0,sizeof(stautotest));

	stautotest.uwProcess = PROCES_STOP;	//dsp 实际测试进程
	stautotest.uwStep = AT_STOP;			//自动测试步骤，ARM根据此步骤显示自动测试项
	stautotest.uwStatus = cTestWaiting;//自动测试状态0：测试等待中1：测试进行中2：测试单项成功3：测试失败4：测试失败5：测试通过

	stautotest.uwResult = 0;
	stautotest.wBreakCnt = 0;
	stautotest.uwBreakTime = 0;//实际保护时间
	stautotest.uwFaultValue = 0;
	stautotest.uwVFValue = 0;//实际保护值
	stautotest.wWaitTime = 0;
	stautotest.uwStopShiftFlag = 0;
	stautotest.uwBreakTimeAdj = 0;

}

void sAutoTestUpShift(void)
{
    switch(stautotest.uwStep )
    {
        case AT_1P15Un_R:
	case AT_1P15Un_S:
	case AT_1P15Un_T:
            if(stautotest.uwStopShiftFlag == 0)
            {
	            if(stautotest.uwVFValue>=0.5)
	            {
	                stautotest.uwVFValue -= 0.5;
	            }
            }
            break;
       case AT_1P10Un_R:
	case AT_1P10Un_S:
	case AT_1P10Un_T:
            if(stautotest.uwStopShiftFlag == 0)
            {
	            if(stautotest.uwVFValue>=0.5)
	            {
	                stautotest.uwVFValue -=0.5;
	            }
            }
            break;

       case AT_0P85Un_R:
	case AT_0P85Un_S:
	case AT_0P85Un_T:
            //if(stautotest.uwVFValue>=20)
            //	{
			stautotest.uwVFValue += 0.5;
            //	}
            break;

        case AT_0P40Un_R:
	 case AT_0P40Un_S:
	 case AT_0P40Un_T:
           // if(stautotest.uwVFValue>=50) 
            	//{
			stautotest.uwVFValue += 5;
            	//}
            break;

        case AT_51P5Hz:
        case AT_50P5Hz:
		  if(stautotest.uwVFValue>=0.25)
		{
			 stautotest.uwVFValue -= 0.25;
		}
            break;

        case AT_47P5Hz:
        case AT_49P5Hz:
           // if(stautotest.uwVFValue>=2)
		//{
			stautotest.uwVFValue += 0.1;
		//}
            break;

        default:
            break;
    }
}

void OPTask_AutoTest(void)
{
	static Uint16 wTestStopCnt = 0;
	static Uint16 wTestTouren = 0;
	static Uint16 wBreakTimeLimit = 0;

	if(stautotest.uwEnableFlag ==DISABLE)
	{
		stautotest.uwProcess = PROCES_STOP;
		stautotest.uwStep  = AT_STOP;
		stautotest.uwStatus = cTestWaiting;
		stautotest.uwResult = 0;
		stautotest.wBreakCnt = 0;
		stautotest.uwBreakTime = 0;
		stautotest.uwFaultValue = 0;
		stautotest.uwVFValue = 0;
		stautotest.wWaitTime = 0;
		stautotest.uwBreakTimeAdj = 0;
		wTestStopCnt = 0;
		return;
	}

	if(stautotest.uwProcess ==PROCES_STOP)
	{
		stautotest.uwVFValue = 0;
		stautotest.uwStep  = AT_STOP;
		stautotest.uwStatus  = cTestWaiting;
		stautotest.uwProcess = PROCES_WAIT;
		return;
	}
	else if(stautotest.uwProcess ==PROCES_WAIT)
	{
		stautotest.uwProcess = PROCES_WORK;
		stautotest.uwStep = AT_STOP;
		stautotest.uwStatus = cTestWaiting;
		stautotest.uwResult = 0;
		stautotest.wBreakCnt = 0;
		stautotest.uwBreakTime = 0;
		stautotest.uwFaultValue = 0;
		stautotest.uwBreakTimeAdj = 0;
		stautotest.uwVFValue = 0;
		stautotest.wWaitTime = 0;
		wTestStopCnt = 0;
		return;
	}
	else if(stautotest.uwProcess ==PROCES_WORK)
	{
		if(cInverterStatus == eInverterStatus)
		{
			wTestStopCnt = 0;
			if(stautotest.wWaitTime  < 250)
			{
				if(stautotest.wWaitTime  == 0)
				{
					//if(stautotest.uwStep  <= AT_49P5Hz)//AT_51P5Hz
					if(stautotest.uwStep  <= AT_51P5Hz)//AT_51P5Hz
					{
						stautotest.uwStep ++;
					}
					stautotest.uwStatus  = cTestWaiting;
					stautotest.wBreakCnt = 0;
					wTestTouren = 50;//27
					stautotest.uwStopShiftFlag = 0;
					switch(stautotest.uwStep )
					{		
						case AT_1P10Un_R:
						case AT_1P10Un_S:
						case AT_1P10Un_T:
							if(DISABLE == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// actually, N wire is connected
							{
								//stautotest.uwVFValue = stGridProtect.uwACOverVolt1;//wGrid_vRMS;
								stautotest.uwVFValue = (float)((Uint32)stGridProtect.uwACOverVolt1*591>>10);
							}
							else
							{
								stautotest.uwVFValue = (float)stGridProtect.uwACOverVolt1;//wGrid_vRMS;
							}
							
							wBreakTimeLimit = (stGridProtect.uwACOverVolt1Time+1)*20;  //wOPVHighTimeLimit1EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACOverVolt1;
							break;

						case AT_0P85Un_R:
						case AT_0P85Un_S:
						case AT_0P85Un_T:
							if(DISABLE == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// actually, N wire is connected
							{
								//stautotest.uwVFValue = stGridProtect.uwACOverVolt1;//wGrid_vRMS;
								stautotest.uwVFValue = (float)((Uint32)stGridProtect.uwACUnderVolt1*591>>10);
							}
							else
							{
								stautotest.uwVFValue = (float)stGridProtect.uwACUnderVolt1;//wGrid_vRMS;
							}
							//stautotest.uwVFValue =stGridProtect.uwACUnderVolt1;// wGrid_vRMS;
							wBreakTimeLimit = (stGridProtect.uwACUnderVolt1Time+1)*20;   //wOPVLowTimeLimit1EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACUnderVolt1;
							break;

						case AT_50P5Hz:                              
							stautotest.uwVFValue = (float)stGridProtect.uwACOverFreq1;//wOPFrecyReal;  //wFreqHigh1EE need to reduce 1  to match the standard value in cSpecLimitV table in M3 code
							wBreakTimeLimit = (stGridProtect.uwACOverFreq1Time+1)*20;  //wOPFHighTimeLimit1EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = (stGridProtect.uwACOverFreq1-1); //wFreqHigh1EE need to reduce 1  to match the standard value in cSpecLimitV table in M3 code
							break;

						case AT_49P5Hz:                           
							stautotest.uwVFValue =(float)stGridProtect.uwACUnderFreq1;// wOPFrecyReal;
							wBreakTimeLimit = (stGridProtect.uwACUnderFreq1Time+1)*20;  //wOPFLowTimeLimit1EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACUnderFreq1;
							break;

						case AT_1P15Un_R: 
						case AT_1P15Un_S: 
						case AT_1P15Un_T: 
							if(DISABLE == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// actually, N wire is connected
							{
								//stautotest.uwVFValue = stGridProtect.uwACOverVolt1;//wGrid_vRMS;
								stautotest.uwVFValue = (float)((Uint32)stGridProtect.uwACOverVolt2*591>>10);
							}
							else
							{
								stautotest.uwVFValue = (float)stGridProtect.uwACOverVolt2;//wGrid_vRMS;
							}
							//stautotest.uwVFValue =stGridProtect.uwACOverVolt2;// wGrid_vRMS;   // 10 min Avg
							wBreakTimeLimit = stGridProtect.uwACOverVolt2Time*20;;  //this value do not need to adjust
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACOverVolt2;
							break;

						case AT_0P40Un_R:
						case AT_0P40Un_S:
						case AT_0P40Un_T:
							if(DISABLE == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// actually, N wire is connected
							{
								//stautotest.uwVFValue = stGridProtect.uwACOverVolt1;//wGrid_vRMS;
								stautotest.uwVFValue =(float)((Uint32)stGridProtect.uwACUnderVolt2*591>>10);
							}
							else
							{
								stautotest.uwVFValue = (float)stGridProtect.uwACUnderVolt2;//wGrid_vRMS;
							}
							//stautotest.uwVFValue = stGridProtect.uwACUnderVolt2;//wGrid_vRMS;
							wBreakTimeLimit = (stGridProtect.uwACUnderVolt2Time+1)*20;   //wOPVLowTimeLimit2EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACUnderVolt2;
							break;

						case AT_51P5Hz:
							stautotest.uwVFValue =(float)stGridProtect.uwACOverFreq2;// wOPFrecyReal;
							wBreakTimeLimit = (stGridProtect.uwACOverFreq2Time+1)*20;  //wOPFHighTimeLimit2EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACOverFreq2;
							break;

						case AT_47P5Hz : 
							stautotest.uwVFValue =(float)stGridProtect.uwACUnderFreq2;// wOPFrecyReal;
							wBreakTimeLimit = (stGridProtect.uwACUnderFreq2Time+0)*20;  //wOPFLowTimeLimit2EE need to add 1  to match the standard value in cSpecLimitV table in M3 code
							stautotest.uwDefaultValue = wBreakTimeLimit;
							stautotest.uwVFDefaultValue = stGridProtect.uwACUnderFreq2;
							break;

						default:
							
							stautotest.uwStatus  = cAllTestOK;   /// all OK
							stautotest.uwStopShiftFlag = 0;
							break;
					}
				}
				else if(stautotest.wWaitTime ==249)
				{ 
					stautotest.uwStatus  = cTestRunning;
				}
				stautotest.wWaitTime ++;
				stautotest.uwResult = 0;
			}
			else if(stautotest.wWaitTime  == 250)
			{
				stautotest.wWaitTime ++;
				sAutoTestUpShift();					 // virtual value change every(300-250)*20ms=1s
				stautotest.uwResult++;
				if(stautotest.uwResult > cTestTimeOver) // test fail
				{
					//stautotest.uwStatus  = cTestFail;
					//stautotest.uwFaultValue = (float)stautotest.uwVFValue;
					//stautotest.uwBreakTime = stautotest.wBreakCnt;
					
				}
			}
			else if(stautotest.wWaitTime  < 300)
			{
				stautotest.wWaitTime ++;
			}
			else
			{
				stautotest.wWaitTime  = 250;
			}
		}
		else
		{
			stautotest.wWaitTime  = 0;
			if((true == stSysFaultReg.unFaultBit.bit.GridVolt) || (true == stSysFaultReg.unFaultBit.bit.GridFreq))
			{
				if(stautotest.uwResult > 0)
				{
					if(wBreakTimeLimit >(stautotest.uwBreakTime + wTestTouren) || (wBreakTimeLimit + wTestTouren)<stautotest.uwBreakTime)
					{
						stautotest.uwStatus  = cTestFail;
					}
					else
					{
						stautotest.uwStatus  = cTestOK;
					}
				}
				else
				{
					stautotest.uwStatus  = cTestWaiting;
				}

				wTestStopCnt++;
				if(wTestStopCnt > cDelay10s_20ms)	// witefor 10s
				{
					stautotest.uwStatus  = cTestFail;
				}
			}
			//else if(uwStoragePVInvMode != cSelfCheckMode)//(cWaitStatus == eInverterStatus)
			else if(cWaitStatus == eInverterStatus)
			{
				wTestStopCnt++;
				if(wTestStopCnt > cDelay3Mins_20ms)
				{
					stautotest.uwStatus  = cTestAllFail;
				}
			}
			else
			{
				//stautotest.uwStatus  = cTestAllFail;
			}
		}
	}
}


