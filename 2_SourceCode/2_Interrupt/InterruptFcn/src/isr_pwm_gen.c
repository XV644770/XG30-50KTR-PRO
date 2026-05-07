/*-------------------------------------------------------
* Solar_PwmGen.c
*
*	Product:			three phase inveter
*	Filename:			Solar_PwmGen.c
*	Purpose:			pwm generate
*	Date				Version		Author			Note
*   20/12/11			V1.01		csx				none
*--------------------------------------------------------*/
#include "dsp_user_include.h"

ST_INV_PWM		stInvPwm;
ST_PWM_CALC	stPwmCalc;
ST_PWM_PARA	stPwmPara;

/* wBusVoltInverse = 1/BusVolt * 2^18
 BusVolt = 600V,612.75V,625.5V,638.25V,.......1008V
 example: BusVolt    Index   BusVoltInverse
            600V       0        436.9
            612.75	   1        427.8
            ...       ....      ....
            1008      32        260          */
#if 0
const int16 wBusVoltInverse[33] ={
	437,428,419,411,403,395,387,380,373,367,360,354,348,342,336,331,326,321,316,311,
	306,302,297,293,289,285,281,277,274,270,266,263,260
};
#else
const int16 wBusVoltInverse[65] ={
	1372,1287,1211,1143,1083,1029,980,935,895,857,823,791,762,735,709,686,664,643,623,
	605,588,571,556,541,527,514,502,490,478,467,457,447,
	437,428,419,411,403,395,388,380,373,367,360,354,348,342,337,331,326,321,316,311,
	307,302,298,293,289,285,281,278,274,270,267,263,260
};
#endif

void PwmGenParaInit()
{
	memset((void*)&stInvPwm,0,sizeof(stInvPwm));
	memset((void*)&stPwmCalc,0,sizeof(stPwmCalc));
	memset((void*)&stPwmPara,0,sizeof(stPwmPara));

	stPwmCalc.stIn.uwDutyCompensation = 20;
	stPwmCalc.stIn.uwModulateMode = SPWM_MODE;
}

static void InvPwmDutyCalc(ST_PWM_CALC *pstPwmCalc);

/*=============================================================================*
 * FUNCTION: InvPwmOnOff()
 * PURPOSE : pwm on or off
 *			 
 * INPUT: 
 * CALLED BY: EPWM2_prd_isr()
 * 
 * 
 *============================================================================*/
#pragma CODE_SECTION(InvPwmOnOff,"ramfuncs");
void InvPwmOnOff(void)
{
	unSysFlag.bit.InvPwmEnableOld = unSysFlag.bit.InvPwmEnable;

	if(0 != stSysFaultReg.unFaultBit.all)		// fault happen
	{
		unSysFlag.bit.InvPwmEnable = 0;			// stop inv pwm
		stInvPwm.unPwmDisableBit.all = 0;
	}
	else
	{
		if((cInverterStatus == eInverterStatus)      // inverter normal working
		||((cWaitStatus==eInverterStatus)&&(1==stWaitStatus.stRelayCheck.Flag.InvPwmOn)&&(0==stWaitStatus.stRelayCheck.Flag.InvCurrOver)))//Relay check open inv PWM
		{
			if(1==stInvPwm.uwPwmDisableDelayCnt)		// Trip OCP Single
			{
				stInvPwm.uwPwmDisableTimersCnt++;		// Record Pwm Disable Times
			}

			if(stInvPwm.uwPwmDisableDelayCnt != 0 )				// Trip OCP Single
			{
				unSysFlag.bit.InvPwmEnable = 0;						// Close Inv PWM

				stInvPwm.uwPwmDisableDelayCnt++;
				if(stInvPwm.uwPwmDisableDelayCnt>=PWM_DISABLE_TIME)	// count down 30ms
				{
					stInvPwm.uwPwmDisableDelayCnt = 0;
					stInvPwm.unPwmDisableBit.all = 0;
					stInvPwm.unPwmDisableBitOld.all = 0;
				}
			}
			else if((0 == unSysFlag.bit.InvPwmEnable)&&(0 == stPllPara.stOut.uwGridPrdCnt))		//enable PWM on zero cross point
			{
				unSysFlag.bit.InvPwmEnable = 1;
			}
		}
		else
		{
			unSysFlag.bit.InvPwmEnable = 0;
		}
	}

	/******************Inv Pwm  AQ Register Config******************/
	if((1 == unSysFlag.bit.InvPwmEnable) && (1 == unSysFlag.bit.InvPwmEnableOld))		// Open Inv Pwm
	{
		EPwm1Regs.AQCSFRC.all = 0;        //CSFB:0,CSFA:0	-> 0x09
		EPwm2Regs.AQCSFRC.all = 0;
		EPwm3Regs.AQCSFRC.all = 0;
		EPwm4Regs.AQCSFRC.all = 0;
		EPwm5Regs.AQCSFRC.all = 0;
		EPwm6Regs.AQCSFRC.all = 0;
		EPwm7Regs.AQCSFRC.all = 0;

	}
	else																			// Close Inv Pwm
	{
		EPwm1Regs.AQCSFRC.all = 0x09;     //CSFB:1,CSFA:1-1001->0x09
		EPwm2Regs.AQCSFRC.all = 0x09;
		EPwm3Regs.AQCSFRC.all = 0x09;
		EPwm4Regs.AQCSFRC.all = 0x09;
		EPwm5Regs.AQCSFRC.all = 0x09;
		EPwm6Regs.AQCSFRC.all = 0x09;
		EPwm7Regs.AQCSFRC.all = 0x09;
	}


	if(stInvPwm.uwTzEnableDelayCnt > 0)
	{
		stInvPwm.uwTzEnableDelayCnt--;
		if(stInvPwm.uwTzEnableDelayCnt < 5)
		{
			EALLOW;
			EPwm1Regs.TZCLR.bit.OST = 1;			// Clear Flag for One-Shot Trip (OST) Latch
			EPwm2Regs.TZCLR.bit.OST = 1;			// Clear Flag for One-Shot Trip (OST) Latch
			EPwm3Regs.TZCLR.bit.OST = 1;
			EPwm4Regs.TZCLR.bit.OST = 1;
			EPwm5Regs.TZCLR.bit.OST = 1;
			EPwm6Regs.TZCLR.bit.OST = 1;
			EPwm7Regs.TZCLR.bit.OST = 1;
			EPwm8Regs.TZCLR.bit.OST = 1;
			EPwm9Regs.TZCLR.bit.OST = 1;
			EPwm10Regs.TZCLR.bit.OST = 1;
			EPwm11Regs.TZCLR.bit.OST = 1;
			EPwm12Regs.TZCLR.bit.OST = 1;
			EPwm1Regs.TZEINT.bit.OST = 1;           //Enable Epwm1 TZ INT
			EDIS;
		}
	}
}

/*=============================================================================*
 * FUNCTION: InvPwmDutyCalc()
 * PURPOSE : PWM Duty Calc
 *
 * INPUT:
 * CALLED BY: EPWM2_prd_isr()
 *============================================================================*/
#pragma CODE_SECTION(InvOutVoltCalcOpenLoop,"ramfuncs");
void InvOutVoltCalcOpenLoop(void)
{
	int16 wBusVoltPositionTmp;
	int16 wBusVoltInverseIndexTmp;
	static int16 swInvVoltSoftCnt=0;
	int16 wGridVolt_dTmp;
	int16 wBusVoltTmp;
	int16 w1DivBus;

	wBusVoltTmp = 7000;			// 700V
	wGridVolt_dTmp = 230*45;	// 230V*1.414*32

	stPwmPara.stIn.wUout_d = ((int32)wGridVolt_dTmp*swInvVoltSoftCnt)>>11;
	stPwmPara.stIn.wUout_q = 0;
	if(swInvVoltSoftCnt < 2048)
	{
		swInvVoltSoftCnt ++;
	}
	else
	{
		swInvVoltSoftCnt = 2048;
	}
	// Limit Inv Voltage (park's d) Loop output
	UPDNLMT(stPwmPara.stIn.wUout_d,14932,-14932);		// 14932/32 = 468.75V/1.414 = 330.0V
	// Limit Inv Voltage (park's q) Loop output
	UPDNLMT(stPwmPara.stIn.wUout_q,4977,-4977);			// 4977/32 = 155.5V/1.414 = 109.9V

	stPwmPara.stOut.wRInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wSinwt >>14)
							   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wCoswt>>14);

	stPwmPara.stOut.wSInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wSinwt_240>>14)
							   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wCoswt_240>>14);

	stPwmPara.stOut.wTInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wSinwt_120>>14)
							   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wCoswt_120>>14);
	/*******************************Inv Pwm Calculate**************************/
	stPwmCalc.stIn.uwModulateMode = SPWM_MODE;
//	wBusVoltPositionTmp = (wBusVoltTmp - 6000);	// 600V - Position_0
//	if(wBusVoltPositionTmp < 0)
//	{
//		wBusVoltPositionTmp = 0;
//	}
//	wBusVoltInverseIndexTmp = (wBusVoltPositionTmp>>7);
//	UPDNLMT16(wBusVoltInverseIndexTmp, 32, 0);
//	// kPwm = Period/BusVolt                  1-----2^8
//	stPwmCalc.stIn.wKPwm = (int)((int32)PWM_INV_PERIOD * (int32)wBusVoltInverse[wBusVoltInverseIndexTmp]>>10);

	w1DivBus = ((int32)2621440/wBusVoltTmp);		
	// kPwm = Period/BusVolt                  1-----2^8
	stPwmCalc.stIn.wKPwm = (int)(((int32)PWM_INV_PERIOD * (int32)w1DivBus)>>10);

	stPwmCalc.stIn.uwDutyCompensation = 20;
	stPwmCalc.stIn.wInvCurrZero = 0;

	stPwmCalc.stIn.wRInvOut = stPwmPara.stOut.wRInvCtrl;
	stPwmCalc.stIn.wSInvOut = stPwmPara.stOut.wSInvCtrl;
	stPwmCalc.stIn.wTInvOut = stPwmPara.stOut.wTInvCtrl;

	stPwmCalc.stIn.wRDCI_PI = 0;	// stACRms.DCI.PI
	stPwmCalc.stIn.wSDCI_PI = 0;	// stACRms.DCI.PI

	InvPwmDutyCalc(&stPwmCalc);

	EPwm1Regs.AQCSFRC.all = 0;        //CSFB:0,CSFA:0
	EPwm2Regs.AQCSFRC.all = 0;
	EPwm3Regs.AQCSFRC.all = 0;
	EPwm4Regs.AQCSFRC.all = 0;
	EPwm5Regs.AQCSFRC.all = 0;
	EPwm6Regs.AQCSFRC.all = 0;
	EPwm7Regs.AQCSFRC.all = 0;
}

#pragma CODE_SECTION(InvOutVoltCalc,"ramfuncs");
void InvOutVoltCalc(void)
{
	int16 wBusVoltPositionTmp;
	int16 wBusVoltInverseIndexTmp;
	static int16 swInvVoltSoftCnt=256;
	static int16 swInvOpenSoftCnt=256;

	if(1 == unSysFlag.bit.InvPwmEnable)
	{
		if((cWaitStatus == eInverterStatus)&&(1 == stWaitStatus.stRelayCheck.Flag.InvPwmOn))//Relay check open inv PWM
		{
			if(cFanInvtOpenCheck == stWaitStatus.eStandbyStep)
			{
				stPwmPara.stIn.wUout_d = ((int32)stInvVoltCtrl.stOut.wForwardVolt_d*swInvOpenSoftCnt)>>12;
				stPwmPara.stIn.wUout_q = ((int32)stInvVoltCtrl.stOut.wForwardVolt_q*swInvOpenSoftCnt)>>12;
				if(swInvOpenSoftCnt < 4096)				// (4096-256)/19200 = 200ms
				{
					swInvOpenSoftCnt ++;
				}
				else
				{
					swInvOpenSoftCnt = 4096;
				}
			}
			else
			{
				//stPwmPara.stIn.wUout_d = ((int32)stInvPara.wVolt_d*swInvVoltSoftCnt)>>12;
				//stPwmPara.stIn.wUout_q = ((int32)stInvPara.wVolt_q*swInvVoltSoftCnt)>>12;

				stPwmPara.stIn.wUout_d = ((int32)stInvVoltCtrl.stOut.wForwardVolt_d*swInvVoltSoftCnt)>>12;
				stPwmPara.stIn.wUout_q = ((int32)stInvVoltCtrl.stOut.wForwardVolt_q*swInvVoltSoftCnt)>>12;
				if(swInvVoltSoftCnt < 4096)				// (4096-256)/19200 = 200ms 
				{
					swInvVoltSoftCnt ++;
				}
				else
				{
					swInvVoltSoftCnt = 4096;
				}
			}
		}
		else
		{
			//stPwmPara.stIn.wUout_d = stInvVoltCtrl.stOut.wForwardVolt_d+stInvCurrCtrl.stOut.wOutCurr_d;
			//stPwmPara.stIn.wUout_q = stInvVoltCtrl.stOut.wForwardVolt_q+stInvCurrCtrl.stOut.wOutCurr_q;
			stPwmPara.stIn.wUout_d = stInvVoltCtrl.stOut.wForwardVolt_d+stInvCurrCtrl.stOut.wOutCurr_d+stInvCurrCtrl.stIn.ReCtrl_Val_Id ;
			stPwmPara.stIn.wUout_q = stInvVoltCtrl.stOut.wForwardVolt_q+stInvCurrCtrl.stOut.wOutCurr_q+ stInvCurrCtrl.stIn.ReCtrl_Val_Iq ;
			swInvVoltSoftCnt = 256;
			swInvOpenSoftCnt = 256;
		}
		// Limit Inv Voltage (park's d) Loop output
		UPDNLMT(stPwmPara.stIn.wUout_d,14932,-14932);		// 14932/32 = 468.75V/1.414 = 330.0V
		// Limit Inv Voltage (park's q) Loop output
		UPDNLMT(stPwmPara.stIn.wUout_q,4977,-4977);			// 4977/32 = 155.5V/1.414 = 109.9V
		if(cFanInvtOpenCheck==stWaitStatus.eStandbyStep)	// Output Voltage Amplitude Control
		{
			stPwmPara.stOut.wRInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wActSin5wt>>14)
									   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wActCos5wt>>14);

			stPwmPara.stOut.wSInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wActSin5wt_240>>14)
									   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wActCos5wt_240>>14);

			stPwmPara.stOut.wTInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wActSin5wt_120>>14)
									   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wActCos5wt_120>>14);
		}
		else
		{
			stPwmPara.stOut.wRInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wActSinwt>>14)
									   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wActCoswt>>14);

		stPwmPara.stOut.wSInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wActSinwt_240>>14)
								   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wActCoswt_240>>14);

			stPwmPara.stOut.wTInvCtrl = ((int32)stPwmPara.stIn.wUout_d*stPllPara.stOut.wActSinwt_120>>14)
									   +((int32)stPwmPara.stIn.wUout_q*stPllPara.stOut.wActCoswt_120>>14);
		}
		/*******************************Inv Pwm Calculate**************************/
		if((cWaitStatus == eInverterStatus)&&(1 == stWaitStatus.stRelayCheck.Flag.InvPwmOn))//Relay check open inv PWM
		{
			stPwmCalc.stIn.uwModulateMode = SPWM_MODE;
		}
		else
		{
			if(1==stDebug.SetData.unSetReg.bit.PWMMode)
			{
				stPwmCalc.stIn.uwModulateMode = DPWM_MODE;
			}
			else
			{
				stPwmCalc.stIn.uwModulateMode = SVPWM_MODE;
			}
		}
#if 0
		wBusVoltPositionTmp = (stDCSample.wBusVolt - 6000);	// 600V - Position_0
		if(wBusVoltPositionTmp < 0)
		{
			wBusVoltPositionTmp = 0;
		}
		wBusVoltInverseIndexTmp = (wBusVoltPositionTmp>>7);
		UPDNLMT16(wBusVoltInverseIndexTmp, 32, 0);
#else
		wBusVoltPositionTmp = (stDCSample.wBusVolt - 1910);	// 191V - Position_0
		if(wBusVoltPositionTmp < 0)
		{
			wBusVoltPositionTmp = 0;
		}
		wBusVoltInverseIndexTmp = (wBusVoltPositionTmp>>7);
		UPDNLMT16(wBusVoltInverseIndexTmp, 64, 0);
#endif
#if 0
		// kPwm = Period/BusVolt                  1-----2^8
		stPwmCalc.stIn.wKPwm = (int)((int32)PWM_INV_PERIOD * (int32)wBusVoltInverse[wBusVoltInverseIndexTmp]>>10);
#else
		// kPwm = Period/BusVolt                  1-----2^8
		stPwmCalc.stIn.wKPwm = (int)(((int32)PWM_INV_PERIOD * (int32)stDCSample.w1DivBusFlt)>>10);
#endif
		stPwmCalc.stIn.uwDutyCompensation = stDebug.SetData.uwDutyOffset;
		stPwmCalc.stIn.wInvCurrZero = 0;

		stPwmCalc.stIn.wRInvOut = stPwmPara.stOut.wRInvCtrl;
		stPwmCalc.stIn.wSInvOut = stPwmPara.stOut.wSInvCtrl;
		stPwmCalc.stIn.wTInvOut = stPwmPara.stOut.wTInvCtrl;

		stPwmCalc.stIn.wRDCI_PI = wRDCI_PI;
		stPwmCalc.stIn.wSDCI_PI = wSDCI_PI;	// stACRms.DCI.PI

		InvPwmDutyCalc(&stPwmCalc);
	}
	else
	{
		stPwmCalc.stIn.uwDutyCompensation = stDebug.SetData.uwDutyOffset;
		swInvVoltSoftCnt = 256;
		swInvOpenSoftCnt = 256;
		memset((void*)&stPwmCalc,0,sizeof(stPwmCalc));
		memset((void*)&stPwmPara,0,sizeof(stPwmPara));
	}
}
/****************************************************************************************
*Functions PWN Duty Claculate
*
* SPwmDuty = VOut/(VBus/2) * Period           -----KPwm = (Period/VBus)*2^8  ----VOut = (U_d*sinwt + U_q*coswt)*2^5
*          = VOut*2^5*Period/VBus*2^8/2
*          = Vout*KPwm*2^5*2^8/2
*          = Vout*KPwm*2^12
*****************************************************************************************/
#pragma CODE_SECTION(InvPwmDutyCalc,"ramfuncs");
static void InvPwmDutyCalc(ST_PWM_CALC *pstPwmCalc)
{
	int16 wPwmMaxTmp=0,wPwmMinTmp=0,wPwmMidTmp=0,wPwmTmp=0;
	int16 wRPwmAbsTmp=0,wSPwmAbsTmp=0,wTPwmAbsTmp=0,wMaxPwmAbsTmp=0;

	// RInvOut - 2^5    KPwm - 2^8   Bus/2
	pstPwmCalc->stOut.wRPWM = (int16)((int32)pstPwmCalc->stIn.wRInvOut * pstPwmCalc->stIn.wKPwm>>12);
	pstPwmCalc->stOut.wSPWM = (int16)((int32)pstPwmCalc->stIn.wSInvOut * pstPwmCalc->stIn.wKPwm>>12);
	pstPwmCalc->stOut.wTPWM = (int16)((int32)pstPwmCalc->stIn.wTInvOut * pstPwmCalc->stIn.wKPwm>>12);

	if(SVPWM_MODE == pstPwmCalc->stIn.uwModulateMode)
	{
		// 3rd harmonic inject
		wPwmMaxTmp = MAX3(pstPwmCalc->stOut.wRPWM,pstPwmCalc->stOut.wSPWM,pstPwmCalc->stOut.wTPWM);
		wPwmMinTmp = MIN3(pstPwmCalc->stOut.wRPWM,pstPwmCalc->stOut.wSPWM,pstPwmCalc->stOut.wTPWM);
		wPwmMidTmp = (wPwmMinTmp+ wPwmMaxTmp)>>1;

		pstPwmCalc->stOut.wRPWMModulate = (pstPwmCalc->stOut.wRPWM-wPwmMidTmp-pstPwmCalc->stIn.wInvCurrZero);
		pstPwmCalc->stOut.wSPWMModulate = (pstPwmCalc->stOut.wSPWM-wPwmMidTmp-pstPwmCalc->stIn.wInvCurrZero);
		pstPwmCalc->stOut.wTPWMModulate = (pstPwmCalc->stOut.wTPWM-wPwmMidTmp-pstPwmCalc->stIn.wInvCurrZero);

		pstPwmCalc->stOut.wRPWMModulate += (int16)((int32)pstPwmCalc->stIn.wRDCI_PI* pstPwmCalc->stIn.wKPwm>>12);
		pstPwmCalc->stOut.wSPWMModulate += (int16)((int32)pstPwmCalc->stIn.wSDCI_PI* pstPwmCalc->stIn.wKPwm>>12);
		pstPwmCalc->stOut.wTPWMModulate -= (int16)((int32)(pstPwmCalc->stIn.wRDCI_PI + pstPwmCalc->stIn.wSDCI_PI)
															* pstPwmCalc->stIn.wKPwm>>12);
	}
	else if(DPWM_MODE == pstPwmCalc->stIn.uwModulateMode)
	{
		wPwmTmp = pstPwmCalc->stOut.wRPWM;
		wMaxPwmAbsTmp=abs(pstPwmCalc->stOut.wRPWM);

		wSPwmAbsTmp=abs(pstPwmCalc->stOut.wSPWM);
		if(wMaxPwmAbsTmp<wSPwmAbsTmp)
		{
			wMaxPwmAbsTmp = wSPwmAbsTmp;
			wPwmTmp=pstPwmCalc->stOut.wSPWM;
		}

		wTPwmAbsTmp=abs(pstPwmCalc->stOut.wTPWM);
		if(wMaxPwmAbsTmp<wTPwmAbsTmp)
		{
			wMaxPwmAbsTmp = wTPwmAbsTmp;
			wPwmTmp=pstPwmCalc->stOut.wTPWM;
		}

		if(wPwmTmp>0)
		{
			wPwmMidTmp=PWM_INV_DUTY_MAX-wPwmTmp;
		}
		else
		{
			wPwmMidTmp=-PWM_INV_DUTY_MAX-wPwmTmp;
		}

		pstPwmCalc->stOut.wRPWMModulate = (pstPwmCalc->stOut.wRPWM+wPwmMidTmp-pstPwmCalc->stIn.wInvCurrZero);
		pstPwmCalc->stOut.wSPWMModulate = (pstPwmCalc->stOut.wSPWM+wPwmMidTmp-pstPwmCalc->stIn.wInvCurrZero);
		pstPwmCalc->stOut.wTPWMModulate = (pstPwmCalc->stOut.wTPWM+wPwmMidTmp-pstPwmCalc->stIn.wInvCurrZero);

		pstPwmCalc->stOut.wRPWMModulate += pstPwmCalc->stIn.wRDCI_PI;
		pstPwmCalc->stOut.wSPWMModulate += pstPwmCalc->stIn.wSDCI_PI;
	}
	else
	{
		pstPwmCalc->stOut.wRPWMModulate = pstPwmCalc->stOut.wRPWM;
		pstPwmCalc->stOut.wSPWMModulate = pstPwmCalc->stOut.wSPWM;
		pstPwmCalc->stOut.wTPWMModulate = pstPwmCalc->stOut.wTPWM;
	}

	if(pstPwmCalc->stOut.wRPWMModulate >= 0)
	{
		pstPwmCalc->stOut.uwRPWMDutyP = pstPwmCalc->stOut.wRPWMModulate;
		if(pstPwmCalc->stOut.wRPWMModulateOld < 0)
		{
			pstPwmCalc->stOut.uwRPWMDutyP = 0;
		}
		pstPwmCalc->stOut.uwRPWMDutyP += pstPwmCalc->stIn.uwDutyCompensation; 				// duty offset
		UPDNLMT16(pstPwmCalc->stOut.uwRPWMDutyP, PWM_INV_DUTY_MAX, PWM_INV_DUTY_MIN);	//for R 1,3PWM
		pstPwmCalc->stOut.uwRPWMDutyN = 0;												//for R 2,4PWM
	}
	else
	{
		pstPwmCalc->stOut.uwRPWMDutyN = pstPwmCalc->stOut.wRPWMModulate;
		if(pstPwmCalc->stOut.wRPWMModulateOld >= 0)
		{
			pstPwmCalc->stOut.uwRPWMDutyN = 0;
		}
		pstPwmCalc->stOut.uwRPWMDutyN = -pstPwmCalc->stOut.uwRPWMDutyN;
		pstPwmCalc->stOut.uwRPWMDutyN += pstPwmCalc->stIn.uwDutyCompensation; 				// duty offset
		UPDNLMT16(pstPwmCalc->stOut.uwRPWMDutyN, PWM_INV_DUTY_MAX, PWM_INV_DUTY_MIN);	//for R 2,4PWM

		pstPwmCalc->stOut.uwRPWMDutyP = 0;												//for R 1,3PWM
	}

	if(pstPwmCalc->stOut.wSPWMModulate >= 0)
	{
		pstPwmCalc->stOut.uwSPWMDutyP = pstPwmCalc->stOut.wSPWMModulate;
		if(pstPwmCalc->stOut.wSPWMModulateOld < 0)
		{
			pstPwmCalc->stOut.uwSPWMDutyP = 0;
		}
		pstPwmCalc->stOut.uwSPWMDutyP += pstPwmCalc->stIn.uwDutyCompensation;				// duty offset
		UPDNLMT16(pstPwmCalc->stOut.uwSPWMDutyP, PWM_INV_DUTY_MAX, PWM_INV_DUTY_MIN);	//for R 1,3PWM
		pstPwmCalc->stOut.uwSPWMDutyN = 0;												//for R 2,4PWM
	}
	else
	{
		pstPwmCalc->stOut.uwSPWMDutyN = pstPwmCalc->stOut.wSPWMModulate;
		if(pstPwmCalc->stOut.wSPWMModulateOld >= 0)
		{
			pstPwmCalc->stOut.uwSPWMDutyN = 0;
		}

		pstPwmCalc->stOut.uwSPWMDutyN = -pstPwmCalc->stOut.uwSPWMDutyN;
		pstPwmCalc->stOut.uwSPWMDutyN += pstPwmCalc->stIn.uwDutyCompensation;				// duty offset
		UPDNLMT16(pstPwmCalc->stOut.uwSPWMDutyN, PWM_INV_DUTY_MAX, PWM_INV_DUTY_MIN);	//for R 2,4PWM

		pstPwmCalc->stOut.uwSPWMDutyP = 0;												//for R 1,3PWM
	}

	if(pstPwmCalc->stOut.wTPWMModulate >= 0)
	{
		pstPwmCalc->stOut.uwTPWMDutyP = pstPwmCalc->stOut.wTPWMModulate;
		if(pstPwmCalc->stOut.wTPWMModulateOld < 0)
		{
			pstPwmCalc->stOut.uwTPWMDutyP = 0;
		}
		pstPwmCalc->stOut.uwTPWMDutyP += pstPwmCalc->stIn.uwDutyCompensation;				// duty offset
		UPDNLMT16(pstPwmCalc->stOut.uwTPWMDutyP, PWM_INV_DUTY_MAX, PWM_INV_DUTY_MIN);	//for R 1,3PWM
		pstPwmCalc->stOut.uwTPWMDutyN = 0;												//for R 2,4PWM
	}
	else
	{
		pstPwmCalc->stOut.uwTPWMDutyN = pstPwmCalc->stOut.wTPWMModulate;
		if(pstPwmCalc->stOut.wTPWMModulateOld >= 0)
		{
			pstPwmCalc->stOut.uwTPWMDutyN = 0;
		}

		pstPwmCalc->stOut.uwTPWMDutyN = -pstPwmCalc->stOut.uwTPWMDutyN;
		pstPwmCalc->stOut.uwTPWMDutyN += pstPwmCalc->stIn.uwDutyCompensation;				// duty offset
		UPDNLMT16(pstPwmCalc->stOut.uwTPWMDutyN, PWM_INV_DUTY_MAX, PWM_INV_DUTY_MIN);	//for R 2,4PWM

		pstPwmCalc->stOut.uwTPWMDutyP = 0;												//for R 1,3PWM
	}
	pstPwmCalc->stOut.wRPWMModulateOld = pstPwmCalc->stOut.wRPWMModulate;
	pstPwmCalc->stOut.wSPWMModulateOld = pstPwmCalc->stOut.wSPWMModulate;
	pstPwmCalc->stOut.wTPWMModulateOld = pstPwmCalc->stOut.wTPWMModulate;
}


//end of file


