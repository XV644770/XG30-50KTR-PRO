/*
 * safty_gfci_check.h
 *
 *  Created on: 2021年5月21日
 *      Author: Administrator
 */


#ifndef SAFTY_GFCI_CHECK_H
#define SAFTY_GFCI_CHECK_H

extern void	GFCIOverCurrCheck(void);
extern void	GFCISelfCheck(void);

#define		GFCI_SELF_CHECK_MAX		3300	// 230mA
#define		GFCI_SELF_CHECK_MIN		1000	// 100mA

#define		GFCI_OFFSET_MAX				300	// 30mA

/********************GFCI**********************************/
#define		GFCI_3MA					30
#define		GFCI_27MA					270
#define		GFCI_30MA					300
#define		GFCI_57MA					570
#define		GFCI_60MA					600
#define		GFCI_100MA					1000
#define		GFCI_145MA					1450
#define		GFCI_150MA					1500
#define		GFCI_300MA					3000


#endif /* 3_TASKAPP_TASK_ACOUTPUT_INC_SAFTY_DCI_CHECK_H_ */
