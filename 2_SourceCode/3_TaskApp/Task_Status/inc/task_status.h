/*
* system_periph_init.h
*
*	Product:			three phase inveter
*	Filename:			system_periph_init.h
*	Purpose:			Stm32 system init
*	Date				Version		Author			Note
*   20/11/18			V1.01		zhaohui			none
*/

#ifndef TASK_STATUS_H
#define TASK_STATUS_H

#include "safty_relay_check.h"

#define BUS_SOFT_START_VOLT    8000	// 800V

#define RELAY_VOLT_MAX		800//500		// 50V
#define RELAY_LINE_MAX		250		// 30V

typedef struct
{
	Uint16      uwF042OnOff;			// 1:Close F042'relay  0:Break F042'relay
	Uint16      uwCheckCnt;
	Uint16		uwCheckStep;
	Uint16      uwFaultTimes;
	int16      	wPhaseVoltRmsMax;
	int16      	wPhaseVoltRmsMin;
	struct
	{
		Uint16 	Success 	    :1;  	// 1:Relay is Normal 0:Relay is abnormal
		Uint16 	Fail		    :1;
		Uint16	InvPwmOn		:1;
		Uint16	InvCurrOver		:1;
		Uint16	VoltNormal		:1;
		Uint16	InvPwmoff		:1;
		Uint16	rsv				:10;
	}Flag;
}ST_RELAY_CHECK;


typedef struct
{
	Uint16      uwStartCnt;
	struct
	{
		Uint16 	Success 	    	:1;  	// 1:Bus Soft Start Normal
		Uint16 	Fail		    		:1;
		Uint16	rsv				:14;
	}Flag;
}ST_BUS_START;

typedef struct
{
	Uint16      uwInvtPwmOpenCnt;
	struct
	{
		Uint16 	Success		    	:1;
		Uint16	Fail				:1;
		Uint16	rsv					:14;
	}Flag;
}ST_FAN_INVT_OPEN_CHECK;

//Standby step for waiting status
typedef enum
{
	cInit,cBusSoftStart,cFanInvtOpenCheck,cRelayCheck,cGridConnect		// 20220719 Revised cFanInvtCheck Step
}ENUM_STANDBY_STEP;

typedef struct
{
	Uint16 uwExitInvStatusCnt;
	ENUM_STANDBY_STEP	eStandbyStep;
	ST_BUS_START		stBusStart;
	ST_FAN_INVT_OPEN_CHECK	stFanInvtOpenCheck;
	ST_RELAY_CHECK		stRelayCheck;

    Uint16 uwWaitBusRefBkp;
    Uint16 uwBusLowCnt;
    Uint16 uwBusRefLowCnt;
}ST_WAIT_STATUS;


//System Status definition for StateMachine
typedef enum
{
	cInitialStatus,cWaitStatus,cInverterStatus,cFaultStatus,cFlashStatus
}ENUM_INVERTER_STATUS;

extern ST_WAIT_STATUS stWaitStatus;
extern ENUM_INVERTER_STATUS eInverterStatus;


extern void StatusTaskInit(void);
extern void StatusTask(void);

#endif /* SYSTEM_PERIPH_INIT_H */
