/************************************************************************************************
SANTAK ELECTRONIC Ltd										*
RTOS kernel for ups ,All rights reserved							*
VERSION:		V00									*
File name:		timer.C									*
Description:		timer tick ISR								*
Update Date:		2003/07/22								*
Writer:			ZD									*
*************************************************************************************************/

#include "OS_KERNEL.h"



#pragma CODE_SECTION(OSTimerTick,"ramfuncs");
interrupt void	OSTimerTick(void)
{
	OS_TCB	*ptcb;
	OS_TCB	*ptcbend;
	INTOS	prio;

#if	OS_TASK_IDLE_EN>0

	OSTime++;
	if(OSTime==OS_TICKS_PER_SEC)
	{
		OSTime=0;
		OSCpuFree=OSIdleCtr*OS_CPUFREE_FACTOR>>20;
		OSIdleCtr=0;
	}
#endif

	prio=0;
	ptcbend=&OSTCBTbl[OS_LOWEST_PRIO];
	ptcb=OSTCBTbl;
	while(ptcb<=ptcbend)
	{
		if(ptcb->TimerPeriod!=0)
		{

			ptcb->TimerCnt++;
			if(ptcb->TimerCnt>=ptcb->TimerPeriod)
			{
				ptcb->TimerCnt=0;

				{
					OSISREventSend(prio,OS_EVENTID_TIMER);
				}

			}
		}
		ptcb++;
		prio++;
	}
}
