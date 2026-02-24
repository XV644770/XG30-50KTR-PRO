//###########################################################################
//
// FILE:   F2807x_EPwm.c
//
// TITLE:  F2807x EPwm Initialization & Support Functions.
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
#include "dsp_user_include.h"
//
// InitEPwmGpio - Initialize all EPWM modules' GPIOs
//
void InitEPwmGpio(void)
{
    InitEPwm1Gpio();
    InitEPwm2Gpio();
    InitEPwm3Gpio();
    InitEPwm4Gpio();
    InitEPwm5Gpio();
    InitEPwm6Gpio();
    InitEPwm7Gpio();
    InitEPwm8Gpio();
    InitEPwm9Gpio();
    InitEPwm10Gpio();
    InitEPwm11Gpio();
//    InitEPwm12Gpio();
}

//
// InitEPwm1Gpio - Initialize EPWM1 GPIOs
//
void InitEPwm1Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = GPIO_PULLUP;    // GPIO_PULLUP-Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = GPIO_PULLUP;    // Disable pull-up on GPIO1 (EPWM1B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO145 = 1;    // Disable pull-up on GPIO145 (EPWM1A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO146 = 1;    // Disable pull-up on GPIO146 (EPWM1B)

    //
    // Configure EPWM-1 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM1 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO145 = 1;   // Configure GPIO145 as EPWM1A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO146 = 1;   // Configure GPIO0146 as EPWM1B

    EDIS;
}

//
// InitEPwm2Gpio - Initialize EPWM2 GPIOs
//
void InitEPwm2Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = GPIO_PULLUP;    // Disable pull-up on GPIO2 (EPWM2A)
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = GPIO_PULLUP;    // Disable pull-up on GPIO3 (EPWM2B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO147 = 1;    // Disable pull-up on GPIO147 (EPWM2A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO148 = 1;    // Disable pull-up on GPIO148 (EPWM2B)

    //
    // Configure EPwm-2 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM2 functional pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // Configure GPIO3 as EPWM2B
   // GpioCtrlRegs.GPEMUX2.bit.GPIO147 = 1;   // Configure GPIO147 as EPWM2A
   // GpioCtrlRegs.GPEMUX2.bit.GPIO148 = 1;   // Configure GPIO148 as EPWM2B

    EDIS;
}

//
// InitEPwm3Gpio - Initialize EPWM3 GPIOs
//
void InitEPwm3Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    //   for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = GPIO_PULLUP;    // Disable pull-up on GPIO4 (EPWM3A)
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = GPIO_PULLUP;    // Disable pull-up on GPIO5 (EPWM3B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO149 = 1;    // Disable pull-up on GPIO149 (EPWM3A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO150 = 1;    // Disable pull-up on GPIO150 (EPWM3B)

    //
    // Configure EPwm-3 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM3 functional pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO4 as EPWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // Configure GPIO5 as EPWM3B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO149 = 1;   // Configure GPIO149 as EPWM3A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO150 = 1;   // Configure GPIO150 as EPWM3B

    EDIS;
}

//
// InitEPwm4Gpio - Initialize EPWM4 GPIOs
//
void InitEPwm4Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    //   for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = GPIO_PULLUP;    // Disable pull-up on GPIO6 (EPWM4A)
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = GPIO_PULLUP;    // Disable pull-up on GPIO7 (EPWM4B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO151 = 1;    // Disable pull-up on GPIO151 (EPWM4A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO152 = 1;    // Disable pull-up on GPIO152 (EPWM4B)

     //
     // Configure EPWM-4 pins using GPIO regs
     // This specifies which of the possible GPIO pins will be EPWM4 functional
     // pins.
     // Comment out other unwanted lines.
     //
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;   // Configure GPIO6 as EPWM4A
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;   // Configure GPIO7 as EPWM4B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO151 = 1;   // Configure GPIO151 as EPWM4A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO152 = 1;   // Configure GPIO152 as EPWM4B

    EDIS;
}

//
// InitEPwm5Gpio - Initialize EPWM5 GPIOs
//
void InitEPwm5Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    //   for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = GPIO_PULLUP;    // Disable pull-up on GPIO8 (EPWM5A)
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = GPIO_PULLUP;    // Disable pull-up on GPIO9 (EPWM5B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO153 = 1;    // Disable pull-up on GPIO153 (EPWM5A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO154 = 1;    // Disable pull-up on GPIO154 (EPWM5B)

    //
    // Configure EPWM-5 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM5 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;   // Configure GPIO8 as EPWM5A
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;   // Configure GPIO9 as EPWM5B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO153 = 1;   // Configure GPIO153 as EPWM5A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO154 = 1;   // Configure GPIO0154 as EPWM5B

    EDIS;
}

//
// InitEPwm6Gpio - Initialize EPWM6 GPIOs
//
void InitEPwm6Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = GPIO_PULLUP;    // Disable pull-up on GPIO10 (EPWM6A)
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = GPIO_PULLUP;    // Disable pull-up on GPIO11 (EPWM6B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO155 = 1;    // Disable pull-up on GPIO155 (EPWM6A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO156 = 1;    // Disable pull-up on GPIO156 (EPWM6B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;   // Configure GPIO10 as EPWM6A
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;   // Configure GPIO11 as EPWM6B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO155 = 1;   // Configure GPIO155 as EPWM6A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO156 = 1;   // Configure GPIO156 as EPWM6B

    EDIS;
}

//
// InitEPwm7Gpio - Initialize EPWM7 GPIOs
//
void InitEPwm7Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = GPIO_PULLUP;    // Disable pull-up on GPIO12 (EPWM7A)
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = GPIO_PULLUP;    // Disable pull-up on GPIO13 (EPWM7B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO157 = 1;    // Disable pull-up on GPIO157 (EPWM7A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO158 = 1;    // Disable pull-up on GPIO158 (EPWM7B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;   // Configure GPIO12 as EPWM7A
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;   // Configure GPIO13 as EPWM7B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO157 = 1;   // Configure GPIO157 as EPWM7A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO158 = 1;   // Configure GPIO158 as EPWM7B

    EDIS;
}

//
// InitEPwm8Gpio - Initialize EPWM8 GPIOs
//
void InitEPwm8Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = GPIO_PULLUP;    // Disable pull-up on GPIO14 (EPWM8A)
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = GPIO_PULLUP;    // Disable pull-up on GPIO15 (EPWM8B)
//  GpioCtrlRegs.GPEPUD.bit.GPIO159 = 1;    // Disable pull-up on GPIO159 (EPWM8A)
//  GpioCtrlRegs.GPFPUD.bit.GPIO160 = 1;    // Disable pull-up on GPIO160 (EPWM8B)

     //
     // Configure EPWM-6 pins using GPIO regs
     // This specifies which of the possible GPIO pins will be EPWM6 functional
     // pins.
     // Comment out other unwanted lines.
     //
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;   // Configure GPIO14 as EPWM8A
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;   // Configure GPIO15 as EPWM8B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO159 = 1;   // Configure GPIO159 as EPWM8A
    // GpioCtrlRegs.GPFMUX1.bit.GPIO160 = 1;   // Configure GPIO160 as EPWM8B

    EDIS;
}

//
// InitEPwm9Gpio - Initialize EPWM9 GPIOs
//
void InitEPwm9Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = GPIO_PULLUP;    // Disable pull-up on GPIO16 (EPWM9A)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = GPIO_PULLUP;    // Disable pull-up on GPIO17 (EPWM9B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM9 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 1;   // Configure GPIO16 as EPWM9A
    GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = 1;   // Configure GPIO17 as EPWM9B

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;	// Configure GPIO16 as EPWM9A
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;	// Configure GPIO17 as EPWM9B
    EDIS;
}

//
// InitEPwm10Gpio - Initialize EPWM10 GPIOs
//
void InitEPwm10Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = GPIO_PULLUP;    // Disable pull-up on GPIO18 (EPWM10A)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = GPIO_PULLUP;    // Disable pull-up on GPIO19 (EPWM10B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM10 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAGMUX2.bit.GPIO18 = 1;   // Configure GPIO18 as EPWM10A
    GpioCtrlRegs.GPAGMUX2.bit.GPIO19 = 1;   // Configure GPIO19 as EPWM10B

    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;	// Configure GPIO18 as EPWM10A
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1;	// Configure GPIO19 as EPWM10B
    EDIS;
}

//
// InitEPwm11Gpio - Initialize EPWM11 GPIOs
//
void InitEPwm11Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = GPIO_PULLUP;    // Disable pull-up on GPIO20 (EPWM11A)
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = GPIO_PULLUP;    // Disable pull-up on GPIO21 (EPWM11B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM11 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAGMUX2.bit.GPIO20 = 1;   // Configure GPIO20 as EPWM11A
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 1;   // Configure GPIO21 as EPWM11B

    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;	// Configure GPIO20 as EPWM11A
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;	// Configure GPIO21 as EPWM11B

    EDIS;
}

//
// InitEPwm12Gpio - Initialize EPWM12 GPIOs
//
void InitEPwm12Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO22 = GPIO_PULLUP;    // Disable pull-up on GPIO22 (EPWM12A)
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = GPIO_PULLUP;    // Disable pull-up on GPIO23 (EPWM12B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAGMUX2.bit.GPIO22 = 1;	// Configure GPIO22 as EPWM12A
    GpioCtrlRegs.GPAGMUX2.bit.GPIO23 = 1;   // Configure GPIO23 as EPWM12B

    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 1;	// Configure GPIO22 as EPWM12A
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 1;	// Configure GPIO23 as EPWM12B

    EDIS;
}

//
// InitEPwm12Gpio - Initialize EPWM12 GPIOs
//
void InitEPwm(void)
{
	//
	// synchronize the time-base clocks of all enabled ePWM
	//
	EALLOW;                                 			// Enable EALLOW protected register access
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm1A = Low state)
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm1B = Low state)
	EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm2A = Low state)
	EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm2B = Low state)
	EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm3A = Low state)
	EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm3B = Low state)
	EPwm4Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm4A = Low state)
	EPwm4Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm4B = Low state)
	EPwm5Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm5A = Low state)
	EPwm5Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm5B = Low state)
	EPwm6Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm6A = Low state)
	EPwm6Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm6B = Low state)
	EPwm7Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm7A = Low state)
	EPwm7Regs.TZCTL.bit.TZB = TZ_FORCE_LO;			// Forced LO (EPwm7B = Low state)
	EPwm8Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm8A = Low state)
	EPwm9Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm8A = Low state)
	EPwm10Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm8A = Low state)
	EPwm11Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm8A = Low state)
	EPwm12Regs.TZCTL.bit.TZA = TZ_FORCE_LO;			// Forced LO (EPwm8A = Low state)

#if TZ_PROTECT_ENABLE
	EPwm1Regs.TZFRC.bit.OST = 1;					// Force a One-Shot Trip Event via Software
	EPwm2Regs.TZFRC.bit.OST = 1;
	EPwm3Regs.TZFRC.bit.OST = 1;
	EPwm4Regs.TZFRC.bit.OST = 1;
	EPwm5Regs.TZFRC.bit.OST = 1;
	EPwm6Regs.TZFRC.bit.OST = 1;
	EPwm7Regs.TZFRC.bit.OST = 1;
	EPwm8Regs.TZFRC.bit.OST = 1;
	EPwm9Regs.TZFRC.bit.OST = 1;
	EPwm10Regs.TZFRC.bit.OST = 1;
	EPwm11Regs.TZFRC.bit.OST = 1;
	EPwm12Regs.TZFRC.bit.OST = 1;
	EPwm1Regs.TZSEL.bit.OSHT1 = 1; 		// Enable TZ1 as a one-shot trip source for this ePWM Module
	EPwm2Regs.TZSEL.bit.OSHT1 = 1;
	EPwm3Regs.TZSEL.bit.OSHT1 = 1;
	EPwm4Regs.TZSEL.bit.OSHT1 = 1;
	EPwm5Regs.TZSEL.bit.OSHT1 = 1;
	EPwm6Regs.TZSEL.bit.OSHT1 = 1;
	EPwm7Regs.TZSEL.bit.OSHT1 = 1;
	EPwm8Regs.TZSEL.bit.OSHT1 = 1;		// EPWM8 - BOOSTA
	EPwm9Regs.TZSEL.bit.OSHT1 = 1;		// EPWM9 - BOOSTB
	EPwm10Regs.TZSEL.bit.OSHT1 = 1;		// EPWM10 - BOOSTC
	EPwm11Regs.TZSEL.bit.OSHT1 = 1;		// EPWM11 - BOOSTD
	EPwm12Regs.TZSEL.bit.OSHT1 = 1;

	EPwm1Regs.TZSEL.bit.OSHT2 = 1; 		// Enable TZ2 as a one-shot trip source for this ePWM Module
	EPwm2Regs.TZSEL.bit.OSHT2 = 1;
	EPwm3Regs.TZSEL.bit.OSHT2 = 1;
	EPwm4Regs.TZSEL.bit.OSHT2 = 1;
	EPwm5Regs.TZSEL.bit.OSHT2 = 1;
	EPwm6Regs.TZSEL.bit.OSHT2 = 1;
	EPwm7Regs.TZSEL.bit.OSHT2 = 1;
	EPwm8Regs.TZSEL.bit.OSHT2 = 1;		// EPWM8 - BOOSTA
	EPwm9Regs.TZSEL.bit.OSHT2 = 1;		// EPWM9 - BOOSTB
	EPwm10Regs.TZSEL.bit.OSHT2 = 1;		// EPWM10 - BOOSTC
	EPwm11Regs.TZSEL.bit.OSHT2 = 1;		// EPWM11 - BOOSTD
	EPwm12Regs.TZSEL.bit.OSHT2 = 1;

	EPwm1Regs.TZSEL.bit.OSHT3 = 1; 		// Enable TZ3 as a one-shot trip source for this ePWM Module
	EPwm2Regs.TZSEL.bit.OSHT3 = 1;
	EPwm3Regs.TZSEL.bit.OSHT3 = 1;
	EPwm4Regs.TZSEL.bit.OSHT3 = 1;
	EPwm5Regs.TZSEL.bit.OSHT3 = 1;
	EPwm6Regs.TZSEL.bit.OSHT3 = 1;
	EPwm7Regs.TZSEL.bit.OSHT3 = 1;
	EPwm8Regs.TZSEL.bit.OSHT3 = 1;		// EPWM8 - BOOSTA
	EPwm9Regs.TZSEL.bit.OSHT3 = 1;		// EPWM9 - BOOSTB
	EPwm10Regs.TZSEL.bit.OSHT3 = 1;		// EPWM10 - BOOSTC
	EPwm11Regs.TZSEL.bit.OSHT3 = 1;		// EPWM11 - BOOSTD
	EPwm12Regs.TZSEL.bit.OSHT3 = 1;

	EPwm1Regs.TZSEL.bit.OSHT4 = 1; 		// Enable TZ4 as a one-shot trip source for this ePWM Module
	EPwm2Regs.TZSEL.bit.OSHT4 = 1;
	EPwm3Regs.TZSEL.bit.OSHT4 = 1;
	EPwm4Regs.TZSEL.bit.OSHT4 = 1;
	EPwm5Regs.TZSEL.bit.OSHT4 = 1;
	EPwm6Regs.TZSEL.bit.OSHT4 = 1;
	EPwm7Regs.TZSEL.bit.OSHT4 = 1;
	EPwm8Regs.TZSEL.bit.OSHT4 = 1;		// EPWM8 - BOOSTA
	EPwm9Regs.TZSEL.bit.OSHT4 = 1;		// EPWM9 - BOOSTB
	EPwm10Regs.TZSEL.bit.OSHT4 = 1;		// EPWM10 - BOOSTC
	EPwm11Regs.TZSEL.bit.OSHT4 = 1;		// EPWM11 - BOOSTD
	//EPwm12Regs.TZSEL.bit.OSHT4 = 1;

#if 0
	// Trip4 is the input to the DCAHCOMPSEL
	EPwm8Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 3;
	EPwm8Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI;
	// DCAEVT1 Source Signal Select
	EPwm8Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1;
	EPwm8Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;
	// 1/0 - Enable/Disable One Shot Mode
	EPwm8Regs.TZSEL.bit.DCAEVT1 = 1;
#endif
#endif
	EPwm1Regs.TZEINT.bit.OST = 0;		// Disable ont-shot interrupt generation
	EPwm1Regs.TZEINT.bit.CBC = 0;		// Disable cycle-by-cycle interrupt generation
	EDIS;

	// EPWM1 config
	EPwm1Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm1Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm1Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm1Regs.TBCTR 				= 0;				// Counter clear
	EPwm1Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm1Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm1Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm1Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm1Regs.TBCTL.bit.SYNCOSEL 	= TB_CTR_ZERO; 		// Sync down-stream module
	EPwm1Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm1Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm1Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm1Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm1Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm1Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm1Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm1Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.
	

	EPwm1Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm1Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm1Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm1Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm1Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm1Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm1Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm1Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm1Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm1Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm1Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm1Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm1Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm1Regs.ETSEL.all = 0x0000;
	EPwm1Regs.ETSEL.bit.INTEN 		= 1;				// Enables EPWMxINT generation
	EPwm1Regs.ETSEL.bit.INTSEL 		= ET_CTR_ZERO;		// Enable CTR = zero event
	EPwm1Regs.ETSEL.bit.SOCAEN 		= 1;				// Enable SOCA pulse
	EPwm1Regs.ETSEL.bit.SOCASEL 	= ET_CTR_PRDZERO;	// Enable CTR = zero and PRD event
	EPwm1Regs.ETPS.all = 0x0000;
	EPwm1Regs.ETPS.bit.INTPRD 		= ET_1ST;				// generate interrupt on INTCNT=01(first event)
	EPwm1Regs.ETPS.bit.SOCAPRD		= ET_1ST;				// Generate pulse on SOCACNT=01(first event)

	// EPWM2 config
	EPwm2Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm2Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm2Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm2Regs.TBCTR 				= 0;				// Counter clear
	EPwm2Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm2Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm2Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm2Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm2Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm2Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm2Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm2Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm2Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm2Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm2Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm2Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm2Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.


	EPwm2Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm2Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm2Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm2Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm2Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm2Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm2Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm2Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm2Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm2Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm2Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm2Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm2Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm2Regs.ETSEL.all = 0x0000;
	EPwm2Regs.ETSEL.bit.INTEN 		= 1;				// Enables EPWMxINT generation
	EPwm2Regs.ETSEL.bit.INTSEL 		= ET_CTR_PRD;		// Enable CTR = prd event
	EPwm2Regs.ETSEL.bit.SOCAEN 		= 0;				// Enable SOCA pulse
	EPwm2Regs.ETSEL.bit.SOCASEL 	= 0;				// Enable CTR = prd event
	EPwm2Regs.ETPS.all = 0x0000;
	EPwm2Regs.ETPS.bit.INTPRD 		= ET_1ST;				// generate interrupt on INTCNT=01(first event)
	EPwm2Regs.ETPS.bit.SOCAPRD		= 0;				// Generate pulse on SOCACNT=01(first event)

	// EPWM3 config
	EPwm3Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm3Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm3Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm3Regs.TBCTR 				= 0;				// Counter clear
	EPwm3Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm3Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm3Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm3Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm3Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm3Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm3Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm3Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm3Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm3Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm3Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm3Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm3Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.

	EPwm3Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm3Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm3Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm3Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm3Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm3Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm3Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm3Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm3Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm3Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm3Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm3Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm3Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm3Regs.ETSEL.all = 0x0000;				// disable EPWMxINT generation
	EPwm3Regs.ETPS.all = 0x0000;

	// EPWM4 config
	EPwm4Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm4Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm4Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm4Regs.TBCTR 				= 0;				// Counter clear
	EPwm4Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm4Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm4Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm4Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm4Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm4Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm4Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm4Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm4Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm4Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm4Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm4Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm4Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.

	EPwm4Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm4Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm4Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm4Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm4Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm4Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm4Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm4Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm4Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm4Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm4Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm4Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm4Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm4Regs.ETSEL.all = 0x0000;				// disable EPWMxINT generation
	EPwm4Regs.ETPS.all = 0x0000;

	// EPWM5 config
	EPwm5Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm5Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm5Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm5Regs.TBCTR 				= 0;				// Counter clear
	EPwm5Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm5Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm5Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm5Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm5Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm5Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down


	EPwm5Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm5Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm5Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm5Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm5Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm5Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm5Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.

	EPwm5Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm5Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm5Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm5Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm5Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm5Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm5Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm5Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm5Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm5Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm5Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm5Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm5Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm5Regs.ETSEL.all = 0x0000;				// disable EPWMxINT generation
	EPwm5Regs.ETPS.all = 0x0000;

	// EPWM6 config
	EPwm6Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm6Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm6Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm6Regs.TBCTR 				= 0;				// Counter clear
	EPwm6Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm6Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm6Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm6Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm6Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm6Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm6Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm6Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm6Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm6Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm6Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm6Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm6Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.

	EPwm6Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm6Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm6Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm6Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm6Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm6Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm6Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm6Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm6Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm6Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm6Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm6Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm6Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm6Regs.ETSEL.all = 0x0000;			// disable EPWMxINT generation
	EPwm6Regs.ETPS.all = 0x0000;

	// EPWM7 config
	EPwm7Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm7Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm7Regs.TBPRD 				= PWM_INV_PERIOD;	// Set timer period
	EPwm7Regs.TBCTR 				= 0;				// Counter clear
	EPwm7Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm7Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm7Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm7Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm7Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm7Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm7Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm7Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm7Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = PRD
	EPwm7Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = PRD

	EPwm7Regs.DBCTL.bit.IN_MODE		= DBA_RED_DBB_FED;
	EPwm7Regs.DBCTL.bit.OUT_MODE 	= DB_FULL_ENABLE;	// DBM is fully disabled
	EPwm7Regs.DBCTL.bit.POLSEL 		= DB_ACTV_HIC;		// Active high complementary (AHC). EPWMxB is inverted.

	EPwm7Regs.DBRED.all 			= PWM_DBED_TIME;	// Dead time 1.5us
	EPwm7Regs.DBFED.all 			= PWM_DBED_TIME;

	// AQ module,
	EPwm7Regs.AQCTLA.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm7Regs.AQCTLA.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm7Regs.AQCTLA.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm7Regs.AQCTLA.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm7Regs.AQCTLB.bit.CAD 		= AQ_SET;          // CNT=CMP no action
	EPwm7Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm7Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm7Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	EPwm7Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	EPwm7Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm7Regs.AQCSFRC.bit.CSFB 		= AQ_SET;

	EPwm7Regs.ETSEL.all = 0x0000;				// disable EPWMxINT generation
	     EPwm7Regs.ETPS.all = 0x0000;

	// EPWM8A-BOOSTA_PWM
	// EPWM9A-BOOSTB_PWM
	// EPWM10A-BOOSTC_PWM
	// EPWM11A-BOOSTD_PWM
	// EPWM8 config
	EPwm8Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm8Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm8Regs.TBPRD 				= PWM_BOOST_PERIOD;	// Set timer period
	EPwm8Regs.TBCTR 				= 0;				// Counter clear
	EPwm8Regs.TBPHS.all 			= 0; 				// Set Phase register to zero

	EPwm8Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm8Regs.TBCTL.bit.PHSEN 		= TB_ENABLE; 		// Master module
	EPwm8Regs.TBPHS.bit.TBPHS		= 0;
	EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;//TB_UP;			// up
	EPwm8Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm8Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module

	EPwm8Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm8Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm8Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm8Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = PRD
	EPwm8Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = PRD

	EPwm8Regs.DBCTL.bit.OUT_MODE 	= DB_DISABLE;		// DBM is disabled

	// AQ module,
	EPwm8Regs.AQCTLA.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm8Regs.AQCTLA.bit.CAU		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm8Regs.AQCTLA.bit.PRD		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm8Regs.AQCTLA.bit.ZRO		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm8Regs.AQCTLB.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm8Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm8Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm8Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	// Action Qualifier Software Force Register
	EPwm8Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	//bit15-8             reserved
	//bit7-6      11:     RLDCSF  Active Register Reload From Shadow Options         00 => CTR=0; 01 =>CTR=PRD, 10 => CTR=0 or CTR=PRD, 11, immediately
	//bit5         0:     OTSFB   One-Time Software Forced Event on Output B         0 = no effect, 1 = Initiates a single software forced event
	//bit4-3      00:     ACTSFB  Action When One-Time Software Force B is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle
	//bit2         0:     OTSFA   One-Time Software Forced Event on Output A         0 = no effect, 1 = initial a single S/w force event
	//bit1-0      00:     ACTSFA  Action When One-Time Software Force A Is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle

	//Action Qualifier Continuous S/W Force Register
	EPwm8Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm8Regs.AQCSFRC.bit.CSFB 		= AQ_SET;
	//bit15-4             reserved
	//bit3-2      01:     CSFB, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect
	//bit1-0      01:     CSFA, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect

	// Event Trigger Selection Register
	EPwm8Regs.ETSEL.all				= 0x0000;
	//bit15       0:      SOCBEN, Enable the ADC Start of Conversion B (EPWMxSOCB) Pulse  0: Disable EPWMxSOCB  1: Enable EPWMxSOCB pulse
	//bit14-12  000:      SOCBSEL EPWMxSOCB Selection Options
	//001 => TBCTR=0, 010 => TBCTR=TBPRD, 011 = TBCTR= TBPRD or 0
	//100 = CMPA or CMPC inc, 101 = CMPA or CMPC dec, 110 = CMPB OR CMPD inc, 111 = CMPB OR CMPD dec.
	//bit11       0:      SOCAEN,    1 = enable SOCA, 0 = disable
	//bit10-8   000:      SOCASEL    Enable the ADC Start of Conversion A (EPWMxSOCA) Pulse  same as SOCBSEL
	//bit7        0:      reserved
	//bit6        0:      INTSELCMP  EPWMxINT Compare Register Selection Options   0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit5        0:      SOCBSELCMP EPWMxSOCB Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit4        0:      SOCASELCMP EPWMxSOCA Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit3        0:      INTEN      EPWMx_INT, 0 = disable interrupt, 1 = enable
	//bit2-0    000:      INTSEL     same as SOCBSEL

	// Event Trigger Pre-Scale Register
	EPwm8Regs.ETPS.all				= 0x0000;
	//bit15-14   00:      SOCBCNT  EPWMxSOCB Counter Register, read-only
	//bit13-12   00:      SOCBPRD  EPWMxSOCB Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCB pulse on the 1/2/3 event.
	//bit11-10   00:      SOCACNT  EPWMxSOCA Counter Register, read-only
	//bit9-8     00:      SOCAPRD  EPWMxSOCA Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.
	//bit7-6   0000:      reserved
	//bit5        0:      SOCPSSEL  EPWMxSOC A/B Pre-Scale Selection Bits  0: Selects ETPS [INTCNT, and INTPRD] registers to determine frequency of events
	//bit4        0:      INTPSSEL  EPWMxINTn Pre-Scale Selection Bits     1: Selects ETINTPS [ INTCNT2, and INTPRD2 ] registers to determine frequency of events (interrupt once every 0-15 events).
	//bit3-2     00:      INTCNT    EPWMx_INT Counter Register
	//bit1-0     00:      INTPRD    EPWMx_INT Period Select00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.

	// EPWM9 config
	EPwm9Regs.ETCLR.bit.INT			= 1;				// Clears the INT flag bit
	EPwm9Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm9Regs.TBPRD 				= PWM_BOOST_PERIOD;	// Set timer period
	EPwm9Regs.TBCTR 				= 0;				// Counter clear

	EPwm9Regs.TBPHS.bit.TBPHS		= (PWM_BOOST_PERIOD>>1); 	// Set Phase register to

	EPwm9Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm9Regs.TBCTL.bit.PHSEN 		= TB_ENABLE; 		// Master module
	EPwm9Regs.TBCTL.bit.PHSDIR		= TB_DOWN;//TB_UP;			// down
	
	EPwm9Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm9Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm9Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm9Regs.CMPCTL.bit.SHDWAMODE 	= CC_SHADOW;		// Compare A Register Shadow mode
	EPwm9Regs.CMPCTL.bit.SHDWBMODE 	= CC_SHADOW;		// Compare B Register Shadow mode
	EPwm9Regs.CMPCTL.bit.LOADAMODE 	= CC_CTR_PRD;		// Load on CTR = PRD
	EPwm9Regs.CMPCTL.bit.LOADBMODE 	= CC_CTR_PRD;		// Load on CTR = PRD

	EPwm9Regs.DBCTL.bit.OUT_MODE 	= DB_DISABLE;		// DBM is disabled

	// AQ module,
	EPwm9Regs.AQCTLA.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm9Regs.AQCTLA.bit.CAU		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm9Regs.AQCTLA.bit.PRD		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm9Regs.AQCTLA.bit.ZRO		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm9Regs.AQCTLB.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm9Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm9Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm9Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	// Action Qualifier Software Force Register
	EPwm9Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	//bit15-8             reserved
	//bit7-6      11:     RLDCSF  Active Register Reload From Shadow Options         00 => CTR=0; 01 =>CTR=PRD, 10 => CTR=0 or CTR=PRD, 11, immediately
	//bit5         0:     OTSFB   One-Time Software Forced Event on Output B         0 = no effect, 1 = Initiates a single software forced event
	//bit4-3      00:     ACTSFB  Action When One-Time Software Force B is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle
	//bit2         0:     OTSFA   One-Time Software Forced Event on Output A         0 = no effect, 1 = initial a single S/w force event
	//bit1-0      00:     ACTSFA  Action When One-Time Software Force A Is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle

	//Action Qualifier Continuous S/W Force Register
	EPwm9Regs.AQCSFRC.bit.CSFA 		= AQ_CLEAR;
	EPwm9Regs.AQCSFRC.bit.CSFB 		= AQ_SET;
	//bit15-4             reserved
	//bit3-2      01:     CSFB, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect
	//bit1-0      01:     CSFA, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect

	// Event Trigger Selection Register
	EPwm9Regs.ETSEL.all				= 0x0000;
	//bit15       0:      SOCBEN, Enable the ADC Start of Conversion B (EPWMxSOCB) Pulse  0: Disable EPWMxSOCB  1: Enable EPWMxSOCB pulse
	//bit14-12  000:      SOCBSEL EPWMxSOCB Selection Options
	//001 => TBCTR=0, 010 => TBCTR=TBPRD, 011 = TBCTR= TBPRD or 0
	//100 = CMPA or CMPC inc, 101 = CMPA or CMPC dec, 110 = CMPB OR CMPD inc, 111 = CMPB OR CMPD dec.
	//bit11       0:      SOCAEN,    1 = enable SOCA, 0 = disable
	//bit10-8   000:      SOCASEL    Enable the ADC Start of Conversion A (EPWMxSOCA) Pulse  same as SOCBSEL
	//bit7        0:      reserved
	//bit6        0:      INTSELCMP  EPWMxINT Compare Register Selection Options   0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit5        0:      SOCBSELCMP EPWMxSOCB Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit4        0:      SOCASELCMP EPWMxSOCA Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit3        0:      INTEN      EPWMx_INT, 0 = disable interrupt, 1 = enable
	//bit2-0    000:      INTSEL     same as SOCBSEL

	// Event Trigger Pre-Scale Register
	EPwm9Regs.ETPS.all				= 0x0000;
	//bit15-14   00:      SOCBCNT  EPWMxSOCB Counter Register, read-only
	//bit13-12   00:      SOCBPRD  EPWMxSOCB Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCB pulse on the 1/2/3 event.
	//bit11-10   00:      SOCACNT  EPWMxSOCA Counter Register, read-only
	//bit9-8     00:      SOCAPRD  EPWMxSOCA Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.
	//bit7-6   0000:      reserved
	//bit5        0:      SOCPSSEL  EPWMxSOC A/B Pre-Scale Selection Bits  0: Selects ETPS [INTCNT, and INTPRD] registers to determine frequency of events
	//bit4        0:      INTPSSEL  EPWMxINTn Pre-Scale Selection Bits     1: Selects ETINTPS [ INTCNT2, and INTPRD2 ] registers to determine frequency of events (interrupt once every 0-15 events).
	//bit3-2     00:      INTCNT    EPWMx_INT Counter Register
	//bit1-0     00:      INTPRD    EPWMx_INT Period Select00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.

	// EPWM10 config
	EPwm10Regs.ETCLR.bit.INT		= 1;				// Clears the INT flag bit
	EPwm10Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm10Regs.TBPRD 				= PWM_BOOST_PERIOD;	// Set timer period
	EPwm10Regs.TBCTR 				= 0;				// Counter clear

	EPwm10Regs.TBPHS.bit.TBPHS		= PWM_BOOST_PERIOD; // Set Phase register to

	EPwm10Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;			// down
	EPwm10Regs.TBCTL.bit.PHSEN 		= TB_ENABLE; 		// Master module
	EPwm10Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm10Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm10Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	
	
	EPwm10Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register Shadow mode
	EPwm10Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare B Register Shadow mode
	EPwm10Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;		// Load on CTR = ZERO
	EPwm10Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;		// Load on CTR = ZERO

	EPwm10Regs.DBCTL.bit.OUT_MODE 	= DB_DISABLE;		// DBM is disabled

	// AQ module,
	EPwm10Regs.AQCTLA.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm10Regs.AQCTLA.bit.CAU		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm10Regs.AQCTLA.bit.PRD		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm10Regs.AQCTLA.bit.ZRO		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm10Regs.AQCTLB.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm10Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm10Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm10Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	// Action Qualifier Software Force Register
	EPwm10Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	//bit15-8             reserved
	//bit7-6      11:     RLDCSF  Active Register Reload From Shadow Options         00 => CTR=0; 01 =>CTR=PRD, 10 => CTR=0 or CTR=PRD, 11, immediately
	//bit5         0:     OTSFB   One-Time Software Forced Event on Output B         0 = no effect, 1 = Initiates a single software forced event
	//bit4-3      00:     ACTSFB  Action When One-Time Software Force B is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle
	//bit2         0:     OTSFA   One-Time Software Forced Event on Output A         0 = no effect, 1 = initial a single S/w force event
	//bit1-0      00:     ACTSFA  Action When One-Time Software Force A Is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle

	//Action Qualifier Continuous S/W Force Register
	EPwm10Regs.AQCSFRC.bit.CSFA 	= AQ_CLEAR;
	EPwm10Regs.AQCSFRC.bit.CSFB 	= AQ_SET;
	//bit15-4             reserved
	//bit3-2      01:     CSFB, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect
	//bit1-0      01:     CSFA, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect

	// Event Trigger Selection Register
	EPwm10Regs.ETSEL.all			= 0x0000;
	//bit15       0:      SOCBEN, Enable the ADC Start of Conversion B (EPWMxSOCB) Pulse  0: Disable EPWMxSOCB  1: Enable EPWMxSOCB pulse
	//bit14-12  000:      SOCBSEL EPWMxSOCB Selection Options
	//001 => TBCTR=0, 010 => TBCTR=TBPRD, 011 = TBCTR= TBPRD or 0
	//100 = CMPA or CMPC inc, 101 = CMPA or CMPC dec, 110 = CMPB OR CMPD inc, 111 = CMPB OR CMPD dec.
	//bit11       0:      SOCAEN,    1 = enable SOCA, 0 = disable
	//bit10-8   000:      SOCASEL    Enable the ADC Start of Conversion A (EPWMxSOCA) Pulse  same as SOCBSEL
	//bit7        0:      reserved
	//bit6        0:      INTSELCMP  EPWMxINT Compare Register Selection Options   0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit5        0:      SOCBSELCMP EPWMxSOCB Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit4        0:      SOCASELCMP EPWMxSOCA Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit3        0:      INTEN      EPWMx_INT, 0 = disable interrupt, 1 = enable
	//bit2-0    000:      INTSEL     same as SOCBSEL

	// Event Trigger Pre-Scale Register
	EPwm10Regs.ETPS.all				= 0x0000;
	//bit15-14   00:      SOCBCNT  EPWMxSOCB Counter Register, read-only
	//bit13-12   00:      SOCBPRD  EPWMxSOCB Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCB pulse on the 1/2/3 event.
	//bit11-10   00:      SOCACNT  EPWMxSOCA Counter Register, read-only
	//bit9-8     00:      SOCAPRD  EPWMxSOCA Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.
	//bit7-6   0000:      reserved
	//bit5        0:      SOCPSSEL  EPWMxSOC A/B Pre-Scale Selection Bits  0: Selects ETPS [INTCNT, and INTPRD] registers to determine frequency of events
	//bit4        0:      INTPSSEL  EPWMxINTn Pre-Scale Selection Bits     1: Selects ETINTPS [ INTCNT2, and INTPRD2 ] registers to determine frequency of events (interrupt once every 0-15 events).
	//bit3-2     00:      INTCNT    EPWMx_INT Counter Register
	//bit1-0     00:      INTPRD    EPWMx_INT Period Select00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.

	// EPWM11 config
	EPwm11Regs.ETCLR.bit.INT		= 1;				// Clears the INT flag bit
	EPwm11Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm11Regs.TBPRD 				= PWM_BOOST_PERIOD;	// Set timer period
	EPwm11Regs.TBCTR 				= 0;				// Counter clear

	EPwm11Regs.TBPHS.bit.TBPHS		= (PWM_BOOST_PERIOD>>1); 	// Set Phase register to

	EPwm11Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm11Regs.TBCTL.bit.PHSDIR		= TB_UP;			// dowm
	
	EPwm11Regs.TBCTL.bit.PHSEN 		= TB_ENABLE; 		// Master module
	EPwm11Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm11Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module
	EPwm11Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	

	EPwm11Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register Shadow mode
	EPwm11Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare B Register Shadow mode
	EPwm11Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;		// Load on CTR = PRD
	EPwm11Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;		// Load on CTR = PRD

	EPwm11Regs.DBCTL.bit.OUT_MODE 	= DB_DISABLE;		// DBM is disabled

	// AQ module,
	EPwm11Regs.AQCTLA.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm11Regs.AQCTLA.bit.CAU		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm11Regs.AQCTLA.bit.PRD		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm11Regs.AQCTLA.bit.ZRO		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm11Regs.AQCTLB.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm11Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm11Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm11Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	// Action Qualifier Software Force Register
	EPwm11Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	//bit15-8             reserved
	//bit7-6      11:     RLDCSF  Active Register Reload From Shadow Options         00 => CTR=0; 01 =>CTR=PRD, 10 => CTR=0 or CTR=PRD, 11, immediately
	//bit5         0:     OTSFB   One-Time Software Forced Event on Output B         0 = no effect, 1 = Initiates a single software forced event
	//bit4-3      00:     ACTSFB  Action When One-Time Software Force B is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle
	//bit2         0:     OTSFA   One-Time Software Forced Event on Output A         0 = no effect, 1 = initial a single S/w force event
	//bit1-0      00:     ACTSFA  Action When One-Time Software Force A Is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle

	//Action Qualifier Continuous S/W Force Register
	EPwm11Regs.AQCSFRC.bit.CSFA 	= AQ_CLEAR;
	EPwm11Regs.AQCSFRC.bit.CSFB 	= AQ_SET;
	//bit15-4             reserved
	//bit3-2      01:     CSFB, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect
	//bit1-0      01:     CSFA, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect

	// Event Trigger Selection Register
	EPwm11Regs.ETSEL.all			= 0x0000;
	//bit15       0:      SOCBEN, Enable the ADC Start of Conversion B (EPWMxSOCB) Pulse  0: Disable EPWMxSOCB  1: Enable EPWMxSOCB pulse
	//bit14-12  000:      SOCBSEL EPWMxSOCB Selection Options
	//001 => TBCTR=0, 010 => TBCTR=TBPRD, 011 = TBCTR= TBPRD or 0
	//100 = CMPA or CMPC inc, 101 = CMPA or CMPC dec, 110 = CMPB OR CMPD inc, 111 = CMPB OR CMPD dec.
	//bit11       0:      SOCAEN,    1 = enable SOCA, 0 = disable
	//bit10-8   000:      SOCASEL    Enable the ADC Start of Conversion A (EPWMxSOCA) Pulse  same as SOCBSEL
	//bit7        0:      reserved
	//bit6        0:      INTSELCMP  EPWMxINT Compare Register Selection Options   0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit5        0:      SOCBSELCMP EPWMxSOCB Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit4        0:      SOCASELCMP EPWMxSOCA Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit3        0:      INTEN      EPWMx_INT, 0 = disable interrupt, 1 = enable
	//bit2-0    000:      INTSEL     same as SOCBSEL

	// Event Trigger Pre-Scale Register
	EPwm11Regs.ETPS.all				= 0x0000;
	//bit15-14   00:      SOCBCNT  EPWMxSOCB Counter Register, read-only
	//bit13-12   00:      SOCBPRD  EPWMxSOCB Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCB pulse on the 1/2/3 event.
	//bit11-10   00:      SOCACNT  EPWMxSOCA Counter Register, read-only
	//bit9-8     00:      SOCAPRD  EPWMxSOCA Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.
	//bit7-6   0000:      reserved
	//bit5        0:      SOCPSSEL  EPWMxSOC A/B Pre-Scale Selection Bits  0: Selects ETPS [INTCNT, and INTPRD] registers to determine frequency of events
	//bit4        0:      INTPSSEL  EPWMxINTn Pre-Scale Selection Bits     1: Selects ETINTPS [ INTCNT2, and INTPRD2 ] registers to determine frequency of events (interrupt once every 0-15 events).
	//bit3-2     00:      INTCNT    EPWMx_INT Counter Register
	//bit1-0     00:      INTPRD    EPWMx_INT Period Select00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.


	// EPWM12 config
	EPwm12Regs.ETCLR.bit.INT		= 1;				// Clears the INT flag bit
	EPwm12Regs.ETCLR.bit.SOCA		= 1;				// Clears the SOCA flag bit

	EPwm12Regs.TBPRD 				= PWM_BOOST_PERIOD;	// Set timer period
	EPwm12Regs.TBCTR 				= 0;				// Counter clear

	EPwm12Regs.TBPHS.all			= 0; 				// Set Phase register to 0

	EPwm12Regs.TBCTL.bit.HSPCLKDIV 	= TB_DIV1;			// TBCLK = SYSCLKOUT = 120Mhz
	EPwm12Regs.TBCTL.bit.CTRMODE 	= TB_FREEZE; 		// symmetrical mode Count up and down
	EPwm12Regs.TBCTL.bit.PHSEN 		= TB_DISABLE; 		// Master module
	EPwm12Regs.TBCTL.bit.PRDLD 		= TB_SHADOW;		// The period register (TBPRD) is loaded from its shadow register when the time-base counter,TBCTR, is equal to zero could be design for immediately, need test
	EPwm12Regs.TBCTL.bit.SYNCOSEL 	= TB_SYNC_IN; 		// Sync down-stream module

	EPwm12Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register Shadow mode
	EPwm12Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare B Register Shadow mode
	EPwm12Regs.CMPCTL.bit.LOADAMODE = CC_CTR_PRD;		// Load on CTR = PRD
	EPwm12Regs.CMPCTL.bit.LOADBMODE = CC_CTR_PRD;		// Load on CTR = PRD

	EPwm12Regs.DBCTL.bit.OUT_MODE 	= DB_DISABLE;		// DBM is disabled

	// AQ module,
	EPwm12Regs.AQCTLA.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm12Regs.AQCTLA.bit.CAU		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm12Regs.AQCTLA.bit.PRD		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm12Regs.AQCTLA.bit.ZRO		= AQ_NO_ACTION;    // CNT=Zero SET

	EPwm12Regs.AQCTLB.bit.CAD		= AQ_SET;          // CNT=CMP no action
	EPwm12Regs.AQCTLB.bit.CAU 		= AQ_CLEAR;        // CNT=CMP up ->0
	EPwm12Regs.AQCTLB.bit.PRD 		= AQ_NO_ACTION;    // CNT=PRD no action
	EPwm12Regs.AQCTLB.bit.ZRO 		= AQ_NO_ACTION;    // CNT=Zero SET
	//Action-qualifier control register A/B
	//bit15-12    0000:   reserved
	//bit11-10    00:     CBD, 00 = do nothing, 01, clear, 10 = set, 11 = toggle
	//bit9-8      00:     CBU,
	//bit7-6      10:     CAD,
	//bit5-4      01:     CAU,
	//bit3-2      00:     PRD,
	//bit1-0      00:     ZRO,

	// Action Qualifier Software Force Register
	EPwm12Regs.AQSFRC.bit.RLDCSF 	= 3;				// Active Register load immediately
	//bit15-8             reserved
	//bit7-6      11:     RLDCSF  Active Register Reload From Shadow Options         00 => CTR=0; 01 =>CTR=PRD, 10 => CTR=0 or CTR=PRD, 11, immediately
	//bit5         0:     OTSFB   One-Time Software Forced Event on Output B         0 = no effect, 1 = Initiates a single software forced event
	//bit4-3      00:     ACTSFB  Action When One-Time Software Force B is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle
	//bit2         0:     OTSFA   One-Time Software Forced Event on Output A         0 = no effect, 1 = initial a single S/w force event
	//bit1-0      00:     ACTSFA  Action When One-Time Software Force A Is Invoked   00 = disable, 01 = clear, 10 = set, 11 = toggle

	//Action Qualifier Continuous S/W Force Register
	EPwm12Regs.AQCSFRC.bit.CSFA 	= AQ_CLEAR;
	EPwm12Regs.AQCSFRC.bit.CSFB 	= AQ_CLEAR;
	//bit15-4             reserved
	//bit3-2      01:     CSFB, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect
	//bit1-0      01:     CSFA, 00 = disable; 01 = contiguous low, 10 = contiguous high, 11 = no effect

	// Event Trigger Selection Register
	EPwm12Regs.ETSEL.all			= 0x0000;
	//bit15       0:      SOCBEN, Enable the ADC Start of Conversion B (EPWMxSOCB) Pulse  0: Disable EPWMxSOCB  1: Enable EPWMxSOCB pulse
	//bit14-12  000:      SOCBSEL EPWMxSOCB Selection Options
	//001 => TBCTR=0, 010 => TBCTR=TBPRD, 011 = TBCTR= TBPRD or 0
	//100 = CMPA or CMPC inc, 101 = CMPA or CMPC dec, 110 = CMPB OR CMPD inc, 111 = CMPB OR CMPD dec.
	//bit11       0:      SOCAEN,    1 = enable SOCA, 0 = disable
	//bit10-8   000:      SOCASEL    Enable the ADC Start of Conversion A (EPWMxSOCA) Pulse  same as SOCBSEL
	//bit7        0:      reserved
	//bit6        0:      INTSELCMP  EPWMxINT Compare Register Selection Options   0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit5        0:      SOCBSELCMP EPWMxSOCB Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit4        0:      SOCASELCMP EPWMxSOCA Compare Register Selection Options  0: Enable Select CMPA/CMPB 1: Enable Select CMPC/CMPD
	//bit3        0:      INTEN      EPWMx_INT, 0 = disable interrupt, 1 = enable
	//bit2-0    000:      INTSEL     same as SOCBSEL

	// Event Trigger Pre-Scale Register
	EPwm12Regs.ETPS.all				= 0x0000;
	EPwm12Regs.ETPS.bit.INTPRD		= ET_1ST;
	//bit15-14   00:      SOCBCNT  EPWMxSOCB Counter Register, read-only
	//bit13-12   00:      SOCBPRD  EPWMxSOCB Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCB pulse on the 1/2/3 event.
	//bit11-10   00:      SOCACNT  EPWMxSOCA Counter Register, read-only
	//bit9-8     00:      SOCAPRD  EPWMxSOCA Period Select, 00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.
	//bit7-6   0000:      reserved
	//bit5        0:      SOCPSSEL  EPWMxSOC A/B Pre-Scale Selection Bits  0: Selects ETPS [INTCNT, and INTPRD] registers to determine frequency of events
	//bit4        0:      INTPSSEL  EPWMxINTn Pre-Scale Selection Bits     1: Selects ETINTPS [ INTCNT2, and INTPRD2 ] registers to determine frequency of events (interrupt once every 0-15 events).
	//bit3-2     00:      INTCNT    EPWMx_INT Counter Register
	//bit1-0     00:      INTPRD    EPWMx_INT Period Select00 = disable, 01,10,11 = Generate EPWMxSOCA pulse on the 1/2/3 event.

	EALLOW;                                 // Enable EALLOW protected register access
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

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
	EDIS;
}



void StartEPwm(void)
{
	DELAY_US(500000);   			//500ms

    EALLOW;                         // Enable EALLOW protected register access
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    EPwm1Regs.TBCTR 	= 0x0010;
    EPwm2Regs.TBCTR 	= 0x0010;
    EPwm3Regs.TBCTR 	= 0x0010;
    EPwm4Regs.TBCTR 	= 0x0010;
    EPwm5Regs.TBCTR 	= 0x0010;
    EPwm6Regs.TBCTR 	= 0x0010;
    EPwm7Regs.TBCTR 	= 0x0010;
    EPwm8Regs.TBCTR 	= 0x0010;
    EPwm9Regs.TBCTR 	= 0x0010;
    EPwm10Regs.TBCTR 	= 0x0010;
    EPwm11Regs.TBCTR 	= 0x0010;
    EPwm12Regs.TBCTR 	= 0x0010;

    EPwm1Regs.CMPA.bit.CMPA 	= 0;
    EPwm1Regs.CMPB.bit.CMPB 	= 0;
    EPwm2Regs.CMPA.bit.CMPA 	= 0;
    EPwm2Regs.CMPB.bit.CMPB 	= 0;
    EPwm3Regs.CMPA.bit.CMPA 	= 0;
    EPwm3Regs.CMPB.bit.CMPB 	= 0;
    EPwm4Regs.CMPA.bit.CMPA 	= 0;
    EPwm4Regs.CMPB.bit.CMPB 	= 0;
    EPwm5Regs.CMPA.bit.CMPA 	= 0;
    EPwm5Regs.CMPB.bit.CMPB 	= 0;
    EPwm6Regs.CMPA.bit.CMPA 	= 0;
    EPwm6Regs.CMPB.bit.CMPB 	= 0;
    EPwm7Regs.CMPA.bit.CMPA 	= 0;
    EPwm7Regs.CMPB.bit.CMPB 	= 0;
    EPwm8Regs.CMPA.bit.CMPA 	= 0;
    EPwm8Regs.CMPB.bit.CMPB 	= 0;
    EPwm9Regs.CMPA.bit.CMPA 	= 0;
    EPwm10Regs.CMPA.bit.CMPA 	= 0;
    EPwm11Regs.CMPB.bit.CMPB 	= 0;
    EPwm12Regs.CMPA.bit.CMPA 	= 0;
    EPwm12Regs.CMPB.bit.CMPB 	= 0;

    // Enable EPWM Timer Count up and down
    EPwm1Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm2Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm3Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm4Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm5Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm6Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm7Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm8Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm9Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm10Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm11Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;
    EPwm12Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;

	EALLOW;
#if TZ_PROTECT_ENABLE
	EPwm1Regs.TZEINT.bit.OST	= 1;			// Enable Epwm1 TZ INT
#else
	EPwm1Regs.TZEINT.bit.OST	= 0;			// Enable Epwm1 TZ INT
#endif

	EPwm1Regs.TZCLR.bit.OST 	= 1;			// Clear Flag for One-Shot Trip (OST) Latch
	EPwm2Regs.TZCLR.bit.OST 	= 1;			// Clear Flag for One-Shot Trip (OST) Latch
	EPwm3Regs.TZCLR.bit.OST 	= 1;
	EPwm4Regs.TZCLR.bit.OST 	= 1;
	EPwm5Regs.TZCLR.bit.OST 	= 1;
	EPwm6Regs.TZCLR.bit.OST 	= 1;
	EPwm7Regs.TZCLR.bit.OST 	= 1;
	EPwm8Regs.TZCLR.bit.OST 	= 1;
	EPwm9Regs.TZCLR.bit.OST 	= 1;
	EPwm10Regs.TZCLR.bit.OST 	= 1;
	EPwm11Regs.TZCLR.bit.OST 	= 1;
	EPwm12Regs.TZCLR.bit.OST 	= 1;

	EDIS;

    EALLOW;                                 // Enable EALLOW protected register access
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}// end of EPWM_StartTimer()

void CfgInvPwmDB(void)
{
	OS_ENTER_CRITICAL();
    	EALLOW;                        

	EPwm1Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;	
	EPwm1Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	
	
	EPwm2Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;
	EPwm2Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	

	EPwm3Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;
	EPwm3Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	

	EPwm4Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;	
	EPwm4Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	

	EPwm5Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;	
	EPwm5Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	

	EPwm6Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;	
	EPwm6Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	

	EPwm7Regs.DBRED.all = stMachineCfg.wEPWMDeadBand;
	EPwm7Regs.DBFED.all = stMachineCfg.wEPWMDeadBand;	
	
    	EDIS;
	OS_EXIT_CRITICAL();
}// end of EPWM_StartTimer()
void CfgBoostPwmInterlace(INT16U uwBoostNo)
{	
	OS_ENTER_CRITICAL();
	EALLOW;

	EPwm8Regs.TBPRD 				= stSysCfg.uwPWMBoostPeriod;	// Set timer period
	EPwm8Regs.TBPHS.all 			= 0; 				// Set Phase register to zero
	EPwm9Regs.TBPRD 				= stSysCfg.uwPWMBoostPeriod;	// Set timer period
	EPwm9Regs.TBPHS.bit.TBPHS		= (stSysCfg.uwPWMBoostPeriod>>1); 	// Set Phase register to	
	EPwm10Regs.TBPRD 				= stSysCfg.uwPWMBoostPeriod;	// Set timer period
	EPwm10Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod; // Set Phase register to
	EPwm11Regs.TBPRD 				= stSysCfg.uwPWMBoostPeriod;	// Set timer period
	EPwm11Regs.TBPHS.bit.TBPHS		= (stSysCfg.uwPWMBoostPeriod>>1); 	// Set Phase register to
	EPwm12Regs.TBPRD 				= stSysCfg.uwPWMBoostPeriod;	// Set timer period
	EPwm12Regs.TBPHS.all			= 0; 				// Set Phase register to 0

	switch(uwBoostNo)
	{
		case 2:									// 0-180
			EPwm8Regs.TBPHS.bit.TBPHS		= 0; 
			EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm9Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod ; 
			EPwm9Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm10Regs.TBPHS.bit.TBPHS		= 0; 
			EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm11Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod ;
			EPwm11Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			break;
		case 3:
			/*EPwm8Regs.TBPHS.bit.TBPHS		= 0; 	// 0-240-120
			EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm9Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 2 / 3) ;
			EPwm9Regs.TBCTL.bit.PHSDIR		= TB_UP;
			EPwm10Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 2 / 3) ;	
			EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm11Regs.TBPHS.bit.TBPHS		= 0 ;
			EPwm11Regs.TBCTL.bit.PHSDIR		= TB_DOWN;*/

			EPwm8Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 2 / 3);//shift 0+ 120
			EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			//EPwm9Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 1 / 3);
			EPwm9Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 2 / 3);//shift 180+60
			EPwm9Regs.TBCTL.bit.PHSDIR		= TB_UP;
			EPwm10Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 2 / 3);//shift 0+120
			EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm11Regs.TBPHS.bit.TBPHS		= (INT16U)((INT32U)stSysCfg.uwPWMBoostPeriod  * 2 / 3);
			EPwm11Regs.TBCTL.bit.PHSDIR		= TB_UP;

			break;
		case 4:// 0-180
			EPwm8Regs.TBPHS.bit.TBPHS		= 0;
			EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm9Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod ;
			EPwm9Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm10Regs.TBPHS.bit.TBPHS		= 0; 
			EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm11Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod ;
			EPwm11Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			
			/*EPwm8Regs.TBPHS.bit.TBPHS		= 0; 	// 0-90-180-270
			EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm9Regs.TBPHS.bit.TBPHS		= (PWM_BOOST_PERIOD>>1) ;
			EPwm9Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm10Regs.TBPHS.bit.TBPHS		= PWM_BOOST_PERIOD;
			EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm11Regs.TBPHS.bit.TBPHS		= (PWM_BOOST_PERIOD>>1) ;
			EPwm11Regs.TBCTL.bit.PHSDIR		= TB_UP;*/
			
			break;
		default:
			EPwm8Regs.TBPHS.bit.TBPHS		= 0; 
			EPwm8Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm9Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod ; 
			EPwm9Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm10Regs.TBPHS.bit.TBPHS		= 0; 
			EPwm10Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			EPwm11Regs.TBPHS.bit.TBPHS		= stSysCfg.uwPWMBoostPeriod ;
			EPwm11Regs.TBCTL.bit.PHSDIR		= TB_DOWN;
			break;
	}
    	EDIS;
	OS_EXIT_CRITICAL();
	
}// end of EPWM_StartTimer()

