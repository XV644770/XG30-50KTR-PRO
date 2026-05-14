/*
 * safty_load_limit.h
 *
 *  Created on: 2021年1月7日
 *      Author: Administrator
 */

#ifndef SAFTY_LOAD_LIMIT_H
#define SAFTY_LOAD_LIMIT_H

#define NORMAL_VOLT		4000		// 230V*1.732 = 4000
#define GRID_VOLT_LIMIT	3810		// 220V*1.732 = 3810

#define PV_VOLT_LIMIT_MIN   2000
#define PV_VOLT_LIMIT_MAX   4800
#define PV_VOLT_LIMIT_MAX_30k   4200

#define	ATE_ADJUST_NORMAL		0
#define	ATE_ADJUST_30			1
#define	ATE_ADJUST_70			2

#define PV_CURR_LIMIT_MIN_60_70K    64		// 2A * 32 = 64

#define	PVVOLT_HIGH_DERATING		1
#define BOOST_TEMP_DERATING			2
#define INV_TEMP_DERATING			3
#define AM_TEMP_DERATING			4
#define SETTING_DERATING			5
#define REMPPT_DERATING				6
#define REACTIVE_DERATING			7
#define LOADSPEED_DERATING			8
#define ACVOLT_HIGH_DERATING		9
#define ATE_ADJUST_DERATING			10
#define ANTI_FLOW_DERATING			11
#define MULTI_ANTI_FLOW_DERATING    12
#define OVER_FREQ_DERATING			13

typedef struct
{

	int16 wRatedActiveCurr;			// Rated active current rms
	int16 wRatedApparentCurr;		// Rated apparent current rms

	int32 dApparentPower;			// Rated apparent power
	int32 dActivePower;			// Rated active power
	int32 dReactivePower;         // Rated Reactive power

	Uint16 uwDeratingMode;

	int32 dReactivePowerRef;
	int16 wReactivePowerRef;
	int32 wVoltReactiveCurrLimit;

	int32 dApparPowerActual;		// Actual apparent power:According to the actual grid voltage calculation
	int32 dActivePowerActual;		// P = ActuralVoltage *RatedCurrent   Actual active power:According to the actual grid voltage calculation

	int32 dActPowerInvBoostTempLimit;
	int32 dActPowerAmTempLimit;
	int32 dActPowerSetLimit;
	int32 dActPowerRemmptLimit;
	int32 dActPowerReactiveLimit;
	int32 dActPowerPVHighVoltLimit;
	int32 dActPowerLoadSpeedLimit;
	int32 dActPowerATEAdjustLimit;
	int32 dActPowerACHighVoltLimit;
	int32 dActPowerAntiFlowLimit;
	int32 dActPowerMultiAntiFlowLimit;
	int32 dActPowerOverFreqLimit;

	int32 dActPowerLimitOutput;
	int32 dActPowerOutputRef;

	int32 dApparentPowerPVHighVoltLimit;
	int32 dApparentPowerPVLowVoltLimit;
	int32 dApparentPowerLimitOutput;
	int32 dApparentPowerRemmptLimit;

	int16 wVoltActiveCurrLimit;
    int16 wBTCurrLimit[PV_MAX_NUM];
    int16 wBTCurrLimitMax;
}ST_LOAD_LIMIT;

extern ST_LOAD_LIMIT stLoadLimit;
extern ST_CTRL_LOOP stReactivePowerCtrl;

extern void LoadLimitParaInit(void);
extern void InverterLoadLimit(void);

#endif
