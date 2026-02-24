/*
 * isr_init.h
 *
 *  Created on: 2020年12月31日
 *      Author: Administrator
 */

#ifndef ISR_INIT_H_
#define ISR_INIT_H_

#include "isr_adc.h"
#include "isr_boost_ctrl.h"
#include "isr_inv_ctrl.h"
#include "isr_pll.h"
#include "isr_protect.h"
#include "isr_pwm_gen.h"

// Limit max Value & min Value
//#define UPDNLMT16(Var,Max,Min)	{Var=(Var>=Max)?Max:Var;Var=(Var<=Min)?Min:Var;}
#define UPDNLMT16(Var,Max,Min)\
    if(Var > Max)\
    {\
        Var = Max;\
    }\
    if(Var < Min)\
    {\
        Var = Min;\
    }\

extern void IsrParaInit(void);



#endif /* ISR_INIT_H_ */
