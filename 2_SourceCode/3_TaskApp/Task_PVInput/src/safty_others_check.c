/*
 * safty_others_check.c
 *
 *  Created on: 2021年7月5日
 *      Author: Administrator
 */

#include "dsp_user_include.h"


void InverterSPDCheck(void)
{
	static Uint16 suwDCSPDSetFaultCnt=0,suwDCSPDClrFaultCnt=0;
	static Uint16 suwACSPDSetFaultCnt=0,suwACSPDClrFaultCnt=0;
	/**********************DC SPD Warning Checking******************************/
	if(ENABLE == stDspReceData.unFuncEnable.bit.DC_SPD)
	{
		if(0==DC_SPD_LEVEL)
		{
			suwDCSPDClrFaultCnt = 0;

			suwDCSPDSetFaultCnt++;
			if(suwDCSPDSetFaultCnt >= CLR_FAULT_TIME)
			{
				suwDCSPDSetFaultCnt = 0;
				stSysFaultReg.unSPDWarn.bit.DC_SPD=1;
				stSysFaultReg.unWarnBit.bit.SPD=1;
			}
		}
		else
		{
			suwDCSPDSetFaultCnt = 0;

			suwDCSPDClrFaultCnt++;
			if(suwDCSPDClrFaultCnt >= CLR_FAULT_TIME)
			{
				suwDCSPDClrFaultCnt = 0;
				stSysFaultReg.unSPDWarn.bit.DC_SPD=0;
			}
		}
	}
	else
	{
		stSysFaultReg.unSPDWarn.bit.DC_SPD=0;
	}

	/**********************AC SPD Warning Checking******************************/
	if(ENABLE == stDspReceData.unFuncEnable.bit.AC_SPD)
	{
		if(0 == AC_SPD_LEVEL)
		{
			suwACSPDClrFaultCnt = 0;

			suwACSPDSetFaultCnt++;
			if(suwACSPDSetFaultCnt >= CLR_FAULT_TIME)
			{
				suwACSPDSetFaultCnt = 0;
				stSysFaultReg.unSPDWarn.bit.AC_SPD=1;
				stSysFaultReg.unWarnBit.bit.SPD=1;
			}
		}
		else
		{
			suwACSPDSetFaultCnt = 0;

			suwACSPDClrFaultCnt++;
			if(suwACSPDClrFaultCnt >= CLR_FAULT_TIME)
			{
				suwACSPDClrFaultCnt = 0;
				stSysFaultReg.unSPDWarn.bit.AC_SPD=0;
			}
		}
	}
	else
	{
		stSysFaultReg.unSPDWarn.bit.AC_SPD=0;
	}

	if((1==stSysFaultReg.unWarnBit.bit.SPD)&&(0==stSysFaultReg.unSPDWarn.all))
	{
		stSysFaultReg.unWarnBit.bit.SPD = 0;
	}
}


void InverterNPECheck(void)
{

}
