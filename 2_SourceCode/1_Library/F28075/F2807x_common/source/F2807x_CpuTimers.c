//###########################################################################
//
// FILE:    F2807x_CpuTimers.c
//
// TITLE:   CPU 32-bit Timers Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2807x Support Library v200 $
// $Release Date: Tue Jun 21 14:11:45 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "F2807x_device.h"
#include "F2807x_Examples.h"

//
// Globals
//
struct CPUTIMER_VARS CpuTimer2;

//
// InitCpuTimers - This function initializes all three CPU timers to a known
//                 state.
//
void InitCpuTimers2(void)
{

    //
    // Initialize address pointers to respective timer registers:
    //
    CpuTimer2.RegsAddr = &CpuTimer2Regs;

    //
    // Initialize timer period to maximum:
    //
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CpuTimer2Regs.TPR.all  = 0;
    CpuTimer2Regs.TPRH.all = 0;

    //
    // Make sure timers are stopped:
    //
    CpuTimer2Regs.TCR.bit.TSS = 1;

    //
    // Reload all counter register with period value:
    //
    CpuTimer2Regs.TCR.bit.TRB = 1;
    //
    // Reset interrupt counters:
    //
    CpuTimer2.InterruptCount = 0;
    //T = Freq * Period / SYSCLK = 120*1000 / 120MHz = 0.001s = 1ms
    ConfigCpuTimer(&CpuTimer2, 120, 1000); // 1ms
    StartCpuTimer2();
}

//
// ConfigCpuTimer - This function initializes the selected timer to the period
//                  specified by the "Freq" and "Period" parameters. The "Freq"
//                  is entered as "MHz" and the period in "uSeconds". The timer
//                  is held in the stopped state after configuration.
//
void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq, float Period)
{
    Uint32 temp;

    //
    // Initialize timer period:
    //
    Timer->CPUFreqInMHz = Freq;
    Timer->PeriodInUSec = Period;
    temp = (long) (Freq * Period);

    //
    // Counter decrements PRD+1 times each period
    //
    Timer->RegsAddr->PRD.all = temp - 1;

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    Timer->RegsAddr->TPR.all  = 0;
    Timer->RegsAddr->TPRH.all  = 0;

    //
    // Initialize timer control register:
    //
    Timer->RegsAddr->TCR.bit.TSS = 1;     // 1 = Stop timer, 0 = Start/Restart
                                          // Timer
    Timer->RegsAddr->TCR.bit.TRB = 1;     // 1 = reload timer
    Timer->RegsAddr->TCR.bit.SOFT = 0;
    Timer->RegsAddr->TCR.bit.FREE = 0;    // Timer Free Run Disabled
    Timer->RegsAddr->TCR.bit.TIE = 1;     // 0 = Disable/ 1 = Enable Timer
                                          // Interrupt

    //
    // Reset interrupt counter:
    //
    Timer->InterruptCount = 0;
}


//
// End of file
//
