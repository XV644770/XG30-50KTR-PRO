/*********************************************************
* solar_Adc.h
*
*	Product:			three phase inveter
*	Filename:			solar_Adc.h
*	Purpose:			system adc sample
*	Date				Version		Author			Note
*   20/12/11			V1.01		chensx			none
***********************************************************/
#ifndef SOLAR_ADC_H
#define SOLAR_ADC_H

#define DELAY_BUF_MAX	128

#if SAMPLE_DEBUG
extern int16 suwSin0Tmp,suwSin120Tmp,suwSin240Tmp;
#endif

// define AD samples AC components' DC offset
#define	AcDefaultOffset	    2048

#define	SAMPLE_OFFSET_MAX    64		// 64/32 = 2A

// AC Sample Struct
typedef struct
{
	int16 		wZeroVal;		// PWM Zero Point sampling value
	int16 		wPrdVal;		// PWM Period Point sampling value
	int16 		wArithVal;		// Sample Real Value
	int16 		wOffset;		// Sample Offset Value

	int32 		dOffsetSumIsr;		// Sample Offset Value
	int32 		dAddSum;		// Real value acc sum in task
	int32 		dAddSumIsr;		// Real value acc sum in interrupt

	int32 		dSquareAddSum;		// Real value square sum in task
	int32		dSquareAddSumIsr;	// Real value square sum in interrupt
}ST_VAL_AC;

// DC Sample Struct
typedef struct
{
	int16 		wZeroVal;		// PWM Zero Point sampling value
	int16 		wPrdVal;		// PWM Period Point sampling value
	int16 		wArithVal;		// Sample Real Value
	int16 		wOffset;		// Sample Offset Value

	int32 		dOffsetSumIsr;		// Sample Offset Value
	int32 		dAddSum;
	int32 		dAddSumIsr;
	int32 		wCalVal;		
}ST_VAL_DC;


typedef struct
{
	Uint16		uwSumCnt;

	Uint16		uwMux2Ch0SumCnt;
	Uint16		uwMux2Ch1SumCnt;

	Uint16		uwMux4Ch0SumCnt;
	Uint16		uwMux4Ch1SumCnt;
	Uint16		uwMux4Ch2SumCnt;
	Uint16		uwMux4Ch3SumCnt;

	Uint16		uwMux8Ch0SumCnt;
	Uint16		uwMux8Ch1SumCnt;
	Uint16		uwMux8Ch2SumCnt;
	Uint16		uwMux8Ch3SumCnt;
	Uint16		uwMux8Ch4SumCnt;
	Uint16		uwMux8Ch5SumCnt;
	Uint16		uwMux8Ch6SumCnt;
	Uint16		uwMux8Ch7SumCnt;

	Uint16		uwAdcOffsetCalcCnt;

	/* AC Sample Value*/
	// Grid Phase Voltage
	ST_VAL_AC	RGridVolt;
	ST_VAL_AC	SGridVolt;
	ST_VAL_AC	TGridVolt;
	// Grid Line Voltage
	ST_VAL_AC	RSGridVolt;
	ST_VAL_AC	STGridVolt;
	ST_VAL_AC	TRGridVolt;
	ST_VAL_AC	RSTGridVolt;
	// Inverter Voltage
	ST_VAL_AC	RInvVolt;
	ST_VAL_AC	SInvVolt;
	ST_VAL_AC	TInvVolt;
	
	ST_VAL_AC	RSInvVolt;
	ST_VAL_AC	STInvVolt;
	ST_VAL_AC	TRInvVolt;
	// Relay Voltage
	ST_VAL_AC	RRlyVolt;
	ST_VAL_AC	SRlyVolt;
	ST_VAL_AC	TRlyVolt;

	ST_VAL_AC	RSRlyVolt;
	ST_VAL_AC	STRlyVolt;
	ST_VAL_AC	TRRlyVolt;
	// Inverter Current
	ST_VAL_AC	RInvCurr;
	ST_VAL_AC	SInvCurr;
	ST_VAL_AC	TInvCurr;
	// Hard DCI - Current
	ST_VAL_AC	RHardDCI;
	ST_VAL_AC	SHardDCI;
	// Software DCI - Current
	ST_VAL_AC	RSoftDCI;
	ST_VAL_AC	SSoftDCI;
	
	// Hard GFCI - Current
	ST_VAL_AC	GFCI;
	// N-PE Voltage
	ST_VAL_AC	NPEVolt;
	ST_VAL_AC	ActivePower;
	ST_VAL_AC	ReactivePower;			// Reactive Power (1/4T delay method)

	// Voltage delay buffer for reactive power calculation
	Uint16		uwDelayLength;			// 1/4 cycle delay length = uwSumCnt / 4
	Uint16		uwDelayBufIdx;			// Circular buffer write index
	int16		wRVoltDelayBuf[DELAY_BUF_MAX];	// R-phase voltage delay buffer
	int16		wSVoltDelayBuf[DELAY_BUF_MAX];	// S-phase voltage delay buffer
	int16		wTVoltDelayBuf[DELAY_BUF_MAX];	// T-phase voltage delay buffer

	/* DC Sample Value*/
	ST_VAL_DC	BusVolt;
	ST_VAL_DC	PBusVolt;
	ST_VAL_DC	NBusVolt;
	ST_VAL_DC	ISOVolt;
	// Temperature
	ST_VAL_DC	InvTemp;
	ST_VAL_DC	BoostTemp;
	ST_VAL_DC	AMTemp;

	ST_VAL_DC	PVVolt[PV_MAX_NUM];
	//	PVVoltCtrl[PV_MAX_NUM];
	ST_VAL_DC	BTCurr[PV_MAX_NUM];
	ST_VAL_DC	BTStrCurr[STR_NUM_MAX];
	//ST_VAL_DC	BTCurrCtrl[PV_MAX_NUM];
}ST_ADC_POOL;
extern ST_ADC_POOL stAdcPool;

extern void CalcAdcOffset(void);
extern void SaveAdcBufZeroSample(void);
extern void CalcAdcRealValue(void);
extern void CalcAdcRealValueSum(void);
extern void CalcAdcRealValueSumTem(void);

#endif 
/* solar_Adc */
