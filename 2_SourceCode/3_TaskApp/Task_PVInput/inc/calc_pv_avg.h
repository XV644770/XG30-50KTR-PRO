/*
* calc_pv_sample.h
*
*	Product:			three phase inveter
*	Filename:			TurnOnCheck.h
*	Purpose:			Check the starting condition of inverter
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef CALC_PV_SAMPLE_H
#define CALC_PV_SAMPLE_H

/* Tempture Limit*/
#define	INV_HIGHEST_TEMP			(1500)
#define	INV_LOWEST_TEMP				(-500)

#define	BOOST_HIGHEST_TEMP			(1200)
#define	BOOST_LOWEST_TEMP			(-400)

#define	AM_HIGHEST_TEMP				(1250)
#define	AM_LOWEST_TEMP				(-400)

typedef struct
{
	int16			wPBusVolt;
	int16			wNBusVolt;
	int16			wBusVolt;
	int16			wBusVolt1ms;
	int16			w1DivBus;
	int16			w1DivBusFlt;
	int16			wISOVolt;
	Uint16			uwISOResistance;
	int16			wISORelay1Volt;
	int16			wISORelay2Volt;

	int16			wAmTemp;
	int16			wBoostTemp;
	int16			wInvTemp;

	int16			wPVVolt[PV_MAX_NUM];
	int16			wPVVoltAdj[PV_MAX_NUM];
	int16			wBTCurr[PV_MAX_NUM];
	int16			wMPPTCurr[PV_MAX_NUM];



	int16			wBTStrCurr[STR_NUM_MAX];

	int16			wPVVoltAvg[PV_MAX_NUM];

	int16			wPVVoltMax;
	int16			wBTCurrMax;

	int32			dPVPowerTotal;

}ST_DC_SAMPLE;

extern ST_DC_SAMPLE stDCSample;

extern void CalcPVVoltCurrAvg(void);
//extern void CalcBTCurrSample(void);
extern void ReadMachineModel(void);
#endif /* SYSTEM_PERIPH_INIT_H */
