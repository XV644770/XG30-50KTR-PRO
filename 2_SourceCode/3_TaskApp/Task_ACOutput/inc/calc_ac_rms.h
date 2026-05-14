/*
* calc_ac_sample.h
*
*	Product:			three phase inveter
*	Filename:			TurnOnCheck.h
*	Purpose:			Check the starting condition of inverter
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef CALC_AC_SAMPLE_H
#define CALC_AC_SAMPLE_H

typedef struct
{
	int16	wRN;
	int16	wSN;
	int16	wTN;
}ST_GRID_PHASE;

typedef struct
{
	int16	wRS;
	int16	wST;
	int16	wTR;
}ST_GRID_LINE;

typedef struct
{
	ST_GRID_PHASE	PhaseVoltRms;		// Grid Phase Voltage
	ST_GRID_LINE	LineVoltRms;		// Grid Line Voltage
	ST_GRID_PHASE	InvVoltRms;			// Inverter Voltage
	ST_GRID_PHASE	RlyVoltRms;			// Relay phase Voltage
	ST_GRID_LINE	RlyLineVoltRms;		// Relay line Voltage
	ST_GRID_PHASE	PhaseCurrRms;		// Inverter Current
	ST_GRID_PHASE	PhaseInvVoltAvg;	// Inverter Voltage
	ST_GRID_LINE	LineInvVoltAvg;		// Grid Line Voltage
	int16			RSTGridVoltRms;		// Three Grid Voltage Rms

	ST_GRID_PHASE	HardDCI;			// Hard DCI - Current
	ST_GRID_PHASE	SoftDCI;			// Software DCI - Current

    int16			wGfciRms;			// Hard GFCI - Current
    int16			wGfciAvg;			// Hard GFCI - Current

	int16			wVoltNPERms;		// N-PE Voltage

    int16 			wPhaseVoltRmsMax;
    int16 			wPhaseVoltRmsMin;

    int16 			wLineVoltRmsMax;
    int16 			wLineVoltRmsMin;
    int16 			wLineVoltRmsMid;

    int16 			wPhaseCurrRmsMax;
    int16 			wLineVoltRmsMaxAvg;
    int16 			wLineVoltRmsMinAvg;

    int16 			wGridFreqReal;
    int16			wGridFreqAvg;

    int16			wPowerFactor;
    Uint16			uwPowerEnergySum;
    int32			dActivePower;
    int32			dReactivePower;
    int32			dApparentPower;

    int32			dActivePowerAvg;
    int32			dApparentPowerAvg;
    int32			dReactivePowerAvg;

    int32			d1DivGridTimFlt;
    int16			wEfficiency;
    // Inv_Volt_Delay
    Uint16			uwDelayLength;
}ST_AC_SAMPLE;

extern ST_AC_SAMPLE stACSample;

extern void CalcOutputPower(void);
extern void CalcACVoltCurrRms(void);
extern void EnergizeDegreeCalc(void);
extern void ReactivePowerCtrl(void);

#endif /* SYSTEM_PERIPH_INIT_H */
