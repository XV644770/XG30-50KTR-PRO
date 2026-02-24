//###########################################################################
//
// FILE:   F2807x_ECap.c
//
// TITLE:  F2807x eCAP Initialization & Support Functions.
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
// InitECap - This function initializes the eCAP(s) to a known state.
// ECAP Clock : 120MHz
//
void InitECap(void)
{
	// Configure ECAP1 Register FAN_PWM
	ECap1Regs.ECEINT.all = 0x0000;             	// Disable all capture interrupts
	ECap1Regs.ECCLR.all = 0xFFFF;              	// Clear all CAP interrupt flags
	ECap1Regs.ECCTL2.bit.CAP_APWM = 1;			// Enable APWM mode
	ECap1Regs.ECCTL2.bit.APWMPOL = 0x0;
	ECap1Regs.ECCTL2.bit.SYNCI_EN = 0x0;		// don't need SYNC
	ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0x0;
	ECap1Regs.CAP1 = 6250;            			// Set Period value = 120MHz/6250 = 19.2KHz
	ECap1Regs.CAP2 = 6250;            			// Set Compare value = 625/6250 = 10%
	ECap1Regs.CTRPHS = 0x0;
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;      	// Start ECAP PWM

	// Configure ECAP2 Register AC Relay PWM
	ECap2Regs.ECEINT.all = 0x0000;             	// Clear all CAP interrupt flags
	ECap2Regs.ECCLR.all = 0xFFFF;              	// Clear all CAP interrupt flags
	ECap2Regs.ECCTL2.bit.CAP_APWM = 1;        	// Enable APWM mode
	ECap2Regs.ECCTL2.bit.APWMPOL = 0x0;
	ECap2Regs.ECCTL2.bit.SYNCI_EN = 0x2;		// don't need SYNC
	ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0x0;
	// Configure peripheral registers
	ECap2Regs.CAP1 = 6000;            			// Set Period value = 120MHz/6000 = 20KHz
	ECap2Regs.CAP2 = 0;            				// Set Compare value = 5000/6000 = 83%
	ECap2Regs.CTRPHS = 0x0;
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;        	// Start ECAP PWM

	// Configure ECAP3 Register AC Output OCP-Ref
	ECap3Regs.ECEINT.all = 0x0000;             	// Disable all capture interrupts
	ECap3Regs.ECCLR.all = 0xFFFF;              	// Clear all CAP interrupt flags
	ECap3Regs.ECCTL2.bit.CAP_APWM = 1;			// Enable APWM mode
	ECap3Regs.ECCTL2.bit.APWMPOL = 0x0;
	ECap3Regs.ECCTL2.bit.SYNCI_EN = 0x0;		// don't need SYNC
	ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0x0;
	ECap3Regs.CAP1 = 6250;            			// Set Period value = 120MHz/6250 = 19.2KHz
//	ECap3Regs.CAP2 = 4250;            			// Set Compare value = 3750/6250 = 60%
	ECap3Regs.CAP2 = 750;            			// Set Compare value = 3750/6250 =70% 	XG40K
	ECap3Regs.CTRPHS = 0x0;
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;      	// Start ECAP PWM

	// Initialize eCAP4   Grid zero rising capture
	ECap4Regs.ECEINT.all = 0x0000;			   	// Disable all capture interrupts
	ECap4Regs.ECCLR.all = 0xFFFF;			   	// Clear all CAP interrupt flags
	ECap4Regs.ECCTL1.bit.PRESCALE = 0;			// Event Filter bypass,evey cap trigger a int
	ECap4Regs.ECCTL1.bit.CAPLDEN = 0;          	// Disable CAP1-CAP4 register loads
	ECap4Regs.ECCTL2.bit.TSCTRSTOP = 0;        	// Make sure the counter is stopped
	ECap4Regs.ECCTL2.bit.CONT_ONESHT = 1;      	//One-shot
	ECap4Regs.ECCTL2.bit.STOP_WRAP = 0;        	// Stop at 1 events,when Event 1 happen,Mod4 counter reset to 0.
	ECap4Regs.ECCTL1.bit.CAP1POL = 1;          	//Capture Event 1 triggered on Falling edge
	ECap4Regs.ECCTL1.bit.CTRRST1 = 0;          	// Not reset counter on Capture Event 1  
	ECap4Regs.ECCTL2.bit.TSCTRSTOP = 1;        	// Start Counter
	ECap4Regs.ECCTL2.bit.REARM = 1;            	// arm one-shot
	ECap4Regs.ECCTL1.bit.CAPLDEN = 1;          	// Enable CAP1-CAP4 register loads
	ECap4Regs.ECEINT.all = 2;         		   	// Enable Capture Event 1 as an Interrupt source

	// Initialize eCAP5   Grid zero rising capture
	ECap5Regs.ECEINT.all = 0x0000;			   	// Disable all capture interrupts
	ECap5Regs.ECCLR.all = 0xFFFF;			   	// Clear all CAP interrupt flags
	ECap5Regs.ECCTL1.bit.PRESCALE = 0;			// Event Filter bypass,evey cap trigger a int
	ECap5Regs.ECCTL1.bit.CAPLDEN = 0;          	// Disable CAP1-CAP4 register loads
	ECap5Regs.ECCTL2.bit.TSCTRSTOP = 0;        	// Make sure the counter is stopped
	ECap5Regs.ECCTL2.bit.CONT_ONESHT = 1;      	//One-shot
	ECap5Regs.ECCTL2.bit.STOP_WRAP = 0;        	// Stop at 1 events,when Event 1 happen,Mod4 counter reset to 0.
	ECap5Regs.ECCTL1.bit.CAP1POL = 1;          	//Capture Event 1 triggered on Falling edge
	ECap5Regs.ECCTL1.bit.CTRRST1 = 0;          	// Not reset counter on Capture Event 1  
	ECap5Regs.ECCTL2.bit.TSCTRSTOP = 1;        	// Start Counter
	ECap5Regs.ECCTL2.bit.REARM = 1;            	// arm one-shot
	ECap5Regs.ECCTL1.bit.CAPLDEN = 1;          	// Enable CAP1-CAP4 register loads
	ECap5Regs.ECEINT.all = 2;         		   	// Enable Capture Event 1 as an Interrupt source

	// Initialize eCAP6   Grid zero rising capture
	ECap6Regs.ECEINT.all = 0x0000;			   	// Disable all capture interrupts
	ECap6Regs.ECCLR.all = 0xFFFF;			   	// Clear all CAP interrupt flags
	ECap6Regs.ECCTL1.bit.PRESCALE = 0;			// Event Filter bypass,evey cap trigger a int
	ECap6Regs.ECCTL1.bit.CAPLDEN = 0;          	// Disable CAP1-CAP4 register loads
	ECap6Regs.ECCTL2.bit.TSCTRSTOP = 0;        	// Make sure the counter is stopped
	ECap6Regs.ECCTL2.bit.CONT_ONESHT = 1;      	//One-shot
	ECap6Regs.ECCTL2.bit.STOP_WRAP = 0;        	// Stop at 1 events,when Event 1 happen,Mod4 counter reset to 0.
	ECap6Regs.ECCTL1.bit.CAP1POL = 1;          	//Capture Event 1 triggered on Falling edge
	ECap6Regs.ECCTL1.bit.CTRRST1 = 0;          	// Not reset counter on Capture Event 1  
	ECap6Regs.ECCTL2.bit.TSCTRSTOP = 1;        	// Start Counter
	ECap6Regs.ECCTL2.bit.REARM = 1;            	// arm one-shot
	ECap6Regs.ECCTL1.bit.CAPLDEN = 1;          	// Enable CAP1-CAP4 register loads
	ECap6Regs.ECEINT.all = 2;         		   	// Enable Capture Event 1 as an Interrupt source

}

//
// InitECapGpio - This function initializes GPIO pins to function as ECAP pins
//                Each GPIO pin can be configured as a GPIO pin or up to 3
//                different peripheral functional pins. By default all pins
//                come up as GPIO inputs after reset.
//                Caution:
//                For each eCAP peripheral
//                Only one GPIO pin should be enabled for ECAP operation.
//                Comment out other unwanted lines.
//
void InitECapGpio()
{
}

//
// InitECap1Gpio - Initialize ECAP1 GPIOs
//
void InitECap1Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT7SELECT = pin;         // Set eCAP1 source to GPIO-pin
    EDIS;
}

//
// InitECap2Gpio - Initialize ECAP2 GPIOs
//
void InitECap2Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT8SELECT = pin;         // Set eCAP2 source to GPIO-pin
    EDIS;
}

//
// InitECap3Gpio - Initialize ECAP3 GPIOs
//
void InitECap3Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT9SELECT = pin;         // Set eCAP3 source to GPIO-pin
    EDIS;
}

//
// InitECap4Gpio - Initialize ECAP4 GPIOs
//
void InitECap4Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT10SELECT = pin;         // Set eCAP4 source to GPIO-pin
    EDIS;
}

//
// InitECap5Gpio - Initialize ECAP5 GPIOs
//
void InitECap5Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT11SELECT = pin;         // Set eCAP5 source to GPIO-pin
    EDIS;
}

//
// InitECap6Gpio - Initialize ECAP6 GPIOs
//
void InitECap6Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT12SELECT = pin;         // Set eCAP6 source to GPIO-pin
    EDIS;
}

//
// InitAPwm1Gpio - Initialize EPWM1 GPIOs
//
void InitAPwm1Gpio()
{
    EALLOW;
    OutputXbarRegs.OUTPUT3MUX0TO15CFG.bit.MUX0 = 3; // Select ECAP1.OUT on Mux0
    OutputXbarRegs.OUTPUT3MUXENABLE.bit.MUX0 = 1;  // Enable MUX0 for ECAP1.OUT
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 3;    // Select OUTPUTXBAR3 on GPIO5
    EDIS;
}


//
// End of file
//
