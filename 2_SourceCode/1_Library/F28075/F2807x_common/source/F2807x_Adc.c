//###########################################################################
//
// FILE:   F2807x_Adc.c
//
// TITLE:  F2807x Adc Support Functions.
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

// Function Prototypes
void ADCDriverConfigure();
void SetupADCParameter();
//
// AdcSetMode - Set the resolution and signalmode for a given ADC. This will
//              ensure that the correct trim is loaded.
//
void AdcSetMode(Uint16 adc, Uint16 resolution, Uint16 signalmode)
{
    Uint16 adcOffsetTrimOTPIndex; //index into OTP table of ADC offset trims
    Uint16 adcOffsetTrim;         //temporary ADC offset trim

    //
    //re-populate INL trim
    //
    CalAdcINL(adc);

    if(0xFFFF != *((Uint16*)GetAdcOffsetTrimOTP))
    {
        //
        //offset trim function is programmed into OTP, so call it
        //

        //
        //calculate the index into OTP table of offset trims and call
        //function to return the correct offset trim
        //
        adcOffsetTrimOTPIndex = 4*adc + 2*resolution + 1*signalmode;
        adcOffsetTrim = (*GetAdcOffsetTrimOTP)(adcOffsetTrimOTPIndex);
    }
    else
    {
        //
        //offset trim function is not populated, so set offset trim to 0
        //
        adcOffsetTrim = 0;
    }

    //
    //Apply the resolution and signalmode to the specified ADC.
    //Also apply the offset trim and, if needed, linearity trim correction.
    //
    switch(adc)
    {
        case ADC_ADCA:
            AdcaRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdcaRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdcaRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //
                //12-bit linearity trim workaround
                //
                AdcaRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdcaRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdcaRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdcaRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
        case ADC_ADCB:
            AdcbRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdcbRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdcbRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //
                //12-bit linearity trim workaround
                //
                AdcbRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdcbRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdcbRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdcbRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
        case ADC_ADCD:
            AdcdRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdcdRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdcdRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //
                //12-bit linearity trim workaround
                //
                AdcdRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdcdRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdcdRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdcdRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
    }
}

//
// CalAdcINL - Loads INL trim values from OTP into the trim registers of the
//             specified ADC. Use only as part of AdcSetMode function, since
//             linearity trim correction is needed for some modes.
//
void CalAdcINL(Uint16 adc)
{
    switch(adc)
    {
        case ADC_ADCA:
            if(0xFFFF != *((Uint16*)CalAdcaINL))
            {
                //
                //trim function is programmed into OTP, so call it
                //
                (*CalAdcaINL)();
            }
            else
            {
                //
                //do nothing, no INL trim function populated
                //
            }
            break;
        case ADC_ADCB:
            if(0xFFFF != *((Uint16*)CalAdcbINL))
            {
                //
                //trim function is programmed into OTP, so call it
                //
                (*CalAdcbINL)();
            }
            else
            {
                //
                //do nothing, no INL trim function populated
                //
            }
            break;
        case ADC_ADCD:
            if(0xFFFF != *((Uint16*)CalAdcdINL))
            {
                //
                //trim function is programmed into OTP, so call it
                //
                (*CalAdcdINL)();
            }
            else
            {
                //
                //do nothing, no INL trim function populated
                //
            }
            break;
    }
}


void InitAdc()
{
	ADCDriverConfigure();
	SetupADCParameter();
}

//Configure the ADC and power it up
//Write ADC configurations and power up the ADC for both ADC A and ADC B
//Write ADC configurations and power up the ADC for both ADC C and ADC D
void ADCDriverConfigure()
{
	EALLOW;

	// write configurations
	AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4

	// set ADC to signalmode
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

	// Set pulse positions to late
    // 1 interrupt pulse generation occurs at the end of conversion
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 0;
	AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 0;
	AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 0;

	// power up the ADCs
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	//delay for 1ms to allow ADC time to power up
	DELAY_US(1000);

	EDIS;
}


//Configure the ADC Parameter
void SetupADCParameter(void)
{

	//Select the channels to convert and end of conversion flag
    //ADCA
     EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL		= 0;  			// SOC0 will convert pin A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcaRegs.ADCSOC1CTL.bit.CHSEL 		= 1;  			// SOC1 will convert pin A1
    AdcaRegs.ADCSOC1CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcaRegs.ADCSOC2CTL.bit.CHSEL 		= 2;  			// SOC2 will convert pin A2
    AdcaRegs.ADCSOC2CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcaRegs.ADCSOC3CTL.bit.CHSEL 		= 3;  			// SOC3 will convert pin A3
    AdcaRegs.ADCSOC3CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcaRegs.ADCSOC4CTL.bit.CHSEL 		= 4;  			// SOC4 will convert pin A4
    AdcaRegs.ADCSOC4CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcaRegs.ADCSOC5CTL.bit.CHSEL 		= 5;  			// SOC5 will convert pin A5
    AdcaRegs.ADCSOC5CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA

    //ADCB
    AdcbRegs.ADCSOC0CTL.bit.CHSEL 		= 0;  			// SOC0 will convert pin B0
    AdcbRegs.ADCSOC0CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcbRegs.ADCSOC1CTL.bit.CHSEL 		= 1;  			// SOC1 will convert pin B1
    AdcbRegs.ADCSOC1CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC1CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcbRegs.ADCSOC2CTL.bit.CHSEL 		= 2;  			// SOC2 will convert pin B2
    AdcbRegs.ADCSOC2CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC2CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcbRegs.ADCSOC3CTL.bit.CHSEL 		= 3;  			// SOC3 will convert pin B3
    AdcbRegs.ADCSOC3CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcbRegs.ADCSOC4CTL.bit.CHSEL 		= 14;  			// SOC4 will convert pin B14
    AdcbRegs.ADCSOC4CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC4CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcbRegs.ADCSOC5CTL.bit.CHSEL 		= 15;  			// SOC5 will convert pin B15
    AdcbRegs.ADCSOC5CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC5CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA

    //ADCD
    AdcdRegs.ADCSOC0CTL.bit.CHSEL 		= 0;  			// SOC0 will convert pin D0
    AdcdRegs.ADCSOC0CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcdRegs.ADCSOC0CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcdRegs.ADCSOC1CTL.bit.CHSEL 		= 1;  			// SOC1 will convert pin D1
    AdcdRegs.ADCSOC1CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcdRegs.ADCSOC1CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcdRegs.ADCSOC2CTL.bit.CHSEL 		= 2;  			// SOC2 will convert pin D2
    AdcdRegs.ADCSOC2CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcdRegs.ADCSOC2CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcdRegs.ADCSOC3CTL.bit.CHSEL 		= 3;  			// SOC3 will convert pin D3
    AdcdRegs.ADCSOC3CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcdRegs.ADCSOC3CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA
    AdcdRegs.ADCSOC4CTL.bit.CHSEL 		= 4;  			// SOC4 will convert pin D4
    AdcdRegs.ADCSOC4CTL.bit.ACQPS 		= ADC_ACQPS; 	// sample window is acqps + 1 SYSCLK cycles
    AdcdRegs.ADCSOC4CTL.bit.TRIGSEL 	= ADC_TRIGSEL;	// SOC0 will begin conversion on ePWM1 SOCA

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL 	= 5; 			// 5 end of SOC5 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E 	= 0;   			// enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 	= 0; 			// make sure INT1 flag is cleared

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL 	= 5; 			// 5 end of SOC5 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E 	= 0;   			// enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 	= 0; 			// make sure INT1 flag is cleared

    AdcdRegs.ADCINTSEL1N2.bit.INT1SEL 	= 4; 			// 4 end of SOC4 will set INT1 flag
    AdcdRegs.ADCINTSEL1N2.bit.INT1E 	= 0;   			// enable INT1 flag
    AdcdRegs.ADCINTFLGCLR.bit.ADCINT1 	= 0; 			// make sure INT1 flag is cleared

    EDIS;
}
//
// End of file
//
