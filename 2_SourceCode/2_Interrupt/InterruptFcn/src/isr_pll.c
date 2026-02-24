#include "dsp_user_include.h"
#include "IQmathLib.h"
#define cPllOkVolt		500
ST_PLL_PARA	stPllPara;
ST_PHSAE_SEQUENCE   	stPhaseSequence;

/*************************************************************************************************
*
*	FunctionName:	PLLParaInit
*	Input:	none
*	Output:	none
*	Description:	Phase sequence check function, used to pass the entrance parameters
*	Note:	none.
************************************************************************************************/
void PLLParaInit(void)
{
	memset((void*)&stPllPara, 0, sizeof(stPllPara));
	memset((void*)&stPhaseSequence, 0, sizeof(stPhaseSequence));
	stPllPara.stPID.stIn.dKp = 8950;	//  100*10^-8 * 2^24*16384/32
	stPllPara.stPID.stIn.dKi = 86;		// 1*10^-8 * 2^24*16384/32
	stPllPara.stOut.uwPllOk = 0;

	stPllPara.stIn.wIslandDisturbKp = 512;

	stPhaseSequence.stIn.uwRatedPhaseVolt = 4000;		// default 400V
	stPhaseSequence.stIn.uwPhaseSequenceCheck = 0;
}

/****************************************************************************************
*Functions For PLL	
*
*****************************************************************************************/
#pragma CODE_SECTION(PhaseLockLoop,"ramfuncs");
void PhaseLockLoop(ST_PLL_PARA *pstPllPara)
{
	int64 ldGridThetaCalcVarTmp;
	int16 wPhaseLockVoltMinTmp = 0;
	

	wPhaseLockVoltMinTmp = VAC30V;//(pstPllPara->stIn.uwRatedPhaseVolt>>1);		// RatedVolt * 0.5

	pstPllPara->stOut.uwGridPrdCnt++;
	if(pstPllPara->stOut.uwGridPrdCnt >= pstPllPara->stOut.uwGridPrdTim)
	{
		pstPllPara->stOut.uwGridPrdCnt = 0;
	}


/*********************************Grid Volt PLL Controller*******************************************/	
	pstPllPara->stPID.stOut.dErrorOld = pstPllPara->stPID.stIn.dError;
	pstPllPara->stPID.stIn.dError = pstPllPara->stIn.wGridVolt_q;		// 2^5

	// R_Volt = Vn*sin(wt) = Vn*sin(2*pi*F*t) = Vn*sin(2*pi*F*step/19200)
	// 2*pi*F/19200 (F:45Hz~65Hz) = (0.014726232~0.021271224)
	// dTheta = 2*pi*F/19200 * 2^24 = 247065(45Hz)~356872(65Hz)
	ldGridThetaCalcVarTmp = pstPllPara->stOut.dTheta;
	ldGridThetaCalcVarTmp = ((ldGridThetaCalcVarTmp<<14)										// (2^24 * 2^14) > 2^30
				 + (pstPllPara->stPID.stIn.dKp*(pstPllPara->stPID.stIn.dError-pstPllPara->stPID.stOut.dErrorOld))
				 + (pstPllPara->stPID.stIn.dKi * pstPllPara->stPID.stIn.dError)
				 + pstPllPara->stPID.stOut.dPIDOut);
	pstPllPara->stOut.dTheta = (int32)(ldGridThetaCalcVarTmp>>14);
	pstPllPara->stPID.stOut.dPIDOut = (int32)(ldGridThetaCalcVarTmp&0x3FFF);
#if PWM_OPEN_LOOP_ENABLE
	pstPllPara->stOut.dTheta = 274517;		// F:50Hz   dTheta = 2*pi*F/19200 * 2^24 = 274517
	stPhaseSequence.stOut.uwPhaseSequenceFlag = PHASE_SEQ_POSITIVE;
#endif
	UPDNLMT16(pstPllPara->stOut.dTheta, pstPllPara->stPID.stIn.dPIMax, pstPllPara->stPID.stIn.dPIMin);

	// dTheta = 2*pi*F/19200 * 2^24  => 19200/F = 2^24*2*pi/dTheta
	// 一个市电周期内的点数 = 19200/F = 2^24*2*pi/dTheta = 105414357;
	pstPllPara->stOut.uwGridPrdTim = (int16)((int32)105414357/pstPllPara->stOut.dTheta);	// 630ns

	pstPllPara->stOut.uwGridPrdTimFlt = ((int32)((int32)pstPllPara->stOut.uwGridPrdTimFlt*610+(int32)pstPllPara->stOut.uwGridPrdTim*414)>>10);		// 0.8*BusFlt + 0.2*Bus
/*********************************Grid Volt Angle Calculate*******************************************/
	//wt : = (dTheta * Cnt) * 2^24
	pstPllPara->stOut.dThetaStep = (pstPllPara->stOut.dTheta * pstPllPara->stOut.uwGridPrdCnt);

	pstPllPara->stOut.wSinwt = _IQ24sin(pstPllPara->stOut.dThetaStep)>>10;				// 630ns

#if 1
	pstPllPara->stOut.wCoswt = _IQ24cos(pstPllPara->stOut.dThetaStep)>>10;				// 630ns

#else
	pstPllPara->stOut.wCoswt =  _IQ24sin(pstPllPara->stOut.dThetaStep+26353619)>>10;
#endif
	if(PHASE_SEQ_NEGATIVE == pstPllPara->stIn.uwPhaseSeqFlag)
	{
		// 2^24 * (2*pi/3) = 35138119;
		// 2^24 * (1*pi/4)  = 26353619
#if 0
		pstPllPara->stOut.wSinwt_240 = _IQ24sin(pstPllPara->stOut.dThetaStep+35138119)>>10;
		pstPllPara->stOut.wCoswt_240 = _IQ24cos(pstPllPara->stOut.dThetaStep+35138119)>>10;
		pstPllPara->stOut.wSinwt_120 = _IQ24sin(pstPllPara->stOut.dThetaStep-35138119)>>10;
		pstPllPara->stOut.wCoswt_120 = _IQ24cos(pstPllPara->stOut.dThetaStep-35138119)>>10;
#else
		// sin(A+B)  = sinA*cosB + cosA*sinB
		// cos(A+B) = cosA*cosB - sinA*sinB
		// sin(A-B)  = sinA*cosB - cosA*sinB
		// cos(A-B) = cosA*cosB + sinA*sinB

		// Sinwt_240 = Sin(wt+2*pi/3) = Sinwt*Cos(2*pi/3) + Coswt*Sin(2*pi/3) = -0.5*Sinwt + (3^0.5/2)*Coswt
		pstPllPara->stOut.wSinwt_240 = ((int32)-8192*pstPllPara->stOut.wSinwt+(int32)14189*pstPllPara->stOut.wCoswt)>>14;
		// Coswt_240 = Cos(wt+2*pi/3) = Coswt*Cos(2*pi/3) - Sinwt*Sin(2*pi/3) = -0.5*Coswt - (3^0.5/2)*Sinwt
		pstPllPara->stOut.wCoswt_240 = ((int32)-8192*pstPllPara->stOut.wCoswt-(int32)14189*pstPllPara->stOut.wSinwt)>>14;
		// Sinwt_120 = Sin(wt-2*pi/3) = Sinwt*Cos(2*pi/3) - Coswt*Sin(2*pi/3) = -0.5*Sinwt - (3^0.5/2)*Coswt
		pstPllPara->stOut.wSinwt_120 = ((int32)-8192*pstPllPara->stOut.wSinwt-(int32)14189*pstPllPara->stOut.wCoswt)>>14;
		// Coswt_120 = Cos(wt-2*pi/3) = Coswt*Cos(2*pi/3) + Sinwt*Sin(2*pi/3) = -0.5*Coswt + (3^0.5/2)*Sinwt
		pstPllPara->stOut.wCoswt_120 = ((int32)-8192*pstPllPara->stOut.wCoswt+(int32)14189*pstPllPara->stOut.wSinwt)>>14;
#endif
	}
	else
	{
		//  2^24 * (2*pi/3 )= 35138119;
		// 2^24 * (3*pi/4)  = 26353619
#if 0
		pstPllPara->stOut.wSinwt_240 = _IQ24sin(pstPllPara->stOut.dThetaStep-35138119)>>10;		// 2^14
		pstPllPara->stOut.wCoswt_240 = _IQ24cos(pstPllPara->stOut.dThetaStep-35138119)>>10;
		pstPllPara->stOut.wSinwt_120 = _IQ24sin(pstPllPara->stOut.dThetaStep+35138119)>>10;
		pstPllPara->stOut.wCoswt_120 = _IQ24cos(pstPllPara->stOut.dThetaStep+35138119)>>10;
#else
		// sin(A+B)  = sinA*cosB + cosA*sinB
		// cos(A+B) = cosA*cosB - sinA*sinB
		// sin(A-B)  = sinA*cosB - cosA*sinB
		// cos(A-B) = cosA*cosB + sinA*sinB

		// Sinwt_240 = Sin(wt-2*pi/3) = Sinwt*Cos(2*pi/3) - Coswt*Sin(2*pi/3) = -0.5*Sinwt - (3^0.5/2)*Coswt
		pstPllPara->stOut.wSinwt_240 = ((int32)-8192*pstPllPara->stOut.wSinwt-(int32)14189*pstPllPara->stOut.wCoswt)>>14;
		// Coswt_240 = Cos(wt-2*pi/3) = Coswt*Cos(2*pi/3) + Sinwt*Sin(2*pi/3) = -0.5*Coswt + (3^0.5/2)*Sinwt
		pstPllPara->stOut.wCoswt_240 = ((int32)-8192*pstPllPara->stOut.wCoswt+(int32)14189*pstPllPara->stOut.wSinwt)>>14;
		// Sinwt_120 = Sin(wt+2*pi/3) = Sinwt*Cos(2*pi/3) + Coswt*Sin(2*pi/3) = -0.5*Sinwt + (3^0.5/2)*Coswt
		pstPllPara->stOut.wSinwt_120 = ((int32)-8192*pstPllPara->stOut.wSinwt+(int32)14189*pstPllPara->stOut.wCoswt)>>14;
		// Coswt_120 = Cos(wt+2*pi/3) = Coswt*Cos(2*pi/3) - Sinwt*Sin(2*pi/3) = -0.5*Coswt - (3^0.5/2)*Sinwt
		pstPllPara->stOut.wCoswt_120 = ((int32)-8192*pstPllPara->stOut.wCoswt-(int32)14189*pstPllPara->stOut.wSinwt)>>14;
#endif
	}
/*********************************Grid Volt PWM Angle Calculate*******************************************/
//wt : = (dTheta * Cnt) * 2^24

	pstPllPara->stOut.uwActGridPrdCnt = (pstPllPara->stOut.uwGridPrdCnt + pstPllPara->stIn.uwAngleOffset);
	if(pstPllPara->stOut.uwActGridPrdCnt >= pstPllPara->stOut.uwGridPrdTim)
	{
		pstPllPara->stOut.uwActGridPrdCnt = (pstPllPara->stOut.uwActGridPrdCnt-pstPllPara->stOut.uwGridPrdTim);
	}
	pstPllPara->stOut.dActThetaStep = (pstPllPara->stOut.dTheta * pstPllPara->stOut.uwActGridPrdCnt);

	pstPllPara->stOut.wActSinwt = _IQ24sin(pstPllPara->stOut.dActThetaStep)>>10;
#if 1
	pstPllPara->stOut.wActCoswt = _IQ24cos(pstPllPara->stOut.dActThetaStep)>>10;

#else
	pstPllPara->stOut.wActCoswt =  _IQ24sin(pstPllPara->stOut.dThetaStep+26353619)>>10;
#endif
	if(PHASE_SEQ_NEGATIVE == pstPllPara->stIn.uwPhaseSeqFlag)
	{
		// 2^24 * (2*pi/3) = 35138119;
		// 2^24 * (1*pi/4)  = 26353619
#if 0
		pstPllPara->stOut.wSinwt_240 = _IQ24sin(pstPllPara->stOut.dThetaStep+35138119)>>10;
		pstPllPara->stOut.wCoswt_240 = _IQ24cos(pstPllPara->stOut.dThetaStep+35138119)>>10;
		pstPllPara->stOut.wSinwt_120 = _IQ24sin(pstPllPara->stOut.dThetaStep-35138119)>>10;
		pstPllPara->stOut.wCoswt_120 = _IQ24cos(pstPllPara->stOut.dThetaStep-35138119)>>10;
#else
		// sin(A+B)  = sinA*cosB + cosA*sinB
		// cos(A+B) = cosA*cosB - sinA*sinB
		// sin(A-B)  = sinA*cosB - cosA*sinB
		// cos(A-B) = cosA*cosB + sinA*sinB

		// Sinwt_240 = Sin(wt+2*pi/3) = Sinwt*Cos(2*pi/3) + Coswt*Sin(2*pi/3) = -0.5*Sinwt + (3^0.5/2)*Coswt
		pstPllPara->stOut.wActSinwt_240 = ((int32)-8192*pstPllPara->stOut.wActSinwt+(int32)14189*pstPllPara->stOut.wActCoswt)>>14;
		// Coswt_240 = Cos(wt+2*pi/3) = Coswt*Cos(2*pi/3) - Sinwt*Sin(2*pi/3) = -0.5*Coswt - (3^0.5/2)*Sinwt
		pstPllPara->stOut.wActCoswt_240 = ((int32)-8192*pstPllPara->stOut.wActCoswt-(int32)14189*pstPllPara->stOut.wActSinwt)>>14;
		// Sinwt_120 = Sin(wt-2*pi/3) = Sinwt*Cos(2*pi/3) - Coswt*Sin(2*pi/3) = -0.5*Sinwt - (3^0.5/2)*Coswt
		pstPllPara->stOut.wActSinwt_120 = ((int32)-8192*pstPllPara->stOut.wActSinwt-(int32)14189*pstPllPara->stOut.wActCoswt)>>14;
		// Coswt_120 = Cos(wt-2*pi/3) = Coswt*Cos(2*pi/3) + Sinwt*Sin(2*pi/3) = -0.5*Coswt + (3^0.5/2)*Sinwt
		pstPllPara->stOut.wActCoswt_120 = ((int32)-8192*pstPllPara->stOut.wActCoswt+(int32)14189*pstPllPara->stOut.wActSinwt)>>14;
#endif
	}
	else
	{
		//  2^24 * (2*pi/3 )= 35138119;
		// 2^24 * (3*pi/4)  = 26353619
#if 0
		pstPllPara->stOut.wSinwt_240 = _IQ24sin(pstPllPara->stOut.dThetaStep-35138119)>>10;		// 2^14
		pstPllPara->stOut.wCoswt_240 = _IQ24cos(pstPllPara->stOut.dThetaStep-35138119)>>10;
		pstPllPara->stOut.wSinwt_120 = _IQ24sin(pstPllPara->stOut.dThetaStep+35138119)>>10;
		pstPllPara->stOut.wCoswt_120 = _IQ24cos(pstPllPara->stOut.dThetaStep+35138119)>>10;
#else
		// sin(A+B)  = sinA*cosB + cosA*sinB
		// cos(A+B) = cosA*cosB - sinA*sinB
		// sin(A-B)  = sinA*cosB - cosA*sinB
		// cos(A-B) = cosA*cosB + sinA*sinB

		// Sinwt_240 = Sin(wt-2*pi/3) = Sinwt*Cos(2*pi/3) - Coswt*Sin(2*pi/3) = -0.5*Sinwt - (3^0.5/2)*Coswt
		pstPllPara->stOut.wActSinwt_240 = ((int32)-8192*pstPllPara->stOut.wActSinwt-(int32)14189*pstPllPara->stOut.wActCoswt)>>14;
		// Coswt_240 = Cos(wt-2*pi/3) = Coswt*Cos(2*pi/3) + Sinwt*Sin(2*pi/3) = -0.5*Coswt + (3^0.5/2)*Sinwt
		pstPllPara->stOut.wActCoswt_240 = ((int32)-8192*pstPllPara->stOut.wActCoswt+(int32)14189*pstPllPara->stOut.wActSinwt)>>14;
		// Sinwt_120 = Sin(wt+2*pi/3) = Sinwt*Cos(2*pi/3) + Coswt*Sin(2*pi/3) = -0.5*Sinwt + (3^0.5/2)*Coswt
		pstPllPara->stOut.wActSinwt_120 = ((int32)-8192*pstPllPara->stOut.wActSinwt+(int32)14189*pstPllPara->stOut.wActCoswt)>>14;
		// Coswt_120 = Cos(wt+2*pi/3) = Coswt*Cos(2*pi/3) - Sinwt*Sin(2*pi/3) = -0.5*Coswt - (3^0.5/2)*Sinwt
		pstPllPara->stOut.wActCoswt_120 = ((int32)-8192*pstPllPara->stOut.wActCoswt-(int32)14189*pstPllPara->stOut.wActSinwt)>>14;
#endif
	}

	if(cFanInvtOpenCheck == stWaitStatus.eStandbyStep)
	{
		pstPllPara->stOut.wActSin5wt = _IQ24sin(pstPllPara->stOut.dActThetaStep*3)>>10;
		pstPllPara->stOut.wActCos5wt = _IQ24cos(pstPllPara->stOut.dActThetaStep*3)>>10;
		pstPllPara->stOut.wSin5wt = _IQ24sin(pstPllPara->stOut.dThetaStep*3)>>10;				// 630ns
		pstPllPara->stOut.wCos5wt = _IQ24cos(pstPllPara->stOut.dThetaStep*3)>>10;				// 630ns

		if(PHASE_SEQ_NEGATIVE == pstPllPara->stIn.uwPhaseSeqFlag)
		{
			// 2^24 * (2*pi/3) = 35138119;
			// 2^24 * (1*pi/4)  = 26353619
		#if 0
			pstPllPara->stOut.wSinwt_240 = _IQ24sin(pstPllPara->stOut.dThetaStep+35138119)>>10;
			pstPllPara->stOut.wCoswt_240 = _IQ24cos(pstPllPara->stOut.dThetaStep+35138119)>>10;
			pstPllPara->stOut.wSinwt_120 = _IQ24sin(pstPllPara->stOut.dThetaStep-35138119)>>10;
			pstPllPara->stOut.wCoswt_120 = _IQ24cos(pstPllPara->stOut.dThetaStep-35138119)>>10;
		#else
			// sin(A+B)  = sinA*cosB + cosA*sinB
			// cos(A+B) = cosA*cosB - sinA*sinB
			// sin(A-B)  = sinA*cosB - cosA*sinB
			// cos(A-B) = cosA*cosB + sinA*sinB

			// Sinwt_240 = Sin(wt+2*pi/3) = Sinwt*Cos(2*pi/3) + Coswt*Sin(2*pi/3) = -0.5*Sinwt + (3^0.5/2)*Coswt
			pstPllPara->stOut.wActSin5wt_240 = ((int32)-8192*pstPllPara->stOut.wActSin5wt+(int32)14189*pstPllPara->stOut.wActCos5wt)>>14;
			// Coswt_240 = Cos(wt+2*pi/3) = Coswt*Cos(2*pi/3) - Sinwt*Sin(2*pi/3) = -0.5*Coswt - (3^0.5/2)*Sinwt
			pstPllPara->stOut.wActCos5wt_240 = ((int32)-8192*pstPllPara->stOut.wActCos5wt-(int32)14189*pstPllPara->stOut.wActSin5wt)>>14;
			// Sinwt_120 = Sin(wt-2*pi/3) = Sinwt*Cos(2*pi/3) - Coswt*Sin(2*pi/3) = -0.5*Sinwt - (3^0.5/2)*Coswt
			pstPllPara->stOut.wActSin5wt_120 = ((int32)-8192*pstPllPara->stOut.wActSin5wt-(int32)14189*pstPllPara->stOut.wActCos5wt)>>14;
			// Coswt_120 = Cos(wt-2*pi/3) = Coswt*Cos(2*pi/3) + Sinwt*Sin(2*pi/3) = -0.5*Coswt + (3^0.5/2)*Sinwt
			pstPllPara->stOut.wActCos5wt_120 = ((int32)-8192*pstPllPara->stOut.wActCos5wt+(int32)14189*pstPllPara->stOut.wActSin5wt)>>14;
		#endif
		}
		else
		{
			//  2^24 * (2*pi/3 )= 35138119;
			// 2^24 * (3*pi/4)  = 26353619
#if 0
			pstPllPara->stOut.wSinwt_240 = _IQ24sin(pstPllPara->stOut.dThetaStep-35138119)>>10;		// 2^14
			pstPllPara->stOut.wCoswt_240 = _IQ24cos(pstPllPara->stOut.dThetaStep-35138119)>>10;
			pstPllPara->stOut.wSinwt_120 = _IQ24sin(pstPllPara->stOut.dThetaStep+35138119)>>10;
			pstPllPara->stOut.wCoswt_120 = _IQ24cos(pstPllPara->stOut.dThetaStep+35138119)>>10;
#else
			// sin(A+B)  = sinA*cosB + cosA*sinB
			// cos(A+B) = cosA*cosB - sinA*sinB
			// sin(A-B)  = sinA*cosB - cosA*sinB
			// cos(A-B) = cosA*cosB + sinA*sinB

			// Sinwt_240 = Sin(wt-2*pi/3) = Sinwt*Cos(2*pi/3) - Coswt*Sin(2*pi/3) = -0.5*Sinwt - (3^0.5/2)*Coswt
			pstPllPara->stOut.wActSin5wt_240 = ((int32)-8192*pstPllPara->stOut.wActSin5wt-(int32)14189*pstPllPara->stOut.wActCos5wt)>>14;
			// Coswt_240 = Cos(wt-2*pi/3) = Coswt*Cos(2*pi/3) + Sinwt*Sin(2*pi/3) = -0.5*Coswt + (3^0.5/2)*Sinwt
			pstPllPara->stOut.wActCos5wt_240 = ((int32)-8192*pstPllPara->stOut.wActCos5wt+(int32)14189*pstPllPara->stOut.wActSin5wt)>>14;
			// Sinwt_120 = Sin(wt+2*pi/3) = Sinwt*Cos(2*pi/3) + Coswt*Sin(2*pi/3) = -0.5*Sinwt + (3^0.5/2)*Coswt
			pstPllPara->stOut.wActSin5wt_120 = ((int32)-8192*pstPllPara->stOut.wActSin5wt+(int32)14189*pstPllPara->stOut.wActCos5wt)>>14;
			// Coswt_120 = Cos(wt+2*pi/3) = Coswt*Cos(2*pi/3) - Sinwt*Sin(2*pi/3) = -0.5*Coswt - (3^0.5/2)*Sinwt
			pstPllPara->stOut.wActCos5wt_120 = ((int32)-8192*pstPllPara->stOut.wActCos5wt-(int32)14189*pstPllPara->stOut.wActSin5wt)>>14;
#endif
	}
}

/*********************************Grid Volt  PLL Ok or Abnormal******************************************/
	if((pstPllPara->stIn.wRGridVoltRms > wPhaseLockVoltMinTmp)
	&& (pstPllPara->stIn.wSGridVoltRms > wPhaseLockVoltMinTmp)
	&& (pstPllPara->stIn.wTGridVoltRms > wPhaseLockVoltMinTmp)
	&& (pstPllPara->stIn.wRSTGridVoltRms<=2880))					// 2880/32 = 90V
	{
		if(FALSE == pstPllPara->stOut.uwPllOk)
		{
			if((pstPllPara->stIn.wGridVolt_q<1800) && (pstPllPara->stIn.wGridVolt_q>-1800))		// 1500/32 = 46.9V				
			{
				pstPllPara->stOut.uwPLLOKCnt++;
				
				if(pstPllPara->stOut.uwPLLOKCnt > 1000)		// 1000*52us=52ms
				{
					pstPllPara->stOut.uwPllOk = TRUE;
					pstPllPara->stOut.uwPLLOKCnt = 0;
					stSysFaultReg.unGridVoltFault.bit.PllFault = 0;
                    stDebug.ReadData.wInvVoltLack = 0;
				}
			}
			else
			{
				
				if(pstPllPara->stOut.uwPLLOKCnt > 0)
				{
					pstPllPara->stOut.uwPLLOKCnt--;
				}
				else
				{
					//stSysFaultReg.unFaultBit.bit.GridVolt = 1;
					//stSysFaultReg.unGridVoltFault.bit.PllFault = 1;
				}
			}
			pstPllPara->stOut.uwPLLFailCnt = 0;
		}
		else
		{
			if((pstPllPara->stIn.wGridVolt_q>2500) || (pstPllPara->stIn.wGridVolt_q<-2500))	// 1500/32 = 46.9V								
			{
				pstPllPara->stOut.uwPLLFailCnt++;
				
				if(pstPllPara->stOut.uwPLLFailCnt > 200)	// 200*52us=10.4ms
				{
					pstPllPara->stOut.uwPllOk = FALSE;
					pstPllPara->stOut.uwPLLFailCnt = 0;
					//stSysFaultReg.unFaultBit.bit.GridVolt = 1;
					//stSysFaultReg.unGridVoltFault.bit.PllFault = 1;
				}
			}
			else
			{
				
				if(pstPllPara->stOut.uwPLLFailCnt > 0)
					pstPllPara->stOut.uwPLLFailCnt--;
			}
			pstPllPara->stOut.uwPLLOKCnt = 0;
		}
	}
	else
	{
		pstPllPara->stOut.uwPllOk = 0;
		pstPllPara->stOut.uwPLLOKCnt = 0;
		pstPllPara->stOut.uwPLLFailCnt = 0;
		
	}

    if((pstPllPara->stOut.uwPllOk == FALSE)&&(eInverterStatus == cWaitStatus))
    {
       if(((stACSample.wPhaseVoltRmsMax > 1000) && (stACSample.wPhaseVoltRmsMin < 900))//if one or two Phase Loss,waitstate
         ||((stACSample.wLineVoltRmsMax > 1500) && (stACSample.wLineVoltRmsMin < 900) && (stACSample.wPhaseVoltRmsMin > 1000))) //PhaseShort
        {
            stDebug.ReadData.wInvVoltLack = 1;
        }
    }
    else
    {
        stDebug.ReadData.wInvVoltLack = 0;
    }

}

/*************************************************************************************************
*
*	FunctionName:	GridVoltPhaseSequenceCheck
*	Input:	none
*	Output:	none
*	Description:	Phase sequence check function, used to pass the entrance parameters
*	Note:	none.
************************************************************************************************/
#pragma CODE_SECTION(GridVoltPhaseLock,"ramfuncs");
void GridVoltPhaseLock(void)
{
	if(cInverterStatus != eInverterStatus)
	{
		stPllPara.stIn.uwPhaseSeqFlag = stPhaseSequence.stOut.uwPhaseSequenceFlag;
		// pll para init
		
		stPllPara.stPID.stIn.dPIMax= ANGLE_FREQ_60HZ_MAX;
		stPllPara.stPID.stIn.dPIMin = ANGLE_FREQ_50HZ_MIN;
		
		stPllPara.stIn.uwRatedPhaseVolt = ((Uint32)stDspReceData.uwACNormalVolt*591)>>10;	// PhaseVolt = NormalVolt/1.732
		stPllPara.stIn.uwAngleOffset = stDebug.SetData.uwAngleOffset;
	}
	
	stPllPara.stIn.wRGridVoltRms = stACSample.PhaseVoltRms.wRN;
	stPllPara.stIn.wSGridVoltRms = stACSample.PhaseVoltRms.wSN;
	stPllPara.stIn.wTGridVoltRms = stACSample.PhaseVoltRms.wTN;
	stPllPara.stIn.wRSTGridVoltRms = stACSample.RSTGridVoltRms;
	
	stPllPara.stIn.wGridVolt_q = stInvPara.wVolt_q;

	if(AC_220V_LEVEL == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand)
	{
		stPllPara.stPID.stIn.dKp = 8950;		//  100*10^-8 * 2^24*16384/32
		stPllPara.stPID.stIn.dKi = 86;		// 1*10^-8 * 2^24*16384/32
	}
	else
	{
		if(FALSE == stPllPara.stOut.uwPllOk)
		{
			stPllPara.stPID.stIn.dKp = 8950;		//  100*10^-8 * 2^24*16384/32
			stPllPara.stPID.stIn.dKi = 86;		// 1*10^-8 * 2^24*16384/32
		}
		else
		{
			stPllPara.stPID.stIn.dKp = 3000;		//  100*10^-8 * 2^24*16384/32
			stPllPara.stPID.stIn.dKi = 30;		// 1*10^-8 * 2^24*16384/32
		}
	}

	PhaseLockLoop(&stPllPara);
}

/********************************************************************************************
*
*	FunctionName:	PhaseSequenceCheck
*	Input:	none	@ pstPhaseSeq->stIn.wSGridVolt;     	// R Phase Grid Voltage Real Vaule
        			@ pstPhaseSeq->stIn.wSGridVolt;     	// S Phase Grid Voltage Real Vaule
        			@ pstPhaseSeq->stIn.wSGridVolt;     	// T Phase Grid Voltage Real Vaule
					@ pstPhaseSeq->stIn.wRGridVoltRms;     	// R Phase Grid Voltage Rms Vaule
					@ pstPhaseSeq->stIn.wRGridVoltRms;     	// S Phase Grid Voltage Rms Vaule
					@ pstPhaseSeq->stIn.wRGridVoltRms;     	// T Phase Grid Voltage Rms Vaule
        			@ pstPhaseSeq->stIn.uwRatedPhaseVolt; 	// Rated Grid Volt Value
        			@ pstPhaseSeq->stIn.uwPhaseSequenceCheck;	// 0:don't need to check	1: need to check
*	Output:	none
*					@ pstPhaseSeq->stOut.uwPhaseSequenceFlag;     	// 0: positive sequence 1:negative sequence
        			@ pstPhaseSeq->stIn.uwPhaseSequenceSetp;     	// 0: standby	1:checking	2:check finished
*	Description:	Phase sequence check function, used to pass the entrance parameters
*	Note:	none.
*********************************************************************************************/
#pragma CODE_SECTION(PhaseSequenceCheck,"ramfuncs");
void PhaseSequenceCheck(ST_PHSAE_SEQUENCE *pstPhaseSeq)
{
	static Uint16 suwNegSeqCnt=0,suwPosSeqCnt=0;
	static Uint16 suwPhaseSeqCheckRestart=0;

	int16	wVoltAlphaTmp,wVoltBetaTmp;     // The α & β  value of the clack transform
	int16	wPhaseSeqCheckVoltMinTmp = 0;

	wVoltAlphaTmp = ((-(int32)pstPhaseSeq->stIn.wSGridVolt + (int32)pstPhaseSeq->stIn.wTGridVolt) * 591>>10);							//1/sqrt(3)*2^10=591
	wVoltBetaTmp = (((int32)2*pstPhaseSeq->stIn.wRGridVolt - (int32)pstPhaseSeq->stIn.wSGridVolt - (int32)pstPhaseSeq->stIn.wTGridVolt) * 341>>10);	//0.3*2^10=341

	pstPhaseSeq->stIn.wVoltAlpha = wVoltAlphaTmp;
	pstPhaseSeq->stIn.wVoltBeta = wVoltBetaTmp;

	wPhaseSeqCheckVoltMinTmp = VAC30V;//(pstPhaseSeq->stIn.uwRatedPhaseVolt>>1);		// RatedVolt * 0.5

	if((wVoltBetaTmp >= 960) && (wVoltAlphaTmp >= 960)						// 960/32 = 30V
	&& (pstPhaseSeq->stIn.wRGridVoltRms >= wPhaseSeqCheckVoltMinTmp)
	&& (pstPhaseSeq->stIn.wSGridVoltRms >= wPhaseSeqCheckVoltMinTmp)
	&& (pstPhaseSeq->stIn.wTGridVoltRms >= wPhaseSeqCheckVoltMinTmp)
	&& (0==pstPhaseSeq->stOut.uwPhaseSequenceSetp))
	{
		pstPhaseSeq->stOut.uwPhaseSequenceSetp = PHASE_SEQ_CHECK_CHECKING;
	}
	else if((1 == pstPhaseSeq->stIn.uwPhaseSequenceCheck)
		&& (0 == suwPhaseSeqCheckRestart))
	{
		suwPhaseSeqCheckRestart = 1;
		pstPhaseSeq->stOut.uwPhaseSequenceSetp = PHASE_SEQ_CHECK_STANDBY;
	}

	if(PHASE_SEQ_CHECK_CHECKING==pstPhaseSeq->stOut.uwPhaseSequenceSetp)
	{
		if((wVoltBetaTmp <= -960)||(wVoltAlphaTmp <= -960))					// 960/32 = 30V
		{
			if(wVoltBetaTmp <= -960)
			{
				suwNegSeqCnt++;
				if(suwNegSeqCnt > 3)
				{
					pstPhaseSeq->stOut.uwPhaseSequenceFlag = PHASE_SEQ_NEGATIVE;	// GridVoltage:negative sequence
					pstPhaseSeq->stOut.uwPhaseSequenceSetp = PHASE_SEQ_CHECK_FINISH;
					pstPhaseSeq->stIn.uwPhaseSequenceCheck = 0;

					suwPhaseSeqCheckRestart = 0; 	// phase check finish
					suwNegSeqCnt = 0;
					suwPosSeqCnt = 0;
				}
			}

			if(wVoltAlphaTmp <= -960)
			{
				suwPosSeqCnt++;
				if(suwPosSeqCnt > 3)
				{
					pstPhaseSeq->stOut.uwPhaseSequenceFlag = PHASE_SEQ_POSITIVE;	// GridVoltage:positive sequence
					pstPhaseSeq->stOut.uwPhaseSequenceSetp = PHASE_SEQ_CHECK_FINISH;
					pstPhaseSeq->stIn.uwPhaseSequenceCheck = 0;

					suwPhaseSeqCheckRestart = 0; 	// phase check finish
					suwNegSeqCnt = 0;
					suwPosSeqCnt = 0;
				}
			}
		}
	}
}

/*************************************************************************************************
*
*	FunctionName:	GridVoltPhaseSequenceCheck
*	Input:	none
*	Output:	none
*	Description:	Phase sequence check function, used to pass the entrance parameters
*	Note:	none.
************************************************************************************************/
#pragma CODE_SECTION(GridVoltPhaseSequenceCheck,"ramfuncs");
void GridVoltPhaseSequenceCheck(void)
{
	stPhaseSequence.stIn.wRGridVolt = stAdcPool.RGridVolt.wArithVal;
	stPhaseSequence.stIn.wSGridVolt = stAdcPool.SGridVolt.wArithVal;
	stPhaseSequence.stIn.wTGridVolt = stAdcPool.TGridVolt.wArithVal;

	stPhaseSequence.stIn.wRGridVoltRms = stACSample.PhaseVoltRms.wRN;
	stPhaseSequence.stIn.wSGridVoltRms = stACSample.PhaseVoltRms.wSN;
	stPhaseSequence.stIn.wTGridVoltRms = stACSample.PhaseVoltRms.wTN;

	stPhaseSequence.stIn.uwRatedPhaseVolt = ((Uint32)stDspReceData.uwACNormalVolt*591)>>10;	// PhaseVolt = NormalVolt/1.732

	if(FALSE == stPllPara.stOut.uwPllOk)
	{
		if((1 == stSysFaultReg.unFaultBit.bit.GridVolt)	// grid voltage fault
		|| (1 == stSysFaultReg.unFaultBit.bit.GridFreq))// grid freqency fault
		{
			stPhaseSequence.stIn.uwPhaseSequenceCheck = 1;		// need to check grid volt phase sequence
		}
	}

    PhaseSequenceCheck(&stPhaseSequence);
}


/*************************************************************************************************
*
*	FunctionName:	GridVoltIslandDisturb
*	Input:	none
*	Output:	none
*	Description:	ISsland Disturb
*	Note:	none.
************************************************************************************************/
#pragma CODE_SECTION(GridVoltIslandDisturb,"ramfuncs");
void GridVoltIslandDisturb(void)
{
	static Uint16 suwIslandDisturbDelayCnt;
	static int32	dThetaStepOld4,dThetaStepOld3,dThetaStepOld2,dThetaStepOld1;
	int32	dIslandDisturbResult=0;
	if((ENABLE == unSysFlag.bit.InvPwmEnable)&& (cInverterStatus == eInverterStatus)&&(stF107Data.unEnable.bit.ISLAND == 1))
	{
		if(96 == stPllPara.stOut.uwGridPrdCnt)		// 96/384=1/4=90°
		{
			dThetaStepOld4 = dThetaStepOld3;
			dThetaStepOld3 = dThetaStepOld2;
			dThetaStepOld2 = dThetaStepOld1;
			dThetaStepOld1 = (stPllPara.stOut.dThetaStep>>8);	// >>14

			suwIslandDisturbDelayCnt++;
			if(suwIslandDisturbDelayCnt >= 150)	// 20ms * 150 = 3s
			{
				suwIslandDisturbDelayCnt = 150;
				dIslandDisturbResult = (dThetaStepOld4*3) - dThetaStepOld3 - dThetaStepOld2 - dThetaStepOld1;
				dIslandDisturbResult = (dIslandDisturbResult*(int32)stPllPara.stIn.wIslandDisturbKp>>10);
				UPDNLMT(dIslandDisturbResult,960,-960);			// 960/32 = +/-30.0A
				stPllPara.wIslandDisturb = dIslandDisturbResult;
			}
		}
	}
	else
	{
		stPllPara.wIslandDisturb = 0;
		suwIslandDisturbDelayCnt = 0;
		dThetaStepOld4 = 0;
		dThetaStepOld3 = 0;
		dThetaStepOld2 = 0;
		dThetaStepOld1 = 0;
	}
}

//===========================================================================
// End of file.
//===========================================================================
