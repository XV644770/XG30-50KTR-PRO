/*
* task_ACOutput.h
*
*	Product:			three phase inveter
*	Filename:			system_periph_init.h
*	Purpose:			Stm32 system init
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef TASK_OUTPUT_H
#define TASK_OUTPUT_H


#include "calc_ac_rms.h"
#include "safty_load_limit.h"
#include "safty_grid_freq_check.h"
#include "safty_grid_volt_check.h"
#include "safty_fan_ctrl.h"
#include "safty_dci_check.h"
#include "safty_gfci_check.h"
#include "reactive_power_ctrl.h"
#include "safty_auto_test.h"
#include "safty_freq_derating.h"

extern void ACOutputTaskParaInit(void);
extern void ACOutputTask(void);

#endif /* SYSTEM_PERIPH_INIT_H */
