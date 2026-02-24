/*
 * safty_dc_sample.h
 *
 *  Created on: 2021年1月7日
 *      Author: Administrator
 */


#ifndef SAFTY_DC_SAMPLE_H
#define SAFTY_DC_SAMPLE_H

#define PV_VOLT_SET_FAULT		10000	// 1000V

#define 	SET_PV_FAULT_TIME		150//100	// 20ms*150 = 3000ms = 3s

#define		PV_SHORT_VOLT	300			// 30V
#define		PV_SHORT_CURR	500			// 5A

#define		PV_REVERSE_VOLT	300			// 30V
#define		PV_REVERSE_CURR	-200		// -2A

#define     wStartPvVolt 2000         // 200V

#define		PV_BUS_DELTA	500		// 50V
#define     PV_BUS_Volt         1000     // 100V


extern void CheckPVVoltRange(void);
extern void PVPanelReserveCheck(void);
extern void PVPanelShortCheck(void);
extern void BusVoltSampleCheck(void);
extern void BTCurrSampleCheck(void);

#endif
