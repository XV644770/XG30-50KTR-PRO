/*-------------------------------------------------------
* isr_init.c
*
*	Product:			three phase inveter
*	Filename:			isr_init.c
*	Purpose:			init the parameters to isr fcn
*	Date				Version		Author			Note
*   20/12/11			V1.01		csx			none
*--------------------------------------------------------*/
#include "dsp_user_include.h"

void IsrParaInit(void)
{
	BoostCtrlParaInit();
	InvCtrlParaInit();
	PwmGenParaInit();
	PLLParaInit();
}


//end of file


