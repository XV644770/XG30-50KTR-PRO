/*
* calc_mppt_condition.h
*
*	Product:			three phase inveter
*	Filename:			TurnOnCheck.h
*	Purpose:			Check the starting condition of inverter
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef CALC_MPPT_CONDITION_H
#define CALC_MPPT_CONDITION_H

#define VOLT_20V			200			
#define VOLT_30V			300			
#define VOLT_50V			500			
#define VOLT_80V			800			
#define VOLT_100V			1000			
#define VOLT_120V			1200			
#define VOLT_750V			7500
#define VOLT_800V			8000
#define VOLT_850V			8500

#define BUS_VOLT_ADD		500		// 50V

#define PV_TURN_OFF_VOLT    2000		// 200V
#define PV_VOLT_MAX		10000	// 10000V

#define PARALLEL_BOOST_ON_VOLT				9800//9600
#define INDEPENDENT_BOOST_ON_VOLT			8700//6700
#define	BOOST_OFF_BUS_VOLT_MAX				9600

#define BUS_REF_MIN			5500	// 220V*1.414*1.732 = 550V
//#define BUS_REF_MIN			7700//5500	// 220V*1.414*1.732 = 550V
#define BUS_WAIT_MIN		PV_TURN_OFF_VOLT
#define BUS_REF_MAX		10000//9500	// 950V

#define DC_SOURCE_BOOST_ON_BUS_REF	8500
#define PV_20V				200		// 20V



extern void PVOnOffCheck(void);
extern void CalcPVVoltMax(void);
extern void BusRefCalc(ENUM_MPPT_MODE  eMpptMode_s);

#endif /* SYSTEM_PERIPH_INIT_H */
