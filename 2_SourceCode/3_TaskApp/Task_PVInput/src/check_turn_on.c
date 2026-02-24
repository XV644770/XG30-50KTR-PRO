/*
 * TurnOnCheck.c
 *
 *  Created on: 2020年12月29日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

ST_TASK_TIMER stTaskTimer;

/****************************************************************************
*
*	FunctionName:	ConnectCountDown
*	Input:	none
*	Output:	none
*	Description:	inverter Grid connection countdown
*	Note:	none.
****************************************************************************/
void ConnectCountDown(void)
{
	static Uint16 suw500msCnt = 0;
	Uint16 uwGridCountDownTmp = 60;		// default time : 30S
	static Uint16	suw10MinCnt=0;
	Uint16 uwrestartcnt= 4;		// default time :4

	if(stautotest.uwEnableFlag ==DISABLE)
	{
		uwrestartcnt =4;
	}
	else
	{
		uwrestartcnt =9;
		stWaitStatus.uwExitInvStatusCnt =0;
	}
		
	suw500msCnt++;
	if(suw500msCnt >= 25)    //20ms * 25 = 500ms
	{
	    suw500msCnt = 0;
	    // Reconnect delay time initialization
	    if((stF107Data.uwAdjustMode==ATE_ADJUST_30)||(stF107Data.uwAdjustMode==ATE_ADJUST_70))
	    {
	    	uwGridCountDownTmp = 40;		// 20s
	    }
	    else if(stWaitStatus.uwExitInvStatusCnt>4)	// Within 10 minutes, it will exit connection for 5 times, Grid connection countdown 1min
	    {
	    	uwGridCountDownTmp = 1200;		// 1200/20ms = 60s
	    }
	    else
	    {
	    	if(stF107Data.uwStartDelayTime >= 10)		// 10s
	    	{
	    		if(1 == unSysFlag.bit.GoToInvOnce)
	    		{
	    			uwGridCountDownTmp = stF107Data.uwReStartDelayTime*2;
	    		}
	    		else
	    		{
	    			uwGridCountDownTmp = stF107Data.uwStartDelayTime*2;
	    		}
	    	}
	    	else
	    	{
	    		uwGridCountDownTmp = 60;		// 30s
	    	}
	    }

	    if(stWaitStatus.uwExitInvStatusCnt != 0)
	    {
	    	suw10MinCnt++;
	    	if(suw10MinCnt>=1200)	// 1200 * 500ms = 600s = 10min
	    	{
	    		suw10MinCnt = 0;
	    		stWaitStatus.uwExitInvStatusCnt = 0;
	    	}
	    }



	    // Grid connection countdown
	    if(cWaitStatus == eInverterStatus)
	    {
	    	unSysFlag.bit.TurnOnOffCheck = uwTurnOnInvtCheck();
	        if(TRUE == unSysFlag.bit.TurnOnOffCheck)
	        {
	            if(stTaskTimer.uwStartDelayTime > 0)
	            {
	            	stTaskTimer.uwStartDelayTime--;
	            }
	            unSysFlag.bit.bInvStart = 1;
	        }
	        else
	        {
	        	stTaskTimer.uwStartDelayTime = uwGridCountDownTmp;
	            unSysFlag.bit.bInvStart = 0;
	            stDebug.ReadData.uwWaitStartError ++;
	        }

	    }
	    else
	    {
	    	stTaskTimer.uwStartDelayTime = uwGridCountDownTmp;
	        unSysFlag.bit.bInvStart = 0;
		
	    }
		
	
	}
}

/****************************************************************************
*
*	FunctionName:	TurnOnInvtCheck
*	Input:	none
*	Output:	none
*	Description:	Check the starting condition of inverter
*	Note:	none.	period 20ms
***************************************************************/
Uint16 uwTurnOnInvtCheck(void)
{

	if(FALSE == stPllPara.stOut.uwPllOk)		// SRF-PLL  is failed
	{
		stDebug.ReadData.uwWaitStartMsgStatus = 1;
		stDebug.ReadData.uwWaitStartError += 1;
		if(stDebug.ReadData.uwWaitStartError > 10)	//10*20ms
		{
			//stSysFaultReg.unFaultBit.bit.GridVolt = 1;
			//stSysFaultReg.unGridVoltFault.bit.PllFault = 1;
		}
		return FALSE;
	}

	if(FREQ_NULL == stFreqRang.uwFreqFlag)
	{
		stDebug.ReadData.uwWaitStartMsgStatus = 2;
		stDebug.ReadData.uwWaitStartError += 10;
		return FALSE;
	}

#if  F107_BOARD
	if((stDspUart.uwComReceFrameEnd&0x0F)!=0x0F)
	{
		return FALSE;
	}
#else
	if(OFF == stDebug.SetData.unSetReg.bit.TurnOnOff)
	{
		stDebug.ReadData.uwWaitStartMsgStatus = 3;
		stDebug.ReadData.uwWaitStartError += 100;
		return FALSE;
	}
#endif

	if((TRUE == unSysFlag.bit.PvVoltLow) || (TRUE ==unSysFlag.bit.PvVoltHigh))
	{
		if(TRUE == unSysFlag.bit.PvVoltLow)
		{
			stDebug.ReadData.uwWaitStartMsgStatus = 5;
			stDebug.ReadData.uwWaitStartError += 10000;
		}
		else
		{
			stDebug.ReadData.uwWaitStartMsgStatus = 6;
			stDebug.ReadData.uwWaitStartError += 20000;
		}

		return FALSE;
	}
	return TRUE;
}


