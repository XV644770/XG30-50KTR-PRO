//###########################################################################
// $TI Release: F2807x Support Library v200 $
// $Release Date: Tue Jun 21 14:11:45 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "dsp_user_include.h"

#pragma DATA_SECTION(uwAppStartMark,"appstart");
const Uint16 uwAppStartMark[2] = {0xAAAA,0xAAAA};

#pragma DATA_SECTION(uwAppEndMark,"append");
const Uint16 uwAppEndMark[2] = {0xAAAA,0xAAAA};

#pragma DATA_SECTION(uwBootStartMark,"bootstart");
const Uint16 uwBootStartMark[2] = {0xAAAA,0xAAAA};

//
// Main
//
void main(void)
{

 	DspBoardInit();		// dsp  system init; includes peripherals and variable

	VariableInit();

	OSInit();			// uCos-II system init

	OSTaskCreate(StatusTask,256,STATUS_PRIO);
	OSTimerStart(STATUS_PRIO,0,10);

	OSTaskCreate(ControlLoopTask,256,CONTROL_LOOP_PRIO);
	OSTimerStart(CONTROL_LOOP_PRIO,0,1);

	OSTaskCreate(PVInputTask,256,PV_INPUT_PRIO);
	OSTimerStart(PV_INPUT_PRIO,0,20);

	OSTaskCreate(ACOutputTask,256,AC_OUTPUT_PRIO);
	OSTimerStart(AC_OUTPUT_PRIO,0,20);

	OSTaskCreate(ComF107Task,256,COM_F107_PRIO);
	OSTimerStart(COM_F107_PRIO,0,100);

	OSTaskCreate(ComF042Task,256,COM_F042_PRIO);
	OSTimerStart(COM_F042_PRIO,0,100);

	OSTaskCreate(ComDebugTask,256,COM_DEBUG_PRIO);
	OSTimerStart(COM_DEBUG_PRIO,0,50);

	OSTaskCreate(ComAfciTask,256,COM_AFCI_PRIO);
	OSTimerStart(COM_AFCI_PRIO,0,100);

	StartEPwm();	// wait then start isr

	OSStart();
}

//
// End of file
//
