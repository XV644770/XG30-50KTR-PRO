//###########################################################################
//
// FILE:   F2807x_Sci_defines.h
//
// TITLE:  #defines used in SCI examples
//
//###########################################################################
// $TI Release: F2807x Support Library v200 $
// $Release Date: Tue Jun 21 14:11:45 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef F2807x_SCI_DEFINES_H
#define F2807x_SCI_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

//
// Defines
//

//
//definitions for baudrate SCI
//
// SCI_BAUD_REG = [LSPCLK/(BAUDRATE*8)]-1
// LSPCLK = SYSCLK / LSPCLKDIV = 120MHz / 4 = 30MHz
#define BAUD_9600	   390
#define BAUD_57600     64

#define DEBUG_BAUD		BAUD_9600
#define F107_BAUD		BAUD_57600
#define F042_BAUD		BAUD_57600

extern void InitSci();

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif   // - end of F2807x_SCI_DEFINES_H

//
// End of file
//
