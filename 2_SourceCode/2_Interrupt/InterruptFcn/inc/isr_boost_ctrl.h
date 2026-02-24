#ifndef SOLAR_BOOSTCTRL_H              			// Prevents multiple inclusion of this file.
#define SOLAR_BOOSTCTRL_H

#define	PID_DISABLE			0	//Disable PID Control
#define POSITIVE_PID		1	//Enable Positive PID Control
#define NEGATIVE_PID		2	//Enable Negative PID Control

typedef struct
{
    struct
    {
        int32	dError;     // PID error value
        int32	dKp; 	    // PID Controller Kp.
        int32	dKi; 	    // PID Controller Ki.

        int32	dPIMax;     // PID Out MAX Value.
        int32	dPIMin;     // PID Out MIN Value.
    }stIn;
    struct
    {
        int32	dErrorOld;   // Save last PID error value
        int32	dPIDOut;	 // PID final out value
    }stOut; //Output parameters
}ST_PID;

typedef struct
{
	struct
	{
		Uint16	uwCtrlLoopEnable;
		int32	dRef;					// Reference value
		int32	dReal;					// Real Value

		int32	dKp_H; 	    // PID Controller Kp.
        	int32	dKi_H; 	    // PID Controller Ki.
        	int32	dKp_L; 	    // PID Controller Kp.
        	int32	dKi_L; 	    // PID Controller Ki.
        	int32 	dCurrLimitMax;
	}stIn;
	struct
	{
		int32 dCtrlLoopOutput;
		int32 dCtrlLoopRemainder;			// remainder
	}stOut;
	ST_PID stPID;
}ST_CTRL_LOOP;

typedef struct
{
	Uint16	uwBtPwmEnable;
	Uint16	uwBtPwmDuty;
}ST_BTCtrl;

extern ST_BTCtrl stBTCtrl[PV_MAX_NUM];
extern ST_CTRL_LOOP stBusVoltSoftStartCtrl[PV_MAX_NUM];
extern ST_CTRL_LOOP stPVVoltCtrl[PV_MAX_NUM];
extern ST_CTRL_LOOP stBTCurrCtrl[PV_MAX_NUM];
extern ST_CTRL_LOOP stActivePowerCtrl;

extern void BoostCtrlParaInit(void);
extern void IncrementalPID(ST_CTRL_LOOP *pstCtrlLoop);
extern void WaitBusVoltLoopCtrl(Uint16 uwPVId);
extern void InvPVVoltLoopCtrl(Uint16 uwPVId);
extern void InvBTCurrLoopCtrl(Uint16 uwPVId);
extern void InvActivePowerLoopCtrl(void);

#if PWM_OPEN_LOOP_ENABLE
extern void BoostPwmOpenLoop(void);
#else
extern void BoostPwmOnff(void);
#endif

#endif
//===========================================================================
// End of file.
//===========================================================================
