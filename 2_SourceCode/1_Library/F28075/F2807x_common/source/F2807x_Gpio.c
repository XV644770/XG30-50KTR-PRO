//###########################################################################
//
// FILE:    F2807x_Gpio.c
//
// TITLE:    GPIO module support functions
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
#include "system_variable_init.h"

//
// InitGpio - Sets all pins to be muxed to GPIO in input mode with pull-ups
//            enabled. Also disables open drain and polarity inversion and
//            sets the qualification  to synchronous. Also unlocks all GPIOs.
//
void InitGpio()
{
	//GPIO0-GPIO23	EPWM  (BOOST & INV PWM)
    InitEPwmGpio();

	//GPIO22	OutputPin	(GFCI_T)
	GPIO_SetupPinMux(22, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(22, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;
	
	//GPIO24	ECAP1.APWM	(FAN PWM) external fan control signal
	GPIO_SetupPinMux(24, GPIO_MUX_CPU1, 1);		// OUTPUTX BAR1
	GPIO_SetupPinOptions(24, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPASET.bit.GPIO24 = 1;

	//GPIO25	ECAP2.APWM	(AC_RELAY_28075)
	GPIO_SetupPinMux(25, GPIO_MUX_CPU1, 1);		// OUTPUTX BAR2
	GPIO_SetupPinOptions(25, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;

	//GPIO26	ECAP3.APWM	(INV.REF)
	GPIO_SetupPinMux(26, GPIO_MUX_CPU1, 1);		// OUTPUTX BAR3
	GPIO_SetupPinOptions(26, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;

	EALLOW;
	OutputXbarRegs.OUTPUT1MUX0TO15CFG.bit.MUX0 = 3; 	// Select ECAP1.OUT on Mux0
	OutputXbarRegs.OUTPUT1MUXENABLE.bit.MUX0 = 1;  		// Enable MUX0 for ECAP1.OUT

	OutputXbarRegs.OUTPUT2MUX0TO15CFG.bit.MUX2 = 3; 	// Select ECAP2.OUT on Mux2
	OutputXbarRegs.OUTPUT2MUXENABLE.bit.MUX2 = 1;  		// Enable MUX2 for ECAP2.OUT

	OutputXbarRegs.OUTPUT3MUX0TO15CFG.bit.MUX4 = 3; 	// Select ECAP3.OUT on Mux4
	OutputXbarRegs.OUTPUT3MUXENABLE.bit.MUX4 = 1;  		// Enable MUX4 for ECAP3.OUT
	EDIS;

	//GPIO27	InputPin	(AC.SPD)
	GPIO_SetupPinMux(27, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(27, GPIO_INPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;

	//GPIO28	OutputPin	(AD.Channle.Switch1)
	GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(28, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;

	//GPIO29	OutputPin	(AD.Channle.Switch2)
	GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;

	//GPIO30	OutputPin	(AD.Channle.Switch3)
	GPIO_SetupPinMux(30, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(30, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;

	//GPIO31-GPIO34	InputPin	(Trip Zone)
	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   	// Enable pullup on GPIO31	INV-OCP
	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;   	// Enable pullup on GPIO32	BUS-OVP
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;   	// Enable pullup on GPIO33	BOOST-OCP
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;   	// Enable pullup on GPIO34	POWER-FAIL

	GpioCtrlRegs.GPAQSEL2.bit.GPIO31 = 1; 	// 3 samples,3/120MHz = 250ns
	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 1; 	// 3 samples,3/120MHz = 250ns
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 1; 	// 3 samples,3/120MHz = 250ns
    GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 1; 	// 3 samples,3/120MHz = 250ns

    InputXbarRegs.INPUT1SELECT = 31;      			// GPIO31 = TZ1 IN  INV-OCP
    InputXbarRegs.INPUT3SELECT = 33;      			// GPIO33 = TZ3 IN  BOOST-OCP

#if FAULT_SHUTDOWN
    InputXbarRegs.INPUT2SELECT = 34;     			// GPIO34 = TZ2 IN  POWER-FAIL
    InputXbarRegs.INPUT4SELECT = 32;				// GPIO32 = TZ4 IN  BUS-OVP
#else
	InputXbarRegs.INPUT2SELECT = 34;     			// GPIO34 = TZ4 IN  POWER-FAIL
	InputXbarRegs.INPUT4SELECT = 32;				//  GPIO32 = TZ2 IN  BUS-OVP
#endif
#if 0
	EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX2 = 1;		// cmpss2 - tripH or tripL
	// Disable all the MUX first
	EPwmXbarRegs.TRIP4MUXENABLE.all = 0x00;
	// Enable Mux0 or Mux2 to generate TRIP4
	EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX0 = 1;
	EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX2 = 1;
#else
	EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX7 = 1;		// GPIO34 = TZ4 IN  POWER-FAIL
	EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX7 = 1;
#endif
	EDIS;

	//GPIO35	OutputPin	(LED)
	GPIO_SetupPinMux(35, GPIO_MUX_CPU1, 0);	
	GPIO_SetupPinOptions(35, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;

	//GPIO36-GPIO39	Reserve
    GPIO_SetupPinMux(36, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(36, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1;
	
	//GPIO40-GPIO44	Reserve
	//GPIO45	InputPin	(FAN_CLK1) extern fan1 clock
    GPIO_SetupPinMux(45, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(45, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO45 = 1;

    //GPIO46	InputPin	(FAN_CLK2) extern fan2 clock
    GPIO_SetupPinMux(46, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(46, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;
	
    //GPIO47	InputPin	(FAN_CLK3) extern fan3 clock
    GPIO_SetupPinMux(47, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(47, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;
	
    //GPIO48	InputPin	(FAN_CLK4) internal fan4 clock
	GPIO_SetupPinMux(48, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(48, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;

    //GPIO49	OutputPin	(FAN_CTRL1)
    GPIO_SetupPinMux(49, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(49, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

    //GPIO50	OutputPin	(FAN_CTRL2)
    GPIO_SetupPinMux(50, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(50, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
	
    //GPIO51	OutputPin	(FAN_CTRL3)
    GPIO_SetupPinMux(51, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(51, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;
	
    //GPIO52	OutputPin	(FAN_CTRL4) internal fan1 clock
	GPIO_SetupPinMux(52, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(52, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;

    //GPIO53	OutputPin	(PID MOS_ON)
	GPIO_SetupPinMux(53, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(53, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;

    //GPIO54	InputPin	(DC.SPD)
    GPIO_SetupPinMux(54, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(54, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;

    //GPIO55	OutputPin	(ISO_RELAY1)
    GPIO_SetupPinMux(55, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(55, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;

    //GPIO56	OutputPin	(ISO_RELAY2)
    GPIO_SetupPinMux(56, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(56, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;

    //GPIO57	OutputPin	(SPS_SHUTDOWN)
    GPIO_SetupPinMux(57, GPIO_MUX_CPU1, 0);// add 10.29
    GPIO_SetupPinOptions(57, GPIO_OUTPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;

    //GPIO58	OutputPin	(MACHINE_ID1)
    GPIO_SetupPinMux(58, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(58, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;

    //GPIO59	OutputPin	(MACHINE_ID2)
    GPIO_SetupPinMux(59, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(59, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;

    //GPIO60	OutputPin	(MACHINE_ID3)
    GPIO_SetupPinMux(60, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(60, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;

    //GPIO61	OutputPin	(MACHINE_ID4)
    GPIO_SetupPinMux(61, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(61, GPIO_INPUT, GPIO_PUSHPULL);
    GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;

    	//GPIO63	InputPin	( RGRID_ZERO)
    	InitECap4Gpio(63);
	GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 0);
    	GPIO_SetupPinOptions(63, GPIO_INPUT, GPIO_ASYNC);
    	
	//GPIO63	InputPin	( SGRID_ZERO)
    	InitECap5Gpio(64);
	GPIO_SetupPinMux(64, GPIO_MUX_CPU1, 0);
    	GPIO_SetupPinOptions(64, GPIO_INPUT, GPIO_ASYNC);

	//GPIO63	InputPin	( TGRID_ZERO)
    	InitECap6Gpio(65);
	GPIO_SetupPinMux(65, GPIO_MUX_CPU1, 0);
    	GPIO_SetupPinOptions(65, GPIO_INPUT, GPIO_ASYNC);

	

	//GPIO66-GPIO69	Reserve
    //GPIO70&71	SCIB_Pin	(communicate with STM32F042)
	GPIO_SetupPinMux(70, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(70, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(71, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(71, GPIO_INPUT, GPIO_PUSHPULL);

	//GPIO72-GPIO83	Reserve

	//GPIO84&85	SCIA_Pin	(communicate with Debug)
	GPIO_SetupPinMux(84, GPIO_MUX_CPU1, 5);
	GPIO_SetupPinOptions(84, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 5);
	GPIO_SetupPinOptions(85, GPIO_INPUT, GPIO_PUSHPULL);

	//GPIO86-GPIO88	Reserve

	//GPIO89&90	SCIC_Pin	(communicate with AFCI)
	GPIO_SetupPinMux(89, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(89, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(90, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(90, GPIO_INPUT, GPIO_PUSHPULL);

	//GPIO91-GPIO92	Reserve

	//GPIO93&94	SCID_Pin	(communicate with STM32F107)
	GPIO_SetupPinMux(93, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(93, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinMux(94, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(94, GPIO_INPUT, GPIO_PUSHPULL);


	//GPIO99	OutputPin	(LED)
	GPIO_SetupPinMux(99, GPIO_MUX_CPU1, 0);
	GPIO_SetupPinOptions(99, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPDCLEAR.bit.GPIO99 = 1;
}


//
// GPIO_SetupPinMux - Set the peripheral muxing for the specified pin.
//                    The appropriate parameters can be found in the GPIO
//                    Muxed Pins table in the datasheet. Use the GPIO index
//                    row (0 to 15) to select a muxing option for the GPIO.
//
void GPIO_SetupPinMux(Uint16 pin, Uint16 cpu, Uint16 peripheral)
{
    volatile Uint32 *gpioBaseAddr;
    volatile Uint32 *mux, *gmux, *csel;
    Uint16 pin32, pin16, pin8;

    pin32 = pin % 32;
    pin16 = pin % 16;
    pin8 = pin % 8;
    gpioBaseAddr = (Uint32 *)&GpioCtrlRegs + (pin/32)*GPY_CTRL_OFFSET;

    //
    //Sanity check for valid cpu and peripheral values
    //
    if (cpu > GPIO_MUX_CPU2CLA || peripheral > 0xF)
    {
        return;
    }

    //
    //Create pointers to the appropriate registers. This is a workaround
    //for the way GPIO registers are defined. The standard definition
    //in the header file makes it very easy to do named accesses of one
    //register or bit, but hard to do arbitrary numerical accesses. It's
    //easier to have an array of GPIO modules with identical registers,
    //including arrays for multi-register groups like GPyCSEL1-4. But
    //the header file doesn't define anything we can turn into an array,
    //so manual pointer arithmetic is used instead.
    //
    mux = gpioBaseAddr + GPYMUX + pin32/16;
    gmux = gpioBaseAddr + GPYGMUX + pin32/16;
    csel = gpioBaseAddr + GPYCSEL + pin32/8;

    //
    //Now for the actual function
    //
    EALLOW;

    //
    //To change the muxing, set the peripheral mux to 0/GPIO first to avoid
    //glitches, then change the group mux, then set the peripheral mux to
    //its target value. Finally, set the CPU select. This procedure is
    //described in the TRM. Unfortunately, since we don't know the pin in
    //advance we can't hardcode a bitfield reference, so there's some tricky
    //bit twiddling here.
    //
    *mux &= ~(0x3UL << (2*pin16));
    *gmux &= ~(0x3UL << (2*pin16));
    *gmux |= (Uint32)((peripheral >> 2) & 0x3UL) << (2*pin16);
    *mux |= (Uint32)(peripheral & 0x3UL) << (2*pin16);

    *csel &= ~(0x3L << (4*pin8));
    *csel |= (Uint32)(cpu & 0x3L) << (4*pin8);

    //
    //WARNING: This code does not touch the analog mode select registers,
    //which are needed to give the USB module control of its IOs.
    //

    EDIS;
}

//
// GPIO_SetupPinOptions - Setup up the GPIO input/output options for the
//                        specified pin. The flags are a 16-bit mask produced
//                        by ORing together options.
//                        For input pins, the valid flags are:
//                        GPIO_PULLUP  Enable pull-up
//                        GPIO_INVERT  Enable input polarity inversion
//                        GPIO_SYNC    Synchronize the input latch to PLLSYSCLK
//                                    (default-you don't need to specify this)
//                        GPIO_QUAL3   Use 3-sample qualification
//                        GPIO_QUAL6   Use 6-sample qualification
//                        GPIO_ASYNC   Do not use synchronization or
//                                     qualification
//                        (Note: only one of SYNC, QUAL3, QUAL6, or ASYNC is
//                         allowed)
//
//                        For output pins, the valid flags are:
//                        GPIO_OPENDRAIN   Output in open drain mode
//                        GPIO_PULLUP      If open drain enabled, also enable
//                                         the pull-up and the input
//                                         qualification flags
//                                        (SYNC/QUAL3/QUAL6/SYNC) listed above.
//
//                        With no flags, the default input state is synchronous
//                        with no pull-up or polarity inversion. The default
//                        output state is the standard digital output.
//
void GPIO_SetupPinOptions(Uint16 pin, Uint16 output, Uint16 flags)
{
    volatile Uint32 *gpioBaseAddr;
    volatile Uint32 *dir, *pud, *inv, *odr, *qsel;
    Uint32 pin32, pin16, pinMask, qual;

    pin32 = pin % 32;
    pin16 = pin % 16;
    pinMask = 1UL << pin32;
    gpioBaseAddr = (Uint32 *)&GpioCtrlRegs + (pin/32)*GPY_CTRL_OFFSET;

    //
    //Create pointers to the appropriate registers. This is a workaround
    //for the way GPIO registers are defined. The standard definition
    //in the header file makes it very easy to do named accesses of one
    //register or bit, but hard to do arbitrary numerical accesses. It's
    //easier to have an array of GPIO modules with identical registers,
    //including arrays for multi-register groups like GPyQSEL1-2. But
    //the header file doesn't define anything we can turn into an array,
    //so manual pointer arithmetic is used instead.
    //
    dir = gpioBaseAddr + GPYDIR;
    pud = gpioBaseAddr + GPYPUD;
    inv = gpioBaseAddr + GPYINV;
    odr = gpioBaseAddr + GPYODR;
    qsel = gpioBaseAddr + GPYQSEL + pin32/16;

    EALLOW;

    //
    //Set the data direction
    //
    *dir &= ~pinMask;
    if (output == 1)
    {
        //
        //Output, with optional open drain mode and pull-up
        //
        *dir |= pinMask;

        //
        //Enable open drain if necessary
        //
        if (flags & GPIO_OPENDRAIN)
        {
            *odr |= pinMask;
        }
        else
        {
            *odr &= ~pinMask;
        }

        //
        //Enable pull-up if necessary. Open drain mode must be active.
        //
        if (flags & (GPIO_OPENDRAIN | GPIO_PULLUP))
        {
            *pud &= ~pinMask;
        }
        else
        {
            *pud |= pinMask;
        }
    }
    else
    {
        //
        //Input, with optional pull-up, qualification, and polarity inversion
        //
        *dir &= ~pinMask;

        //
        //Enable pull-up if necessary
        //
        if (flags & GPIO_PULLUP)
        {
            *pud &= ~pinMask;
        }
        else
        {
            *pud |= pinMask;
        }

        //
        //Invert polarity if necessary
        //
        if (flags & GPIO_INVERT)
        {
            *inv |= pinMask;
        }
        else
        {
            *inv &= ~pinMask;
        }
    }

    //
    //Extract the qualification parameter and load it into the register.
    //This is also needed for open drain outputs, so we might as well do it
    //all the time.
    //
    qual = (flags & GPIO_ASYNC) / GPIO_QUAL3;
    *qsel &= ~(0x3L << (2 * pin16));
    if (qual != 0x0)
    {
        *qsel |= qual << (2 * pin16);
    }

    EDIS;
}

//
// GPIO_SetupLock - Enable or disable the GPIO register bit lock for the
//                  specified pin.
//                  The valid flags are:
//                  GPIO_UNLOCK   Unlock the pin setup register bits for the
//                                specified pin
//                  GPIO_LOCK     Lock the pin setup register bits for the
//                                specified pin
//
void GPIO_SetupLock(Uint16 pin, Uint16 flags)
{
    volatile Uint32 *gpioBaseAddr;
    volatile Uint32 *lock;
    Uint32 pin32, pinMask;

    pin32 = pin % 32;
    pinMask = 1UL << pin32;
    gpioBaseAddr = (Uint32 *)&GpioCtrlRegs + (pin/32)*GPY_CTRL_OFFSET;

    //
    //Create pointers to the appropriate registers. This is a workaround
    //for the way GPIO registers are defined. The standard definition
    //in the header file makes it very easy to do named accesses of one
    //register or bit, but hard to do arbitrary numerical accesses. It's
    //easier to have an array of GPIO modules with identical registers,
    //including arrays for multi-register groups like GPyQSEL1-2. But
    //the header file doesn't define anything we can turn into an array,
    //so manual pointer arithmetic is used instead.
    //
    lock = gpioBaseAddr + GPYLOCK;

    EALLOW;
    if(flags)
    {
        //
        //Lock the pin
        //
        *lock |= pinMask;
    }
    else
    {
        //
        //Unlock the pin
        //
        *lock &= ~pinMask;
    }
    EDIS;
}

//
// GPIO_SetupXINT1Gpio - External interrupt setup
//
void GPIO_SetupXINT1Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT4SELECT = pin;       //Set XINT1 source to GPIO-pin
    EDIS;
}

//
// GPIO_SetupXINT2Gpio - External interrupt setup
//
void GPIO_SetupXINT2Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT5SELECT = pin;       //Set XINT2 source to GPIO-pin
    EDIS;
}

//
// GPIO_SetupXINT3Gpio - External interrupt setup
//
void GPIO_SetupXINT3Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT6SELECT = pin;       //Set XINT3 source to GPIO-pin
    EDIS;
}

//
// GPIO_SetupXINT4Gpio - External interrupt setup
//
void GPIO_SetupXINT4Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT13SELECT = pin;     //Set XINT4 source to GPIO-pin
    EDIS;
}

//
// GPIO_SetupXINT5Gpio - External interrupt setup
//
void GPIO_SetupXINT5Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT14SELECT = pin;     //Set XINT5 source to GPIO-pin
    EDIS;
}

//
// GPIO_EnableUnbondedIOPullupsFor176Pin - Enable pullups for the unbonded
//                                         GPIOs on the 176PTP package:
//                                         GPIOs     Grp Bits
//                                         95-132    C   31
//                                                   D   31:0
//                                                   E   4:0
//                                         134-168   E   31:6
//                                                   F   8:0
//
void GPIO_EnableUnbondedIOPullupsFor176Pin()
{
    EALLOW;
    GpioCtrlRegs.GPCPUD.all = ~0x80000000;  //GPIO 95
    GpioCtrlRegs.GPDPUD.all = ~0xFFFFFFF7;  //GPIOs 96-127
    GpioCtrlRegs.GPEPUD.all = ~0xFFFFFFDF;  //GPIOs 128-159 except for 133
    GpioCtrlRegs.GPFPUD.all = ~0x000001FF;  //GPIOs 160-168
    EDIS;
}

//
// GPIO_EnableUnbondedIOPullupsFor100Pin - Enable pullups for the unbonded
//                                         GPIOs on the 100PZ package:
//                                         GPIOs     Grp Bits
//                                         0-1       A   1:0
//                                         5-9       A   9:5
//                                         22-40     A   31:22
//                                                   B   8:0
//                                         44-57     B   25:12
//                                         67-68     C   4:3
//                                         74-77     C   13:10
//                                         79-83     C   19:15
//                                         93-168    C   31:29
//                                                   D   31:0
//                                                   E   31:0
//                                                   F   8:0
//
void GPIO_EnableUnbondedIOPullupsFor100Pin()
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.all = ~0xFFC003E3;  //GPIOs 0-1, 5-9, 22-31
    GpioCtrlRegs.GPBPUD.all = ~0x03FFF1FF;  //GPIOs 32-40, 44-57
    GpioCtrlRegs.GPCPUD.all = ~0xE10FBC18;  //GPIOs 67-68, 74-77, 79-83, 93-95
    GpioCtrlRegs.GPDPUD.all = ~0xFFFFFFF7;  //GPIOs 96-127
    GpioCtrlRegs.GPEPUD.all = ~0xFFFFFFFF;  //GPIOs 128-159
    GpioCtrlRegs.GPFPUD.all = ~0x000001FF;  //GPIOs 160-168
    EDIS;
}

//
// GPIO_EnableUnbondedIOPullups - Enable IO pullups for specific packages
//
void GPIO_EnableUnbondedIOPullups()
{
    unsigned char pin_count = ((DevCfgRegs.PARTIDL.all & 0x00000700) >> 8) ;

    //
    // 5 = 100 pin
    // 6 = 176 pin
    // 7 = 337 pin
    //
    if(pin_count == 5)
    {
        GPIO_EnableUnbondedIOPullupsFor100Pin();
    }
    else if (pin_count == 6)
    {
        GPIO_EnableUnbondedIOPullupsFor176Pin();
    }
    else
    {
        //
        //do nothing - this is 337 pin package
        //
    }
}

//
// GPIO_ReadPin - Read the GPyDAT register bit for the specified pin. Note that
//                this returns the actual state of the pin, not the state of
//                the output latch.
//
Uint16 GPIO_ReadPin(Uint16 pin)
{
    volatile Uint32 *gpioDataReg;
    Uint16 pinVal;

    gpioDataReg = (volatile Uint32 *)&GpioDataRegs + (pin/32)*GPY_DATA_OFFSET;
    pinVal = (gpioDataReg[GPYDAT] >> (pin % 32)) & 0x1;

    return pinVal;
}

//
// GPIO_WritePin - Set the GPyDAT register bit for the specified pin.
//
void GPIO_WritePin(Uint16 pin, Uint16 outVal)
{
    volatile Uint32 *gpioDataReg;
    Uint32 pinMask;

    gpioDataReg = (volatile Uint32 *)&GpioDataRegs + (pin/32)*GPY_DATA_OFFSET;
    pinMask = 1UL << (pin % 32);

    if (outVal == 0)
    {
        gpioDataReg[GPYCLEAR] = pinMask;
    }
    else
    {
        gpioDataReg[GPYSET] = pinMask;
    }
}

//
// End of file
//
