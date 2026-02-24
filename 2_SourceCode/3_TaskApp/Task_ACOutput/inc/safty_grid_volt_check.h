/*
 * safty_grid_volt_check.h
 *
 *  Created on: 2021年1月9日
 *      Author: Administrator
 */

#ifndef SAFTY_GRID_VOLT_CHECK_H
#define SAFTY_GRID_VOLT_CHECK_H

#define PHASE_VOLT_MIN		200//1000		// 100V

extern void GridVoltProtection(void);
extern void TemperatureProtection(void);


#endif /* SAFTY_GRID_VOLT_CHECK_H_ */
