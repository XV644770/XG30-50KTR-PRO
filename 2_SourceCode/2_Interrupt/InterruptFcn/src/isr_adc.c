/*=============================================================================*
 *  PRODUCT  : ADC Module 
 *
 *  FILENAME : Adc.c 
 *  PURPOSE  : ADC Module initialization configuration and basic data process..
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *----------------------------------------------------------------------------
 *============================================================================*/
#include "dsp_user_include.h"
#include "IQmathLib.h"

#if SAMPLE_DEBUG
#include "math.h"
int16 suwSin0Tmp,suwSin120Tmp,suwSin240Tmp;
#endif

/*=================================================================*
 * 	Local Variables declaration
 *=================================================================*/
/* AD result buffers for GeneralADC and SVPWM_ADC */
ST_ADC_POOL		stAdcPool;




/*=============================================================================*
 * FUNCTION: sSaveAdcBufZeroSample()
 * PURPOSE : to get sample when epwm zero int
 *			 To verify the channels are corresponding to hardware circuit.
 * INPUT: 
 * CALLED BY: isr_AdcDealZero()
 * 
 * 
 *============================================================================*/
#pragma CODE_SECTION(SaveAdcBufZeroSample,"ramfuncs");
void SaveAdcBufZeroSample(void)
{
	while((AdcaRegs.ADCCTL1.bit.ADCBSY == 1)
			||(AdcbRegs.ADCCTL1.bit.ADCBSY == 1)
			||(AdcdRegs.ADCCTL1.bit.ADCBSY == 1));

#if SAMPLE_DEBUG
	Uint16 uwGridFreqTmp = 5000;		// 52.50Hz-366-122 4550-422-141
	static int16 suwGridPrdCnt = 0;
	//int16 suwSin0Tmp,suwSin120Tmp,suwSin240Tmp;

	suwSin0Tmp = _IQ30sinPU((int32)uwGridFreqTmp*suwGridPrdCnt*559)>>20;	// 2*PI*F*t = 2*PI*52.5*cnt/19200
	suwSin120Tmp = _IQ30sinPU((int32)uwGridFreqTmp*(suwGridPrdCnt - 128)*559)>>20;	// +98->negtive segquence -98->postive segquence
	suwSin240Tmp = _IQ30sinPU((int32)uwGridFreqTmp*(suwGridPrdCnt + 128)*559)>>20;	// -98->negtive segquence +98->postive segquence
	suwGridPrdCnt++;
	if(suwGridPrdCnt >= 384)	//1920000/uwGridFreqTmp
	{
		suwGridPrdCnt = 0;
	}

	stAdcPool.RGridVolt.wZeroVal = (int16)((int32)VOLT_M_DEBUG*suwSin0Tmp>>10);
	stAdcPool.SGridVolt.wZeroVal = (int16)((int32)VOLT_M_DEBUG*suwSin120Tmp>>10);
	stAdcPool.TGridVolt.wZeroVal = (int16)((int32)VOLT_M_DEBUG*suwSin240Tmp>>10);

	stAdcPool.RInvVolt.wZeroVal = (int16)((int32)VOLT_M_DEBUG*suwSin0Tmp>>10);
	stAdcPool.SInvVolt.wZeroVal = (int16)((int32)VOLT_M_DEBUG*suwSin120Tmp>>10);
	stAdcPool.TInvVolt.wZeroVal = (int16)((int32)VOLT_M_DEBUG*suwSin240Tmp>>10);

	stAdcPool.RInvCurr.wZeroVal = (int16)((int32)CURR_M_DEBUG*suwSin0Tmp>>5);
	stAdcPool.SInvCurr.wZeroVal = (int16)((int32)CURR_M_DEBUG*suwSin120Tmp>>5);
	stAdcPool.TInvCurr.wZeroVal = (int16)((int32)CURR_M_DEBUG*suwSin240Tmp>>5);

	//stAdcPool.RHardDCI.wZeroVal = (int16)((int32)DCI_M_DEBUG*suwSin0Tmp>>6) + DCI_M_DEBUG>>1;
	//stAdcPool.SHardDCI.wZeroVal = (int16)((int32)DCI_M_DEBUG*suwSin120Tmp>>6) + DCI_M_DEBUG>>1;

	//stAdcPool.NPEVolt.wZeroVal = (int16)((int32)VNPE_M_DEBUG*suwSin0Tmp>>5);

	//stAdcPool.GFCI.wZeroVal = (int16)((int32)GFCI_M_DEBUG*suwSin0Tmp>>5);

	//stAdcPool.ISOVolt.wZeroVal = ISOVOLT_M_DEBUG;

	stAdcPool.PVVolt[PVA].wZeroVal = PVVOLT_M_DEBUG;
	stAdcPool.PVVolt[PVB].wZeroVal = PVVOLT_M_DEBUG;
	stAdcPool.PVVolt[PVC].wZeroVal = PVVOLT_M_DEBUG;
	stAdcPool.PVVolt[PVD].wZeroVal = PVVOLT_M_DEBUG;

	stAdcPool.BTCurr[PVA].wZeroVal = PVCURR_M_DEBUG-372;
	stAdcPool.BTCurr[PVB].wZeroVal = PVCURR_M_DEBUG-372;
	stAdcPool.BTCurr[PVC].wZeroVal = PVCURR_M_DEBUG-372;
	stAdcPool.BTCurr[PVD].wZeroVal = PVCURR_M_DEBUG-372;

	stAdcPool.NBusVolt.wPrdVal = 3*PVVOLT_M_DEBUG;
	stAdcPool.PBusVolt.wPrdVal = 3*PVVOLT_M_DEBUG;

#else	
	stAdcPool.RGridVolt.wZeroVal	= AdcaResultRegs.ADCRESULT0 - 2026;//2030;//2048;
	stAdcPool.SGridVolt.wZeroVal	= AdcaResultRegs.ADCRESULT1 - 2030;//2027;//2048;
	stAdcPool.TGridVolt.wZeroVal	= AdcaResultRegs.ADCRESULT2 - 2035;//2026;//2048;

	stAdcPool.RGridVolt.wArithVal = (int16)(((int32)(stAdcPool.RGridVolt.wZeroVal + stAdcPool.RGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);		// 1V-->32 AC_V=Vsample*305/2^5
	stAdcPool.SGridVolt.wArithVal = (int16)(((int32)(stAdcPool.SGridVolt.wZeroVal + stAdcPool.SGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);
	stAdcPool.TGridVolt.wArithVal = (int16)(((int32)(stAdcPool.TGridVolt.wZeroVal + stAdcPool.TGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);

	stAdcPool.RInvCurr.wZeroVal 	= AdcbResultRegs.ADCRESULT0 - 2048;
	stAdcPool.SInvCurr.wZeroVal 	= AdcbResultRegs.ADCRESULT1 - 2048;
	stAdcPool.TInvCurr.wZeroVal 	= AdcbResultRegs.ADCRESULT2 - 2048;

	// Only when the relay is checked can the inverter voltage be sampled
	if(((cRelayCheck == stWaitStatus.eStandbyStep)||(cFanInvtOpenCheck == stWaitStatus.eStandbyStep))
	&& (cWaitStatus == eInverterStatus))
	{
		stAdcPool.RInvVolt.wZeroVal = AdcaResultRegs.ADCRESULT3 - 2048;
		stAdcPool.SInvVolt.wZeroVal = AdcaResultRegs.ADCRESULT4 - 2048;
		stAdcPool.TInvVolt.wZeroVal = AdcaResultRegs.ADCRESULT5 - 2048;
	}

	// 2选1片选采样，分别对应0、1
	if(AD1_MUX_LEVEL == 0)
	{
		stAdcPool.PVVolt[PVA].wZeroVal = AdcbResultRegs.ADCRESULT3;
		stAdcPool.PVVolt[PVC].wZeroVal = AdcdResultRegs.ADCRESULT0;
		stAdcPool.BTCurr[PVA].wZeroVal = AdcdResultRegs.ADCRESULT1-372;	// Hard Baisc 0.054328V
		stAdcPool.BTCurr[PVC].wZeroVal = AdcdResultRegs.ADCRESULT2-372;	// Hard Baisc 0.054328V
	}
	else
	{
		stAdcPool.PVVolt[PVB].wZeroVal = AdcbResultRegs.ADCRESULT3;
		stAdcPool.PVVolt[PVD].wZeroVal = AdcdResultRegs.ADCRESULT0;
		stAdcPool.BTCurr[PVB].wZeroVal = AdcdResultRegs.ADCRESULT1-372;	// Hard Baisc 0.054328V
		stAdcPool.BTCurr[PVD].wZeroVal = AdcdResultRegs.ADCRESULT2-372;	// Hard Baisc 0.054328V
	}

	// 4选1片选，分别对应0、1、2、3
	switch(AD_SWITCH_4IN1)
	{
		case  0:
			stAdcPool.RHardDCI.wZeroVal = AdcdResultRegs.ADCRESULT3-2048;			// Hard Baisc 1.5V
			stAdcPool.GFCI.wZeroVal = AdcdResultRegs.ADCRESULT4-stSysCfg.wGFCIBias;	// Hard Baisc
		break;

		case  2:
			stAdcPool.SHardDCI.wZeroVal = AdcdResultRegs.ADCRESULT3-2048;			// Hard Baisc 1.5V
			stAdcPool.NPEVolt.wZeroVal	= AdcdResultRegs.ADCRESULT4-2048;			// Hard Baisc 1.5V
		break;

		default:	// 1 & 3
			stAdcPool.NBusVolt.wZeroVal = AdcdResultRegs.ADCRESULT3;
			stAdcPool.PBusVolt.wZeroVal = AdcdResultRegs.ADCRESULT4;
		break;
	}
#endif
}


/*=============================================================================*
 * FUNCTION: sSaveAdcBufPrdSample()
 * PURPOSE : to get sample when epwm prd int
 *			 To verify the channels are corresponding to hardware circuit.
 * INPUT:
 * CALLED BY: isr_AdcDealPrd()
 *
 *
 *============================================================================*/

#pragma CODE_SECTION(uwCheckOffset,"ramfuncs");
int16 uwCheckOffset(int16 wRelaValue)
{
	if((wRelaValue > SAMPLE_OFFSET_MAX) || (wRelaValue < -SAMPLE_OFFSET_MAX))
	{
		wRelaValue = 0;
	}
	return(wRelaValue);
}

#pragma CODE_SECTION(CalcAdcOffset,"ramfuncs");
void CalcAdcOffset(void)
{

	if((cWaitStatus == eInverterStatus)
	&&(0 == stTaskTimer.uwStartDelayTime)
	&&(0 == unSysFlag.bit.SampleOffsetFinished))
	{
		stAdcPool.uwAdcOffsetCalcCnt++;

		if(stAdcPool.uwAdcOffsetCalcCnt <= 10)
		{
			// clear R/S/T current zero offset
			stAdcPool.RInvCurr.wOffset = 0;
			stAdcPool.SInvCurr.wOffset = 0;
			stAdcPool.TInvCurr.wOffset = 0;
			stAdcPool.RInvCurr.dOffsetSumIsr = 0;
			stAdcPool.SInvCurr.dOffsetSumIsr = 0;
			stAdcPool.TInvCurr.dOffsetSumIsr = 0;

			// clear PV current zero offset
			stAdcPool.BTCurr[PVA].wOffset = 0;
			stAdcPool.BTCurr[PVB].wOffset = 0;
			stAdcPool.BTCurr[PVC].wOffset = 0;
			stAdcPool.BTCurr[PVD].wOffset = 0;

			stAdcPool.BTCurr[PVA].dOffsetSumIsr = 0;
			stAdcPool.BTCurr[PVB].dOffsetSumIsr = 0;
			stAdcPool.BTCurr[PVC].dOffsetSumIsr = 0;
			stAdcPool.BTCurr[PVD].dOffsetSumIsr = 0;
			
			// clear STR current zero offset
			stAdcPool.BTStrCurr[STR0].wOffset = 0;
			stAdcPool.BTStrCurr[STR1].wOffset = 0;
			stAdcPool.BTStrCurr[STR2].wOffset = 0;
			stAdcPool.BTStrCurr[STR3].wOffset = 0;
			stAdcPool.BTStrCurr[STR4].wOffset = 0;
			stAdcPool.BTStrCurr[STR5].wOffset = 0;
			stAdcPool.BTStrCurr[STR6].wOffset = 0;
			stAdcPool.BTStrCurr[STR7].wOffset = 0;

			stAdcPool.BTStrCurr[STR0].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR1].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR2].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR3].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR4].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR5].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR6].dOffsetSumIsr = 0;
			stAdcPool.BTStrCurr[STR7].dOffsetSumIsr = 0;

			// clear DCI zero offset
			stAdcPool.RHardDCI.wOffset = 0;
			stAdcPool.SHardDCI.wOffset = 0;
			stAdcPool.RHardDCI.dOffsetSumIsr = 0;
			stAdcPool.SHardDCI.dOffsetSumIsr = 0;
		}
		else
		{
			// calculate the sum of zero offset
			stAdcPool.RInvCurr.dOffsetSumIsr += stAdcPool.RInvCurr.wArithVal;
			stAdcPool.SInvCurr.dOffsetSumIsr += stAdcPool.SInvCurr.wArithVal;
			stAdcPool.TInvCurr.dOffsetSumIsr += stAdcPool.TInvCurr.wArithVal;

			stAdcPool.BTCurr[PVA].dOffsetSumIsr += stAdcPool.BTCurr[PVA].wArithVal;
			stAdcPool.BTCurr[PVB].dOffsetSumIsr += stAdcPool.BTCurr[PVB].wArithVal;
			stAdcPool.BTCurr[PVC].dOffsetSumIsr += stAdcPool.BTCurr[PVC].wArithVal;
			stAdcPool.BTCurr[PVD].dOffsetSumIsr += stAdcPool.BTCurr[PVD].wArithVal;
			
			stAdcPool.BTStrCurr[STR0].dOffsetSumIsr += stAdcPool.BTStrCurr[STR0].wArithVal;
			stAdcPool.BTStrCurr[STR1].dOffsetSumIsr += stAdcPool.BTStrCurr[STR1].wArithVal;
			stAdcPool.BTStrCurr[STR2].dOffsetSumIsr += stAdcPool.BTStrCurr[STR2].wArithVal;
			stAdcPool.BTStrCurr[STR3].dOffsetSumIsr += stAdcPool.BTStrCurr[STR3].wArithVal;
			stAdcPool.BTStrCurr[STR4].dOffsetSumIsr += stAdcPool.BTStrCurr[STR4].wArithVal;
			stAdcPool.BTStrCurr[STR5].dOffsetSumIsr += stAdcPool.BTStrCurr[STR5].wArithVal;
			stAdcPool.BTStrCurr[STR6].dOffsetSumIsr += stAdcPool.BTStrCurr[STR6].wArithVal;
			stAdcPool.BTStrCurr[STR7].dOffsetSumIsr += stAdcPool.BTStrCurr[STR7].wArithVal;

			stAdcPool.RHardDCI.dOffsetSumIsr += stAdcPool.RHardDCI.wOffset;
			stAdcPool.SHardDCI.dOffsetSumIsr += stAdcPool.SHardDCI.wOffset;

			if(stAdcPool.uwAdcOffsetCalcCnt >= 74)		// 74 - 10 = 64
			{
				stAdcPool.uwAdcOffsetCalcCnt = 0;

				stAdcPool.BTCurr[PVA].wOffset = (stAdcPool.BTCurr[PVA].dOffsetSumIsr>>6);
				stAdcPool.BTCurr[PVB].wOffset = (stAdcPool.BTCurr[PVB].dOffsetSumIsr>>6);
				stAdcPool.BTCurr[PVC].wOffset = (stAdcPool.BTCurr[PVC].dOffsetSumIsr>>6);
				stAdcPool.BTCurr[PVD].wOffset = (stAdcPool.BTCurr[PVD].dOffsetSumIsr>>6);

				stAdcPool.BTCurr[PVA].wOffset = uwCheckOffset(stAdcPool.BTCurr[PVA].wOffset);
				stAdcPool.BTCurr[PVB].wOffset = uwCheckOffset(stAdcPool.BTCurr[PVB].wOffset);
				stAdcPool.BTCurr[PVC].wOffset = uwCheckOffset(stAdcPool.BTCurr[PVC].wOffset);
				stAdcPool.BTCurr[PVD].wOffset = uwCheckOffset(stAdcPool.BTCurr[PVD].wOffset);

				stAdcPool.BTCurr[PVA].dOffsetSumIsr = 0;
				stAdcPool.BTCurr[PVB].dOffsetSumIsr = 0;
				stAdcPool.BTCurr[PVC].dOffsetSumIsr = 0;
				stAdcPool.BTCurr[PVD].dOffsetSumIsr = 0;

				stAdcPool.BTStrCurr[STR0].wOffset = (stAdcPool.BTStrCurr[STR0].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR1].wOffset = (stAdcPool.BTStrCurr[STR1].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR2].wOffset = (stAdcPool.BTStrCurr[STR2].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR3].wOffset = (stAdcPool.BTStrCurr[STR3].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR4].wOffset = (stAdcPool.BTStrCurr[STR4].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR5].wOffset = (stAdcPool.BTStrCurr[STR5].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR6].wOffset = (stAdcPool.BTStrCurr[STR6].dOffsetSumIsr>>6);
				stAdcPool.BTStrCurr[STR7].wOffset = (stAdcPool.BTStrCurr[STR7].dOffsetSumIsr>>6);

				stAdcPool.BTStrCurr[STR0].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR0].wOffset);
				stAdcPool.BTStrCurr[STR1].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR1].wOffset);
				stAdcPool.BTStrCurr[STR2].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR2].wOffset);
				stAdcPool.BTStrCurr[STR3].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR3].wOffset);
				stAdcPool.BTStrCurr[STR4].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR4].wOffset);
				stAdcPool.BTStrCurr[STR5].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR5].wOffset);
				stAdcPool.BTStrCurr[STR6].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR6].wOffset);
				stAdcPool.BTStrCurr[STR7].wOffset = uwCheckOffset(stAdcPool.BTStrCurr[STR7].wOffset);
				
				stAdcPool.BTStrCurr[STR0].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR1].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR2].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR3].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR4].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR5].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR6].dOffsetSumIsr = 0;
				stAdcPool.BTStrCurr[STR7].dOffsetSumIsr = 0;
				
				stAdcPool.RInvCurr.wOffset = (stAdcPool.RInvCurr.dOffsetSumIsr>>6);
				stAdcPool.SInvCurr.wOffset = (stAdcPool.SInvCurr.dOffsetSumIsr>>6);
				stAdcPool.TInvCurr.wOffset = (stAdcPool.TInvCurr.dOffsetSumIsr>>6);

				stAdcPool.RInvCurr.wOffset = uwCheckOffset(stAdcPool.RInvCurr.wOffset);
				stAdcPool.SInvCurr.wOffset = uwCheckOffset(stAdcPool.SInvCurr.wOffset);
				stAdcPool.TInvCurr.wOffset = uwCheckOffset(stAdcPool.TInvCurr.wOffset);

				stAdcPool.RInvCurr.dOffsetSumIsr = 0;
				stAdcPool.SInvCurr.dOffsetSumIsr = 0;
				stAdcPool.TInvCurr.dOffsetSumIsr = 0;

				stAdcPool.RHardDCI.wOffset = (stAdcPool.RHardDCI.dOffsetSumIsr>>6);
				stAdcPool.SHardDCI.wOffset = (stAdcPool.SHardDCI.dOffsetSumIsr>>6);
				stAdcPool.RHardDCI.wOffset = uwCheckOffset(stAdcPool.RHardDCI.wOffset);
				stAdcPool.SHardDCI.wOffset = uwCheckOffset(stAdcPool.SHardDCI.wOffset);

				stAdcPool.RHardDCI.dOffsetSumIsr = 0;
				stAdcPool.SHardDCI.dOffsetSumIsr = 0;

				unSysFlag.bit.SampleOffsetFinished = 1;
			}
		}
	}
	else
	{
		stAdcPool.uwAdcOffsetCalcCnt = 0;
	}
}

/*=============================================================================*
 * FUNCTION: CalcAdcRealValue()
 * PURPOSE : General purpose ADC Sample Result Get and Scaled to the real value,
 *			 each channels are self-explaination.
 * INPUT:
 * CALLED BY: isr_AdcDealZero()
 *
 *
 *============================================================================*/




/*=============================================================================*
 * FUNCTION: CalcAdcRealValueSum()
 * PURPOSE : 1.calculate the ac sample value sum of squares for cal rms
 *			 2.calculate the dc sample value sum for cal average value
 * INPUT: 
 * CALLED BY: isr_AdcDealPrd()
 * 
 * 
 *============================================================================*/
#pragma CODE_SECTION(CalcAdcRealValueSum,"ramfuncs");
void CalcAdcRealValueSum(void)
{		
	static Uint16 suwSumCntTmp=0;
	static Uint16 suwMux2Ch0SumCntTmp=0,	suwMux2Ch1SumCntTmp=0;
	static Uint16 suwMux4Ch0SumCntTmp=0,	suwMux4Ch2SumCntTmp=0,	suwMux4Ch3SumCntTmp=0;
	static Uint16 suwMux8Ch0SumCntTmp=0,	suwMux8Ch1SumCntTmp=0,	suwMux8Ch2SumCntTmp=0,	suwMux8Ch3SumCntTmp=0;
	static Uint16 suwMux8Ch4SumCntTmp=0,	suwMux8Ch5SumCntTmp=0,	suwMux8Ch6SumCntTmp=0,	suwMux8Ch7SumCntTmp=0;
	static Uint16 suwGridZeroIsrCnt=0;
	Uint16 uwRSTVoltZeroTmp;
	// clac Q
	Uint16 uwIdx;
	int16  wRVoltDelayed, wSVoltDelayed, wTVoltDelayed;
	int16  wRCapCurr, wSCapCurr, wTCapCurr;


	if(stPllPara.stOut.uwGridPrdCnt == 0)
	{
		stAdcPool.uwSumCnt = suwSumCntTmp;
		suwSumCntTmp = 0;

		// AC Phase Voltage
		stAdcPool.RGridVolt.dSquareAddSum = stAdcPool.RGridVolt.dSquareAddSumIsr;
		stAdcPool.SGridVolt.dSquareAddSum = stAdcPool.SGridVolt.dSquareAddSumIsr;
		stAdcPool.TGridVolt.dSquareAddSum = stAdcPool.TGridVolt.dSquareAddSumIsr;
		// AC Line Voltage
		stAdcPool.RSGridVolt.dSquareAddSum = stAdcPool.RSGridVolt.dSquareAddSumIsr;
		stAdcPool.STGridVolt.dSquareAddSum = stAdcPool.STGridVolt.dSquareAddSumIsr;
		stAdcPool.TRGridVolt.dSquareAddSum = stAdcPool.TRGridVolt.dSquareAddSumIsr;
		// AC RST Phase Voltage Sum
		stAdcPool.RSTGridVolt.dSquareAddSum = stAdcPool.RSTGridVolt.dSquareAddSumIsr;

		stAdcPool.RGridVolt.dSquareAddSumIsr = 0;
		stAdcPool.SGridVolt.dSquareAddSumIsr = 0;
		stAdcPool.TGridVolt.dSquareAddSumIsr = 0;

		stAdcPool.RSGridVolt.dSquareAddSumIsr = 0;
		stAdcPool.STGridVolt.dSquareAddSumIsr = 0;
		stAdcPool.TRGridVolt.dSquareAddSumIsr = 0;

		stAdcPool.RSTGridVolt.dSquareAddSumIsr = 0;

		if(((cRelayCheck == stWaitStatus.eStandbyStep)||(cFanInvtOpenCheck == stWaitStatus.eStandbyStep))
			&& (cWaitStatus == eInverterStatus))
		{
			stAdcPool.RInvVolt.dSquareAddSum = stAdcPool.RInvVolt.dSquareAddSumIsr;
			stAdcPool.SInvVolt.dSquareAddSum = stAdcPool.SInvVolt.dSquareAddSumIsr;
			stAdcPool.TInvVolt.dSquareAddSum = stAdcPool.TInvVolt.dSquareAddSumIsr;

			stAdcPool.RRlyVolt.dSquareAddSum = stAdcPool.RRlyVolt.dSquareAddSumIsr;
			stAdcPool.SRlyVolt.dSquareAddSum = stAdcPool.SRlyVolt.dSquareAddSumIsr;
			stAdcPool.TRlyVolt.dSquareAddSum = stAdcPool.TRlyVolt.dSquareAddSumIsr;

			stAdcPool.RSRlyVolt.dSquareAddSum = stAdcPool.RSRlyVolt.dSquareAddSumIsr;
			stAdcPool.STRlyVolt.dSquareAddSum = stAdcPool.STRlyVolt.dSquareAddSumIsr;
			stAdcPool.TRRlyVolt.dSquareAddSum = stAdcPool.TRRlyVolt.dSquareAddSumIsr;
			

			stAdcPool.RInvVolt.dAddSum = stAdcPool.RInvVolt.dAddSumIsr;
			stAdcPool.SInvVolt.dAddSum = stAdcPool.SInvVolt.dAddSumIsr;
			stAdcPool.TInvVolt.dAddSum = stAdcPool.TInvVolt.dAddSumIsr;

			stAdcPool.RInvVolt.dSquareAddSumIsr = 0;
			stAdcPool.SInvVolt.dSquareAddSumIsr = 0;
			stAdcPool.TInvVolt.dSquareAddSumIsr = 0;

			stAdcPool.RRlyVolt.dSquareAddSumIsr = 0;
			stAdcPool.SRlyVolt.dSquareAddSumIsr = 0;
			stAdcPool.TRlyVolt.dSquareAddSumIsr = 0;

			stAdcPool.RSRlyVolt.dSquareAddSumIsr = 0;
			stAdcPool.STRlyVolt.dSquareAddSumIsr = 0;
			stAdcPool.TRRlyVolt.dSquareAddSumIsr = 0;

			stAdcPool.RInvVolt.dAddSumIsr = 0;
			stAdcPool.SInvVolt.dAddSumIsr = 0;
			stAdcPool.TInvVolt.dAddSumIsr = 0;
		}
		else
		{
			// AC Sofrware DCI
			stAdcPool.RSoftDCI.dAddSum = stAdcPool.RSoftDCI.dAddSumIsr;
			stAdcPool.SSoftDCI.dAddSum = stAdcPool.SSoftDCI.dAddSumIsr;
			//stAdcPool.TSoftDCI.dAddSum = stAdcPool.TSoftDCI.dAddSumIsr;
			stAdcPool.RSoftDCI.dAddSumIsr = 0;
			stAdcPool.SSoftDCI.dAddSumIsr = 0;
			//stAdcPool.TSoftDCI.dAddSumIsr = 0;

			// Active Power Sum
			stAdcPool.ActivePower.dAddSum = stAdcPool.ActivePower.dAddSumIsr;
			stAdcPool.ActivePower.dAddSumIsr = 0;

			// Reactive Power Sum (1/4T delay method)
			stAdcPool.ReactivePower.dAddSum = stAdcPool.ReactivePower.dAddSumIsr;
			stAdcPool.ReactivePower.dAddSumIsr = 0;

			// GFCI Average 
			stAdcPool.GFCI.dAddSum = stAdcPool.GFCI.dAddSumIsr;
			stAdcPool.GFCI.dAddSumIsr = 0;
		}
		// AC Inv Current Square Sum
		stAdcPool.RInvCurr.dSquareAddSum = stAdcPool.RInvCurr.dSquareAddSumIsr;
		stAdcPool.SInvCurr.dSquareAddSum = stAdcPool.SInvCurr.dSquareAddSumIsr;
		stAdcPool.TInvCurr.dSquareAddSum = stAdcPool.TInvCurr.dSquareAddSumIsr;

		stAdcPool.RInvCurr.dSquareAddSumIsr = 0;
		stAdcPool.SInvCurr.dSquareAddSumIsr = 0;
		stAdcPool.TInvCurr.dSquareAddSumIsr = 0;

		suwGridZeroIsrCnt++;
		if(suwGridZeroIsrCnt>=4)
		{
			suwGridZeroIsrCnt = 0;
		}
		// 4选1片选，分别对应0、1、2、3
		switch(suwGridZeroIsrCnt)
		{
			case 0:
				/*********Mux2-Channel0***************PV Voltage Current Calculate******************/
				stAdcPool.uwMux2Ch0SumCnt = suwMux2Ch0SumCntTmp;
				suwMux2Ch0SumCntTmp = 0;

				// PV Voltage
				stAdcPool.PVVolt[PVA].dAddSum = stAdcPool.PVVolt[PVA].dAddSumIsr;
				stAdcPool.PVVolt[PVC].dAddSum = stAdcPool.PVVolt[PVC].dAddSumIsr;
				// BOOST Current
				stAdcPool.BTCurr[PVA].dAddSum = stAdcPool.BTCurr[PVA].dAddSumIsr;
				stAdcPool.BTCurr[PVC].dAddSum = stAdcPool.BTCurr[PVC].dAddSumIsr;

				stAdcPool.PVVolt[PVA].dAddSumIsr = 0;
				stAdcPool.PVVolt[PVC].dAddSumIsr = 0;
				stAdcPool.BTCurr[PVA].dAddSumIsr = 0;
				stAdcPool.BTCurr[PVC].dAddSumIsr = 0;

				/*********Mux4-Channel0*****************************************************/
				stAdcPool.uwMux4Ch0SumCnt = suwMux4Ch0SumCntTmp;
				suwMux4Ch0SumCntTmp = 0;

				stAdcPool.RHardDCI.dAddSum = stAdcPool.RHardDCI.dAddSumIsr;
				stAdcPool.GFCI.dSquareAddSum = stAdcPool.GFCI.dSquareAddSumIsr;

				stAdcPool.RHardDCI.dAddSumIsr = 0;
				stAdcPool.GFCI.dSquareAddSumIsr = 0;
			break;

			case 1:
				/*********Mux2-Channel1*****************************************************/
				stAdcPool.uwMux2Ch1SumCnt = suwMux2Ch1SumCntTmp;
				suwMux2Ch1SumCntTmp = 0;

				// PV Voltage
				stAdcPool.PVVolt[PVB].dAddSum = stAdcPool.PVVolt[PVB].dAddSumIsr;
				stAdcPool.PVVolt[PVD].dAddSum = stAdcPool.PVVolt[PVD].dAddSumIsr;
				// BOOST Current
				stAdcPool.BTCurr[PVB].dAddSum = stAdcPool.BTCurr[PVB].dAddSumIsr;
				stAdcPool.BTCurr[PVD].dAddSum = stAdcPool.BTCurr[PVD].dAddSumIsr;

				stAdcPool.PVVolt[PVB].dAddSumIsr = 0;
				stAdcPool.PVVolt[PVD].dAddSumIsr = 0;
				stAdcPool.BTCurr[PVB].dAddSumIsr = 0;
				stAdcPool.BTCurr[PVD].dAddSumIsr = 0;

			break;

		    case 2:
		        /*********Mux4-Channel0*****************************************************/
		        stAdcPool.uwMux4Ch2SumCnt = suwMux4Ch2SumCntTmp;
		        suwMux4Ch2SumCntTmp = 0;

		        stAdcPool.SHardDCI.dAddSum = stAdcPool.SHardDCI.dAddSumIsr;
		        stAdcPool.SHardDCI.dAddSumIsr = 0;

		        stAdcPool.NPEVolt.dSquareAddSum = stAdcPool.NPEVolt.dSquareAddSumIsr;
		        stAdcPool.NPEVolt.dSquareAddSumIsr = 0;

		        /*********Mux8-Channel0*****************************************************/
		        stAdcPool.uwMux8Ch0SumCnt = suwMux8Ch0SumCntTmp;
		        suwMux8Ch0SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR0].dAddSum = stAdcPool.BTStrCurr[STR0].dAddSumIsr;

		        stAdcPool.BTStrCurr[STR0].dAddSumIsr = 0;
		        /*********Mux8-Channel1*****************************************************/
		        stAdcPool.uwMux8Ch1SumCnt = suwMux8Ch1SumCntTmp;
		        suwMux8Ch1SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR1].dAddSum = stAdcPool.BTStrCurr[STR1].dAddSumIsr;

		        stAdcPool.BTStrCurr[STR1].dAddSumIsr = 0;
		        /*********Mux8-Channel2*****************************************************/
		        stAdcPool.uwMux8Ch2SumCnt = suwMux8Ch2SumCntTmp;
		        suwMux8Ch2SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR2].dAddSum = stAdcPool.BTStrCurr[STR2].dAddSumIsr;

		        stAdcPool.BTStrCurr[STR2].dAddSumIsr = 0;
		        /*********Mux8-Channel3*****************************************************/
		        stAdcPool.uwMux8Ch3SumCnt = suwMux8Ch3SumCntTmp;
		        suwMux8Ch3SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR3].dAddSum = stAdcPool.BTStrCurr[STR3].dAddSumIsr;
		        stAdcPool.ISOVolt.dAddSum = stAdcPool.ISOVolt.dAddSumIsr;

		        stAdcPool.BTStrCurr[STR3].dAddSumIsr = 0;
		        stAdcPool.ISOVolt.dAddSumIsr = 0;
		        break;

		    default:
		        /*********Mux3-Channel0*****************************************************/
		        stAdcPool.uwMux4Ch3SumCnt = suwMux4Ch3SumCntTmp;
		        suwMux4Ch3SumCntTmp = 0;

		        stAdcPool.PBusVolt.dAddSum  = stAdcPool.PBusVolt.dAddSumIsr;
		        stAdcPool.NBusVolt.dAddSum  = stAdcPool.NBusVolt.dAddSumIsr;

		        stAdcPool.PBusVolt.dAddSumIsr = 0;
		        stAdcPool.NBusVolt.dAddSumIsr = 0;
		        /*********Mux8-Channel4*****************************************************/
		        stAdcPool.uwMux8Ch4SumCnt = suwMux8Ch4SumCntTmp;
		        suwMux8Ch4SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR4].dAddSum = stAdcPool.BTStrCurr[STR4].dAddSumIsr;
		        stAdcPool.BTStrCurr[STR4].dAddSumIsr = 0;
		        /*********Mux8-Channel5*****************************************************/
		        stAdcPool.uwMux8Ch5SumCnt = suwMux8Ch5SumCntTmp;
		        suwMux8Ch5SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR5].dAddSum = stAdcPool.BTStrCurr[STR5].dAddSumIsr;
		        stAdcPool.BTStrCurr[STR5].dAddSumIsr = 0;
		        /*********Mux8-Channel6*****************************************************/
		        stAdcPool.uwMux8Ch6SumCnt = suwMux8Ch6SumCntTmp;
		        suwMux8Ch6SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR6].dAddSum = stAdcPool.BTStrCurr[STR6].dAddSumIsr;
		        stAdcPool.BTStrCurr[STR6].dAddSumIsr = 0;
		        /*********Mux8-Channel7*****************************************************/
		        stAdcPool.uwMux8Ch7SumCnt = suwMux8Ch7SumCntTmp;
		        suwMux8Ch7SumCntTmp = 0;
		        stAdcPool.BTStrCurr[STR7].dAddSum = stAdcPool.BTStrCurr[STR7].dAddSumIsr;
		        stAdcPool.BTStrCurr[STR7].dAddSumIsr = 0;
		        break;
	}

		OSISREventSend(PV_INPUT_PRIO,PVINPUT_GRID_ZERO);
		OSISREventSend(AC_OUTPUT_PRIO,ACOUTPUT_GRID_ZERO);
	}

	while((AdcaRegs.ADCCTL1.bit.ADCBSY == 1)
				||(AdcbRegs.ADCCTL1.bit.ADCBSY == 1)
				||(AdcdRegs.ADCCTL1.bit.ADCBSY == 1));

	/*************************** GRID Phase Voltage***********************************/
	stAdcPool.RGridVolt.wPrdVal = AdcaResultRegs.ADCRESULT0 - 2026;//2030;//2048;
	stAdcPool.SGridVolt.wPrdVal = AdcaResultRegs.ADCRESULT1 - 2030;//2027;//2048;
	stAdcPool.TGridVolt.wPrdVal = AdcaResultRegs.ADCRESULT2 - 2035;//2026;//2048;

	stAdcPool.RGridVolt.wArithVal = (int16)(((int32)(stAdcPool.RGridVolt.wZeroVal + stAdcPool.RGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);		// 1V-->32 AC_V=Vsample*305/2^5
	stAdcPool.SGridVolt.wArithVal = (int16)(((int32)(stAdcPool.SGridVolt.wZeroVal + stAdcPool.SGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);
	stAdcPool.TGridVolt.wArithVal = (int16)(((int32)(stAdcPool.TGridVolt.wZeroVal + stAdcPool.TGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);

	if(1 == stF107Data.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// Enable Soft N Line
	{
		uwRSTVoltZeroTmp = ((int32)(stAdcPool.RGridVolt.wArithVal + stAdcPool.SGridVolt.wArithVal + stAdcPool.TGridVolt.wArithVal)*341>>10);	// (Va+Vb+Vc)/3
		stAdcPool.RGridVolt.wArithVal	= stAdcPool.RGridVolt.wArithVal - uwRSTVoltZeroTmp;
		stAdcPool.SGridVolt.wArithVal	= stAdcPool.SGridVolt.wArithVal - uwRSTVoltZeroTmp;
		stAdcPool.TGridVolt.wArithVal	= stAdcPool.TGridVolt.wArithVal - uwRSTVoltZeroTmp;
	}

	suwSumCntTmp++;

	// AC Phase Voltage Square Sum
	stAdcPool.RGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.RGridVolt.wArithVal * stAdcPool.RGridVolt.wArithVal>>6);		// (2^5 * 2^5) / 2^6 = 2^4
	stAdcPool.SGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.SGridVolt.wArithVal * stAdcPool.SGridVolt.wArithVal>>6);
	stAdcPool.TGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.TGridVolt.wArithVal * stAdcPool.TGridVolt.wArithVal>>6);

	/********************* Three Phase Grid Voltage Sum*****************************/
	stAdcPool.RSTGridVolt.wArithVal = (stAdcPool.RGridVolt.wArithVal+stAdcPool.SGridVolt.wArithVal+stAdcPool.TGridVolt.wArithVal);
	// AC RST Phase Grid Voltage Squarre Sum
	stAdcPool.RSTGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.RSTGridVolt.wArithVal*stAdcPool.RSTGridVolt.wArithVal>>6);	// (2^5 * 2^5) / 2^6 = 2^4

	/********************* GRID Line Voltage*************************************/
	stAdcPool.RSGridVolt.wArithVal = stAdcPool.RGridVolt.wArithVal - stAdcPool.SGridVolt.wArithVal;
	stAdcPool.STGridVolt.wArithVal = stAdcPool.SGridVolt.wArithVal - stAdcPool.TGridVolt.wArithVal;
	stAdcPool.TRGridVolt.wArithVal = stAdcPool.TGridVolt.wArithVal - stAdcPool.RGridVolt.wArithVal;
	// AC Line Voltage Suqare Sum
	stAdcPool.RSGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.RSGridVolt.wArithVal * stAdcPool.RSGridVolt.wArithVal>>6);	// (2^5 * 2^5) / 2^6 = 2^4
	stAdcPool.STGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.STGridVolt.wArithVal * stAdcPool.STGridVolt.wArithVal>>6);
	stAdcPool.TRGridVolt.dSquareAddSumIsr += ((int32)stAdcPool.TRGridVolt.wArithVal * stAdcPool.TRGridVolt.wArithVal>>6);
	
	/********************* GRID Inv Current*************************************/
	stAdcPool.RInvCurr.wPrdVal  = AdcbResultRegs.ADCRESULT0 - 2048;
	stAdcPool.SInvCurr.wPrdVal  = AdcbResultRegs.ADCRESULT1 - 2048;
	stAdcPool.TInvCurr.wPrdVal  = AdcbResultRegs.ADCRESULT2 - 2048;

	// AC Inv Current Calculate
	stAdcPool.RInvCurr.wArithVal = (int16)(((int32)(stAdcPool.RInvCurr.wZeroVal + stAdcPool.RInvCurr.wPrdVal)*stSysCfg.wACCurrRatio)>>10)-stAdcPool.RInvCurr.wOffset;		// 1A-->32 AC_I=Vsample*1083/2^9
	stAdcPool.SInvCurr.wArithVal = (int16)(((int32)(stAdcPool.SInvCurr.wZeroVal + stAdcPool.SInvCurr.wPrdVal)*stSysCfg.wACCurrRatio)>>10)-stAdcPool.SInvCurr.wOffset;
	stAdcPool.TInvCurr.wArithVal = (int16)(((int32)(stAdcPool.TInvCurr.wZeroVal + stAdcPool.TInvCurr.wPrdVal)*stSysCfg.wACCurrRatio)>>10)-stAdcPool.TInvCurr.wOffset;

	// AC Inv Current Square Calculate
	stAdcPool.RInvCurr.dSquareAddSumIsr += ((int32)stAdcPool.RInvCurr.wArithVal * stAdcPool.RInvCurr.wArithVal>>4);			// (2^5 * 2^5) / 2^4 = 2^6
	stAdcPool.SInvCurr.dSquareAddSumIsr += ((int32)stAdcPool.SInvCurr.wArithVal * stAdcPool.SInvCurr.wArithVal>>4);
	stAdcPool.TInvCurr.dSquareAddSumIsr += ((int32)stAdcPool.TInvCurr.wArithVal * stAdcPool.TInvCurr.wArithVal>>4);
	
	/********************* RST Inv Voltage*************************************/
	// Only when the relay is checked can the inverter voltage be sampled
	if(((cRelayCheck == stWaitStatus.eStandbyStep)||(cFanInvtOpenCheck == stWaitStatus.eStandbyStep))
	&& (cWaitStatus == eInverterStatus))
	{
		stAdcPool.RInvVolt.wPrdVal = AdcaResultRegs.ADCRESULT3 - 2048;
		stAdcPool.SInvVolt.wPrdVal = AdcaResultRegs.ADCRESULT4 - 2048;
		stAdcPool.TInvVolt.wPrdVal = AdcaResultRegs.ADCRESULT5 - 2048;
		
		//AC  Inv Voltage Calculate
		stAdcPool.RInvVolt.wArithVal = (int16)(((int32)(stAdcPool.RInvVolt.wZeroVal + stAdcPool.RInvVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);
		stAdcPool.SInvVolt.wArithVal = (int16)(((int32)(stAdcPool.SInvVolt.wZeroVal + stAdcPool.SInvVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);
		stAdcPool.TInvVolt.wArithVal = (int16)(((int32)(stAdcPool.TInvVolt.wZeroVal + stAdcPool.TInvVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>6);

		if(1 == stF107Data.stProtectEnvrmt.unEnvrmtBit.bit.Neutral)		// Enable Soft N Line
		{
			uwRSTVoltZeroTmp = ((int32)(stAdcPool.RInvVolt.wArithVal + stAdcPool.SInvVolt.wArithVal + stAdcPool.TInvVolt.wArithVal)*341>>10);	// (Va+Vb+Vc)/3
			// inv phase volt
			stAdcPool.RInvVolt.wArithVal	= stAdcPool.RInvVolt.wArithVal - uwRSTVoltZeroTmp;
			stAdcPool.SInvVolt.wArithVal	= stAdcPool.SInvVolt.wArithVal - uwRSTVoltZeroTmp;
			stAdcPool.TInvVolt.wArithVal	= stAdcPool.TInvVolt.wArithVal - uwRSTVoltZeroTmp;
		}
		//AC  Inv Voltage Square Sum Calculate
		stAdcPool.RInvVolt.dSquareAddSumIsr += ((int32)stAdcPool.RInvVolt.wArithVal*stAdcPool.RInvVolt.wArithVal>>6);
		stAdcPool.SInvVolt.dSquareAddSumIsr += ((int32)stAdcPool.SInvVolt.wArithVal*stAdcPool.SInvVolt.wArithVal>>6);
		stAdcPool.TInvVolt.dSquareAddSumIsr += ((int32)stAdcPool.TInvVolt.wArithVal*stAdcPool.TInvVolt.wArithVal>>6);
		//AC  Inv Voltage Add Sum Calculate
#if OLD_INV_VOLT_SAMPLE

		stAdcPool.RInvVolt.dAddSumIsr += (int32)stAdcPool.RInvVolt.wArithVal;
		stAdcPool.SInvVolt.dAddSumIsr += (int32)stAdcPool.SInvVolt.wArithVal;
		stAdcPool.TInvVolt.dAddSumIsr += (int32)stAdcPool.TInvVolt.wArithVal;
		/*********************************** AC RST Relay Voltage*************************/
		// relay voltage = variance
		stAdcPool.RRlyVolt.wArithVal = stAdcPool.RInvVolt.wArithVal - stACSample.PhaseInvVoltAvg.wRN - stAdcPool.RGridVolt.wArithVal;
		stAdcPool.SRlyVolt.wArithVal = stAdcPool.SInvVolt.wArithVal - stACSample.PhaseInvVoltAvg.wSN - stAdcPool.SGridVolt.wArithVal;
		stAdcPool.TRlyVolt.wArithVal = stAdcPool.TInvVolt.wArithVal - stACSample.PhaseInvVoltAvg.wTN - stAdcPool.TGridVolt.wArithVal;
#else
		/*********************************** AC RST Relay Voltage*************************/
		stAdcPool.RRlyVolt.wArithVal = stAdcPool.RInvVolt.wArithVal - stAdcPool.RGridVolt.wArithVal;
		stAdcPool.SRlyVolt.wArithVal = stAdcPool.SInvVolt.wArithVal - stAdcPool.SGridVolt.wArithVal;
		stAdcPool.TRlyVolt.wArithVal = stAdcPool.TInvVolt.wArithVal - stAdcPool.TGridVolt.wArithVal;

	////test begin

		stAdcPool.RSInvVolt.wArithVal = stAdcPool.RInvVolt.wArithVal - stAdcPool.SInvVolt.wArithVal;
		stAdcPool.STInvVolt.wArithVal = stAdcPool.SInvVolt.wArithVal - stAdcPool.TInvVolt.wArithVal;
		stAdcPool.TRInvVolt.wArithVal = stAdcPool.TInvVolt.wArithVal - stAdcPool.RInvVolt.wArithVal;

		stAdcPool.RSRlyVolt.wArithVal = stAdcPool.RSInvVolt.wArithVal - stAdcPool.RSGridVolt.wArithVal;
		stAdcPool.STRlyVolt.wArithVal = stAdcPool.STInvVolt.wArithVal - stAdcPool.STGridVolt.wArithVal;
		stAdcPool.TRRlyVolt.wArithVal = stAdcPool.TRInvVolt.wArithVal - stAdcPool.TRGridVolt.wArithVal;
	
		stDebug.ReadData.wDebug8 = (((int32)(0 + stAdcPool.TGridVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>5);
		//stDebug.ReadData.wDebug1 =(((int32)(stAdcPool.TGridVolt.wZeroVal + 0)*stSysCfg.wACVoltRatio)>>5);
		//stDebug.ReadData.wDebug2 =(((int32)(0+ stAdcPool.TInvVolt.wPrdVal)*stSysCfg.wACVoltRatio)>>5);
		stDebug.ReadData.wDebug3 = (((int32)(stAdcPool.TInvVolt.wZeroVal + 0)*stSysCfg.wACVoltRatio)>>5);


	/////test end
#endif
		// AC RST Relay Voltage Square Sum Calculate
		stAdcPool.RRlyVolt.dSquareAddSumIsr += ((int32)stAdcPool.RRlyVolt.wArithVal*stAdcPool.RRlyVolt.wArithVal>>6);
		stAdcPool.SRlyVolt.dSquareAddSumIsr += ((int32)stAdcPool.SRlyVolt.wArithVal*stAdcPool.SRlyVolt.wArithVal>>6);
		stAdcPool.TRlyVolt.dSquareAddSumIsr += ((int32)stAdcPool.TRlyVolt.wArithVal*stAdcPool.TRlyVolt.wArithVal>>6);

		stAdcPool.RSRlyVolt.dSquareAddSumIsr += ((int32)stAdcPool.RSRlyVolt.wArithVal*stAdcPool.RSRlyVolt.wArithVal>>6);
		stAdcPool.STRlyVolt.dSquareAddSumIsr += ((int32)stAdcPool.STRlyVolt.wArithVal*stAdcPool.STRlyVolt.wArithVal>>6);
		stAdcPool.TRRlyVolt.dSquareAddSumIsr += ((int32)stAdcPool.TRRlyVolt.wArithVal*stAdcPool.TRRlyVolt.wArithVal>>6);
	}
	else
	{
		// Active Power Calculate
		stAdcPool.ActivePower.dOffsetSumIsr = (((int32)stAdcPool.RGridVolt.wArithVal * stAdcPool.RInvCurr.wArithVal>>6)			// (2^5 * 2^5) / 2^6 = 2^4
											  +((int32)stAdcPool.SGridVolt.wArithVal * stAdcPool.SInvCurr.wArithVal>>6)
											  +((int32)stAdcPool.TGridVolt.wArithVal * stAdcPool.TInvCurr.wArithVal>>6));
		// Active Power Sum Calculate
		stAdcPool.ActivePower.dAddSumIsr += stAdcPool.ActivePower.dOffsetSumIsr;

		// Reactive Power Calculate (1/4T delay method)
		// Q = 1/T * integral{ u(t-T/4) * i(t) dt }
		{
			uwIdx = stAdcPool.uwDelayBufIdx;

			// 1. Read delayed voltage (T/4 ago) from circular buffer
			wRVoltDelayed = stAdcPool.wRVoltDelayBuf[uwIdx];
			wSVoltDelayed = stAdcPool.wSVoltDelayBuf[uwIdx];
			wTVoltDelayed = stAdcPool.wTVoltDelayBuf[uwIdx];

			// 2. u(t-T/4) * i(t) accumulate + capacitor current compensation
			wRCapCurr = (int16)((int32)wRVoltDelayed * stSysCfg.wCapCurrCoeff >> 15);	// = -I_cap_R(t)
			wSCapCurr = (int16)((int32)wSVoltDelayed * stSysCfg.wCapCurrCoeff >> 15);	// = -I_cap_S(t)
			wTCapCurr = (int16)((int32)wTVoltDelayed * stSysCfg.wCapCurrCoeff >> 15);	// = -I_cap_T(t)
			stAdcPool.ReactivePower.dOffsetSumIsr = (((int32)wRVoltDelayed * (stAdcPool.RInvCurr.wArithVal + wRCapCurr) >> 6)
													+((int32)wSVoltDelayed * (stAdcPool.SInvCurr.wArithVal + wSCapCurr) >> 6)
													+((int32)wTVoltDelayed * (stAdcPool.TInvCurr.wArithVal + wTCapCurr) >> 6));
			stAdcPool.ReactivePower.dAddSumIsr += stAdcPool.ReactivePower.dOffsetSumIsr;

			// 3. Overwrite buffer position with current voltage
			stAdcPool.wRVoltDelayBuf[uwIdx] = stAdcPool.RGridVolt.wArithVal;
			stAdcPool.wSVoltDelayBuf[uwIdx] = stAdcPool.SGridVolt.wArithVal;
			stAdcPool.wTVoltDelayBuf[uwIdx] = stAdcPool.TGridVolt.wArithVal;

			// 4. Advance circular index
			uwIdx++;
			if(uwIdx >= stAdcPool.uwDelayLength)
			{
				uwIdx = 0;
			}
			stAdcPool.uwDelayBufIdx = uwIdx;
		}

		// SofrWare DCI
		stAdcPool.RSoftDCI.dAddSumIsr += (stAdcPool.RInvCurr.wArithVal);
		stAdcPool.SSoftDCI.dAddSumIsr += (stAdcPool.SInvCurr.wArithVal);
		//stAdcPool.TSoftDCI.dAddSumIsr += (stAdcPool.TInvCurr.wArithVal);

		stAdcPool.GFCI.dAddSumIsr += ((int32)stAdcPool.GFCI.wArithVal);
	}

	// 2选1片选采样，分别对应0、1
	if(AD1_MUX_LEVEL == 0)
	{
		stAdcPool.PVVolt[PVA].wPrdVal = AdcbResultRegs.ADCRESULT3;
		stAdcPool.PVVolt[PVC].wPrdVal = AdcdResultRegs.ADCRESULT0;
		stAdcPool.BTCurr[PVA].wPrdVal = AdcdResultRegs.ADCRESULT1-372;	// Hard Baisc 0.054328V
		stAdcPool.BTCurr[PVC].wPrdVal = AdcdResultRegs.ADCRESULT2-372;	// Hard Baisc 0.054328V

		stAdcPool.PVVolt[PVA].wArithVal = (int16)((int32)(stAdcPool.PVVolt[PVA].wZeroVal + stAdcPool.PVVolt[PVA].wPrdVal)*stSysCfg.wPVVoltRatio>>7);		// 1V-->10 PV_V=Vsample*176/2^6
		//stAdcPool.PVVolt[PVA].wArithVal = 3000;
		stAdcPool.PVVolt[PVC].wArithVal = (int16)((int32)(stAdcPool.PVVolt[PVC].wZeroVal + stAdcPool.PVVolt[PVC].wPrdVal)*stSysCfg.wPVVoltRatio>>7);		// 1V-->10 PV_V=Vsample*176/2^6
		stAdcPool.BTCurr[PVA].wArithVal = (int16)((int32)(stAdcPool.BTCurr[PVA].wZeroVal + stAdcPool.BTCurr[PVA].wPrdVal)*stSysCfg.wBTCurrRatio1>>10)-stAdcPool.BTCurr[PVA].wOffset;	// 1A-->32 DC_I=Vsample*221/2^9
		stAdcPool.BTCurr[PVC].wArithVal = (int16)((int32)(stAdcPool.BTCurr[PVC].wZeroVal + stAdcPool.BTCurr[PVC].wPrdVal)*stSysCfg.wBTCurrRatio1>>10)-stAdcPool.BTCurr[PVC].wOffset;	// 1A-->32 DC_I=Vsample*221/2^9
		if(stSysCfg.uwPVNumber < 3)
		{
			stAdcPool.PVVolt[PVC].wArithVal = 0;
			stAdcPool.BTCurr[PVC].wArithVal = 0;
		}
		suwMux2Ch0SumCntTmp++;
		stAdcPool.PVVolt[PVA].dAddSumIsr += (stAdcPool.PVVolt[PVA].wArithVal);
		stAdcPool.BTCurr[PVA].dAddSumIsr += (stAdcPool.BTCurr[PVA].wArithVal);
		
		stAdcPool.PVVolt[PVC].dAddSumIsr += (stAdcPool.PVVolt[PVC].wArithVal);
		stAdcPool.BTCurr[PVC].dAddSumIsr += (stAdcPool.BTCurr[PVC].wArithVal);
	}
	else
	{
		stAdcPool.PVVolt[PVB].wPrdVal = AdcbResultRegs.ADCRESULT3;
		stAdcPool.PVVolt[PVD].wPrdVal = AdcdResultRegs.ADCRESULT0;
		stAdcPool.BTCurr[PVB].wPrdVal = AdcdResultRegs.ADCRESULT1-372;	// Hard Baisc 0.054328V
		stAdcPool.BTCurr[PVD].wPrdVal = AdcdResultRegs.ADCRESULT2-372;	// Hard Baisc 0.054328V

		stAdcPool.PVVolt[PVB].wArithVal = (int16)((int32)(stAdcPool.PVVolt[PVB].wZeroVal + stAdcPool.PVVolt[PVB].wPrdVal)*stSysCfg.wPVVoltRatio>>7);		// 1V-->10 PV_V=Vsample*176/2^6
		stAdcPool.PVVolt[PVD].wArithVal = (int16)((int32)(stAdcPool.PVVolt[PVD].wZeroVal + stAdcPool.PVVolt[PVD].wPrdVal)*stSysCfg.wPVVoltRatio>>7);		// 1V-->10 PV_V=Vsample*176/2^6
		stAdcPool.BTCurr[PVB].wArithVal = (int16)((int32)(stAdcPool.BTCurr[PVB].wZeroVal + stAdcPool.BTCurr[PVB].wPrdVal)*stSysCfg.wBTCurrRatio2>>10)-stAdcPool.BTCurr[PVB].wOffset;	// 1A-->32 DC_I=Vsample*221/2^9
		stAdcPool.BTCurr[PVD].wArithVal = (int16)((int32)(stAdcPool.BTCurr[PVD].wZeroVal + stAdcPool.BTCurr[PVD].wPrdVal)*stSysCfg.wBTCurrRatio2>>10)-stAdcPool.BTCurr[PVD].wOffset;	// 1A-->32 DC_I=Vsample*221/2^9

		if(stSysCfg.uwPVNumber < 4)
		{
			stAdcPool.PVVolt[PVD].wArithVal = 0;
			stAdcPool.BTCurr[PVD].wArithVal = 0;
		}
		suwMux2Ch1SumCntTmp++;
		stAdcPool.PVVolt[PVB].dAddSumIsr += (stAdcPool.PVVolt[PVB].wArithVal);
		stAdcPool.BTCurr[PVB].dAddSumIsr += (stAdcPool.BTCurr[PVB].wArithVal);
		
		stAdcPool.PVVolt[PVD].dAddSumIsr += (stAdcPool.PVVolt[PVD].wArithVal);
		stAdcPool.BTCurr[PVD].dAddSumIsr += (stAdcPool.BTCurr[PVD].wArithVal);
	}

	// 4选1片选，分别对应0、1、2、3
	switch(AD_SWITCH_4IN1)
	{
		case 0:
			stAdcPool.RHardDCI.wPrdVal = AdcdResultRegs.ADCRESULT3-2048;			// Hard Baisc 1.5V
			stAdcPool.GFCI.wPrdVal = AdcdResultRegs.ADCRESULT4-stSysCfg.wGFCIBias;	// Hard Baisc

			stAdcPool.RHardDCI.wArithVal = (int16)((int32)(stAdcPool.RHardDCI.wZeroVal + stAdcPool.RHardDCI.wPrdVal)*stSysCfg.wDCIRatio>>11) - stAdcPool.RHardDCI.wOffset;
			stAdcPool.GFCI.wArithVal = (int16)(((int32)(stAdcPool.GFCI.wZeroVal + stAdcPool.GFCI.wPrdVal)*stSysCfg.wGFCIRatio)>>11) - stAdcPool.GFCI.wOffset;	// 1A-->10 AC_I=Vsample*2880/2^10

			suwMux4Ch0SumCntTmp++;
			stAdcPool.RHardDCI.dAddSumIsr += stAdcPool.RHardDCI.wArithVal;
			stAdcPool.GFCI.dSquareAddSumIsr += ((int32)stAdcPool.GFCI.wArithVal * stAdcPool.GFCI.wArithVal)>>4;
		break;

		case 2:
			stAdcPool.SHardDCI.wPrdVal = AdcdResultRegs.ADCRESULT3-2048;	// Hard Baisc 1.5V
			stAdcPool.NPEVolt.wPrdVal = AdcdResultRegs.ADCRESULT4-2048;		// Hard Baisc 1.5V

			stAdcPool.SHardDCI.wArithVal = (int16)((int32)(stAdcPool.SHardDCI.wZeroVal + stAdcPool.SHardDCI.wPrdVal)*stSysCfg.wDCIRatio>>11)- stAdcPool.SHardDCI.wOffset;
			stAdcPool.NPEVolt.wArithVal = (int16)(((int32)(stAdcPool.NPEVolt.wZeroVal + stAdcPool.NPEVolt.wPrdVal)*stSysCfg.wNPEVoltRatio)>>7);	// 1V-->10 NPE_V=Vsample*439/2^6

			suwMux4Ch2SumCntTmp++;
			stAdcPool.SHardDCI.dAddSumIsr += stAdcPool.SHardDCI.wArithVal;
			stAdcPool.NPEVolt.dSquareAddSumIsr += ((int32)stAdcPool.NPEVolt.wArithVal*stAdcPool.NPEVolt.wArithVal>>4);
		break;

		default:			// 1 & 3
			stAdcPool.NBusVolt.wPrdVal = AdcdResultRegs.ADCRESULT3;
			stAdcPool.PBusVolt.wPrdVal = AdcdResultRegs.ADCRESULT4;

			stAdcPool.PBusVolt.wArithVal	= (int16)((int32)(stAdcPool.PBusVolt.wZeroVal + stAdcPool.PBusVolt.wPrdVal)*stSysCfg.wBusVoltRatio>>7);					// 1V-->32 BUS_V=Vsample*291/2^6
			stAdcPool.NBusVolt.wArithVal	= (int16)((int32)(stAdcPool.NBusVolt.wZeroVal + stAdcPool.NBusVolt.wPrdVal)*stSysCfg.wBusVoltRatio>>7);
			//stAdcPool.PBusVolt.wArithVal = 1500;
			//stAdcPool.NBusVolt.wArithVal = 1500;

			stAdcPool.BusVolt.wArithVal	= stAdcPool.PBusVolt.wArithVal + stAdcPool.NBusVolt.wArithVal;

			suwMux4Ch3SumCntTmp++;
			stAdcPool.PBusVolt.dAddSumIsr += (stAdcPool.PBusVolt.wArithVal);
			stAdcPool.NBusVolt.dAddSumIsr += (stAdcPool.NBusVolt.wArithVal);
		break;
	}

	// 8选1片选，分别对应0、1、2、3、4、5、6、7
	switch(AD_SWITCH_8IN1)
	{
	    case 0:
	        stAdcPool.BTStrCurr[STR0].wPrdVal = AdcbResultRegs.ADCRESULT4 - stSysCfg.wStrCurrBias;       // Hard Baisc 1.61V
	        stAdcPool.AMTemp.wPrdVal = AdcbResultRegs.ADCRESULT5;
  		stAdcPool.BTStrCurr[STR0].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR0].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR0].wOffset; 
	        suwMux8Ch0SumCntTmp++;
	        stAdcPool.BTStrCurr[STR0].dAddSumIsr += (stAdcPool.BTStrCurr[STR0].wArithVal);
	        AD3_MUX_CLEAR;
	        AD2_MUX_CLEAR;
	        AD1_MUX_SET;
	    break;

	    case 1:
	        stAdcPool.BTStrCurr[STR1].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        stAdcPool.InvTemp.wPrdVal = AdcbResultRegs.ADCRESULT5;
	        stAdcPool.BTStrCurr[STR1].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR1].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR1].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)
	        suwMux8Ch1SumCntTmp++;
	        stAdcPool.BTStrCurr[STR1].dAddSumIsr += (stAdcPool.BTStrCurr[STR1].wArithVal);
	        AD3_MUX_CLEAR;
	        AD2_MUX_SET;
	        AD1_MUX_CLEAR;
	    break;

	    case 2:
	        stAdcPool.BTStrCurr[STR2].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        stAdcPool.BoostTemp.wPrdVal = AdcbResultRegs.ADCRESULT5;
	        stAdcPool.BTStrCurr[STR2].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR2].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR2].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)
	        suwMux8Ch2SumCntTmp++;
	        stAdcPool.BTStrCurr[STR2].dAddSumIsr += (stAdcPool.BTStrCurr[STR2].wArithVal);
	        AD3_MUX_CLEAR;
	        AD2_MUX_SET;
	        AD1_MUX_SET;
	    break;

	    case 3:
	        stAdcPool.BTStrCurr[STR3].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        stAdcPool.ISOVolt.wPrdVal = AdcbResultRegs.ADCRESULT5 - 361;        // Hard Baisc 0.00259979
	        stAdcPool.BTStrCurr[STR3].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR3].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR3].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)
	        stAdcPool.ISOVolt.wArithVal = (int16)(((int32)(stAdcPool.ISOVolt.wPrdVal)*stSysCfg.wISOVoltRatio)>>6);    // 1V-->10 ISO_V=Vsample*180/2^6
			suwMux8Ch3SumCntTmp++;
	        stAdcPool.BTStrCurr[STR3].dAddSumIsr += (stAdcPool.BTStrCurr[STR3].wArithVal);
	        stAdcPool.ISOVolt.dAddSumIsr += (stAdcPool.ISOVolt.wArithVal);
	        AD3_MUX_SET;
	        AD2_MUX_CLEAR;
	        AD1_MUX_CLEAR;
	    break;

	    case 4:
	        stAdcPool.BTStrCurr[STR4].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        //stAdcPool.stPID_BusVolt.wPrdVal = AdcbResultRegs.ADCRESULT5;
	        stAdcPool.BTStrCurr[STR4].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR4].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR4].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)

	        suwMux8Ch4SumCntTmp++;
	        stAdcPool.BTStrCurr[STR4].dAddSumIsr += (stAdcPool.BTStrCurr[STR4].wArithVal);
	        AD3_MUX_SET;
	        AD2_MUX_CLEAR;
	        AD1_MUX_SET;
	    break;
	    case 5:
	        stAdcPool.BTStrCurr[STR5].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        stAdcPool.BTStrCurr[STR5].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR5].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR5].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)
	        suwMux8Ch5SumCntTmp++;
	        stAdcPool.BTStrCurr[STR5].dAddSumIsr += (stAdcPool.BTStrCurr[STR5].wArithVal);
	        AD3_MUX_SET;
	        AD2_MUX_SET;
	        AD1_MUX_CLEAR;
	    break;

	    case 6:
	       stAdcPool.BTStrCurr[STR6].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        stAdcPool.BTStrCurr[STR6].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR6].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR6].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)

	        suwMux8Ch6SumCntTmp++;
	        stAdcPool.BTStrCurr[STR6].dAddSumIsr += (stAdcPool.BTStrCurr[STR6].wArithVal);
	        AD3_MUX_SET;
	        AD2_MUX_SET;
	        AD1_MUX_SET;
	    break;

	    default:
	        stAdcPool.BTStrCurr[STR7].wPrdVal = AdcbResultRegs.ADCRESULT4  - stSysCfg.wStrCurrBias;
	        stAdcPool.BTStrCurr[STR7].wArithVal = (int16)((int32)(stAdcPool.BTStrCurr[STR7].wPrdVal)*stSysCfg.wStrCurrRatio>>9) - stAdcPool.BTStrCurr[STR7].wOffset;      // 1A --> 32 (DC_I=Vsample*79/2^9)

	        suwMux8Ch7SumCntTmp++;
	        stAdcPool.BTStrCurr[STR7].dAddSumIsr += (stAdcPool.BTStrCurr[STR7].wArithVal);
	        AD3_MUX_CLEAR;
	        AD2_MUX_CLEAR;
	        AD1_MUX_CLEAR;
	    break;
	}
}

#pragma CODE_SECTION(CalcAdcRealValueSumTemp,"ramfuncs");
void CalcAdcRealValueSumTemp(void)
{	
	static Uint16 suwSumCntTemp=0;
	static int32 dInvTemp_AddSum =0;
	static int32 dBoostTemp_AddSum =0;
	static int32 dAMTemp_AddSum =0;
	static int16 dInvTemp=0;
	static int16 dBoostTemp=0;
	static int16 dAMTemp=0;
	
	dInvTemp_AddSum += stAdcPool.InvTemp.wPrdVal;
	dBoostTemp_AddSum += stAdcPool.BoostTemp.wPrdVal;
	dAMTemp_AddSum += stAdcPool.AMTemp.wPrdVal;
	
	 suwSumCntTemp++;
	 if(suwSumCntTemp>=4)//192ms
	 {
		 dInvTemp = dInvTemp_AddSum/suwSumCntTemp;
		 dBoostTemp = dBoostTemp_AddSum/suwSumCntTemp;
		 dAMTemp = dAMTemp_AddSum/suwSumCntTemp;
			
		 suwSumCntTemp = 0;
		 dInvTemp_AddSum =0;
		 dBoostTemp_AddSum =0;
		 dAMTemp_AddSum =0;
	 }
	 		 
	 stAdcPool.InvTemp.wCalVal = dInvTemp;
	stAdcPool.BoostTemp.wCalVal = dBoostTemp;
	 stAdcPool.AMTemp.wCalVal = dAMTemp;

}

//--- end of file -----------------------------------------------------



