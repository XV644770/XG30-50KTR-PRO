#include "update_dsp.h"
#include "F28x_Project.h"
#include "F021_F2837xD_C28x.h"

//definitions for baudrate SCI
//
// SCI_BAUD_REG = [LSPCLK/(BAUDRATE*8)]-1
// LSPCLK = SYSCLK / LSPCLKDIV = 120MHz / 4 = 30MHz
#define BOOT_BAUD_9600	   390
#define BOOT_BAUD_57600     64

/****************the length of boot flash************************/
#define RECE_FLASH_LENGTH       	256
#define BOOT_FLASH_LENGTH  			(RECE_FLASH_LENGTH/2)

#define RECE_FLASH_CMD_LENGTH         (1 + 1 + 2 + RECE_FLASH_LENGTH + 1) // 1 start + 1 order + 2 data_index + 256data + 1 XOR
#define RECE_OTHER_CMD_LENGTH         32 // 1 start + 1 order  + 29data + 1 XOR

/*****************the command of update flash**********************/
#define	KEY_RECE_CMD			0x24
#define	ERASE_FLASH_CMD			0x25
#define	DOWNLOAD_FLASH_CMD  	0x27
#define	UPDATE_FINISHED_CMD		0x28

#define FAULT_STEP           	0
#define	KEY_RECE_STEP			1
#define	ERASE_FLASH_STEP		2
#define	DOWNLOAD_FLASH_STEP		3
#define	UPDATE_FINISHED_STEP	4

#define	BOOT_DSP_ADDR		0xAA

#define	SUCCESSED				0x00
#define	FAILED					0x01
#define	FAILED_XOR				0x02


static void BootDspBoardInit(void);
static void BootInitFlashSectors(void);
static void BootMemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);
static void BootInitPeripheralClocks(void);
static void BootInitSysPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel);
static void BootInitGpio(void);
static void BootInitScid(void);
static void BootGPIO_SetupPinMux(Uint16 pin, Uint16 cpu, Uint16 peripheral);
static void BootGPIO_SetupPinOptions(Uint16 pin, Uint16 output, Uint16 flags);
static Uint16 uwCalcXORCheck(Uint16 *puwBuf, Uint16 uwBufLen);
static void BootDspUpdate(void);

#pragma DATA_SECTION(uwDspSciRxBuf, "bootbufram")
Uint16 uwDspSciRxBuf[RECE_FLASH_CMD_LENGTH];

#pragma DATA_SECTION(uwDspSciTxBuf, "bootbufram")
Uint16 uwDspSciTxBuf[RECE_OTHER_CMD_LENGTH];

#pragma DATA_SECTION(uwDspFlashBuf, "bootbufram")
Uint16 uwDspFlashBuf[BOOT_FLASH_LENGTH];


#pragma CODE_SECTION(BootLoader,"codeupdate");
void BootLoader(void)
{
	BootDspBoardInit();
	BootInitFlashSectors();
	BootDspUpdate();

}

#pragma CODE_SECTION(BootDspUpdate,"codeupdateuser");
static void BootDspUpdate(void)
{
	Uint16 	i,j;
	Uint32 	udSciOverTimeTmp=0;
	Uint16	uwXorCheckResultTmp=0;
	Uint16 	uwSciDataIndexTmp=0, uwSciRxDataLenTmp=0;
	Uint16	uwFlashStatusTmp=0,uwFlashStepTmp=0;
	Uint32	udDspFlashAddrTmp=0,udDspFlashAddrOldTmp=0;
	Uint16	uwDspResetFlagTmp=0;

	Fapi_StatusType oReturnCheck;
	Uint32 udSectorAddress[6];

	/**********************bootloader variable init**********************************/
	udSciOverTimeTmp = 0;
	uwSciDataIndexTmp = 0;
	uwXorCheckResultTmp = 0;
	uwSciRxDataLenTmp = RECE_OTHER_CMD_LENGTH;
	uwFlashStatusTmp = FAILED;
	uwFlashStepTmp = FAULT_STEP;
	uwDspResetFlagTmp = 0;


	udSectorAddress[0]  = Bzero_SectorB_start;
	udSectorAddress[1]  = Bzero_SectorC_start;
	udSectorAddress[2]  = Bzero_SectorD_start;
	udSectorAddress[3]  = Bzero_SectorE_start;
	udSectorAddress[4]  = Bzero_SectorF_start;
	udSectorAddress[5]  = Bzero_SectorG_start;

	while(1)
	{
		/**************received data from the sci*************/
		while(1)
		{

			ScidRegs.SCICTL1.bit.RXENA = 1;		// SCI receiver enable
			if(0 == ScidRegs.SCIRXST.bit.RXRDY)	// Receiver ready flag
			{
				// Break-detect flag || Receiver error flag
				if((1==ScidRegs.SCIRXST.bit.BRKDT)||(1==ScidRegs.SCIRXST.bit.RXERROR))
				{
					//  SCID Re-Init
					BootInitScid();
				}
				udSciOverTimeTmp++;
				if(udSciOverTimeTmp>6000000)		// over time 3s
				{
					udSciOverTimeTmp = 0;
					GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;		// led low level
					for(i=0;i<RECE_FLASH_CMD_LENGTH;i++)
					{
						uwDspSciRxBuf[i]=0;
					}

					for(i=0;i<BOOT_FLASH_LENGTH;i++)
					{
						uwDspFlashBuf[i]=0;
					}

					uwSciDataIndexTmp=0;
				}
				continue;			// End the cycle
			}
			udSciOverTimeTmp=0;

			/**************read data from the  SCID register*************/
			uwDspSciRxBuf[uwSciDataIndexTmp]=ScidRegs.SCIRXBUF.bit.SAR;

			if((0==uwSciDataIndexTmp)&&(uwDspSciRxBuf[0]!=BOOT_DSP_ADDR))		// byte 0
			{
				uwSciDataIndexTmp = 0;
				continue;			// End the cycle,The index does not increase
			}
			else if(1==uwSciDataIndexTmp)							// byte 1
			{
				if((KEY_RECE_CMD==uwDspSciRxBuf[1])
				|| (ERASE_FLASH_CMD==uwDspSciRxBuf[1])
				|| (UPDATE_FINISHED_CMD==uwDspSciRxBuf[1]))
				{
					uwSciRxDataLenTmp = RECE_OTHER_CMD_LENGTH;
				}
				else if(DOWNLOAD_FLASH_CMD==uwDspSciRxBuf[1])
				{
					uwSciRxDataLenTmp = RECE_FLASH_CMD_LENGTH;
				}
				else
				{
					uwSciDataIndexTmp = 0;
					uwSciRxDataLenTmp = RECE_OTHER_CMD_LENGTH;
					continue;								// End the cycle
				}
			}
			else if(uwSciDataIndexTmp>=(uwSciRxDataLenTmp-1))	/***************data receive finish*******************/
			{
				uwXorCheckResultTmp = uwCalcXORCheck(uwDspSciRxBuf,(uwSciRxDataLenTmp-1));
				if(uwXorCheckResultTmp == uwDspSciRxBuf[uwSciDataIndexTmp])
				{
					uwFlashStatusTmp = SUCCESSED;
				}
				else
				{
					uwFlashStatusTmp = FAILED_XOR;
				}
				uwSciDataIndexTmp = 0;
				break;									// quit the cycle to send message
			}
			uwSciDataIndexTmp++;
		}	// while(1)
		GpioDataRegs.GPBTOGGLE.bit.GPIO35 = 1;			// blink led


		if(SUCCESSED==uwFlashStatusTmp)
		{
			/**************Update DSP Programmer Status from STM32*************/
			switch(uwDspSciRxBuf[1])
			{
				case KEY_RECE_CMD:	uwFlashStepTmp = KEY_RECE_STEP;	break;		// step1: receive key

				case ERASE_FLASH_CMD:											//step2: Erase Flash
					if(uwFlashStepTmp >= KEY_RECE_STEP)
					{
						uwFlashStepTmp = ERASE_FLASH_STEP;
					}
					else
					{
						uwFlashStepTmp = FAULT_STEP;
					}
				break;

				case DOWNLOAD_FLASH_CMD:										//step3: download flash files
					if(uwFlashStepTmp >= ERASE_FLASH_STEP)
					{
						uwFlashStepTmp = DOWNLOAD_FLASH_STEP;
					}
					else
					{
						uwFlashStepTmp = FAULT_STEP;
					}
				break;

				case UPDATE_FINISHED_CMD:									// step: check update status
					if(uwFlashStepTmp >= DOWNLOAD_FLASH_STEP)
					{
						uwFlashStepTmp = UPDATE_FINISHED_STEP;
					}
					else
					{
						uwFlashStepTmp = FAULT_STEP;
					}
				break;

				default:
					uwFlashStepTmp = FAULT_STEP;
				break;
			}//end of switch

			/**************Update DSP Programmer Status from step*************/
			switch(uwFlashStepTmp)
			{
				case FAULT_STEP:	uwFlashStatusTmp = FAILED;		break;

				case KEY_RECE_STEP:
				{
					oReturnCheck = Fapi_Status_FsmBusy;
					uwFlashStatusTmp = SUCCESSED;
				}
				break;

				case ERASE_FLASH_STEP:			// Erase DSP Flash
					EALLOW;
					if(Fapi_Status_Success == oReturnCheck)		// Prevent repeated erase dsp flash
					{
						uwFlashStatusTmp = SUCCESSED;
					}
					else
					{

						for(j = 0; j < 3; j++)		// Maximum erase 3 times
						{
							// Erase FLASHB-G
							for(i = 0; i < 6; i++)
							{

								oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,(Uint32 *)(udSectorAddress[i]));
								while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}
								if(Fapi_Status_Success != oReturnCheck)
								{
									uwFlashStatusTmp = FAILED;
									break;
								}
							}
							// Erase successfully, jump out of the for loop
							if(Fapi_Status_Success == oReturnCheck)
							{
								uwFlashStatusTmp = SUCCESSED;
								break;
							}
							else
							{
								uwFlashStatusTmp = FAILED;
							}
						}
					}
					EDIS;
					udDspFlashAddrOldTmp = 0;
					break;

				case DOWNLOAD_FLASH_STEP:				// Download DSP Flash
					udDspFlashAddrTmp = (Uint32)(uwDspSciRxBuf[2]*256 + uwDspSciRxBuf[3])*128;
					oReturnCheck = Fapi_Status_FsmBusy;
					EALLOW;
					if(udDspFlashAddrTmp <= (udDspFlashAddrOldTmp + 128))
					{
						if(((udDspFlashAddrTmp+Bzero_SectorA_start) >= Bzero_SectorB_start)			// 0x82000
						 &&((udDspFlashAddrTmp+Bzero_SectorA_start+128) <= Bzero_SectorH_start))	// 0xA0000
						{
							for(i=0,j=0;i<128;i++)
							{
								uwDspFlashBuf[i] = (uwDspSciRxBuf[(i*2)+5]*256)+(uwDspSciRxBuf[(i*2)+4]);	// Bin Files

								j++;
								if(j >= 8)
								{
									j = 0;
									oReturnCheck = Fapi_issueProgrammingCommand((Uint32 *)(udDspFlashAddrTmp+Bzero_SectorA_start+i-7),
											(Uint16 *)(uwDspFlashBuf+i-7),8, 0, 0, Fapi_AutoEccGeneration);
									while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);

									if(Fapi_Status_Success == oReturnCheck)
									{
										uwFlashStatusTmp = SUCCESSED;
									}
									else
									{
										uwFlashStatusTmp = FAILED;
										break;
									}
								}
							}
						}
						else if(((udDspFlashAddrTmp+Bzero_SectorA_start) >= Bzero_SectorA_start)			// 0x80000
							  &&((udDspFlashAddrTmp+Bzero_SectorA_start+128) <= Bzero_SectorN_End))			// 0xC0000
						{
							uwFlashStatusTmp = SUCCESSED;
						}
						else
						{
							uwFlashStatusTmp = FAILED;
						}
						udDspFlashAddrOldTmp = udDspFlashAddrTmp;

					} // if(udDspFlashAddrTmp <= (udDspFlashAddrOldTmp + 128))
					else
					{
						uwFlashStatusTmp = FAILED;
					}
					EDIS;
					break;

				case UPDATE_FINISHED_STEP:
					uwFlashStatusTmp = SUCCESSED;
					uwDspResetFlagTmp = 1;
					break;

				default:
					uwFlashStatusTmp = FAILED;
				break;

			}	// switch(uwFlashStepTmp)
		}	//if(SUCCESSED==uwFlashStatusTmp)
		else
		{
			uwFlashStatusTmp = FAILED_XOR;
		}
		uwDspSciTxBuf[0] = BOOT_DSP_ADDR;
		uwDspSciTxBuf[1] = uwDspSciRxBuf[1];
		uwDspSciTxBuf[2] = uwFlashStatusTmp;

		for(i=3; i<RECE_OTHER_CMD_LENGTH; i++)
		{
			uwDspSciTxBuf[i] = 0xFF;
		}
		if(DOWNLOAD_FLASH_CMD==uwDspSciTxBuf[1])	// download flash
		{
			uwDspSciTxBuf[3] = (udDspFlashAddrTmp>>7)>>8;
			uwDspSciTxBuf[4] = (udDspFlashAddrTmp>>7)&0xff;
		}
		uwDspSciTxBuf[RECE_OTHER_CMD_LENGTH-1] = uwCalcXORCheck(uwDspSciTxBuf, (RECE_OTHER_CMD_LENGTH-1));

		for(i=0; i<RECE_OTHER_CMD_LENGTH; i++)
		{
			ScidRegs.SCITXBUF.bit.TXDT = uwDspSciTxBuf[i];
			while(ScidRegs.SCICTL2.bit.TXEMPTY==0){};
		}

		if(1==uwDspResetFlagTmp)
		{
			uwDspResetFlagTmp = 0;
			for(i=0;i<60000;i++);		// Delay 1 s in order to send the end command

			// Enable Watchdog Control Register for reset dsp
			EALLOW;
			WdRegs.WDCR.all = 0x0028;
			WdRegs.WDKEY.bit.WDKEY = 0x0055;
			WdRegs.WDKEY.bit.WDKEY = 0x00AA;
			EDIS;

			while(1);			// Waiting for Watchdog reset
		}
	}// while(1)
}

#pragma CODE_SECTION(BootDspBoardInit,"codeupdate");
static void BootDspBoardInit(void)
{
	volatile Uint16 temp;

	/*DisableDog - This function disables the watchdog timer.*/
	DINT;
    EALLOW;
    temp = WdRegs.WDCR.all & 0x0007;
    WdRegs.WDCR.all = 0x0068 | temp;
	EDIS;

	/*Copy time critical code and Flash setup code to RAM.*/
	BootMemCopy(&FlashAPILoadStart, &FlashAPILoadEnd, &FlashAPIRunStart);

	/*This function initializes the PLL registers.*/
	BootInitSysPll(XTAL_OSC,IMULT_12,FMULT_0,PLLCLK_BY_2);
	/*This function initializes the Peripheral Clock registers.*/
	BootInitPeripheralClocks();

	BootInitGpio();

	BootInitScid();
}

#pragma CODE_SECTION(BootMemCopy,"codeupdate");
static void BootMemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

//
// BootInitFlashSectors - Initialize flash sectors
//
#pragma CODE_SECTION(BootInitFlashSectors,"codeupdate");
static void BootInitFlashSectors(void)
{
    Fapi_StatusType oReturnCheck;
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 120);
    if(oReturnCheck != Fapi_Status_Success)
    {
      __asm("    ESTOP0");
    }
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if(oReturnCheck != Fapi_Status_Success)
    {
      __asm("    ESTOP0");
    }
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;
    EDIS;
}


//
// InitSysPll - This function initializes the PLL registers.
//
// Note: The internal oscillator CANNOT be used as the PLL source if the
// PLLSYSCLK is configured to frequencies above 194 MHz.
//
// Note: This function uses the Watchdog as a monitor for the PLL. The user
// watchdog settings will be modified and restored upon completion.
//
#pragma CODE_SECTION(BootInitSysPll,"codeupdate");
static void BootInitSysPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel)
{
    Uint16 SCSR, WDCR, WDWCR, intStatus;
    if((clock_source == ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL)    &&
       (imult        == ClkCfgRegs.SYSPLLMULT.bit.IMULT)           &&
       (fmult        == ClkCfgRegs.SYSPLLMULT.bit.FMULT)           &&
       (divsel       == ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV))
    {
        //
        // Everything is set as required, so just return
        //
        return;
    }

    if(clock_source != ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL)
    {
        switch (clock_source)
        {
            case INT_OSC1:
            	EALLOW;
				ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 2;     // Clk Src = INTOSC1
				EDIS;
                break;

            case INT_OSC2:
            	EALLOW;
				ClkCfgRegs.CLKSRCCTL1.bit.INTOSC2OFF=0;         // Turn on INTOSC2
				ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0;     // Clk Src = INTOSC2
				EDIS;
                break;

            case XTAL_OSC:
				EALLOW;
				ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;            // Turn on XTALOSC
				ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1;     // Clk Src = XTAL
				EDIS;
                break;
        }
    }

    EALLOW;
    if(imult != ClkCfgRegs.SYSPLLMULT.bit.IMULT ||
       fmult != ClkCfgRegs.SYSPLLMULT.bit.FMULT)
    {
        Uint16 i;

        //
        // This bit is reset only by POR
        //
        if(DevCfgRegs.SYSDBGCTL.bit.BIT_0 == 1)
        {
            //
            // The user can optionally insert handler code here. This will only
            // be executed if a watchdog reset occurred after a failed system
            // PLL initialization. See your device user's guide for more
            // information.
            //
            // If the application has a watchdog reset handler, this bit should
            // be checked to determine if the watchdog reset occurred because
            // of the PLL.
            //
            // No action here will continue with retrying the PLL as normal.
            //
        }

        //
        // Bypass PLL and set dividers to /1
        //
        ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
        asm(" RPT #20 || NOP");
        ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0;

        //
        // Lock the PLL five times. This helps ensure a successful start.
        // Five is the minimum recommended number. The user can increase this
        // number according to allotted system initialization time.
        //
        for(i = 0; i < 5; i++)
        {
            //
            // Turn off PLL
            //
            ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
            asm(" RPT #20 || NOP");

            //
            // Write multiplier, which automatically turns on the PLL
            //
            ClkCfgRegs.SYSPLLMULT.all = ((fmult << 8U) | imult);

            //
            // Wait for the SYSPLL lock counter
            //
            while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1)
            {
                //
                // Uncomment to service the watchdog
                //
                // ServiceDog();
            }
        }
    }

    //
    // Set divider to produce slower output frequency to limit current increase
    //
    if(divsel != PLLCLK_BY_126)
    {
         ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel + 1;
    }else
    {
         ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;
    }

    //
    //      *CAUTION*
    // It is recommended to use the following watchdog code to monitor the PLL
    // startup sequence. If your application has already cleared the watchdog
    // SCRS[WDOVERRIDE] bit this cannot be done. It is recommended not to clear
    // this bit until after the PLL has been initiated.
    //

    //
    // Backup User Watchdog
    //
    SCSR = WdRegs.SCSR.all;
    WDCR = WdRegs.WDCR.all;
    WDWCR = WdRegs.WDWCR.all;

    //
    // Disable windowed functionality, reset counter
    //
    EALLOW;
    WdRegs.WDWCR.all = 0x0;
    WdRegs.WDKEY.bit.WDKEY = 0x55;
    WdRegs.WDKEY.bit.WDKEY = 0xAA;

    //
    // Disable global interrupts
    //
    intStatus = __disable_interrupts();

    //
    // Configure for watchdog reset and to run at max frequency
    //
    WdRegs.SCSR.all = 0x0;
    WdRegs.WDCR.all = 0x28;

    //
    // This bit is reset only by power-on-reset (POR) and will not be cleared
    // by a WD reset
    //
    DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 1;

    //
    // Enable PLLSYSCLK is fed from system PLL clock
    //
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;

    //
    // Delay to ensure system is clocking from PLL prior to clearing status bit
    //
    asm(" RPT #20 || NOP");

    //
    // Clear bit
    //
    DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 0;

    //
    // Restore user watchdog, first resetting counter
    //
    WdRegs.WDKEY.bit.WDKEY = 0x55;
    WdRegs.WDKEY.bit.WDKEY = 0xAA;

    WDCR |= 0x28;                     // Setup WD key--KEY bits always read 0
    WdRegs.WDCR.all = WDCR;
    WdRegs.WDWCR.all = WDWCR;
    WdRegs.SCSR.all = SCSR & 0xFFFE;  // Mask write to bit 0 (W1toClr)

    //
    // Restore state of ST1[INTM]. This was set by the __disable_interrupts()
    // intrinsic previously.
    //
    if(!(intStatus & 0x1))
    {
        EINT;
    }

    //
    // Restore state of ST1[DBGM]. This was set by the __disable_interrupts()
    // intrinsic previously.
    //
    if(!(intStatus & 0x2))
    {
        asm(" CLRC DBGM");
    }

    //
    // 200 PLLSYSCLK delay to allow voltage regulator to stabilize prior
    // to increasing entire system clock frequency.
    //
    asm(" RPT #200 || NOP");

    //
    // Set the divider to user value
    //
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;
    EDIS;
}

// PLLSYSCLK(120MHz)	:To GS RAMs,GPIOs,and NMIWDs,ECAP
// CPU1.CPUCLK(120MHz)	:To local memories
// CPU1.SYSCLK(120MHz)	:To ePIEs,LS RAMs,CLA message RAMs,and DCSMs
// LSPCLK(30MHz)		:To SCIs,SPIs,and McBSPs
// EPWMCLK(120MHz)		:To ePWMs
#pragma CODE_SECTION(BootInitPeripheralClocks,"codeupdate");
static void BootInitPeripheralClocks(void)
{
    EALLOW;
    // EPWM Clock  = PLLSYSCLK / 1 = 120MHz
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 0;
    // LSP Clock  = PLLSYSCLK / 4 = 30MHz
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0x02;

    CpuSysRegs.PCLKCR0.bit.CLA1 = 0;
    CpuSysRegs.PCLKCR0.bit.DMA = 0;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 0;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER1 = 0;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER2 = 0;
    CpuSysRegs.PCLKCR0.bit.HRPWM = 0;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    CpuSysRegs.PCLKCR1.bit.EMIF1 = 0;

    CpuSysRegs.PCLKCR2.bit.EPWM1 = 0;		// R-Phase 1,3
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 0;		// R-Phase 2,4
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 0;		// S-Phase 1,3
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 0;		// S-Phase 2,4
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 0;		// reserve
    CpuSysRegs.PCLKCR2.bit.EPWM6 = 0;		// T-Phase 1,3
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 0;		// T-Phase 2,4
    CpuSysRegs.PCLKCR2.bit.EPWM8 = 0;		// BOOST-1
    CpuSysRegs.PCLKCR2.bit.EPWM9 = 0;		// BOOST-2
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 0;		// BOOST-3
    CpuSysRegs.PCLKCR2.bit.EPWM11 = 0;		// BOOST-4
    CpuSysRegs.PCLKCR2.bit.EPWM12 = 0;		// fan pwm
    CpuSysRegs.PCLKCR3.bit.ECAP1 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP2 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP3 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP4 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP5 = 0;
    CpuSysRegs.PCLKCR3.bit.ECAP6 = 0;

    CpuSysRegs.PCLKCR4.bit.EQEP1 = 0;
    CpuSysRegs.PCLKCR4.bit.EQEP2 = 0;
    CpuSysRegs.PCLKCR4.bit.EQEP3 = 0;

    CpuSysRegs.PCLKCR6.bit.SD1 = 0;
    CpuSysRegs.PCLKCR6.bit.SD2 = 0;

    CpuSysRegs.PCLKCR7.bit.SCI_A = 0;		// SCI_A -> DEBUG
    CpuSysRegs.PCLKCR7.bit.SCI_B = 0;		// SCI_B -> F042
    CpuSysRegs.PCLKCR7.bit.SCI_C = 0;
    CpuSysRegs.PCLKCR7.bit.SCI_D = 1;		// SCI_D -> F107

    CpuSysRegs.PCLKCR8.bit.SPI_A = 0;
    CpuSysRegs.PCLKCR8.bit.SPI_B = 0;
    CpuSysRegs.PCLKCR8.bit.SPI_C = 0;

    CpuSysRegs.PCLKCR9.bit.I2C_A = 0;
    CpuSysRegs.PCLKCR9.bit.I2C_B = 0;

    CpuSysRegs.PCLKCR10.bit.CAN_A = 0;
    CpuSysRegs.PCLKCR10.bit.CAN_B = 0;

    CpuSysRegs.PCLKCR11.bit.McBSP_A = 0;
    CpuSysRegs.PCLKCR11.bit.McBSP_B = 0;
    CpuSysRegs.PCLKCR11.bit.USB_A = 0;

    CpuSysRegs.PCLKCR13.bit.ADC_A = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 0;

    CpuSysRegs.PCLKCR14.bit.CMPSS1 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS2 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS3 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS4 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS5 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS6 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS7 = 0;
    CpuSysRegs.PCLKCR14.bit.CMPSS8 = 0;

    CpuSysRegs.PCLKCR16.bit.DAC_A = 0;
    CpuSysRegs.PCLKCR16.bit.DAC_B = 0;
    CpuSysRegs.PCLKCR16.bit.DAC_C = 0;

    EDIS;
}

// InitSysPll - This function initializes the GPIO registers.
#pragma CODE_SECTION(BootInitGpio,"codeupdate");
static void BootInitGpio(void)
{
    volatile Uint32 *gpioBaseAddr;
    Uint16 regOffset;

    //
    //Disable pin locks
    //
    EALLOW;
    GpioCtrlRegs.GPALOCK.all = 0x00000000;
    GpioCtrlRegs.GPBLOCK.all = 0x00000000;
    GpioCtrlRegs.GPCLOCK.all = 0x00000000;
    GpioCtrlRegs.GPDLOCK.all = 0x00000000;
    GpioCtrlRegs.GPELOCK.all = 0x00000000;
    GpioCtrlRegs.GPFLOCK.all = 0x00000000;

    //
    //Fill all registers with zeros. Writing to each register separately
    //for six GPIO modules would make this function *very* Uint32. Fortunately,
    //we'd be writing them all with zeros anyway, so this saves a lot of space.
    //
    gpioBaseAddr = (Uint32 *)&GpioCtrlRegs;
    for (regOffset = 0; regOffset < sizeof(GpioCtrlRegs)/2; regOffset++)
    {
        //
        //Hack to avoid enabling pull-ups on all pins. GPyPUD is offset
        //0x0C in each register group of 0x40 words. Since this is a
        //32-bit pointer, the addresses must be divided by 2.
        //
        if (regOffset % (0x40/2) != (0x0C/2))
        {
            gpioBaseAddr[regOffset] = 0x00000000;
        }
    }

    gpioBaseAddr = (Uint32 *)&GpioDataRegs;
    for (regOffset = 0; regOffset < sizeof(GpioDataRegs)/2; regOffset++)
    {
        gpioBaseAddr[regOffset] = 0x00000000;
    }

    EDIS;


	//GPIO24	OutputPin	(FAN PWM PIN)
	BootGPIO_SetupPinMux(24, GPIO_MUX_CPU1, 0);
	BootGPIO_SetupPinOptions(24, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPASET.bit.GPIO24 = 1;

    //GPIO35	OutputPin	(LED)
	BootGPIO_SetupPinMux(35, GPIO_MUX_CPU1, 0);
	BootGPIO_SetupPinOptions(35, GPIO_OUTPUT, GPIO_PUSHPULL);
	GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;

    //GPIO93&94	SCID_Pin	(communicate with STM32F107)
	BootGPIO_SetupPinMux(93, GPIO_MUX_CPU1, 6);
	BootGPIO_SetupPinOptions(93, GPIO_OUTPUT, GPIO_ASYNC);
	BootGPIO_SetupPinMux(94, GPIO_MUX_CPU1, 6);
	BootGPIO_SetupPinOptions(94, GPIO_INPUT, GPIO_PUSHPULL);
}

// InitSysPll - This function initializes the SCID registers.
#pragma CODE_SECTION(BootInitScid,"codeupdate");
static void BootInitScid(void)
{
	Uint16 i;
    // 15 SCI reset rx/tx channels,FIFO reset
	ScidRegs.SCIFFTX.all = 0xA000;
	// FIFO overflow
	ScidRegs.SCIFFRX.all = 0x201F;
	ScidRegs.SCIFFCT.all = 0;

	ScidRegs.SCICTL1.bit.SWRESET=0;
	for(i=0;i<20;i++);
	ScidRegs.SCICTL1.bit.SWRESET=1;
	/*2:0 Character length control
	3 ADDR/IDLE Mode control  - 8 Char
	4 Loop Back enable        -disable
	5 Parity enable           -disable
	6 Even or Odd Parity      - No parity
	7 Number of Stop Bits     - One Stop Bits*/
	ScidRegs.SCICCR.all		= 0x0007;

	/*0 SCI receiver enable  - enable
	1 SCI transmitter enable - enable*/
	ScidRegs.SCICTL1.all 	= 0x0003;

	ScidRegs.SCIHBAUD.all 	= BOOT_BAUD_57600>>8;
	ScidRegs.SCILBAUD.all 	= BOOT_BAUD_57600 & 0x00ff;

	/* 1 Receiver-buffer break enable - disable*/
	ScidRegs.SCICTL2.all	= 0x00;
	/*Software reset */
	ScidRegs.SCICTL1.all 	= 0x0023;     // Relinquish SCI from Reset
}

//
// BootGPIO_SetupPinMux - Set the peripheral muxing for the specified pin.
//                    The appropriate parameters can be found in the GPIO
//                    Muxed Pins table in the datasheet. Use the GPIO index
//                    row (0 to 15) to select a muxing option for the GPIO.
//
#pragma CODE_SECTION(BootGPIO_SetupPinMux,"codeupdate");
static void BootGPIO_SetupPinMux(Uint16 pin, Uint16 cpu, Uint16 peripheral)
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
#pragma CODE_SECTION(BootGPIO_SetupPinOptions,"codeupdate");
static void BootGPIO_SetupPinOptions(Uint16 pin, Uint16 output, Uint16 flags)
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

#pragma CODE_SECTION(uwCalcXORCheck,"codeupdate");
Uint16 uwCalcXORCheck(Uint16 *puwBuf, Uint16 uwBufLen)
{
	Uint16 uwXORCheckResultTmp = 0;

	while(uwBufLen--)
	{
		uwXORCheckResultTmp ^= (*(puwBuf++))&0xff;
	}
	return uwXORCheckResultTmp;
}

