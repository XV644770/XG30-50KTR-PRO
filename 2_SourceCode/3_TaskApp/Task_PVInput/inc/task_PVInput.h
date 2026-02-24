/*
* task_pv_input.h
*
*	Product:			three phase inveter
*	Filename:			system_periph_init.h
*	Purpose:			Stm32 system init
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef TASK_PVINPUT_H
#define TASK_PVINPUT_H



#include "calc_pv_avg.h"
#include "check_turn_on.h"
#include "calc_mppt_condition.h"
#include "run_mppt.h"
#include "safty_dc_sample.h"
#include "safty_iso_check.h"
#include "safty_others_check.h"
#include "IV_Curve.h"

#define PV_CONNECT_TIME			200		// 200*20ms = 4s
#define PV_DISCONNECT_TIME		50		// 50*20ms = 1s


extern void PVInputTask(void);
extern void PVInputTaskParaInit(void);

#endif /* SYSTEM_PERIPH_INIT_H */
