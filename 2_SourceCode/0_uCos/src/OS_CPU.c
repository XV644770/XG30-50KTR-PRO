/************************************************************************************************
SANTAK ELECTRONIC Ltd										*
RTOS kernel for ups ,All rights reserved							*
VERSION:		V00									*
File name:		OS_CPU.C								*
Description:		function related with cpu structure					*
Update Date:		2003/07/22								*
Writer:			ZD									*
*************************************************************************************************/

#include	"OS_CPU.h"
OS_STK	*stk;

extern	void c_int0(void);

OS_STK	*OSTaskStkInit(void (*task)(void), OS_STK *ptos)
{
//	OS_STK	*stk;
	stk=ptos;   			/* 		Load stack pointer          			*/
	
	*stk++= (INTOS) 0x0080;		/*		ST0=0						*/
	*stk++= (INTOS) 0x0000;		/*		T=0						*/
	*stk++= (INTOS)	0x0000;		/*		AL=0						*/
	*stk++= (INTOS) 0x0000;		/*		AH=0						*/
	*stk++= (INTOS) 0x0000;		/*		PL=0						*/
	*stk++= (INTOS) 0x0000;		/*		PH=0						*/
	*stk++= (INTOS) 0x0000;		/*		AR0=0						*/
	*stk++= (INTOS) 0x0000;		/*		AR1=0						*/
	*stk++= (INTOS) 0x0A0A;		/*		ST1=0
							Interrupts are enabled				*/
	*stk++= (INTOS) 0x0000;		/*		DP=0						*/
	*stk++= (INTOS) 0x0002|0x0004|0x0008|0x0080|0x0100|0x2000;

	*stk++= (INTOS)	0x0000;		/*		DBGSTAT=0					*/
	*stk++= (INT32U) task & 0x0000FFFF;		/*		PC:L H				*/
	*stk++= (INT32U) task >>16;
	
	*stk++= (INTOS) 0x0A0A;		/*		ST1=0					
							Interrupts are enabled				*/
	*stk++= (INTOS) 0x0000;		/*		DP=0						*/

	*stk++= (INTOS) 0x0000;		/*		RPC						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XT						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XAR7						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XAR6						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XAR5						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XAR4						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XAR3						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		XAR2						*/
	*stk++= (INTOS) 0x0000;		/*								*/
	*stk++= (INTOS) 0x0000;		/*		AR1H:AR0H						*/
	*stk++= (INTOS) 0x0000;		/*								*/

	stk++;
	return(stk);
}
