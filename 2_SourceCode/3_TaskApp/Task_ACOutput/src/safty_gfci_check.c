/*
 * safty_gfci_check.c
 *
 *  Created on: 2021年5月21日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

void	GFCIOverCurrCheck(void)
{
	static Uint16	uwGFCIOverCurrSetFaultCnt;
	static Uint16	uwGFCIOverCurrClrFaultCnt;
	static int16	wGfciAvgJump1 = 0, wGfciAvgJump2 = 0;
	static int16	wGfciRmsJump1 = 0,wGfciRmsJump2 = 0;
	static int16	wGFCI30mAJumpFaultCnt,wGFCI60mAJumpFaultCnt;
	static int16	wGfciAvgOld1 = 0, wGfciAvgOld2 = 0;
	static int16	wGfciRmsOld1 = 0, wGfciRmsOld2 = 0;
	static int16	wEnterInverterStatusCnt=0;
	
	if(cInitialStatus == eInverterStatus)
		return;
	
//	if(ENABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.GFCI)
//	{
		if(0==stSysFaultReg.unHardwareFault.bit.GFCIModuleFault		// self-check finish & GFCI Module is ok
		 &&(1==unSysFlag.bit.GFCISelfCheckFinished))
		{
	        stDspReceData.wGFCIRmsFault = stDspReceData.wGFCIRmsFaultTmp + stDspReceData.wGFCIRmsFaultDerate * 10;
			if(1==stSysFaultReg.unFaultBit.bit.GFCIOverCurr)		// clear fault Logic
			{
				if((stACSample.wGfciRms < stDspReceData.wGFCIRmsFault)&&(abs(stACSample.wGfciAvg)<stDspReceData.wGFCIRmsFault))
				{
					uwGFCIOverCurrClrFaultCnt++;
					if(uwGFCIOverCurrClrFaultCnt>=CLR_FAULT_TIME)
					{
						uwGFCIOverCurrClrFaultCnt = 0;
						stSysFaultReg.unFaultBit.bit.GFCIOverCurr = 0;
						stSysFaultReg.unGFCIFault.bit.OverRatedCurr = 0;
						stSysFaultReg.unGFCIFault.bit.Jump30mA = 0;
						stSysFaultReg.unGFCIFault.bit.Jump60mA = 0;
						stSysFaultReg.unGFCIFault.bit.Jump150mA = 0;
						wGFCI30mAJumpFaultCnt = 0;
						wGFCI60mAJumpFaultCnt = 0;
						uwGFCIOverCurrSetFaultCnt = 0;
					}
				}
				else
				{
					if(uwGFCIOverCurrClrFaultCnt>0)
					{
						uwGFCIOverCurrClrFaultCnt--;
					}
				}
			}
			else
			{
				uwGFCIOverCurrClrFaultCnt = 0;
				/**************GFCI Current > 300mA & Time > 300ms **************************/
				if((stACSample.wGfciRms > stDspReceData.wGFCIRmsFault) || (abs(stACSample.wGfciAvg) > stDspReceData.wGFCIRmsFault))
				{
					uwGFCIOverCurrSetFaultCnt++;
					if(uwGFCIOverCurrSetFaultCnt>=11)		// 14*20ms = 280ms
					{
						uwGFCIOverCurrSetFaultCnt = 0;
						stSysFaultReg.unFaultBit.bit.GFCIOverCurr = 1;
						stSysFaultReg.unGFCIFault.bit.OverRatedCurr = 1;
					}
				}
				else
				{
					if(uwGFCIOverCurrSetFaultCnt>0)
					{
						uwGFCIOverCurrSetFaultCnt--;
					}
				}

				if(DISABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.GFCI)
                {
                    stSysFaultReg.unGFCIFault.bit.Jump30mA = 0;
                    stSysFaultReg.unGFCIFault.bit.Jump60mA = 0;
                    stSysFaultReg.unGFCIFault.bit.Jump150mA = 0;

                    wGFCI30mAJumpFaultCnt = 0;
                    wGFCI60mAJumpFaultCnt = 0;
                    wGfciAvgOld1 = 0;
                    wGfciAvgOld2 =0;

                    wGfciRmsOld1 = 0;
                    wGfciRmsOld2 =0;
                    return;
                }

				/**************(Jump Change)GFCI Current Protect***********************/
				if((DISABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.NonStand)&&(stACSample.dActivePowerAvg>5000)
				&& (cInverterStatus==eInverterStatus))
				{

					wGfciAvgJump1 =  abs(wGfciAvgOld1-stACSample.wGfciAvg);
					wGfciAvgJump2 = abs(wGfciAvgOld2-stACSample.wGfciAvg);

					wGfciRmsJump1 =  abs(wGfciRmsOld1-stACSample.wGfciRms);
					wGfciRmsJump2 = abs(wGfciRmsOld2-stACSample.wGfciRms);

					wGfciAvgOld2 = wGfciAvgOld1;
					wGfciAvgOld1 = stACSample.wGfciAvg;

					wGfciRmsOld2 = wGfciRmsOld1;
					wGfciRmsOld1 = stACSample.wGfciRms;

					wEnterInverterStatusCnt++;
					if(wEnterInverterStatusCnt>=500)		// 500*20ms = 10s
					{
						wEnterInverterStatusCnt = 500;
						/**************(Jump Change)GFCI Current > 30mA & Time > 300ms **************************/
						if(wGFCI30mAJumpFaultCnt==0)
						{
							if((wGfciAvgJump1>=GFCI_27MA)||(wGfciAvgJump2>=GFCI_27MA)
							||(wGfciRmsJump1>=GFCI_27MA)||(wGfciRmsJump2>=GFCI_27MA))
							{
								wGFCI30mAJumpFaultCnt = 1;

							}

						}
						else
						{

							if((abs(stACSample.wGfciAvg)>=GFCI_27MA) || (stACSample.wGfciRms>=GFCI_27MA))
							{
								wGFCI30mAJumpFaultCnt++;
								if(wGFCI30mAJumpFaultCnt>=12)		// 12*20ms = 240ms
								{
									wGFCI30mAJumpFaultCnt = 0;
									stSysFaultReg.unFaultBit.bit.GFCIOverCurr = 1;
									stSysFaultReg.unGFCIFault.bit.Jump30mA = 1;
								}
							}
							else
							{
								wGFCI30mAJumpFaultCnt = 0;
							}
						}
						/**************(Jump Change)GFCI Current > 60mA & Time > 150ms **************************/

						if(wGFCI60mAJumpFaultCnt==0)
						{
							if((wGfciAvgJump1>=GFCI_57MA)||(wGfciAvgJump2>=GFCI_57MA)
							||(wGfciRmsJump1>=GFCI_57MA)||(wGfciRmsJump2>=GFCI_57MA))
							{
								wGFCI60mAJumpFaultCnt = 1;
							}
						}
						else
						{

							if((abs(stACSample.wGfciAvg) >= GFCI_57MA)|| (stACSample.wGfciRms >= GFCI_57MA))
							{
								wGFCI60mAJumpFaultCnt++;
								if(wGFCI60mAJumpFaultCnt>=3)		// 3*20ms = 60ms
								{
									wGFCI60mAJumpFaultCnt = 0;
									stSysFaultReg.unFaultBit.bit.GFCIOverCurr = 1;
									stSysFaultReg.unGFCIFault.bit.Jump60mA = 1;
								}
							}
							else
							{
								wGFCI60mAJumpFaultCnt = 0;
							}
						}
						
						/**************(Jump Change)GFCI Current > 150mA & Time > 40ms **************************/
						if((wGfciAvgJump1>=GFCI_145MA)||(wGfciAvgJump2>=GFCI_145MA)
							||(wGfciRmsJump1>=GFCI_145MA)||(wGfciRmsJump2>=GFCI_145MA))
						{
							if((abs(stACSample.wGfciAvg) >= GFCI_145MA)|| (stACSample.wGfciRms >= GFCI_145MA))
							{
								stSysFaultReg.unFaultBit.bit.GFCIOverCurr = 1;
								stSysFaultReg.unGFCIFault.bit.Jump150mA = 1;
							}
						}
					}
				}
				else
				{
					wEnterInverterStatusCnt = 0;
					wGFCI30mAJumpFaultCnt = 0;
					wGFCI60mAJumpFaultCnt = 0;

					wGfciAvgOld1 = 0;
					wGfciAvgOld2 =0;

					wGfciRmsOld1 = 0;
					wGfciRmsOld2 =0;
				}
			}
		}
		else
		{
			uwGFCIOverCurrSetFaultCnt = 0;
			uwGFCIOverCurrClrFaultCnt = 0;
			wGFCI30mAJumpFaultCnt = 0;
			wGFCI60mAJumpFaultCnt = 0;
			wGfciAvgOld1 = 0;
			wGfciAvgOld2 =0;

			wGfciRmsOld1 = 0;
			wGfciRmsOld2 =0;
		}
//	}
//	else
//	{
//		uwGFCIOverCurrClrFaultCnt = 0;
//		uwGFCIOverCurrSetFaultCnt = 0;
//		stSysFaultReg.unFaultBit.bit.GFCIOverCurr = 0;
//		stSysFaultReg.unGFCIFault.bit.OverRatedCurr = 0;
//		stSysFaultReg.unGFCIFault.bit.Jump30mA = 0;
//		stSysFaultReg.unGFCIFault.bit.Jump60mA = 0;
//		stSysFaultReg.unGFCIFault.bit.Jump150mA = 0;
//
//		wGFCI30mAJumpFaultCnt = 0;
//		wGFCI60mAJumpFaultCnt = 0;
//	}
}

void	GFCISelfCheck(void)
{
	static Uint16	suwGFCISelfCheckCnt=0;
	static Uint16	suwGFCISelfFaultCnt=0,suwGFCISelfFaultTimes=0;

	if((cInitialStatus == eInverterStatus)
	 ||(1==unSysFlag.bit.PvVoltLow)
	 ||(1==unSysFlag.bit.GFCISelfCheckFinished)
	 ||(FREQ_NULL == stFreqRang.uwFreqFlag))
	{
		suwGFCISelfFaultCnt = 0;
		suwGFCISelfCheckCnt = 0;
		suwGFCISelfFaultTimes = 0;
		GFCI_TEST_CLEAR;		// GFCI Test Single Low Level
		GFCI_TEST_SET_60KW;
		
	}
	else if(ENABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.GFCI)		// enable GFCI check
	{
		suwGFCISelfCheckCnt++;
		if(suwGFCISelfCheckCnt<=50)		// 50*20ms = 1000ms
		{
			suwGFCISelfFaultCnt = 0;
			GFCI_TEST_CLEAR;			// GFCI Test Single Low Level
			GFCI_TEST_SET_60KW;
		}
		else if(suwGFCISelfCheckCnt<=133)		// (133-83)*20ms = 1000ms
		{
			GFCI_TEST_SET;		// GFCI Test Single High Level
			GFCI_TEST_SET_60KW;
		}
		else if((suwGFCISelfCheckCnt<=183) && (suwGFCISelfCheckCnt > 138))		// (183-133)*20ms = 1000ms		check 50 timers
		{
			if((stACSample.wGfciRms > GFCI_SELF_CHECK_MAX)		// GFCI Self-Check Failed
			|| (stACSample.wGfciRms < GFCI_SELF_CHECK_MIN))
			{
				suwGFCISelfFaultCnt++;
				if(suwGFCISelfFaultCnt>=5)			// 5-timers  GFCI Fault
				{
					suwGFCISelfFaultCnt = 0;
					suwGFCISelfFaultTimes++;
					if(suwGFCISelfFaultTimes>=2)	// 2*2.8s = 7.32 self-check finished
					{
						stSysFaultReg.unHardwareFault.bit.GFCIModuleFault = 1;		// set GFCI Hardware Fault
					}
					else
					{
						suwGFCISelfCheckCnt = 0;		// Re-Start Self-Check  GFCI
					}
				}
			}
			else
			{
				suwGFCISelfFaultCnt = 0;		// clear self-fail fault
			}
		}
		else if(suwGFCISelfCheckCnt>=184)
		{
			GFCI_TEST_CLEAR;			// GFCI Test Single Low Level
			GFCI_TEST_SET_60KW;
			unSysFlag.bit.GFCISelfCheckFinished = 1;
			suwGFCISelfFaultTimes = 0;
		}
	}
	else			// disable GFCI check
	{
		GFCI_TEST_CLEAR;			// GFCI Test Single Low Level
		GFCI_TEST_SET_60KW;
		suwGFCISelfCheckCnt = 0;
		unSysFlag.bit.GFCISelfCheckFinished = 1;
		stSysFaultReg.unHardwareFault.bit.GFCIModuleFault = 0;		// clear GFCI Hardware Fault
		suwGFCISelfFaultTimes = 0;
	}
}
