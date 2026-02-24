//###########################################################################
//
// FILE:   F2807x_Sci.c
//
// TITLE:  F2807x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2807x Support Library v200 $
// $Release Date: Tue Jun 21 14:11:45 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F2807x_device.h"     // F2807x Headerfile Include File
#include "F2807x_Examples.h"   // F2807x Examples Include File

// SCI_BAUD_REG = [LSPCLK/(BAUDRATE*8)]-1
// LSPCLK = SYSCLK / LSPCLKDIV = 120MHz / 4 = 30MHz

// Function Prototypes
void InitScia(void);
void InitScib(void);
void InitScic(void);
void InitScid(void);

void InitSci()
{
	InitScia();
	InitScib();
	InitScic();			
	InitScid();
}


// Communication with  computer
void InitScia(void)
{
    /*2:0 Character length control
    3 ADDR/IDLE Mode control  - 8 Char
    4 Loop Back enable        -disable
    5 Parity enable           -disable
    6 Even or Odd Parity      - No parity
    7 Number of Stop Bits     - One Stop Bits*/
	SciaRegs.SCICCR.all		= 0x0007;

	/*0 SCI receiver enable  - enable
	1 SCI transmitter enable - enable*/
	SciaRegs.SCICTL1.all 	= 0x0003;

	SciaRegs.SCIHBAUD.all 	= DEBUG_BAUD>>8;
	SciaRegs.SCILBAUD.all 	= DEBUG_BAUD & 0x00ff;

	/* 1 Receiver-buffer break enable - enable*/
	SciaRegs.SCICTL2.all	= 0x03;
	/*Software reset */
	SciaRegs.SCICTL1.all 	= 0x0023;     // Relinquish SCI from Reset
}

// Communication with  STM32F042
void InitScib(void)
{
	 /*2:0 Character length control
	3 ADDR/IDLE Mode control  - 8 Char
	4 Loop Back enable        -disable
	5 Parity enable           -disable
	6 Even or Odd Parity      - No parity
	7 Number of Stop Bits     - One Stop Bits*/
	ScibRegs.SCICCR.all		= 0x0007;

	/*0 SCI receiver enable  - enable
	1 SCI transmitter enable - enable*/
	ScibRegs.SCICTL1.all 	= 0x0003;

	ScibRegs.SCIHBAUD.all 	= F042_BAUD>>8;
	ScibRegs.SCILBAUD.all 	= F042_BAUD & 0x00ff;

	/* 1 Receiver-buffer break enable - enable*/
	ScibRegs.SCICTL2.all	= 0x03;
	/*Software reset */
	ScibRegs.SCICTL1.all 	= 0x0023;     // Relinquish SCI from Reset
}


// Communication with  STM32F107
void InitScic(void)
{
	 /*2:0 Character length control
	3 ADDR/IDLE Mode control  - 8 Char
	4 Loop Back enable        -disable
	5 Parity enable           -disable
	6 Even or Odd Parity      - No parity
	7 Number of Stop Bits     - One Stop Bits*/
	ScicRegs.SCICCR.all		= 0x0007;

	/*0 SCI receiver enable  - enable
	1 SCI transmitter enable - enable*/
	ScicRegs.SCICTL1.all 	= 0x0003;

	ScicRegs.SCIHBAUD.all 	= DEBUG_BAUD>>8;
	ScicRegs.SCILBAUD.all 	= DEBUG_BAUD & 0x00ff;

	/* 1 Receiver-buffer break enable - enable*/
	ScicRegs.SCICTL2.all	= 0x03;
	/*Software reset */
	ScicRegs.SCICTL1.all 	= 0x0023;     // Relinquish SCI from Reset
}


// reserved
void InitScid(void)
{
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

	ScidRegs.SCIHBAUD.all 	= F107_BAUD>>8;
	ScidRegs.SCILBAUD.all 	= F107_BAUD & 0x00ff;

	/* 1 Receiver-buffer break enable - enable*/
	ScidRegs.SCICTL2.all	= 0x03;
	/*Software reset */
	ScidRegs.SCICTL1.all 	= 0x0023;     // Relinquish SCI from Reset
}
//
// End of file
//
