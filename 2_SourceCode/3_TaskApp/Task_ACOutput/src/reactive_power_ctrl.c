/*
 * reactive_power_ctrl.c
 *
 *  Created on: 2021年5月10日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

ST_QUWAVE       stQUWave;
ST_QPWAVE       stQPWave;

static void ReactivePFCtrl(int16 wSetReactivePF)
{
	int16	wReactivePFAbsTmp;
	int32   dReactivePowerLimitTmp;     //Revised
	int16	wPFSinValueTmp;
	int32	dPFSinSquareTmp;


	/*******Limit Active Power by Reactive Control*************/
	wReactivePFAbsTmp = abs(wSetReactivePF);
	// P = S*Cos
	stLoadLimit.dActPowerReactiveLimit = (stLoadLimit.dApparentPowerLimitOutput*wReactivePFAbsTmp)/1000;
	dReactivePowerLimitTmp = ((Uint32)stLoadLimit.dApparentPowerLimitOutput*4843/10000);
	/*******Calculate sin^2 = (1-cos^2) = (1-cos)*(1+cos)************/
	dPFSinSquareTmp = (int32)(1000+wSetReactivePF)*(1000-wSetReactivePF);
	wPFSinValueTmp = uwCalcRms(dPFSinSquareTmp,1);
	// To prevent the excess of apparent power in the process of reactive power regulation
	if(stACSample.dActivePowerAvg  > (stLoadLimit.dActPowerReactiveLimit+8000))		// 800w
	{
		stLoadLimit.dReactivePowerRef = 0;
	}
	else
	{
		if(stSysCfg.uwFastDCMode == 1)
		{
			stLoadLimit.dReactivePowerRef = (stACSample.dActivePower*wPFSinValueTmp)/wSetReactivePF;
		}
		else
		{
			stLoadLimit.dReactivePowerRef = (stACSample.dActivePowerAvg*wPFSinValueTmp)/wSetReactivePF;
		}
	}
	if(wPFSinValueTmp != 0)
	{
		if(stLoadLimit.dReactivePowerRef >= dReactivePowerLimitTmp)
		{
		    stLoadLimit.dReactivePowerRef = dReactivePowerLimitTmp;
		    stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dReactivePowerRef*wSetReactivePF/wPFSinValueTmp;
	    	}
		else if(stLoadLimit.dReactivePowerRef <= -dReactivePowerLimitTmp)
		{
		    stLoadLimit.dReactivePowerRef = -dReactivePowerLimitTmp;
		    stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dReactivePowerRef*wSetReactivePF/wPFSinValueTmp;
		}
	}
	else
	{
		stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
		stLoadLimit.dReactivePowerRef = 0;
	}
}

static void ReactivePercentCtrl(int16 wSetReactivePercent)
{
	int32	dReactivePowerRefTmp;
	int16	wPFCosValueTmp;
	int32	dPFCosSquareTmp;

	/*******Calculate cos^2 = (1-sin^2) = (1-sin)*(1+sin)************/
	dPFCosSquareTmp = (int32)(1000+wSetReactivePercent)*(1000-wSetReactivePercent);
	wPFCosValueTmp = uwCalcRms(dPFCosSquareTmp,1);
    // Q = S*Percent  percent = sin
    if(ENABLE == stDspReceData.unFuncEnable.bit.SVG)
    {
        if(true == SvgParam.bEnterSVGStateFlag)
        {
            dReactivePowerRefTmp = ((int32)(stDspReceData.udOutputApparentPower)*wSetReactivePercent/1000);
            stLoadLimit.dActPowerReactiveLimit = 0;

        }
        else
        {
            dReactivePowerRefTmp = (stLoadLimit.dApparentPowerLimitOutput*wSetReactivePercent/1000);
            stLoadLimit.dActPowerReactiveLimit = (stLoadLimit.dApparentPowerLimitOutput*wPFCosValueTmp)/1000;
        }
    }
    else
    {
        dReactivePowerRefTmp = (stLoadLimit.dApparentPowerLimitOutput*wSetReactivePercent/1000);
        stLoadLimit.dActPowerReactiveLimit = (stLoadLimit.dApparentPowerLimitOutput*wPFCosValueTmp)/1000;
    }
	// P = S*Cos


	// To prevent the excess of apparent power in the process of reactive power regulation
	if(stACSample.dActivePowerAvg  > (stLoadLimit.dActPowerReactiveLimit+8000))		// 800w
	{
		stLoadLimit.dReactivePowerRef = 0;
	}
	else
	{
		stLoadLimit.dReactivePowerRef = dReactivePowerRefTmp;
	}
}

static void m_QUWave(ST_QUWAVE *pstQUWave_s)
{
	static int32 sdwReactPowerCmdVar = 0;
	static int16	swQvPowerLockInCnt,swQvPowerLockOutCnt,swQvPowerLockFlag;
	int16	wActivePowerPercentTmp;

	/************Enter/Quit Qv Mode***************/
	wActivePowerPercentTmp = stACSample.dActivePowerAvg*1000/stDspReceData.udOutputActivePower;		// ActivePower Percent
	if(wActivePowerPercentTmp<=pstQUWave_s->stIn.uwPowerLockOut)
	{
		swQvPowerLockInCnt = 0;
		swQvPowerLockOutCnt++;
		if(swQvPowerLockOutCnt>=10)	// 10*20ms = 200ms
		{
			swQvPowerLockOutCnt=0;
			swQvPowerLockFlag = 0;
		}
	}
	else if(wActivePowerPercentTmp>=pstQUWave_s->stIn.uwPowerLockIn)
	{
		swQvPowerLockOutCnt = 0;
		swQvPowerLockInCnt++;
		if(swQvPowerLockInCnt>=10)	// 10*20ms = 200ms
		{
			swQvPowerLockInCnt=0;
			swQvPowerLockFlag = 1;
		}
	}
	else
	{
		swQvPowerLockOutCnt = 0;
		swQvPowerLockInCnt = 0;
	}

	/************Qv Mode Start***************/
	if(swQvPowerLockFlag==1)
	{
		if(pstQUWave_s->stIn.uwGridVoltRmsMax > pstQUWave_s->stIn.uwHighVoltStartPoint)
		{
			if(pstQUWave_s->stIn.uwGridVoltRmsMax < pstQUWave_s->stIn.uwHighVoltEndPoint)
			{
				sdwReactPowerCmdVar = (-(int32)pstQUWave_s->stIn.wQperbyVhi*(pstQUWave_s->stIn.uwGridVoltRmsMax - pstQUWave_s->stIn.uwHighVoltStartPoint))
									 /(pstQUWave_s->stIn.uwHighVoltEndPoint - pstQUWave_s->stIn.uwHighVoltStartPoint);
			}
			else
			{
				sdwReactPowerCmdVar = pstQUWave_s->stIn.wQperbyVhi;
			}
		}
		else if(pstQUWave_s->stIn.uwGridVoltRmsMax < pstQUWave_s->stIn.uwLowVoltStartPoint)
		{
			if(pstQUWave_s->stIn.uwGridVoltRmsMax > pstQUWave_s->stIn.uwLowVoltEndPoint)
			{
				sdwReactPowerCmdVar = (-(int32)pstQUWave_s->stIn.wQperbyVlo*(pstQUWave_s->stIn.uwLowVoltStartPoint - pstQUWave_s->stIn.uwGridVoltRmsMax))
									 /(pstQUWave_s->stIn.uwLowVoltStartPoint - pstQUWave_s->stIn.uwLowVoltEndPoint);
			}
			else
			{
				sdwReactPowerCmdVar = pstQUWave_s->stIn.wQperbyVlo;
			}
		}
		else
		{
			sdwReactPowerCmdVar = 0;
		}
	}
	else
	{
		sdwReactPowerCmdVar = 0;
	}
	stDebug.ReadData.wQvPFValue = sdwReactPowerCmdVar;
	ReactivePercentCtrl(sdwReactPowerCmdVar);
}

static void m_QUPFWave(ST_QUWAVE *pstQUWave_s)
{
	static Uint16 suwPFReduceFlag = 0;
	static Uint16 suwLoadLimitFlag = 0;
    static Uint16 suwLoadLimitFlagStartcnt = 0;
    static Uint16 suwLoadLimitFlagEndcnt = 0;
	static Uint16 suwPFTemp = 1000;
	static Uint16 suwLoadDelt = 0;
	static Uint16 suwLoadBackDelt = 0;
	static float32 sfTan = 0;
	static float32 sfSin = 0;
	static float32 sfCos = 0;
	static Uint32 sdwLoadLimitMin = 0;
	static Uint16 suwRatedPhaseVolt= 0;
	static int32 sdwReactPowerCmdVar = 0;
	int32	dReactivePowerRefTmp;
	int16	wPFCosValueTmp;
	int32	dPFCosSquareTmp;
	
	static Uint32 dwPFLoadLimit= 0;

	/************Enter/Quit Qv Mode***************/
	suwLoadDelt = stDspReceData.udOutputActivePower*0.025;
	suwLoadBackDelt =stDspReceData.udOutputActivePower*0.005;
	
	if(pstQUWave_s->stIn.uwGridVoltRmsMax > pstQUWave_s->stIn.uwPFStartVolt)
	{
		suwPFReduceFlag = 1;
	}
	else if((pstQUWave_s->stIn.uwGridVoltRmsMax < pstQUWave_s->stIn.uwPFBackVolt)&&(stDspReceData.udOutputActivePower ==dwPFLoadLimit))
	{
		suwPFReduceFlag = 0;
	}

	if(eInverterStatus== cInverterStatus)
	{
		if(1 == suwPFReduceFlag)
		{
			if(suwPFTemp > pstQUWave_s->stIn.uwPFEndPoint)//stF107Data.uwQvPFAutoAdjustPF
			{
				suwPFTemp = suwPFTemp - 2;//reduce PF from 1.0 to stF107Data.uwQvPFAutoAdjustPF 0.9
			}
			else
			{
				suwPFTemp = pstQUWave_s->stIn.uwPFEndPoint;//stF107Data.uwQvPFAutoAdjustPF
			}
		}
		else
		{
			if(suwPFTemp < 1000)
			{
				suwPFTemp = suwPFTemp + 2;//increase PF from stF107Data.uwQvPFAutoAdjustPF to 1.0
			}
			else
			{
				stDebug.SetData.wIqRef = 0;
				suwPFTemp = 1000;
			}
		}
		
		if((pstQUWave_s->stIn.uwGridVoltRmsMax > pstQUWave_s->stIn.uwPowerStartVolt)&&(pstQUWave_s->stIn.uwPFEndPoint == suwPFTemp))
		{
		    suwLoadLimitFlagStartcnt++;
		    if(suwLoadLimitFlagStartcnt > 20)
		    {
		        suwLoadLimitFlagStartcnt = 0;
		    	suwLoadLimitFlag = 1;
		    }
            suwLoadLimitFlagEndcnt = 0;
		}
		else if(pstQUWave_s->stIn.uwGridVoltRmsMax < (pstQUWave_s->stIn.uwPowerBackVolt - 7))//7
		{
            suwLoadLimitFlagEndcnt++;
            if(suwLoadLimitFlagEndcnt > 20)
            {
                suwLoadLimitFlagEndcnt = 0;
                suwLoadLimitFlag = 0;
            }
            suwLoadLimitFlagStartcnt = 0;
		}

		if(1 == suwLoadLimitFlag)
		{
            if(DC_SOURCE == stSysCfg.eMpptMode)
            {
                if(dwPFLoadLimit > pstQUWave_s->stIn.uwPowerEndPoint)
                {
                    dwPFLoadLimit = dwPFLoadLimit - suwLoadDelt;//reduce output RatePower from 100.0% to stF107Data.uwQvPFAutoAdjustPF %
                    if(stACSample.dActivePower > AC12000W)
                    {
                        stDebug.SetData.wIqRef = -8;
                    }
                    else if(stACSample.dActivePower > AC8000W)
                    {
                        stDebug.SetData.wIqRef = -3;
                    }
                    else if(stACSample.dActivePower > AC4000W)
                    {
                        stDebug.SetData.wIqRef = 2;
                    }
                    else
                    {
                        stDebug.SetData.wIqRef = 0;
                    }
                }
                else
                {
                    dwPFLoadLimit = pstQUWave_s->stIn.uwPowerEndPoint;//stF107Data.uwQvPFAutoAdjustPF
                    stDebug.SetData.wIqRef += 12;
                    if(stDebug.SetData.wIqRef > 35)
                    {
                        stDebug.SetData.wIqRef = 35;
                    }
                }
            }
            else
            {
                if(dwPFLoadLimit > pstQUWave_s->stIn.uwPowerEndPoint)
                {
                    stDebug.ReadData.wQVPFFlag = 1;
                    dwPFLoadLimit = dwPFLoadLimit - suwLoadDelt;//reduce output RatePower from 100.0% to stF107Data.uwQvPFAutoAdjustPF %
                    if(stACSample.dActivePower > AC12000W)
                    {
                        stDebug.SetData.wIqRef = -8;//-8
                    }
                    else if(stACSample.dActivePower > AC8000W)
                    {
                        stDebug.SetData.wIqRef = -3;//-3
                    }
                    else if(stACSample.dActivePower > AC4000W)
                    {
                        stDebug.SetData.wIqRef = 2;//2
                    }
                    else
                    {
                        stDebug.SetData.wIqRef = 0;//0
                    }
                }
                else
                {
                    dwPFLoadLimit = pstQUWave_s->stIn.uwPowerEndPoint;//stF107Data.uwQvPFAutoAdjustPF
                    stDebug.SetData.wIqRef = 2;/////////////////////////////////////////////////////
                    stDebug.ReadData.wQVPFFlag = 0;
                }
            }

		}
		else
		{
			if(dwPFLoadLimit <stDspReceData.udOutputActivePower)
			{
				dwPFLoadLimit= dwPFLoadLimit + suwLoadBackDelt;//increse output RatePower from o stF107Data.uwQvPFAutoAdjustPF % to 100.0% 
				stDebug.ReadData.wQVPFFlag = 1;
				if(stACSample.dActivePower < AC8000W)
				{
					stDebug.SetData.wIqRef = 40;
				}
				else
				{
					stDebug.SetData.wIqRef = 35;
				}
			}
			else
			{
				dwPFLoadLimit = stDspReceData.udOutputActivePower;
				stDebug.ReadData.wQVPFFlag = 0;
			}
		}

	}
	else
	{
		suwPFTemp = 1000;
		dwPFLoadLimit = stDspReceData.udOutputActivePower;
		suwLoadLimitFlag = 0;
        suwLoadLimitFlagStartcnt = 0;
        suwLoadLimitFlagEndcnt = 0;
	}

	stDebug.ReadData.wQvPFValue = suwPFTemp;
	ReactivePFCtrl(suwPFTemp);
	
	if(stACSample.dActivePowerAvg  > (stLoadLimit.dActPowerReactiveLimit+8000))		// 800w
	{
		stLoadLimit.dActPowerReactiveLimit = 0;
	}
	else
	{
		stLoadLimit.dActPowerReactiveLimit = dwPFLoadLimit;
	}
	
}
static void m_QPWave(ST_QPWAVE *pstQPWave_s)
{
    static int32 sdwReactPowerCmdVar = 0;
    int16   wActivePowerPercentTmp=0;

    /************Enter/Quit QP Mode***************/
    wActivePowerPercentTmp = pstQPWave_s->stIn.dwCurrentActPower*1000/pstQPWave_s->stIn.dwRateActiveOutput;     // ActivePower Percent
    if(wActivePowerPercentTmp>1000)
    {
        wActivePowerPercentTmp = 1000;
    }
    if(STRANDARD_GERMANY_VDE4110 ==stDspReceData.unSafetyOdm.bit.SafetyStandard)
    {
        if((wActivePowerPercentTmp > 900)&&(wActivePowerPercentTmp <= 1000))
        {
            sdwReactPowerCmdVar = pstQPWave_s->stIn.dwRateActiveOutput*33/100;
        }
         else if(wActivePowerPercentTmp > 600)
        {
            sdwReactPowerCmdVar = pstQPWave_s->stIn.dwRateActiveOutput*(((wActivePowerPercentTmp-600)*28/300)+5)/100;
        }
        else if(wActivePowerPercentTmp > 500)
        {
            sdwReactPowerCmdVar = pstQPWave_s->stIn.dwRateActiveOutput*((wActivePowerPercentTmp-500)*5/100)/100;
        }
        else
        {
          sdwReactPowerCmdVar = 0;
        }
    }
    else if(STRANDARD_AUSTRIA ==stDspReceData.unSafetyOdm.bit.SafetyStandard)
    {
        if((wActivePowerPercentTmp > 500)&&(wActivePowerPercentTmp <= 1000))
        {
            sdwReactPowerCmdVar = pstQPWave_s->stIn.dwRateActiveOutput*((int32)(wActivePowerPercentTmp-500)*484/500)/1000;
        }
        else
        {
          sdwReactPowerCmdVar = 0;
        }
    }
    else
    {
        if((wActivePowerPercentTmp > 500)&&(wActivePowerPercentTmp <= 1000))
        {
            sdwReactPowerCmdVar = pstQPWave_s->stIn.dwRateActiveOutput*((int32)(wActivePowerPercentTmp-500)*484/500)/1000;
        }
        else
        {
          sdwReactPowerCmdVar = 0;
        }
    }

    stLoadLimit.dActPowerReactiveLimit = pstQPWave_s->stIn.dwRateActiveOutput*pstQPWave_s->stIn.uwPowerPercent/1000;
    stLoadLimit.dReactivePowerRef = sdwReactPowerCmdVar;
}
static void ReactiveQvCtrl(void)
{
	Uint16	uwTaiWan_ACNormalVolt = 3810;//tai wan grid voltage
	stQUWave.stIn.uwGridSpec = stDspReceData.unSafetyOdm.bit.SafetyStandard;
	stQUWave.stIn.uwGridVoltRmsMax = stACSample.wLineVoltRmsMax;
	stQUWave.stIn.uwHighVoltStartPoint = stF107Data.uwQvHighVoltStartPoint;
	stQUWave.stIn.uwHighVoltEndPoint = stF107Data.uwQvHighVoltEndPoint;
	stQUWave.stIn.wQperbyVhi = (int16)stF107Data.uwQvHighVoltReactivePrt;
	stQUWave.stIn.uwLowVoltStartPoint = stF107Data.uwQvLowVoltStartPoint;
	stQUWave.stIn.uwLowVoltEndPoint = stF107Data.uwQvLowVoltEndPoint;
	stQUWave.stIn.wQperbyVlo = (int16)stF107Data.uwQvLowVoltReactivePrt;
	stQUWave.stIn.uwPowerLockIn = 200;		// 20% Power Enter Qv
	stQUWave.stIn.uwPowerLockOut = 100;			// 10% Power Quit Qv

	//tai'wan AEC
	stQUWave.stIn.uwPFStartVolt = (Uint32)uwTaiWan_ACNormalVolt * stF107Data.uwDeratPFVoltPrt / 100;
	stQUWave.stIn.uwPFBackVolt = (Uint32)uwTaiWan_ACNormalVolt * (stF107Data.uwDeratPFVoltPrt - 1) / 100;
	stQUWave.stIn.uwPFEndPoint = stF107Data.uwQvPFAutoAdjustPF;
	
	stQUWave.stIn.uwPowerStartVolt = (Uint32)stF107Data.uwQvPFPvHighVoltPrt * uwTaiWan_ACNormalVolt / 100;
	stQUWave.stIn.uwPowerBackVolt = (Uint32)(stF107Data.uwQvPFPvHighVoltPrt - 1) * uwTaiWan_ACNormalVolt / 100;//back Volt default to (stF107Data.uwQvPFPvHighVoltPrt - 1%)
	stQUWave.stIn.uwPowerEndPoint = (Uint32)stDspReceData.udOutputActivePower * stF107Data.uwQvPFAutoAdjustPower / 1000;
	
	stQUWave.stIn.uwAutoAdjustPower = (Uint32)stF107Data.uwQvPFAutoAdjustPower;
	stQUWave.stIn.uwLoadLimitVolt =105;
	
	if(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard
	&& MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType)
	{
		m_QUPFWave(&stQUWave);
		if(DC_SOURCE == stSysCfg.eMpptMode)
		{
			stSysCfg.uwFastDCMode = 1;
		}
		else
		{
			stSysCfg.uwFastDCMode = 0;
		}
	}
	else
	{
		m_QUWave(&stQUWave);
	}
			
}

static void ActivePFCtrl(int16	wActivePercent)
{
	static swPowerPFValue;

	swPowerPFValue= (5500-wActivePercent)/5;		// y = -0.2*(x-5500)

	ReactivePFCtrl(swPowerPFValue);
}
static void ReactiveQPCtrl(void)
{
    stQPWave.stIn.uwPowerPercent = stF107Data.uwActiveRate;   //~{SP9&9&BJ0Y7V1H~}
    stQPWave.stIn.uwGridVoltRmsMax = (Uint16)stACSample.wLineVoltRmsMaxAvg;   //~{Wn4sO_5gQ9~}
    stQPWave.stIn.dwCurrentActPower = (Uint32)stACSample.dActivePowerAvg;     //~{51G09&BJ~}
    stQPWave.stIn.dwRateActiveOutput = stDspReceData.udOutputActivePower;    //~{6n6(9&BJ~}

    m_QPWave(&stQPWave);
}
static void PowerPFCtrl(void)
{
    static int16  swPowerPFValue;
    static int16  swActPowerPercent,swLineVoltRmsStart,swLineVoltRmsEnd;

    swActPowerPercent = stACSample.dActivePowerAvg*1000/stDspReceData.udOutputActivePower;
    swLineVoltRmsStart = ((int32)stDspReceData.uwACNormalVolt*stF107Data.swP_PFLineVoltPercentStart)/1000;
    swLineVoltRmsEnd = ((int32)stDspReceData.uwACNormalVolt*stF107Data.swP_PFLineVoltPercentRecover)/1000;

    if(((STRANDARD_IRELAND_EN50549 ==stDspReceData.unSafetyOdm.bit.SafetyStandard)||(STRNDARD_BRAZIL ==stDspReceData.unSafetyOdm.bit.SafetyStandard)
       ||(STRANDARD_ITALY_CEI021_INTERNAL ==stDspReceData.unSafetyOdm.bit.SafetyStandard))&&(stACSample.wLineVoltRmsMaxAvg > swLineVoltRmsStart))
    {
        if(swActPowerPercent>1000)
        {
            swActPowerPercent = 1000;
        }
        if(swActPowerPercent < stF107Data.swP_PFActPowerPercentStart)
        {
            stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
            stLoadLimit.dReactivePowerRef = 0;
        }
        else
        {
            swPowerPFValue = 1000-100*(swActPowerPercent-stF107Data.swP_PFActPowerPercentStart)/(1000-stF107Data.swP_PFActPowerPercentStart);
            if(swPowerPFValue >= 1000)
            {
                swPowerPFValue = 1000;
            }
            if(swPowerPFValue <= -1000)
            {
                swPowerPFValue = -1000;
            }
            ReactivePFCtrl(swPowerPFValue);
        }
    }
    else if(stACSample.wLineVoltRmsMaxAvg > swLineVoltRmsStart)
    {
        if(swActPowerPercent>1000)
        {
            swActPowerPercent = 1000;
        }
        if(swActPowerPercent < stF107Data.swP_PFActPowerPercentStart)
        {
            stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
            stLoadLimit.dReactivePowerRef = 0;
        }
        else
        {
            swPowerPFValue = 1000-100*(swActPowerPercent-stF107Data.swP_PFActPowerPercentStart)/(1000-stF107Data.swP_PFActPowerPercentStart);
            if(swPowerPFValue >= 1000)
            {
                swPowerPFValue = 1000;
            }
            if(swPowerPFValue <= -1000)
            {
                swPowerPFValue = -1000;
            }
            ReactivePFCtrl(swPowerPFValue);
        }
    }
    else if(stACSample.wLineVoltRmsMaxAvg < swLineVoltRmsEnd)
    {
            stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
            stLoadLimit.dReactivePowerRef = 0;
    }
}

void ReactivePowerCtrl(void)
{
	static int16	swActivePowerPercent;
	if((cInverterStatus == eInverterStatus)&&(stACSample.dActivePowerAvg>5000)&&(false == SvgParam.bEnterSVGStateFlag))	// 500w
	{
#if F107_BOARD
		if((((stF107Data.wCosPhi>=PF_MIN)&&(stF107Data.wCosPhi<PF_MAX))		// +0.8 ~ +1.0
		 || ((stF107Data.wCosPhi>-PF_MAX)&&(stF107Data.wCosPhi<=-PF_MIN)))		// -0.8 ~ -1.0
		 &&  (FIXED_PF_MODE==stF107Data.uwPFCtrlMode))
		{
			ReactivePFCtrl(stF107Data.wCosPhi);
		}
		else if((stF107Data.wReactiveRate<=REQCTIVE_PERCENT_MAX)
			  &&(stF107Data.wReactiveRate>=-REQCTIVE_PERCENT_MAX)
			  &&(REACTIVE_PERCENT_MODE==stF107Data.uwPFCtrlMode))
		{
			ReactivePercentCtrl(stF107Data.wReactiveRate);
		}
		else if(REACTIVE_QV_MODE==stF107Data.uwPFCtrlMode)
		{
			ReactiveQvCtrl();
		}
		else if(ACTIVE_PF_MODE==stF107Data.uwPFCtrlMode)
		{
			swActivePowerPercent = stACSample.dActivePowerAvg*1000/stDspReceData.udOutputActivePower;
			if(swActivePowerPercent<500)
			{
				stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
				stLoadLimit.dReactivePowerRef = 0;
			}
			else
			{
				ActivePFCtrl(swActivePowerPercent);
			}
		}
//        else if(REACTIVE_QP_MODE == stF107Data.uwPFCtrlMode)
//        {
//            ReactiveQPCtrl();
//        }
        else if(POWER_PF_MODE==stF107Data.uwPFCtrlMode)
        {
            PowerPFCtrl();
        }

#else
		if((((stDebug.SetData.wReacitvePF>=PF_MIN)&&(stDebug.SetData.wReacitvePF<PF_MAX))		// +0.8 ~ +1.0
		 || ((stDebug.SetData.wReacitvePF>-PF_MAX)&&(stDebug.SetData.wReacitvePF<=-PF_MIN)))		// -0.8 ~ -1.0
		&&	 (FIXED_PF_MODE==stDebug.SetData.uwReactiveModel))
		{
			ReactivePFCtrl(stDebug.SetData.wReacitvePF);
		}
		else if((stDebug.SetData.wReacitvePercent<=REQCTIVE_PERCENT_MAX)
			  &&(stDebug.SetData.wReacitvePercent>=-REQCTIVE_PERCENT_MAX)
			  &&(REACTIVE_PERCENT_MODE==stDebug.SetData.uwReactiveModel))
		{
			ReactivePercentCtrl(stDebug.SetData.wReacitvePercent);
		}
#endif
		else
		{
			stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
			stLoadLimit.dReactivePowerRef = 0;
		}
	}
	else if((ENABLE == stDspReceData.unFuncEnable.bit.SVG)&&(true == SvgParam.bEnterSVGStateFlag))
	{
	    ReactivePercentCtrl(stF107Data.wReactiveRate);
	}
	else
	{
		stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
		stLoadLimit.dReactivePowerRef = 0;
	}

	if(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard \
	&& MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType\
	&& REACTIVE_QV_MODE != stF107Data.uwPFCtrlMode)
	{
		stSysCfg.uwFastDCMode = 0;
	}
}
