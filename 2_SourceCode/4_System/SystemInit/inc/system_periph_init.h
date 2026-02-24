/*
* system_periph_init.h
*
*	Product:			three phase inveter
*	Filename:			system_periph_init.h
*	Purpose:			Stm32 system init
*	Date				Version		Author			Note
*   20/11/18			V1.01		zhaohui			none
*/

#ifndef SYSTEM_PERIPH_INIT_H
#define SYSTEM_PERIPH_INIT_H

/*****************************ADC Sample Switch Pin************************************/
// AD2 2-1 MUX select
#define	AD1_MUX_SET		        	(GpioDataRegs.GPASET.bit.GPIO28 = 1)
#define	AD1_MUX_CLEAR		    	(GpioDataRegs.GPACLEAR.bit.GPIO28 = 1)
#define	AD1_MUX_LEVEL		    		(GpioDataRegs.GPADAT.bit.GPIO28)

// AD2 4-1 MUX select
#define	AD2_MUX_SET		        	(GpioDataRegs.GPASET.bit.GPIO29 = 1)
#define	AD2_MUX_CLEAR		    	(GpioDataRegs.GPACLEAR.bit.GPIO29 = 1)
#define	AD2_MUX_LEVEL		    		(GpioDataRegs.GPADAT.bit.GPIO29)

// AD1 8-1 MUX select
#define	AD3_MUX_SET			    	(GpioDataRegs.GPASET.bit.GPIO30 = 1)
#define	AD3_MUX_CLEAR		    	(GpioDataRegs.GPACLEAR.bit.GPIO30 = 1)
#define	AD3_MUX_LEVEL		    		(GpioDataRegs.GPADAT.bit.GPIO30)

#define 	AD_SWITCH_4IN1          		(AD1_MUX_LEVEL + (AD2_MUX_LEVEL<<1))
#define 	AD_SWITCH_8IN1          		(AD1_MUX_LEVEL + (AD2_MUX_LEVEL<<1) + (AD3_MUX_LEVEL<<2))

/*****************************Machine Model Pin************************************/
#define 	ID1_MUX_LEVEL 			(GpioDataRegs.GPBDAT.bit.GPIO58)
#define 	ID2_MUX_LEVEL 			(GpioDataRegs.GPBDAT.bit.GPIO59)
#define 	ID3_MUX_LEVEL 			(GpioDataRegs.GPBDAT.bit.GPIO60)
#define 	ID4_MUX_LEVEL 			(GpioDataRegs.GPBDAT.bit.GPIO61)
#define 	ID5_MUX_LEVEL 			(GpioDataRegs.GPBDAT.bit.GPIO40)
#define 	MACHINE_TYPE           	 	(ID4_MUX_LEVEL + 2*ID3_MUX_LEVEL + 4*ID2_MUX_LEVEL + 8*ID1_MUX_LEVEL+ 16*ID5_MUX_LEVEL)



// SPS control
#define 	SPS_SHUTDOWN				(GpioDataRegs.GPCDAT.bit.GPIO57)
#define 	mSetSPS_SHUTDOWN()		(GpioDataRegs.GPBSET.bit.GPIO57=1)
#define 	mClrSPS_SHUTDOWN()		(GpioDataRegs.GPBCLEAR.bit.GPIO57=1)

/*****************************GFCT TEST pin control************************************/
#define 	GFCI_TEST_LEVEL_60KW				(GpioDataRegs.GPBDAT.bit.GPIO51)
#define 	GFCI_TEST_SET_60KW				(GpioDataRegs.GPBSET.bit.GPIO51=1)
#define 	GFCI_TEST_CLEAR_60KW				(GpioDataRegs.GPBCLEAR.bit.GPIO51=1)

#define 	GFCI_TEST_LEVEL				(GpioDataRegs.GPADAT.bit.GPIO22)
#define 	GFCI_TEST_SET					(GpioDataRegs.GPASET.bit.GPIO22=1)
#define 	GFCI_TEST_CLEAR				(GpioDataRegs.GPACLEAR.bit.GPIO22=1)

/*****************************relay pin control************************************/
#define 	DSP_RELAY_HIGH			(ECap2Regs.CAP2 = 6000)
#define 	DSP_RELAY_PWM(Var)		(ECap2Regs.CAP2 = Var)		// 3300/6000 = 55%
#define 	DSP_RELAY_LOW				(ECap2Regs.CAP2 = 0)

/*****************************INV OCP pin control************************************/
#define 	INV_OCP(Var)				(ECap3Regs.CAP2 = Var)		// 3300/6000 = 55%
/*****************************fan pin control************************************/
#define FAN_EXTR_PWM(x)			(ECap1Regs.CAP2 = x)	// external fan1,2,3 Control PWM signal
#define FAN1_EXTR_ON            (GpioDataRegs.GPBSET.bit.GPIO49 = 1)
#define FAN1_EXTR_OFF           (GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1)
#define FAN2_EXTR_ON            (GpioDataRegs.GPBSET.bit.GPIO50 = 1)
#define FAN2_EXTR_OFF           (GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1)
#define FAN3_EXTR_ON            (GpioDataRegs.GPBSET.bit.GPIO51 = 1)
#define FAN3_EXTR_OFF           (GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1)
#define FAN4_INTR_ON			(GpioDataRegs.GPBSET.bit.GPIO52 = 1)
#define FAN4_INTR_OFF			(GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1)

/*****************************fan pulse pin************************************/
#define FAN1_EXTR_LEVEL			(GpioDataRegs.GPBDAT.bit.GPIO45)		// external fan speed signal
#define FAN2_EXTR_LEVEL			(GpioDataRegs.GPBDAT.bit.GPIO46)		// external fan speed signal
#define FAN3_EXTR_LEVEL			(GpioDataRegs.GPBDAT.bit.GPIO47)		// external fan speed signal
#define FAN4_INTR_LEVEL			(GpioDataRegs.GPBDAT.bit.GPIO48)		// internal fan speed signal

/*****************************PV ISO relay  pin control************************************/
#define 	ISO_RELAY1_SET				(GpioDataRegs.GPBSET.bit.GPIO55 = 1)
#define 	ISO_RELAY1_CLEAR			(GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1)
#define 	ISO_RELAY2_SET				(GpioDataRegs.GPBSET.bit.GPIO56 = 1)
#define 	ISO_RELAY2_CLEAR			(GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1)

#define		AC_SPD_LEVEL				(GpioDataRegs.GPADAT.bit.GPIO27)
#define		DC_SPD_LEVEL    			(GpioDataRegs.GPBDAT.bit.GPIO54)
/*****************************led pin control************************************/
#define	TOGGLE_PIN35				(GpioDataRegs.GPBTOGGLE.bit.GPIO35 = 1) 		// LED Blink
#define	TOGGLE_PIN99				(GpioDataRegs.GPDTOGGLE.bit.GPIO99 = 1) 		// LED Blink
#define	mPIN63_TOGGLE				(GpioDataRegs.GPBTOGGLE.bit.GPIO63 = 1)
#define	mPIN63_ON					(GpioDataRegs.GPBSET.bit.GPIO63 = 1)
#define	mPIN63_OFF					(GpioDataRegs.GPBCLEAR.bit.GPIO63 = 1)
#define	mPIN65_TOGGLE				(GpioDataRegs.GPCTOGGLE.bit.GPIO65 = 1) 		
#define	mPIN65_ON					(GpioDataRegs.GPCSET.bit.GPIO65 = 1)
#define	mPIN65_OFF					(GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1)

extern void DspBoardInit(void);

extern interrupt void SCIATransmitInterrupt(void);
extern interrupt void SCIAReceiveInterrupt(void);
extern interrupt void SCIBReceiveInterrupt(void);
extern interrupt void SCIBTransmitInterrupt(void);

extern interrupt void SCICTransmitInterrupt(void);
extern interrupt void SCICReceiveInterrupt(void);
extern interrupt void SCIDReceiveInterrupt(void);
extern interrupt void SCIDTransmitInterrupt(void);

extern interrupt void EPWM1_zero_isr(void);
extern interrupt void EPWM2_prd_isr(void);
extern interrupt void EPWM1_TZ_isr(void);
interrupt void RLineZeroCrossInterrupt(void);
interrupt void SLineZeroCrossInterrupt(void);
interrupt void TLineZeroCrossInterrupt(void);

#endif /* SYSTEM_PERIPH_INIT_H */
