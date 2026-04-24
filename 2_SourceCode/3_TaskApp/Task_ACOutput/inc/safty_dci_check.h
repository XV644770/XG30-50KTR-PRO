/*
 * safty_dci_check.h
 *
 *  Created on: 2021年3月12日
 *      Author: Administrator
 */

#ifndef SAFTY_DCI_CHECK_H
#define SAFTY_DCI_CHECK_H

#define PHASE				2

typedef enum
{
   R_Phase,
   S_Phase
}  PHASE_ENUM;

extern void DCICtrlLoopInit(void);
extern void DCIAdjust(void);
extern void DCIProtection(void);

extern int16 wRDCI_PI,wSDCI_PI;
extern int16 wDCI_PI_SET;
extern int16 wDCI_Injection_Enable;

#endif /* 3_TASKAPP_TASK_ACOUTPUT_INC_SAFTY_DCI_CHECK_H_ */
