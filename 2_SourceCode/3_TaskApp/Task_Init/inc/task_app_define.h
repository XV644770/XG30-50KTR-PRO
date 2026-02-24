/******************************************************************
*	Copyright (c) 2020-2021, Invt Co.Lrd.
*	ALL  RIGHTS  RESERVED

*	Product:           three phase inveter
*	Filename:        task_app_define.h
*	Purpose:          Stm32 system init
*	Date              Version           Author            Note
*   20/11/18          V1.01             csx            none
***************************************************************/


#ifndef TASK_APP_DEFINE_H
#define TASK_APP_DEFINE_H


/*******************************************************************/
/*						Task priority definition         		   */
/*******************************************************************/
#define STATUS_PRIO				0
#define CONTROL_LOOP_PRIO		1
#define PV_INPUT_PRIO			2
#define AC_OUTPUT_PRIO			3
#define COM_F107_PRIO			4
#define COM_F042_PRIO			5
#define COM_DEBUG_PRIO			6
#define COM_AFCI_PRIO			7

/*******************************************************************/
/*          		task's event definition						   */
/*******************************************************************/

// super task's event
#define STATUS_TASK_PERIOD		0x00		// 10ms
#define STATUS_TO_WAIT			0x01
#define STATUS_TO_INVERTER		0x02
#define STATUS_TO_FAULT			0x03
#define STATUS_TO_UPDATE		0x04

//ControlLoop task's event
#define CONTROL_LOOP_PERIOD	0x00		// 1ms

//PVinput task's event
#define PVINPUT_TASK_PERIOD		0x00		// 20ms
#define PVINPUT_GRID_ZERO		0x01

//GridOutput task's event
#define ACOUTPUT_TASK_PERIOD	0x00		// 20ms
#define ACOUTPUT_GRID_ZERO		0x01
#define ACOUTPUT_GRID_HALF		0x02

// SCIB Task  Debug communication

// COM_F107_PRIO
#define COM_107_TIMER			0x00		// 100ms
#define COM_107_RX_ENDED		0x01

// COM_F042_PRIO
#define COM_F042_TIMER			0x00		// 100ms
#define COM_F042_RX_ENDED		0x01

// COM_AFCI_PRIO
#define COM_AFCI_TIMER			0x00		// 100ms
#define COM_AFCI_RX_ENDED		0x01






#endif

