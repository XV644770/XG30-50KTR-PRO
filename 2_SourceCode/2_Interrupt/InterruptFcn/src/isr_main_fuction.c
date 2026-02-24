/*******************************************************************************
Copyright 2020, INVT Corporation.
File Name: 	 interrupt.c
Description: This file is boost and inverter control
Version:  	 V1.0
Author:  	 
Date:  		 2020-12-10
*******************************************************************************/

/*******************************************************************************
Include Head files
*******************************************************************************/

/********************************************************************************************
*internal variables																			*
*********************************************************************************************/

/********************************************************************************************
*interface to other tasks																	*
*********************************************************************************************/
#include "dsp_user_include.h"

#define c80HzUsPeriod 			12500


#pragma CODE_SECTION(SCIAReceiveInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCIATransmitInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCIBReceiveInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCIBTransmitInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCICReceiveInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCICTransmitInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCIDReceiveInterrupt,"ramfuncs");
#pragma CODE_SECTION(SCIDTransmitInterrupt,"ramfuncs");

#pragma CODE_SECTION(EPWM1_zero_isr,"ramfuncs");
#pragma CODE_SECTION(EPWM2_prd_isr,"ramfuncs");
#pragma CODE_SECTION(EPWM1_TZ_isr,"ramfuncs");
#pragma CODE_SECTION(RLineZeroCrossInterrupt,"ramfuncs");
#pragma CODE_SECTION(SLineZeroCrossInterrupt,"ramfuncs");
#pragma CODE_SECTION(TLineZeroCrossInterrupt,"ramfuncs");
//*****************************************************
//
//
//*****************************************************
interrupt void SCIAReceiveInterrupt(void)
{
	Uint16 sciatemp;

	if(SciaRegs.SCIRXST.bit.RXERROR == 1)
	{
		// One stop bit, no parity, 8-bit character length
		//SciaRegs.SCICCR.all=0x07;
		// Enable transmit and receive
		SciaRegs.SCICTL1.all = 0x03;
		SciaRegs.SCICTL2.all = 0x03;
		SciaRegs.SCICTL1.bit.RXERRINTENA = 1; //wj 100913  could be simplified
		SciaRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
	}
	
	sciatemp = SciaRegs.SCIRXBUF.all;

	UART_RXData(RS485_UART4,sciatemp);

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	return;
}

interrupt void SCIATransmitInterrupt(void)
{

	UART_TxData(RS485_UART4,&SciaRegs);

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	return;
}

	

interrupt void SCIBReceiveInterrupt(void)
{
	Uint16 scibtemp;

	if(ScibRegs.SCIRXST.bit.RXERROR == 1)
	{
		// One stop bit, no parity, 8-bit character length
		//SciaRegs.SCICCR.all=0x07;
		// Enable transmit and receive
		ScibRegs.SCICTL1.all = 0x03;
		ScibRegs.SCICTL2.all = 0x03;
		ScibRegs.SCICTL1.bit.RXERRINTENA = 1;
		ScibRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
	}

	scibtemp = ScibRegs.SCIRXBUF.all;
	UART_RXData(DSP_F042_SCIB,scibtemp);

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	return;
}

interrupt void SCIBTransmitInterrupt(void)
{
	UART_TxData(DSP_F042_SCIB,&ScibRegs);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	return;
}


interrupt void SCICReceiveInterrupt(void)
{
	Uint16 scictemp;

	if(ScicRegs.SCIRXST.bit.RXERROR == 1)
	{
		// One stop bit, no parity, 8-bit character length
		//SciaRegs.SCICCR.all=0x07;
		// Enable transmit and receive
		ScicRegs.SCICTL1.all = 0x03;
		ScicRegs.SCICTL2.all = 0x03;
		ScicRegs.SCICTL1.bit.RXERRINTENA = 1;
		ScicRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
	}

	scictemp = ScicRegs.SCIRXBUF.all;
	UART_RXData(DSP_AFCI_SCIC,scictemp);

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	return;
}


interrupt void SCICTransmitInterrupt(void)
{
	UART_TxData(DSP_AFCI_SCIC,&ScicRegs);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	return;
}


interrupt void SCIDTransmitInterrupt(void)
{
	UART_TxData(DSP_F107_SCID,&ScidRegs);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	return;
}

interrupt void SCIDReceiveInterrupt(void)
{
	Uint16 scidtemp;

	if(ScidRegs.SCIRXST.bit.RXERROR == 1)
	{
		// One stop bit, no parity, 8-bit character length
		//SciaRegs.SCICCR.all=0x07;
		// Enable transmit and receive
		ScidRegs.SCICTL1.all = 0x03;
		ScidRegs.SCICTL2.all = 0x03;
		ScidRegs.SCICTL1.bit.RXERRINTENA = 1;
		ScidRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
	}

	scidtemp = ScidRegs.SCIRXBUF.all;
	UART_RXData(DSP_F107_SCID,scidtemp);
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	return;
}

//*************************************************************
//for inv control
//
//*************************************************************
interrupt void  EPWM1_zero_isr(void)//23.4us
{
	SoftOverProtect();

	SaveAdcBufZeroSample();			// Adc convertion 2.9us, funttion 1.6us
	
	GridVoltPhaseSequenceCheck();		// 1.5us
	
	CalcAdcOffset();
	
	InvVoltLoopCtrl();					// 2.3us
	
	
	InvBusVoltLoopCtrl();					// 2.7us
	
	
	InvCurrLoopCtrl();					// 1.7us		
	
	
	InvOutVoltCalc();					// 7us
	
#if PWM_OPEN_LOOP_ENABLE
	//Open loop program does not need to do any processing
#else
	
	InvPwmOnOff();						//  1us
	
#endif



	
	EPwm1Regs.CMPA.bit.CMPA = stPwmCalc.stOut.uwRPWMDutyP;
	EPwm2Regs.CMPA.bit.CMPA = stPwmCalc.stOut.uwRPWMDutyN;
	EPwm3Regs.CMPA.bit.CMPA = stPwmCalc.stOut.uwSPWMDutyP;
	EPwm4Regs.CMPA.bit.CMPA = stPwmCalc.stOut.uwSPWMDutyN;
	EPwm6Regs.CMPA.bit.CMPA = stPwmCalc.stOut.uwTPWMDutyP;
	EPwm7Regs.CMPA.bit.CMPA = stPwmCalc.stOut.uwTPWMDutyN;
	
	EPwm1Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all 	= PIEACK_GROUP3;
	return;
}

interrupt void EPWM2_prd_isr(void)//24.7us
{
	static Uint16 suwPeriodIsrDivFreq=0;
	static Uint16 suwPVIndex=0;
	//GpioDataRegs.GPBSET.bit.GPIO35= 1;
	CalcAdcRealValueSum();	// 6.72us
	
	GridVoltPhaseLock();	// 9.62us
	
	suwPVIndex++;
	if(suwPVIndex>=PV_MAX_NUM)
	{
		suwPVIndex = 0;
	}
	if(suwPVIndex < stSysCfg.uwPVNumber)
	{
		WaitBusVoltLoopCtrl(suwPVIndex);	
		InvBTCurrLoopCtrl(suwPVIndex);		// 0.94us
	}

	if(stSysCfg.uwFastDCMode == 1)
	{
		InvActivePowerLoopCtrl();
	}
	
#if PWM_OPEN_LOOP_ENABLE
	BoostPwmOpenLoop();
#else
	BoostPwmOnff();		// 1us
#endif

	if(0==suwPeriodIsrDivFreq)
	{
		RecordingWave();					// 1.1us
		suwPeriodIsrDivFreq = 1;
		
		//FanSpeedDetect();                              // 1.7us
	}
	else
	{
		suwPeriodIsrDivFreq = 0;
		LowVoltRideThrough();
	}
	GridVoltIslandDisturb();

	mPIN65_OFF;
	EPwm2Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
	
	//GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;
	return;
}
//****************************************************************************************
//*Functions provided to  bus bat protection task	
//
//****************************************************************************************
interrupt void EPWM1_TZ_isr(void)
{
	stInvPwm.uwTzEnableDelayCnt = 10;
	EPwm1Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm2Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm3Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm4Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm5Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm6Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm7Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm8Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm9Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm10Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm11Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)
	EPwm12Regs.AQCSFRC.all = 0x09;	// CSFB(10-High),CSFA(01-Low)

	stInvPwm.uwHardTripTimes++;
#if FAULT_SHUTDOWN
	stSysFaultReg.unFaultBit.bit.OverCurr =1;
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST1)   // INV-OCP
	{
		stSysFaultReg.unOverCurrFault.bit.INVHardWare = 1;
	}
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST2)  // POWER-FAIL		Fault
	{
		stSysFaultReg.unOverCurrFault.bit.PowerHardWare = 1;
	}
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST3)  // BOOST-OCP
	{
		stSysFaultReg.unOverCurrFault.bit.BTHardWare = 1;
	}
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST4)  // Bus-OVP		Fault
	{
		stSysFaultReg.unOverCurrFault.bit.BusOverHardWare = 1;
	}
#else
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST1)   // INV-OCP		Close Boost&Inv Pwm
	{
		stInvPwm.unPwmDisableBit.bit.HardInvOCP = 1;
		stInvPwm.uwHardInvOCPTimes++;		// Fault-01
	}
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST2)  // Bus-OVP		Fault
	{
		stSysFaultReg.unFaultBit.bit.OverCurr = 1;
		stSysFaultReg.unOverCurrFault.bit.BusOverHardWare = 1;
	}
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST3)  // BOOST-OCP		Close Boost&Inv Pwm
	{
		stInvPwm.unPwmDisableBit.bit.HardBoostOCP = 1;
		stInvPwm.uwHardBoostOCPTimes++;
	}
	if(1 == EPwm1Regs.TZOSTFLG.bit.OST4)  // POWER-FAIL		Fault
	{
		stSysFaultReg.unFaultBit.bit.OverCurr = 1;
		stSysFaultReg.unOverCurrFault.bit.PowerHardWare = 1;
	}
	stInvPwm.uwPwmDisableDelayCnt = 1;

#endif

	EALLOW;
	EPwm1Regs.TZCLR.bit.INT = 1;				// Clear TZ Interrupt flag
	EPwm1Regs.TZEINT.bit.OST = 0;				// Disable TZ Interrupt
	EDIS;		
	
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

/******************************************************************
* Function Name: 	RSLineZeroCross                                 *
*                                                                 *
* Parameters                                                      *
*                                                                 *
* Retruns                                                         *
*                                                                 *
*                                                                 *
*******************************************************************/

interrupt void RLineZeroCrossInterrupt(void)
{	
 
	//----------------------------------------//
	stGridFreq.uwRLossCnt = 0;
	
	
	 stGridFreq.udRLineZeroTimeNew =  ECap4Regs.TSCTR;
	 stGridFreq.uwRLinePeriodTemp = (Uint16)((stGridFreq.udRLineZeroTimeNew - stGridFreq.udRLineZeroTimeOld)/120);
	 if(stGridFreq.uwRLinePeriodTemp > c80HzUsPeriod)
	 {
		stGridFreq.udRLineZeroTimeOld = stGridFreq.udRLineZeroTimeNew;
		stGridFreq.uwRLinePeriodNew= stGridFreq.uwRLinePeriodTemp;
		RGridFreqCal(stGridFreq.uwRLinePeriodNew);
	 }
	
  	ECap4Regs.ECCLR.bit.CEVT1 = 1;
	ECap4Regs.ECCLR.bit.INT = 1;
	ECap4Regs.ECCTL2.bit.REARM = 1;
	// To recieve more interrupts from this PIE group, acknowledge this interrupt 
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

/******************************************************************
* Function Name: 	STLineZeroCross                              *
*                                                                 *
* Parameters                                                      *
*                                                                 *
* Retruns                                                         *
*                                                                 *
*                                                                 *
*******************************************************************/
interrupt void SLineZeroCrossInterrupt(void)
{		  
  	//--------------------------------//
  	stGridFreq.uwSLossCnt = 0;
  	stGridFreq.udSLineZeroTimeNew =  ECap5Regs.TSCTR;
	stGridFreq.uwSLinePeriodTemp = (Uint16)((stGridFreq.udSLineZeroTimeNew - stGridFreq.udSLineZeroTimeOld)/120);
	if(stGridFreq.uwSLinePeriodTemp > c80HzUsPeriod)
	{	
		stGridFreq.udSLineZeroTimeOld = stGridFreq.udSLineZeroTimeNew;
		stGridFreq.uwSLinePeriodNew= stGridFreq.uwSLinePeriodTemp;
		SGridFreqCal(stGridFreq.uwSLinePeriodNew);
	}
		
	//------------------------------------------------//
  	ECap5Regs.ECCLR.bit.CEVT1 = 1;
	ECap5Regs.ECCLR.bit.INT = 1;
	ECap5Regs.ECCTL2.bit.REARM = 1;
	// To recieve more interrupts from this PIE group, acknowledge this interrupt 
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

/******************************************************************
* Function Name: 	TRLineZeroCross                              *
*                                                                 *
* Parameters                                                      *
*                                                                 *
* Retruns                                                         *
*                                                                 *
*                                                                 *
*******************************************************************/
interrupt void TLineZeroCrossInterrupt(void)
{	

  	//--------------------------------//
  	stGridFreq.uwTLossCnt = 0;
  	stGridFreq.udTLineZeroTimeNew =  ECap6Regs.TSCTR;
	stGridFreq.uwTLinePeriodTemp = (Uint16)((stGridFreq.udTLineZeroTimeNew - stGridFreq.udTLineZeroTimeOld)/120);
	if(stGridFreq.uwTLinePeriodTemp > c80HzUsPeriod)
	{	
		stGridFreq.udTLineZeroTimeOld = stGridFreq.udTLineZeroTimeNew;
		stGridFreq.uwTLinePeriodNew= stGridFreq.uwTLinePeriodTemp;
		TGridFreqCal(stGridFreq.uwTLinePeriodNew);
	}
	
	//------------------------------------------------//
	ECap6Regs.ECCLR.bit.CEVT1 = 1;
	ECap6Regs.ECCLR.bit.INT = 1;
	ECap6Regs.ECCTL2.bit.REARM = 1;
	// To recieve more interrupts from this PIE group, acknowledge this interrupt 
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

/****************************************************END********************************************************/


