/******************************************************************
*	Copyright (c) 2020-2021, Invt Co.Lrd.
*	ALL  RIGHTS  RESERVED

*	Product:           three phase inveter
*	Filename:        system_periph_init.c
*	Purpose:          dsp system init
*	Date              Version           Author            Note
*   20/11/24          V1.01             zhaohui            none
***************************************************************/
#include "dsp_user_include.h"

/****************************************************************************
*
*	FunctionName:	DspBoardInit
*	Input:	none
*	Output:	none
*	Description:	dsp  system init; includes peripherals and variable
*	Note:	none.
***************************************************************/
void DspBoardInit(void)
{
	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	InitSysCtrl();

	// Step 2. Clear all interrupts and initialize PIE vector table:
	DINT;		// Disable CPU interrupts

	// Initialize the PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	InitPieCtrl();

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in F2807x_DefaultIsr.c.
	// This function is found in F2807x_PieVect.c.
	InitPieVectTable();

	// Step 4. User specific code, enable interrupts:
	InitGpio();			// init GPIO

	InitEPwm();			// init EPWM

	InitECap();			// init ECAP

	InitAdc();			// init ADC

	InitSci();			// init SCI

	InitCpuTimers2();	// init cpu timer2

	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	//
	EALLOW; // This is needed to write to EALLOW protected registers
	PieVectTable.TIMER2_INT		= &OSTimerTick;
	PieVectTable.USER12_INT 	= &OSCtxSw;

	PieVectTable.SCIA_RX_INT 	= &SCIAReceiveInterrupt;
	PieVectTable.SCIA_TX_INT 	= &SCIATransmitInterrupt;
	PieVectTable.SCIB_RX_INT 	= &SCIBReceiveInterrupt;
	PieVectTable.SCIB_TX_INT 	= &SCIBTransmitInterrupt;

	PieVectTable.SCIC_RX_INT 	= &SCICReceiveInterrupt;
	PieVectTable.SCIC_TX_INT 	= &SCICTransmitInterrupt;
	PieVectTable.SCID_RX_INT 	= &SCIDReceiveInterrupt;
	PieVectTable.SCID_TX_INT 	= &SCIDTransmitInterrupt;

	PieVectTable.EPWM1_INT   	= &EPWM1_zero_isr;
	PieVectTable.EPWM2_INT   	= &EPWM2_prd_isr;
	PieVectTable.EPWM1_TZ_INT	= &EPWM1_TZ_isr;

	//PIE group 4 
	PieVectTable.ECAP4_INT = &RLineZeroCrossInterrupt;
	PieVectTable.ECAP5_INT = &SLineZeroCrossInterrupt;
	PieVectTable.ECAP6_INT = &TLineZeroCrossInterrupt;

	EDIS;   // This is needed to disable write to EALLOW protected registers
	//
	// Enable CPU INT3 which is connected to EPWM1-3 INT:
	//
	DINT;

	IER = 0x0000;	// CPU Interrupt Enable PIE Group 3
	IFR = 0x0000;

	IER = (M_INT2|M_INT3|M_INT4|M_INT8|M_INT9|M_INT14);

	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;	// EPWM1_TZint
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1; 	// EPWM1_INT
	PieCtrlRegs.PIEIER3.bit.INTx2 = 1; 	// EPWM2_INT
	PieCtrlRegs.PIEIER4.bit.INTx4=1;  	// R Grid cross CAP
	PieCtrlRegs.PIEIER4.bit.INTx5=1;  	// S Grid cross CAP
	PieCtrlRegs.PIEIER4.bit.INTx6=1;  	// T Grid cross CAP
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1; 	// SCIRXINTA
	PieCtrlRegs.PIEIER9.bit.INTx2 = 1; 	// SCITXINTA
	PieCtrlRegs.PIEIER9.bit.INTx3 = 1; 	// SCIRXINTB
	PieCtrlRegs.PIEIER9.bit.INTx4 = 1; 	// SCITXINTB

	PieCtrlRegs.PIEIER8.bit.INTx5 = 1; 	// SCIRXINTC
	PieCtrlRegs.PIEIER8.bit.INTx6 = 1; 	// SCITXINTC
	PieCtrlRegs.PIEIER8.bit.INTx7 = 1; 	// SCIRXINTD
	PieCtrlRegs.PIEIER8.bit.INTx8 = 1; 	// SCITXINTD

}




