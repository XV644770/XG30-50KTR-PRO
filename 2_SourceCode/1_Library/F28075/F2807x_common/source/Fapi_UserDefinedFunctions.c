//###########################################################################
//
// FILE:    Fapi_UserDefinedFunctions.c
//
// Description: Contains all user defined functions that the Fapi functions
//              use.
//
//###########################################################################
// $TI Release: F2807x Support Library v200 $
// $Release Date: Tue Jun 21 14:11:45 CDT 2016 $
// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Uncomment the appropriate include file for your device
//

// #include "F021_FMC_BE.h"
// #include "F021_FMC_LE.h"
// #include "F021_Concerto_C28x.h"
// #include "F021_Concerto_Cortex.h"
#include "F021_F2837xD_C28x.h"

//
// Fapi_serviceWatchdogTimer - Watchdog servicing function
//
#pragma CODE_SECTION(Fapi_serviceWatchdogTimer,"codeupdate");
Fapi_StatusType Fapi_serviceWatchdogTimer(void)
{
   //
   // User to add their own watchdog servicing code here
   //

   return(Fapi_Status_Success);
}

//
// Fapi_setupEepromSectorEnable - Enables EEPROM sectors for bank and sector
//                                erase
//
#pragma CODE_SECTION(Fapi_setupEepromSectorEnable,"codeupdate");
Fapi_StatusType Fapi_setupEepromSectorEnable(void)
{
   //
   // Value must be 0xFFFF to enable erase and programming of the
   // EEPROM bank, 0 to disable
   //
   Fapi_GlobalInit.m_poFlashControlRegisters->Fbse.u32Register = 0xFFFF;

   //
   // Enables sectors 32-63 for bank and sector erase
   //
   FAPI_WRITE_LOCKED_FSM_REGISTER(Fapi_GlobalInit.m_poFlashControlRegisters->FsmSector.u32Register,
                                  0x0U);

   //
   // Enables sectors 0-31 for bank and sector erase
   //
   FAPI_WRITE_LOCKED_FSM_REGISTER(Fapi_GlobalInit.m_poFlashControlRegisters->FsmSector1.u32Register,
                                  0x0U);

   //
   // Enables sectors 32-63 for bank and sector erase
   //
   FAPI_WRITE_LOCKED_FSM_REGISTER(Fapi_GlobalInit.m_poFlashControlRegisters->FsmSector2.u32Register,
                                  0x0U);

   return(Fapi_Status_Success);
}

//
// Fapi_setupBankSectorEnable - Setup and enable bank sectors 0-15
//
#pragma CODE_SECTION(Fapi_setupBankSectorEnable,"codeupdate");
Fapi_StatusType Fapi_setupBankSectorEnable(void)
{
   //
   // Enable sectors 0-15 for erase and programming
   //
   Fapi_GlobalInit.m_poFlashControlRegisters->Fbse.u32Register = 0xFFFF;
   FAPI_WRITE_LOCKED_FSM_REGISTER(Fapi_GlobalInit.m_poFlashControlRegisters->FsmSector.u32Register,
                                  0x0U);

   return(Fapi_Status_Success);
}

//
// End of file
//

