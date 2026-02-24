/*
 * safty_freq_derating.h
 *
 *  Created on: 2021Äê11ÔÂ8ÈÕ
 *      Author: Administrator
 */

#ifndef SAFTY_FREQ_DERATING_H
#define SAFTY_FREQ_DERATING_H_


#define cFrozFreqDisable			0
#define cFrozFreqEnable				1

typedef struct
{
	struct
	{
		Uint16  uwFreqDeratEnable;
		Uint16 	uwFreqDeratRate;
		Uint16  uwFreqDeratStartPoint;
        Uint16  uwFreqDeratEndPoint;
		Uint16  uwFreqDeratRecoverPoint;
		Uint16  uwCurrentFreq;
		Uint32 	dwRateActiveOutput;
		Uint32 	dwCurrentActPower;
		Uint16 	uwLoadRecoverSpeed;
	}stIn;

	struct
	{
		Uint32 	dwFreqDeratActPowerLimitOut;
	}stOut;

}ST_FREQ_DERATE;

extern void App_FreqDerating(void);

extern ST_FREQ_DERATE  stFreqDerating;

#endif /* 3_TASKAPP_TASK_ACOUTPUT_INC_SAFTY_FREQ_DERATING_H_ */
