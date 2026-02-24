/*
 * safty_relay_check.c
 *
 *  Created on: 2021年5月21日
 *      Author: Administrator
 */

#include "dsp_user_include.h"


/****************************************************************************
*
*	FunctionName:	SingleRelayCheck
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
void SingleRelayCheck(void)
{
	static Uint16 suwRelayVoltNormalCnt=0;
	stWaitStatus.stRelayCheck.uwCheckCnt++;
	/***********STEP1:DSP&F042 Relay Break,Disconnect the relay and check for sticking *********/
	if(200 == stWaitStatus.stRelayCheck.uwCheckCnt) 	//  200* 10ms = 2s	// wait Bus Voltage Stable
	{
		stDebug.ReadData.uwRelayCheckStep = 1;
		OS_ENTER_CRITICAL();
		DSP_RELAY_LOW;									// DSP  Relay	Break
 		stWaitStatus.stRelayCheck.uwF042OnOff = 0;		// F042 Relay	Break
		OS_EXIT_CRITICAL();
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt >= 250)&&(stWaitStatus.stRelayCheck.uwCheckCnt < 280))	// 280-250=30times
	{
		/***********Check Relay Voltage*********/
		if((stACSample.RlyVoltRms.wRN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wSN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wTN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax))
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt>=5)
			{
				suwRelayVoltNormalCnt = 0;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;			// DSP&F04 Relay Voltage normal
				stWaitStatus.stRelayCheck.uwCheckCnt = 279;				// jump out of this cycle
				stDebug.ReadData.uwRelayCheckStep = 2;
			}
		}
		else
		{
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
			stDebug.ReadData.uwRelayCheckStep = 3;
		}
	}
	/***********STEP2:DSP Relay Break,F042 Relay Close;	Inv Open Loop *********/
	else if(280==stWaitStatus.stRelayCheck.uwCheckCnt)
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)		// relay abnormal
		{
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
			stDebug.ReadData.uwRelayCheckStep = 4;
		}
		else													// relay normal
		{
			OS_ENTER_CRITICAL();
			if(ON == stDebug.SetData.unSetReg.bit.InvPwmLoop)			// inv stop run,check inv pwm wave
			{
				stWaitStatus.stRelayCheck.uwF042OnOff = 0;				// Don't Close Relay
			}
			else
			{
				stWaitStatus.stRelayCheck.uwF042OnOff = 1;				// Close Relay
			}
			OS_EXIT_CRITICAL();
			stDebug.ReadData.uwRelayCheckStep = 5;
		}
	}
	else if(330 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  300* 10ms = 3s		// Open Inv Loop,output sin wave
	{
		stDebug.ReadData.uwRelayCheckStep = 6;
		OS_ENTER_CRITICAL();
		stWaitStatus.stRelayCheck.Flag.InvPwmOn = 1;
		OS_EXIT_CRITICAL();
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt>=430)&&(stWaitStatus.stRelayCheck.uwCheckCnt<440))	//  400 * 10ms = 4s
	{
#if OLD_INV_VOLT_SAMPLE
		if((stACSample.RlyVoltRms.wRN >= RELAY_VOLT_MAX)     //check Open Loop PWM
		|| (stACSample.RlyVoltRms.wSN >= RELAY_VOLT_MAX)
		|| (stACSample.RlyVoltRms.wTN >= RELAY_VOLT_MAX)
		|| (stACSample.LineInvVoltAvg.wRS >= RELAY_LINE_MAX)
		|| (stACSample.LineInvVoltAvg.wST >= RELAY_LINE_MAX)
		|| (stACSample.LineInvVoltAvg.wTR >= RELAY_LINE_MAX))
		{
					stDebug.ReadData.uwRelayCheckStep = 7;
					suwRelayVoltNormalCnt = 0;
					stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
		else
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt >= 5)		// 5*10ms = 50ms
			{
				suwRelayVoltNormalCnt = 5;
				stDebug.ReadData.uwRelayCheckStep = 8;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;
			}
		}
#else


		//if((stACSample.RlyVoltRms.wRN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)     //check Open Loop PWM
		//&& (stACSample.RlyVoltRms.wSN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)
		//&& (stACSample.RlyVoltRms.wTN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin))
		if((stACSample.RlyLineVoltRms.wRS  <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)     // phase volt have a dc volt
		&& (stACSample.RlyLineVoltRms.wST<= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)
		&& (stACSample.RlyLineVoltRms.wTR<= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)) 
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt >= 5)		// 5*10ms = 50ms
			{
				suwRelayVoltNormalCnt = 0;
				stDebug.ReadData.uwRelayCheckStep = 7;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;
				stWaitStatus.stRelayCheck.uwCheckCnt = 439;
			}
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 8;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
			
		}
#endif
		if(ON == stDebug.SetData.unSetReg.bit.InvPwmLoop)			// inv stop run,check inv pwm wave
		{
			stWaitStatus.stRelayCheck.uwCheckCnt = 430;
		}
	}
	else if(440 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  410* 10ms = 4.10s
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)
		{
			stDebug.ReadData.uwRelayCheckStep = 9;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 10;
			OS_ENTER_CRITICAL();
			DSP_RELAY_HIGH;
			OS_EXIT_CRITICAL();
		}
	}
//	else if(441 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  600* 10ms = 6s
//	{
//		OS_ENTER_CRITICAL();
//		//stWaitStatus.stRelayCheck.Flag.InvPwmoff = 1;
//		OS_EXIT_CRITICAL();
//	}
	else if(442 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  413* 10ms = 4.13s
	{
		stDebug.ReadData.uwRelayCheckStep = 11;
		OS_ENTER_CRITICAL();
		stWaitStatus.stRelayCheck.Flag.InvPwmOn = 0;
		OS_EXIT_CRITICAL();
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt>=490)&&(stWaitStatus.stRelayCheck.uwCheckCnt<520))	//  460* 10ms = 4.60s
	{
#if OLD_INV_VOLT_SAMPLE
		if((stACSample.RlyVoltRms.wRN >= RELAY_VOLT_MAX)     //check Open Loop PWM
		||(stACSample.RlyVoltRms.wSN >= RELAY_VOLT_MAX)
		||(stACSample.RlyVoltRms.wTN >= RELAY_VOLT_MAX))
		{
			stDebug.ReadData.uwRelayCheckStep = 12;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 13;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;
		}
#else

		if((stACSample.RlyVoltRms.wRN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)     //check Open Loop PWM
		&& (stACSample.RlyVoltRms.wSN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)
		&& (stACSample.RlyVoltRms.wTN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin))
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt >= 5)		// 5*10ms = 50ms
			{
				suwRelayVoltNormalCnt = 5;
				stDebug.ReadData.uwRelayCheckStep = 12;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;
				stWaitStatus.stRelayCheck.uwCheckCnt = 519;
			}
		}
		else
		{

			stDebug.ReadData.uwRelayCheckStep = 13;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
#endif
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt>=520)&&(stWaitStatus.stRelayCheck.uwCheckCnt<526))	//  520* 10ms = 4.90s
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)
		{
			stDebug.ReadData.uwRelayCheckStep = 14;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 15;
			stWaitStatus.stRelayCheck.Flag.Success = 1;
		}
	}
	stDebug.ReadData.uwRelayCheckStatus =  (stDebug.ReadData.uwRelayCheckStep*1000)+ stWaitStatus.stRelayCheck.uwCheckCnt;
}

/****************************************************************************
*
*	FunctionName:	DoubleRelayCheck
*	Input:	none
*	Output:	none
*	Description:	Double Relay Check
*	Note:	none.
***************************************************************/
#if 1
void DoubleRelayCheck(void)
{
	static Uint16 suwRelayVoltNormalCnt=0;

	stWaitStatus.stRelayCheck.uwCheckCnt++;

	/***********STEP1:DSP&F042 Relay Break,Disconnect the relay and check for sticking *********/
	if(200 == stWaitStatus.stRelayCheck.uwCheckCnt) 	//  200* 10ms = 2s	// wait Bus Voltage Stable
	{
		stDebug.ReadData.uwRelayCheckStep = 1;
		OS_ENTER_CRITICAL();
		DSP_RELAY_LOW;									// DSP  Relay	Break
 		stWaitStatus.stRelayCheck.uwF042OnOff = 0;		// F042 Relay	Break
		OS_EXIT_CRITICAL();
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt >= 250)&&(stWaitStatus.stRelayCheck.uwCheckCnt < 280))	// 280-250=30times
	{

		/***********Check Relay Voltage*********/
		if((stACSample.RlyVoltRms.wRN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wSN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wTN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax))
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt>=5)
			{
				suwRelayVoltNormalCnt = 0;
				stDebug.ReadData.uwRelayCheckStep = 2;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;			// DSP&F04 Relay Voltage normal
				stWaitStatus.stRelayCheck.uwCheckCnt = 279;				// jump out of this cycle
			}
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 3;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
	}
	/***********STEP2:F042 Relay Break,DSP Relay Close;	Disconnect F042 relay and check(F042) for sticking *********/
	else if(280==stWaitStatus.stRelayCheck.uwCheckCnt)
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)		// relay abnormal
		{
			stDebug.ReadData.uwRelayCheckStep = 4;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else													// relay normal
		{
			stDebug.ReadData.uwRelayCheckStep = 5;
			OS_ENTER_CRITICAL();
			DSP_RELAY_HIGH;								// Close DSP Relay
			stWaitStatus.stRelayCheck.uwF042OnOff = 0;	// Break F042 Relay
			OS_EXIT_CRITICAL();
		}
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt >= 330)&&(stWaitStatus.stRelayCheck.uwCheckCnt < 360))	// 360-330=30times
	{
		/***********Check Relay Voltage*********/
		if((stACSample.RlyVoltRms.wRN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wSN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wTN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax))
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt>=5)
			{
				suwRelayVoltNormalCnt = 0;
				stDebug.ReadData.uwRelayCheckStep = 6;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;			// DSP&F04 Relay Voltage normal
				stWaitStatus.stRelayCheck.uwCheckCnt = 359;				// jump out of this cycle
			}
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 7;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
	}
	/***********STEP3:DSP Relay Break,F042 Relay Close;	Disconnect DSP relay and check DSP for sticking *********/
	// DSP Relay Break
	else if(360==stWaitStatus.stRelayCheck.uwCheckCnt)
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)		// relay abnormal
		{
			stDebug.ReadData.uwRelayCheckStep = 8;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else													// relay normal
		{
			stDebug.ReadData.uwRelayCheckStep = 9;
			OS_ENTER_CRITICAL();
			DSP_RELAY_LOW;								// Close DSP Relay
			OS_EXIT_CRITICAL();
		}
	}
	// F042 Relay Close
	else if(410==stWaitStatus.stRelayCheck.uwCheckCnt)
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)		// relay abnormal
		{
			stDebug.ReadData.uwRelayCheckStep = 10;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else													// relay normal
		{
			stDebug.ReadData.uwRelayCheckStep = 11;
			OS_ENTER_CRITICAL();
			stWaitStatus.stRelayCheck.uwF042OnOff = 1;	// Break F042 Relay
			OS_EXIT_CRITICAL();
		}
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt >= 460)&&(stWaitStatus.stRelayCheck.uwCheckCnt < 490))	// 490-460=30times
	{
		/***********Check Relay Voltage*********/
		if((stACSample.RlyVoltRms.wRN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wSN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax)
		 &&(stACSample.RlyVoltRms.wTN >= stWaitStatus.stRelayCheck.wPhaseVoltRmsMax))
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt>=5)
			{
				suwRelayVoltNormalCnt = 0;
				stDebug.ReadData.uwRelayCheckStep = 12;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;			// DSP&F04 Relay Voltage normal
				stWaitStatus.stRelayCheck.uwCheckCnt = 489;				// jump out of this cycle
			}
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 13;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
	}
	/***********STEP4:DSP Relay Break,F042 Relay Close;	Open Inv Loop check PWM*********/
	else if(490==stWaitStatus.stRelayCheck.uwCheckCnt)
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)		// relay abnormal
		{
			stDebug.ReadData.uwRelayCheckStep = 14;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else													// relay normal
		{
			stDebug.ReadData.uwRelayCheckStep = 15;
			OS_ENTER_CRITICAL();
			stWaitStatus.stRelayCheck.Flag.InvPwmOn = 1;
			OS_EXIT_CRITICAL();
		}
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt>=590)&&(stWaitStatus.stRelayCheck.uwCheckCnt<600))	// 600-590 = 10times
	{
		//if((stACSample.RlyVoltRms.wRN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)     //check Open Loop PWM
		//&& (stACSample.RlyVoltRms.wSN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)
		//&& (stACSample.RlyVoltRms.wTN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)) 
		if((stACSample.RlyLineVoltRms.wRS  <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)     // phase volt have a dc volt
		&& (stACSample.RlyLineVoltRms.wST<= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)
		&& (stACSample.RlyLineVoltRms.wTR<= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)) 
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt >= 5)		// 5*10ms = 50ms
			{
				suwRelayVoltNormalCnt = 0;
				stDebug.ReadData.uwRelayCheckStep = 16;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;
				stWaitStatus.stRelayCheck.uwCheckCnt = 599;
			}
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 17;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
	}
	else if(600 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  600* 10ms = 6s
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)
		{
			stDebug.ReadData.uwRelayCheckStep = 18;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 19;
			OS_ENTER_CRITICAL();
			DSP_RELAY_HIGH;
			OS_EXIT_CRITICAL();
		}
	}
//	else if(601 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  600* 10ms = 6s
//	{
//		//stDebug.ReadData.uwRelayCheckStep = 20;
//		OS_ENTER_CRITICAL();
//		//stWaitStatus.stRelayCheck.Flag.InvPwmoff = 1;
//		OS_EXIT_CRITICAL();
//		stDebug.ReadData.wDebug5[3]++;
//	}
	else if(602 == stWaitStatus.stRelayCheck.uwCheckCnt)	//  603* 10ms = 6.03s
	{
		stDebug.ReadData.uwRelayCheckStep = 20;
		OS_ENTER_CRITICAL();
		stWaitStatus.stRelayCheck.Flag.InvPwmOn = 0;
		OS_EXIT_CRITICAL();
		suwRelayVoltNormalCnt = 0;
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt>=650)&&(stWaitStatus.stRelayCheck.uwCheckCnt<680))	//  460* 10ms = 4.60s
	{
		if((stACSample.RlyVoltRms.wRN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)     //check Open Loop PWM
		&& (stACSample.RlyVoltRms.wSN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin)
		&& (stACSample.RlyVoltRms.wTN <= stWaitStatus.stRelayCheck.wPhaseVoltRmsMin))
		{
			suwRelayVoltNormalCnt++;
			if(suwRelayVoltNormalCnt >= 5)		// 5*10ms = 50ms
			{
				suwRelayVoltNormalCnt = 5;
				stDebug.ReadData.uwRelayCheckStep = 21;
				stWaitStatus.stRelayCheck.Flag.VoltNormal = 1;
				stWaitStatus.stRelayCheck.uwCheckCnt = 679;
			}
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 22;
			suwRelayVoltNormalCnt = 0;
			stWaitStatus.stRelayCheck.Flag.VoltNormal = 0;
		}
	}
	else if((stWaitStatus.stRelayCheck.uwCheckCnt>=680)&&(stWaitStatus.stRelayCheck.uwCheckCnt<686))	//  696* 10ms = 6.96s
	{
		if(0 == stWaitStatus.stRelayCheck.Flag.VoltNormal)
		{
			stDebug.ReadData.uwRelayCheckStep = 23;
			stWaitStatus.stRelayCheck.Flag.Fail = 1;
		}
		else
		{
			stDebug.ReadData.uwRelayCheckStep = 24;
			stWaitStatus.stRelayCheck.Flag.Success = 1;
		}
	}

	stDebug.ReadData.uwRelayCheckStatus =  (stDebug.ReadData.uwRelayCheckStep*1000)+ stWaitStatus.stRelayCheck.uwCheckCnt;
}
#endif
