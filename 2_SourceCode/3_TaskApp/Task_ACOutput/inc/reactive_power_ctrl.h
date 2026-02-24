/*
 * reactive_power_ctrl.h
 *
 *  Created on: 2021骞?5鏈?10鏃?
 *      Author: Administrator
 */

#ifndef REACTIVE_POWER_CTRL_H
#define REACTIVE_POWER_CTRL_H


#define		PF_MAX		1000		// 1000/1000 = 1
#define		PF_MIN		800			// 800/1000 = 0.8

#define		REQCTIVE_PERCENT_MAX	500

#define		FIXED_PF_MODE			0
#define		REACTIVE_PERCENT_MODE	1
#define		REACTIVE_QV_MODE		2
#define		ACTIVE_PF_MODE			3
#define     REACTIVE_QP_MODE        4
#define     POWER_PF_MODE           5

typedef struct
{
	struct
	{
		Uint16  uwGridSpec;
		Uint16  uwHighVoltStartPoint;
		Uint16  uwHighVoltEndPoint;
		int16   wQperbyVhi;
		Uint16  uwLowVoltStartPoint;
		Uint16  uwLowVoltEndPoint;
		int16   wQperbyVlo;
		Uint16  uwGridVoltRmsMax;

		Uint16	uwPowerLockIn;
		Uint16	uwPowerLockOut;

		Uint16	uwPFStartVolt;
		Uint16	uwPFBackVolt;
		Uint16	uwPFEndPoint;
		
		Uint16	uwPowerStartVolt;
		Uint16	uwPowerBackVolt;
		Uint16	uwPowerEndPoint;
		
		Uint16	uwAutoAdjustPower;
		Uint16	uwAutoAdjustPF;
		Uint16	uwLoadLimitVolt;
	}stIn;

	struct
	{
		Uint32 	dwLoadLimit;
	}stOut;

}ST_QUWAVE;
typedef struct
{
    struct
    {
        Uint16  uwPowerPercent;    //?鈥??鸥?戮氓藛鈥犆β?鈥?
        Uint32  dwCurrentActPower;   //氓陆鈥??聧?鸥?鈥?
        Uint32  dwRateActiveOutput;   //茅垄聺氓庐拧?鸥?鈥?
        Uint16  uwGridVoltRmsMax;    //?鈧?氓陇搂莽潞驴?碌氓沤鈥?
    }stIn;

    struct
    {
        Uint32  dwLoadLimit;
    }stOut;

}ST_QPWAVE;
extern ST_QUWAVE       stQUWave;
extern ST_QPWAVE       stQPWave;

#endif /* 3_TASKAPP_TASK_ACOUTPUT_INC_REACTIVE_POWER_CTRL_H_ */
