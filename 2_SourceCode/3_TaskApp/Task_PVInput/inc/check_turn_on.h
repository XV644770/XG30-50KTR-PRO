/*
* CheckTurnOn.h
*
*	Product:			three phase inveter
*	Filename:			TurnOnCheck.h
*	Purpose:			Check the starting condition of inverter
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef CHECK_TURN_ON_H
#define CHECK_TURN_ON_H

typedef struct
{
	Uint16      uwStartDelayTime;
	Uint16      wExistInverterCount;
	Uint16      wReConnectConFlag;
} ST_TASK_TIMER;

extern ST_TASK_TIMER stTaskTimer;

extern Uint16 uwTurnOnInvtCheck(void);
extern void ConnectCountDown(void);

#endif /* SYSTEM_PERIPH_INIT_H */
