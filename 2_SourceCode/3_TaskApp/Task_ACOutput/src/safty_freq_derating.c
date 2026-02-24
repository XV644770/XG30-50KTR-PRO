/*
 * safty_freq_derating.c
 *
 *  Created on: 2021?ê11??8??
 *      Author: Administrator
 */


#include "dsp_user_include.h"


ST_FREQ_DERATE  stFreqDerating;

static void m_FreqDerating(ST_FREQ_DERATE *pstFreqDerate_s);


void App_FreqDerating(void)
{
	if((ENABLE == stF107Data.unEnable.bit.OverFreqDerate)&&(cInverterStatus==eInverterStatus))		// Over Frequency Derating
	{
		stFreqDerating.stIn.uwFreqDeratEnable = ENABLE;
		stFreqDerating.stIn.uwFreqDeratStartPoint = stF107Data.uwOverFreqDeratePoint;
        stFreqDerating.stIn.uwFreqDeratEndPoint = stF107Data.uwFreqDeratEndPoint;
		stFreqDerating.stIn.uwFreqDeratRecoverPoint = (stF107Data.uwOverFreqDeratePoint-10);
		stFreqDerating.stIn.dwCurrentActPower = stACSample.dActivePowerAvg;
		stFreqDerating.stIn.uwCurrentFreq = stACSample.wGridFreqAvg;
		stFreqDerating.stIn.uwFreqDeratRate = stF107Data.uwFreqPowerChangeRate;		// 40.0%P/Hz
		stFreqDerating.stIn.uwLoadRecoverSpeed = stF107Data.uwStartLoadSpeed;
		stFreqDerating.stIn.dwRateActiveOutput = stLoadLimit.dActivePower;

		m_FreqDerating(&stFreqDerating);

		if(stFreqDerating.stOut.dwFreqDeratActPowerLimitOut > stLoadLimit.dActivePower)
		{
			stFreqDerating.stOut.dwFreqDeratActPowerLimitOut = stLoadLimit.dActivePower;
		}
	}
	else
	{
		stFreqDerating.stIn.uwFreqDeratEnable = DISABLE;
		stFreqDerating.stOut.dwFreqDeratActPowerLimitOut = stLoadLimit.dActivePower;
	}
	stLoadLimit.dActPowerOverFreqLimit = stFreqDerating.stOut.dwFreqDeratActPowerLimitOut;
}

static void m_FreqDerating(ST_FREQ_DERATE *pstFreqDerate_s)
{
	static Uint16 suwLockCnt = 0;
	static Uint32 sdwFrozenPower = 0;
	static Uint32 sdwDeltaPower = 0;
	static Uint16 suwFreqLockBackCnt = 0;
	static Uint16 suwLoadLimitFlag = 0;
	static Uint16 suwPowerFrozenFlag = 0;
	static Uint32 sdwLoadRecoverDelta = 0;
	static Uint16 suwLoadSpeed100msCnt;

	if(ENABLE == pstFreqDerate_s->stIn.uwFreqDeratEnable)
	{
        if(pstFreqDerate_s->stIn.uwCurrentFreq > pstFreqDerate_s->stIn.uwFreqDeratStartPoint)
        {
			if(suwLockCnt < 15)
			{
				suwLockCnt++;
			}
			else
			{
				if(0 == suwPowerFrozenFlag)
				{
					suwPowerFrozenFlag = 1;
					sdwFrozenPower = pstFreqDerate_s->stIn.dwCurrentActPower;
				}
                if(pstFreqDerate_s->stIn.uwCurrentFreq <= pstFreqDerate_s->stIn.uwFreqDeratEndPoint)
                {
                    sdwDeltaPower = (sdwFrozenPower*((int32)pstFreqDerate_s->stIn.uwCurrentFreq - (int32)pstFreqDerate_s->stIn.uwFreqDeratStartPoint))/100;
                    sdwDeltaPower = (sdwDeltaPower*(int32)pstFreqDerate_s->stIn.uwFreqDeratRate)/1000;
                }
                else
                {
                     sdwDeltaPower = (sdwFrozenPower*((int32)pstFreqDerate_s->stIn.uwFreqDeratEndPoint - (int32)pstFreqDerate_s->stIn.uwFreqDeratStartPoint))/100;
                     sdwDeltaPower = (sdwDeltaPower*(int32)pstFreqDerate_s->stIn.uwFreqDeratRate)/1000;
                }
				if(sdwFrozenPower > sdwDeltaPower )
				{
					pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut = sdwFrozenPower - sdwDeltaPower;
				}
				else
				{
					pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut = 0;
				}
				suwLoadLimitFlag = 1;
			}
			suwFreqLockBackCnt = 0;
			suwLoadSpeed100msCnt = 0;
		}
		else
		{
			suwLockCnt = 0;
			if(pstFreqDerate_s->stIn.uwCurrentFreq < pstFreqDerate_s->stIn.uwFreqDeratRecoverPoint)
			{
				if(suwFreqLockBackCnt < 15)
				{
					suwFreqLockBackCnt++;
				}
				else
				{
					suwPowerFrozenFlag = 0;
				}
			}
			else
			{
				suwFreqLockBackCnt = 0;
			}

			if((1 == suwLoadLimitFlag)&&(0 == suwPowerFrozenFlag))
			{
				suwLoadSpeed100msCnt++;
				if(suwLoadSpeed100msCnt>=5)		// 20ms * 5 = 100ms
				{
					suwLoadSpeed100msCnt = 0;
					sdwLoadRecoverDelta = (pstFreqDerate_s->stIn.dwRateActiveOutput*pstFreqDerate_s->stIn.uwLoadRecoverSpeed/600000);
					pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut = pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut + sdwLoadRecoverDelta;

					if(pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut >= pstFreqDerate_s->stIn.dwRateActiveOutput)
					{
						pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut = pstFreqDerate_s->stIn.dwRateActiveOutput;
						suwLoadLimitFlag = 0;
					}
				}
			}
			else
			{
				suwLoadSpeed100msCnt = 0;
			}
		}
	}
	else
	{
		pstFreqDerate_s->stOut.dwFreqDeratActPowerLimitOut = pstFreqDerate_s->stIn.dwRateActiveOutput;
	}
}




