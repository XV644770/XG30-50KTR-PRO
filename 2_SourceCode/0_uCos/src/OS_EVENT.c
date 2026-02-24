/************************************************************************************************
KSTAR ELECTRONIC Ltd										*
RTOS kernel for ups ,All rights reserved							*
VERSION:		V00									*
File name:		OS_EVENT.C								*
Description:		EVENT OPERATION								*
Update Date:		2003/07/22								*
Writer:			ZD									*
*************************************************************************************************/

#include <OS_KERNEL.h>
/*
*************************************************************************************************
*                                             EVENT PEND					*
*												*
* Description: wait an event,if no event happened then suspend the task ,otherwise clear the 	*
*	       event in tcb and return the event						*
* Arguments  : none										*
*												*
* Returns    : event										*
*************************************************************************************************
*/

TASK_EVENT	OSMaskEventPend(TASK_EVENT mask)
{
	TASK_EVENT	event;
	OS_ENTER_CRITICAL();

	OSTCBCur->OSEventBitMask=mask;

	event=OSTCBCur->OSEvent;

	if((event&(~mask))==0)
	{
		OSTaskSuspend(OSPrioCur);
		OS_EXIT_CRITICAL();

		OSSched();

		OS_ENTER_CRITICAL();
		event=OSTCBCur->OSEvent;
		OSTCBCur->OSEvent&=mask;
		OS_EXIT_CRITICAL();
		return(event&(~mask));
	}
	else
	{
		OSTCBCur->OSEvent&=mask;
		OS_EXIT_CRITICAL();
		return(event&(~mask));
	}
}


/*
*************************************************************************************************
*                                             EVENT GET						*
*												*
* Description: wait an event,if no event happened then return empty event ,otherwise clear the 	*
*	       event in tcb and return the event						*
* Arguments  : none										*
*												*
* Returns    : event										*
*************************************************************************************************
*/

TASK_EVENT	OSEventGet()
{
	TASK_EVENT	event;
	OS_ENTER_CRITICAL();
	event=OSTCBCur->OSEvent;
	OSTCBCur->OSEvent=0;
	OS_EXIT_CRITICAL();
	return(event);
}
/*
*************************************************************************************************
*                                             EVENT Send in TASK				*
*												*
* Description: Post an event to the task whose priority is prio,the EventId is bit number in 	*
*	       the OSEvent,This function only can be called in task level			*
* Arguments  : prio:	the task's priority							*
*	       EventId:	the bit number in the OSEvent						*
*												*
* Returns    : none										*
*************************************************************************************************
*/

void	OSEventSend(INTOS prio,INTOS EventId)
{
	OS_TCB	*ptcb;
	OS_ENTER_CRITICAL();
	ptcb=&OSTCBTbl[prio];
	ptcb->OSEvent |= 1<<EventId;
	if((ptcb->OSEvent)& (~ptcb->OSEventBitMask))
	{
		OSTaskRdy(prio);
	}
	OS_EXIT_CRITICAL();
}

/*
*************************************************************************************************
*                                             EVENT Send in ISR					*
*												*
* Description: Post an event to the task whose priority is prio,the EventId is bit number in 	*
*	       the OSEvent,this function can be called in ISRs					*
* Arguments  : prio:	the task's priority							*
*	       EventId:	the bit number in the OSEvent						*
*												*
* Returns    : none										*
*************************************************************************************************
*/
#pragma CODE_SECTION(OSISREventSend,"ramfuncs");
void	OSISREventSend(INTOS prio,INTOS EventId)
{
	OS_TCB	*ptcb;
	ptcb=&OSTCBTbl[prio];
	ptcb->OSEvent|=1<<EventId;
	if((ptcb->OSEvent)&(~ptcb->OSEventBitMask))
	{
		OSTaskRdy(prio);
	}
}
