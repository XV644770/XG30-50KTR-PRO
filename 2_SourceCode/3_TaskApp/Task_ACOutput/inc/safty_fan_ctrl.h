/*
* safty_fan_ctrl.h
*
*	Product:			three phase inveter
*	Filename:			TurnOnCheck.h
*	Purpose:			Check the starting condition of inverter
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef SAFTY_FAN_CTRL_H
#define SAFTY_FAN_CTRL_H

#define	FAN_NUM_MAX	4

#define FAN1_EXTR	0
#define FAN2_EXTR	1
#define FAN3_EXTR	2
#define FAN4_INTR	3

/*******Internal Fan Normal Frequency:138Hz*********/
/********FanFreq>138*1.6		Set Fault */
/********FanFreq<138*1.4		Clear Fault */

//#define FAN4_INTR_PERIOD_FAULT_50_70K		56			// 19200Hz/2/F(Fan)/2*1.6
//#define FAN4_INTR_PERIOD_RECORVE_50_70K		49			// 19200Hz/2/F(Fan)/2*1.4

#define FAN4_INTR_PERIOD_FAULT_50_70K		25			
#define FAN4_INTR_PERIOD_RECORVE_50_70K		20		


/*******External Fan Normal Freqency:182Hz(half-speed) 276Hz(full-speed)*********/
/********FanFreq>182*1.6		Set Fault */
/********FanFreq<182*1.4		Clear Fault */

//#define FAN123_EXTR_PERIOD_FAULT_50_70K		42			// 19200Hz/2/F(Fan)/2*1.6
//#define FAN123_EXTR_PREIOD_RECORVE_50_70K	37			// 19200Hz/2/F(Fan)/2*1.4
#define FAN123_EXTR_PERIOD_FAULT_50_70K		25			
#define FAN123_EXTR_PREIOD_RECORVE_50_70K	20			

/*******XG25-40kW Internal Fan Normal Frequency:133Hz*********/
/********FanFreq>133*1.6		Set Fault */
/********FanFreq<133*1.4		Clear Fault */

//#define FAN4_INTR_PERIOD_FAULT_25_40K		58			// 19200Hz/2/F(Fan)/2*1.6
//#define FAN4_INTR_PERIOD_RECORVE_25_40K		51			// 19200Hz/2/F(Fan)/2*1.4
#define FAN4_INTR_PERIOD_FAULT_25_40K		25			
#define FAN4_INTR_PERIOD_RECORVE_25_40K		20			

/*******XG25-40kW External Fan Standard************************************/
/*******External Fan Normal Freqency:127Hz(half-speed) 233Hz(full-speed)*********/
/********FanFreq>127*1.6		Set Fault */
/********FanFreq<127*1.4		Clear Fault */

//#define FAN123_EXTR_PERIOD_FAULT_25_40K		60			// 19200Hz/2/F(Fan)/2*1.6
//#define FAN123_EXTR_PREIOD_RECORVE_25_40K	52			// 19200Hz/2/F(Fan)/2*1.4
#define FAN123_EXTR_PERIOD_FAULT_25_40K		25			
#define FAN123_EXTR_PREIOD_RECORVE_25_40K	20			

#define FAN123_EXTR_PERIOD_FAULT_25_40K_Plus		90			
#define FAN123_EXTR_PREIOD_RECORVE_25_40K_Plus	80		

/*******XG15-25kW Internal Fan Normal Frequency:103Hz*********/
/********FanFreq>103*1.6        Set Fault */
/********FanFreq<103*1.4        Clear Fault */

//#define FAN4_INTR_PERIOD_FAULT_15_25K       75          // 19200Hz/2/F(Fan)/2*1.6
//#define FAN4_INTR_PERIOD_RECORVE_15_25K     65          // 19200Hz/2/F(Fan)/2*1.4
#define FAN4_INTR_PERIOD_FAULT_15_25K      25        
#define FAN4_INTR_PERIOD_RECORVE_15_25K     20          

/*******XG15-25kW External Fan Standard************************************/
/*******External Fan Normal Freqency:154Hz(half-speed) 307Hz(full-speed)*********/
/********FanFreq>154*1.6        Set Fault */
/********FanFreq<154*1.4        Clear Fault */

//#define FAN123_EXTR_PERIOD_FAULT_15_25K     50          // 19200Hz/2/F(Fan)/2*1.6
//#define FAN123_EXTR_PREIOD_RECORVE_15_25K   44          // 19200Hz/2/F(Fan)/2*1.4
#define FAN123_EXTR_PERIOD_FAULT_15_25K     25         
#define FAN123_EXTR_PREIOD_RECORVE_15_25K   20         

#define	FAN_FAULT_SET_TIME			16//12		// 16*500ms = 8s
#define	FAN_FAULT_CLR_TIME			12//10		// 10*500ms = 5s

#define	FAN_HALF_SPEED	1
#define	FAN_FULL_SPEED	2

#define FAN_SPEED_0	       0
#define FAN_SPEED_10	1
#define FAN_SPEED_20	2
#define FAN_SPEED_30	3
#define FAN_SPEED_40	4
#define FAN_SPEED_50	5
#define FAN_SPEED_60	6
#define FAN_SPEED_70	7
#define FAN_SPEED_80	8
#define FAN_SPEED_90	9
#define FAN_SPEED_100	10

typedef union
{
	Uint16  all;
	struct  ST_FLAG_BITS
	{
		Uint16	FreqFault:		1;		// bit0		0:(Fan Frequency normal)	1:(Fan Frequency abnormal)
		Uint16	OnOff:			4;		// bit1-4	0000:OFF 0001:10% ~ 1010:100%//	2;// bit1		00:OFF	01:(Half-Speed)		02:(Full-Speed)
		Uint16	FanTest:		4;		// bit5-8
		Uint16	rsv:				7;		// bit9-bit15
	}bit;
}UN_FLAG_BITS;


typedef struct
{
	UN_FLAG_BITS	unFlagBits[FAN_NUM_MAX];
	Uint16			uwFanPeriod[FAN_NUM_MAX];
}ST_FAN;

extern	ST_FAN	stFan;

extern	void FanApplication(void);
extern	void FanOnOffCtrl(void);
extern	void FanSpeedDetect(void);

#endif /* SAFTY_FAN_CTRL_H */
