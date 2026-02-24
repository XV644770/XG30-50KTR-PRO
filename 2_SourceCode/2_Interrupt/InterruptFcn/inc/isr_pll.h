#ifndef SOLAR_PLL_H              			// Prevents multiple inclusion of this file.
#define SOLAR_PLL_H

//--------------------------------------------------------------------
//     PLL limits
//--------------------------------------------------------------------
//PLL limits  50hz
#define FREQ_NULL		0
#define FREQ_50HZ		1
#define FREQ_60HZ		2



//Phaselock DEFINE 60.00Hz:
#define ANGLE_FREQ_60HZ_MAX		383708//356872		//	0.000052(19.2kHz)*2*PI*70Hz*2^24,wTs
#define ANGLE_FREQ_60HZ_MIN		301969				//	0.000052(19.2kHz)*2*PI*55Hz*2^24
#define ANGLE_FREQ_50HZ_MAX		301968				//	0.000052(19.2kHz)*2*PI*55Hz*2^24
#define ANGLE_FREQ_50HZ_MIN		219262//247065		//	0.000052(19.2kHz)*2*PI*40Hz*2^24

#define PHASE_SEQ_CHECK_STANDBY		0
#define PHASE_SEQ_CHECK_CHECKING	1
#define PHASE_SEQ_CHECK_FINISH		2

#define PHASE_SEQ_POSITIVE			0
#define PHASE_SEQ_NEGATIVE			1


typedef struct
{
	struct
	{
		int16	wRGridVoltRms;     		// R Phase Grid Voltage Rms Vaule
		int16	wSGridVoltRms;     		// S Phase Grid Voltage Rms Vaule
		int16	wTGridVoltRms;     		// T Phase Grid Voltage Rms Vaule
		int16	wRSTGridVoltRms;     	// RST Phase Grid Voltage Rms Vaule
		int16   uwRatedPhaseVolt; 		// Rated Grid Volt Value
		int16	wGridVoltErrRms;     	// T Phase Grid Voltage Rms Vaule
		int16	wGridVolt_q;			//The q value of the Grid Voltage
		Uint16	uwAngleOffset;
		Uint16	uwPhaseSeqFlag;		// 0: positive sequence 1:negative sequence
		int16	wIslandDisturbKp;
	}stIn;

	struct
	{
		int16	wSinwt;     			// Sin value of Grid Voltage angle
		int16	wSinwt_120;     		// Sin120 value of Grid Voltage angle
		int16	wSinwt_240;     		// Sin240 value of Grid Voltage angle

		int16	wCoswt;     			// Cos value of Grid Voltage angle
		int16	wCoswt_120;     		// Cos120 value of Grid Voltage angle
		int16	wCoswt_240;     		// Cos240 value of Grid Voltage angle

		int16	wSin5wt;     			// Sin value of Grid Voltage angle
		int16	wSin5wt_120;     		// Sin120 value of Grid Voltage angle
		int16	wSin5wt_240;     		// Sin240 value of Grid Voltage angle

		int16	wCos5wt;     			// Cos value of Grid Voltage angle
		int16	wCos5wt_120;     		// Cos120 value of Grid Voltage angle
		int16	wCos5wt_240;     		// Cos240 value of Grid Voltage angle

		int16	wActSinwt;     			// Sin value of Grid Voltage angle
		int16	wActSinwt_120;     		// Sin120 value of Grid Voltage angle
		int16	wActSinwt_240;     		// Sin240 value of Grid Voltage angle

		int16	wActCoswt;     			// Cos value of Grid Voltage angle
		int16	wActCoswt_120;     		// Cos120 value of Grid Voltage angle
		int16	wActCoswt_240;     		// Cos240 value of Grid Voltage angle

		int16	wActSin5wt;     			// Sin value of Grid Voltage angle
		int16	wActSin5wt_120;     		// Sin120 value of Grid Voltage angle
		int16	wActSin5wt_240;     		// Sin240 value of Grid Voltage angle

		int16	wActCos5wt;     			// Cos value of Grid Voltage angle
		int16	wActCos5wt_120;     		// Cos120 value of Grid Voltage angle
		int16	wActCos5wt_240;     		// Cos240 value of Grid Voltage angle

		Uint16	uwActGridPrdCnt;
		int32	dActThetaStep;

		int32	dThetaStep;
		int32	dTheta;						// the angle of Grid Voltage
		
		Uint16	uwGridPrdCnt;				//
		Uint16	uwGridPrdTim;	  			// the count number of Grid Voltage Period
		Uint16	uwGridPrdTimFlt;            // the count number of Grid Voltage Period
		Uint32	udGridCntFiltSum;           // the SUM of count number of Grid Voltage Period
		Uint16	uwGridZeroCrossFlg;			// the Zero Point of Grid Voltage
		Uint16	uwPllOk ;                   // the flag means Phase Lock is OK

		int32	dGridVoltqSum;
		int32	dGridVoltqSumFlt;
		Uint16	uwPLLOKCnt;                 // PLL OK count
		Uint16	uwPLLFailCnt;               // PLL Fail count

	}stOut;

	ST_PID stPID;
	int16	wIslandDisturb;
}ST_PLL_PARA;

typedef struct
{
    struct
    {
        int16	wRGridVolt;     	// R Phase Grid Voltage Real Vaule
        int16	wSGridVolt;     	// S Phase Grid Voltage Real Vaule
        int16	wTGridVolt;     	// T Phase Grid Voltage Real Vaule

        int16	wRGridVoltRms;     	// R Phase Grid Voltage Rms Vaule
        int16	wSGridVoltRms;     	// S Phase Grid Voltage Rms Vaule
        int16	wTGridVoltRms;     	// T Phase Grid Voltage Rms Vaule

        int16	wVoltAlpha;
        int16	wVoltBeta;

        int16   uwRatedPhaseVolt; 	// Rated Grid Volt Value
        Uint16  uwPhaseSequenceCheck;	// 0:don't need to check	1: need to check
    }stIn;

    struct
    {
        Uint16   uwPhaseSequenceFlag;  	// 0: positive sequence 1:negative sequence
        Uint16   uwPhaseSequenceSetp;
    }stOut;
}ST_PHSAE_SEQUENCE;

extern ST_PHSAE_SEQUENCE   	stPhaseSequence;
extern ST_PLL_PARA			stPllPara;

extern void PLLParaInit(void);
extern void GridVoltPhaseLock(void);
extern void GridVoltPhaseSequenceCheck(void);
extern void GridVoltIslandDisturb(void);

#endif
//===========================================================================
// End of file.
//===========================================================================
