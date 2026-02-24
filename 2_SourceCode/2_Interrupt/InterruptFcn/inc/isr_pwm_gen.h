#ifndef SOLAR_PWMGEN_H              			// Prevents multiple inclusion of this file.
#define SOLAR_PWMGEN_H 

#define SPWM_MODE	0
#define SVPWM_MODE	1
#define	DPWM_MODE	2

#define PWM_INV_DUTY_MAX		3124

#define	PWM_DISABLE_TIME		576		// 576/19200 = 30ms

typedef struct
{
	struct
	{

		int16 wUout_d;
		int16 wUout_q;
	}stIn;

	struct
	{

		int16	wRInvCtrl;           	// R Phase Invert control voltage
		int16	wSInvCtrl;           	// S Phase Invert control voltage
		int16	wTInvCtrl;           	// T Phase Invert control voltage

	}stOut;
}ST_PWM_PARA;

typedef struct
{
	struct
	{
		int16	wKPwm;				// Q8   1---2^8
		Uint16	uwModulateMode;
		Uint16	uwDutyCompensation;

		int16	wRInvOut;           	// R Phase invert control voltage
		int16	wSInvOut;           	// S Phase invert control voltage
		int16	wTInvOut;			// T Phase invert control voltage

		int16	wRDCI_PI;			// R Phase DCI
		int16	wSDCI_PI;			// R Phase DCI

		int16	wInvCurrZero;

	}stIn;

	struct
	{
		int16 wRPWM;              		// R Phase PWM Duty
		int16 wSPWM;              		// S Phase PWM Duty
		int16 wTPWM;              		// T Phase PWM Duty

		int16 wRPWMModulate;		// R Phase PWM Duty by algorithm calculation
		int16 wSPWMModulate;		// S Phase PWM Duty by algorithm calculation
		int16 wTPWMModulate;		// T Phase PWM Duty by algorithm calculation
		int16 wRPWMModulateOld;	// R Phase PWM Duty by algorithm calculation
		int16 wSPWMModulateOld;	// S Phase PWM Duty by algorithm calculation
		int16 wTPWMModulateOld;	// T Phase PWM Duty by algorithm calculation

		Uint16 uwRPWMDutyP;         //R Phase Positive Duty
		Uint16 uwRPWMDutyN;         //R Phase Negative Duty
		Uint16 uwSPWMDutyP;         //S Phase Positive Duty
		Uint16 uwSPWMDutyN;         //S Phase Negative Duty
		Uint16 uwTPWMDutyP;         //T Phase Positive Duty
		Uint16 uwTPWMDutyN;         //T Phase Negative Duty
	}stOut;
}ST_PWM_CALC;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	HardInvOCP				:1;		// Fault-01
    	Uint16	HardBoostOCP			:1;		// Fault-02
    	Uint16	SoftUnderVolt			:1;		// Fault-03
    	Uint16	SoftBoostOCP			:1;		// Fault-04
    	Uint16	SoftInvOCP				:1;		// Fault-05
    	Uint16	SoftVoltq				:1;		// Fault-06
    	Uint16	LowVoltThrough			:1;		// Fault-07
    	Uint16	LowVoltThroughOld		:1;		// Fault-08
    	Uint16	HighVoltThrough			:1;		// Fault-09
		Uint16	SoftBusOCP				:1;		// Fault-010
        Uint16  OverVoltOCP             :1;     // Fault-11
    	Uint16	Reserve8_16				:5;		// Fault 12-16
    }bit;
}UN_PWM_DISABLE_BIT;


typedef struct
{
	UN_PWM_DISABLE_BIT	unPwmDisableBit;
	UN_PWM_DISABLE_BIT	unPwmDisableBitOld;
	Uint16	uwPwmDisableDelayCnt;
	Uint16	uwPwmDisableTimersCnt;
	Uint16 uwTzEnableDelayCnt;

	Uint16	uwHardInvOCPTimes;			// Fault-01
	Uint16	uwHardBoostOCPTimes;		// Fault-02
	Uint16	uwSoftUnderVoltTimes;		// Fault-03
	Uint16	uwSoftBoostOCPTimes;		// Fault-04
	Uint16	uwSoftInvOCPTimes;			// Fault-05
	Uint16	uwSoftVoltqTimes;			// Fault-06
	Uint16	uwHardTripTimes;			// Fault-07
	Uint16	uwOVPHardTripTimes;			// Fault-08
	Uint16	uwPowerHardTripTimes;			// Fault-09
	Uint16	uwSoftBusOCPTimes;			// Fault-10
    Uint16  uwOverVoltOCPTimes;          // Fault-13
}ST_INV_PWM;

extern ST_PWM_PARA	stPwmPara;
extern ST_PWM_CALC	stPwmCalc;
extern ST_INV_PWM	stInvPwm;

extern void InvPwmOnOff(void);
extern void InvOutVoltCalc(void);
extern void PwmGenParaInit();

#endif
//===========================================================================
// End of file.
//===========================================================================
