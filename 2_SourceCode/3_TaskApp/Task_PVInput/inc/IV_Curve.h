/*
 * IV_Curve.h
 *
 *  Created on: 2022年12月17日
 *   Author: mgs
 */

#ifndef IV_CURVE_H
#define IV_CURVE_H


#define cCurveAnalyStateOff				0
#define cCurveAnalyStateOn			    1
#define cCurveAnalyStateDataSend		2

#define PV_CUVRE_POINT_NUM  100
#define  cDisturbCycle  100

typedef enum
{
	STOP = 0,
	START,
}ENUM_IV_TSATE;

typedef struct
{
	struct
	{
		float32 fPvVoltAvg;
		float32 fPvCurrAvg;
		Uint16 uwIntialflag;
		//float32 fMpptVolt;
		int16 fMpptVolt;
		Uint16 uwOverInput;
	}stIn;

	struct
	{
		float32 fPVVoltCurveRef;
		float32 fCurveOpenVolt;
		float32 fPvVoltCurveStep;
		Uint16 uwCurveAnalyStartCnt;
		Uint16 uwWaitcnt;
		Uint16 uwMpptcnt;

		Uint16 uwCurveAnalyRunCnt;
		Uint16 uwCurveAnalyRunEnd;
		ENUM_IV_TSATE eIVState;
		Uint16 uwCurveAnalyIndex;
		Uint16 uwPvVoltCurve[PV_CUVRE_POINT_NUM];
		Uint16 uwPvCurrCurve[PV_CUVRE_POINT_NUM];
		Uint16 uwPvVoltCurveDisplay[PV_CUVRE_POINT_NUM];
		Uint16 uwPvCurrCurveDisplay[PV_CUVRE_POINT_NUM];
	}stOut;

}ST_PV_CURVE_ANALY_CTRL;

typedef union
{
	Uint16  all;
	struct  ST_IV_BITS
	{
		Uint16	StartEnable:		1;		// bit0
		Uint16	DataFlag:	1;			// bit1
		Uint16	AnalyState:	1;			// bit2
		Uint16	EndFlag:	1;			// bit3 
		Uint16	rsv1:	1;					// bit4
		Uint16	rsv2:	1;					// bit5
		Uint16	rsv:		10;				// bit6-bit15
	}bit;
}UN_IV_BITS;


typedef struct
{
	UN_IV_BITS	unIVCurveBits;
	int16 cLvtMpptDelayCnt;
	int16 CurveAnalyDataID ;
	int16 CurveAnalyDataSuccess ;
	int16 CurveAnalyStartEnable;
	int16 CurveAnalyState ;
	int16 CurveMpptRecover;
	int16 CurveAnalyEndFlag ;
	int16 CurveMpptRecoverCnt;
	int16 CurveDataFlag ;
	int16  bOverInput[PV_MAX_NUM];
	int16  bFullInput[PV_MAX_NUM];
	
}IV_CURVE;

extern IV_CURVE stIVCurve;

extern void IVCurve(Uint16 uwPVIndexTmp);
//extern void sPV1msAvgCurrentCal(void);

extern void m_PVCurveAnalyCtrl(ST_PV_CURVE_ANALY_CTRL  *pstPvCurveAnalyCtrl);
extern void m_PVCurveParaReset(ST_PV_CURVE_ANALY_CTRL  *pstPvCurveAnalyCtrl);
extern void ClearPVCurveAnalyCtrlPara(Uint16 uwMpptID_s);
extern ST_PV_CURVE_ANALY_CTRL stPvCurveAnalyCtrl[PV_MAX_NUM];
extern void m_PVVoltRecover(void);


#endif /*IV_CURVE_H */

