/*
 * safty_grid_freq_check.h
 *
 *  Created on: 2021年1月9日
 *      Author: Administrator
 */

#ifndef SAFTY_GRID_FREQ_CHECK_H
#define SAFTY_GRID_FREQ_CHECK_H

#define FREQ_60HZ_MAX	7000
#define FREQ_60HZ_MIN	5500
#define FREQ_50HZ_MAX	5500
#define FREQ_50HZ_MIN	4000

#define	SET_AC_START_FAULT_TIME	100		// 100*20ms = 2s

typedef struct
{
	int16	wGridFreq;              		// Grid Voltage Freqency
	Uint16	uwFreqRangChkOver;     	// Freqency range
	Uint16	uwFreqFlag;            		//function output parameter :0:50Hz  1:60Hz
}ST_FREQ_RANG;
extern ST_FREQ_RANG stFreqRang;

typedef struct
{
	Uint32 udRLineZeroTimeNew;
	Uint16 uwRLinePeriodNew;
	Uint16 uwRLinePeriodTemp;
	Uint32 udRLineZeroTimeOld;
	Uint16 uwRLinePeriodOld;


	Uint32 udSLineZeroTimeNew;
	Uint16 uwSLinePeriodNew;
	Uint16 uwSLinePeriodTemp;
	Uint32 udSLineZeroTimeOld;
	Uint16 uwSLinePeriodOld;

	Uint32 udTLineZeroTimeNew;
	Uint16 uwTLinePeriodNew;
	Uint16 uwTLinePeriodTemp;
	Uint32 udTLineZeroTimeOld;
	Uint16 uwTLinePeriodOld;

	Uint16 uwRGridFreqNew;
	Uint16 uwSGridFreqNew;
	Uint16 uwTGridFreqNew;

	Uint16 uwRGridFreqOld;
	Uint16 uwSGridFreqOld;
	Uint16 uwTGridFreqOld;

	Uint16 uwRLossCnt;
	Uint16 uwSLossCnt;
	Uint16 uwTLossCnt;
	
	int16	wGridFreqMax;     
	int16	wGridFreqMin; 
}ST_GRID_FREQ;
extern ST_GRID_FREQ stGridFreq;

typedef struct
{
	Uint16 uwACStartOverVolt;
	Uint16 uwACStartUnderVolt;
	Uint16 uwACStartOverFreq;
	Uint16 uwACStartUnderFreq;

	Uint16 uwACUnderVolt1;
	Uint16 uwACOverVolt1;
	Uint16 uwACUnderVolt1Time;
	Uint16 uwACOverVolt1Time;

	Uint16 uwACUnderVolt2;
	Uint16 uwACUnderVolt2Time;
	Uint16 uwACOverVolt2;
	Uint16 uwACOverVolt2Time;

    Uint16 uwACUnderVolt3;
    Uint16 uwACUnderVolt3Time;
	Uint16 uwAC10MinOverVolt;
	/*****************F107 TX Frame3*******************/
	Uint16 uwACUnderFreq1;
	Uint16 uwACUnderFreq2;
	Uint16 uwACUnderFreq1Time;
	Uint16 uwACUnderFreq2Time;

	Uint16 uwACOverFreq1;
	Uint16 uwACOverFreq2;
	Uint16 uwACOverFreq1Time;
	Uint16 uwACOverFreq2Time;
}ST_GRID_PROTECT;   //The structure for Freqency Protect parameters

extern ST_GRID_PROTECT	stGridProtect;

extern void GridFreqRangeCheck(void);
extern void GridFreqProtection(void);
extern void GridFreqParaInit(void);
extern void RGridFreqCal(Uint16 uwRGridPeriodNew);
extern void SGridFreqCal(Uint16 uwSGridPeriodNew);
extern void TGridFreqCal(Uint16 uwTGridPeriodNew);
extern void ZeroLossChk(void)  ;
#endif /* SAFTY_GRID_FREQ_CHECK_H_ */
