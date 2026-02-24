/*
* run_mppt.h
*
*	Product:			three phase inveter
*	Filename:			system_periph_init.h
*	Purpose:			Stm32 system init
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef RUN_MPPT_H
#define RUN_MPPT_H


#define	MPPT_VOLT_MAX              10000			// 1000V
#define	MPPT_VOLT_MIN			2000			// 200V

#define 	MPPT_POWER_LOW_TIME		60//30000	//60=2s*60=120s  10min		// 20ms*30000 = 600s = 10min
#define 	ANTIFLOW_TIME			25	//25*20ms

typedef struct
{
	Uint16	uwPVONNum;
	Uint16	uwPVVoltRefMaxID;
	Uint16	uwPVVoltMaxID;
	Uint16	uwBusRef;							// Bus reference voltage
	int16	wBusStandardVolt;					// Bus ref limit by grid 
	Uint16	uwBTOnOff[PV_MAX_NUM];
	Uint16	uwPVConnect[PV_MAX_NUM];
} ST_BT_CTRL_TASK;

typedef struct
{
	int16	wPVVolt;
	int16	wPVOpenVolt;
	int32	dMpptPower;
	int32	dMpptPowerSum;
	int32	dMpptPowerAvg;
} ST_MPPT_PARA;

typedef enum
{
	Increase, 
	PredictIncrease, 
	PredictDecrease, 
	Decrease
} ENUM_DISTURB_DIR;

typedef enum
{
	Initing,
	Runing
} ENUM_TRACK_STATUS;


typedef union
{
	Uint16  all;
	struct  ST_MPPT_BITS
	{
		Uint16	Enable:		1;		// bit0
		Uint16	LowPower:	1;		// bit1
		Uint16	ZeroPower:	1;		// bit2
		Uint16	OverPower:	2;		// bit3-4
		Uint16	OverBTCurr:	1;		// bit5
		Uint16	LimitBTCurr:1;	    // bit6
		Uint16	OverBusRef:	1;	    // bit7
		Uint16	AntiOverPower:	2;	// bit8-9
		Uint16	rsv:		6;		// bit10-bit15
	}bit;
}UN_MPPT_BITS;


typedef struct
{
	UN_MPPT_BITS	unMpptBits;
	ENUM_TRACK_STATUS eTrackStatus;
	int16	wPVVoltRef;

	int16	wMpptVoltRefMax;
	int16	wMpptVoltRefMin;

	int16	wMpptVoltRef;
	int16	wMpptVoltRefOld;

	int32	dMpptPower;			// Current MPPT Power 
	int32	dMpptPowerOld;		  	// Last MPPT Power

	int32	dDecreasePowerError;
	int32	dDecreasePowerErrorOld;
	int32	dIncreasePowerError;
	int32	dIncreasePowerErrorOld;

	ENUM_DISTURB_DIR eTrackDir;
	ENUM_DISTURB_DIR eTrackDirOld;

	Uint16	uwMpptDisturbCnt;

	Uint16	uwMpptPowerCalcCnt;
	Uint16	uwMpptTrackInitCnt;
	Uint16	uwFastMpptCnt;
    Uint16  uwFastMpptOffCnt;
	Uint16	uwLowPowerCnt;
	Uint16	uwZeroPowerCnt;

	Uint16	uwDeadCnt;
	Uint16	uwMpptDeadStus;

	Uint16	uwMpptAddCnt;
	Uint16	uwMpptSubCnt;
	Uint16 	uwMpptWattIncreaCnt;
	Uint16 	uwMpptWattReducCnt;
	Uint16 	uwMpptLeftSideFlag;
	int16 	wMpptStepWattPer;

	int32	dStepDeltPvWatt;
	int32	dAntiStep;
	Uint16 	uwAntiFlowCnt;
	Uint16	uwFastMpptFlag;
	
}ST_MPPT_DISTURB;

extern ST_MPPT_PARA stMpptPara[PV_MAX_NUM];
extern ST_MPPT_DISTURB stMpptDisturb[PV_MAX_NUM];
extern ST_BT_CTRL_TASK stMpptTskCtrl;

extern void MpptInit(void);
extern void MPPT(Uint16 uwPVIndex);

#endif /* SYSTEM_PERIPH_INIT_H */
