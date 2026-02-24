/*
* update_dsp.h
*
*	Product:			three phase inveter
*	Filename:			update_dsp.h
*	Purpose:			update_dsp
*	Date				Version		Author			Note
*   20/12/11			V1.01		chensx			none
*/
#ifndef UPDATE_DSP_H
#define UPDATE_DSP_H

#define Bzero_SectorN_start         0xBE000
#define Bzero_SectorN_End           0xBFFFF

#define Bzero_SectorM_start         0xBC000
#define Bzero_SectorM_End           0xBDFFF

#define Bzero_SectorL_start         0xBA000
#define Bzero_SectorL_End           0xBBFFF

#define Bzero_SectorK_start         0xB8000
#define Bzero_SectorK_End           0xB9FFF

#define Bzero_SectorJ_start         0xB0000
#define Bzero_SectorJ_End           0xB7FFF

#define Bzero_SectorI_start         0xA8000
#define Bzero_SectorI_End           0xAFFFF

#define Bzero_SectorH_start         0xA0000
#define Bzero_SectorH_End           0xA7FFF

#define Bzero_SectorG_start         0x98000
#define Bzero_SectorG_End           0x9FFFF

#define Bzero_SectorF_start         0x90000
#define Bzero_SectorF_End           0x97FFF

#define Bzero_SectorE_start         0x88000
#define Bzero_SectorE_End           0x8FFFF

#define Bzero_SectorD_start	        0x86000
#define Bzero_SectorD_End	        0x87FFF

#define Bzero_SectorC_start	        0x84000
#define Bzero_SectorC_End	        0x85FFF

#define Bzero_SectorB_start         0x82000
#define Bzero_SectorB_End	        0x83FFF

#define Bzero_SectorA_start	        0x80000
#define Bzero_SectorA_End           0x81FFF

#define Bzero_16KSector_u32length   0x1000
#define Bzero_64KSector_u32length   0x4000

extern void BootLoader(void);



#endif /* INCLUDE_CFG_SYSPARA_INIT_H */
