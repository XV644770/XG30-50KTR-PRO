/************************************************************************************************
KSTAR ELECTRONIC Ltd										*
RTOS kernel for ups ,All rights reserved							*
VERSION:		V00									*
File name:		OS_CORE.C								*
Description:		internal functions							*
Update Date:		2003/07/22								*
Writer:			ZD									*
*************************************************************************************************/

#include <OS_KERNEL.h>

#pragma DATA_SECTION(OSSystemStk,"OSStack");

OS_STK  OSSystemStk[OS_MAX_STK_SIZE];   /* Stack of SYSTEM task	*/

#if	OS_TABLE_IN_FLASH>0
const INTOS  OSUnMapTbl[16]= {
	255, 0, 1, 0,
	2  , 0, 1, 0,
	3  , 0, 1, 0,
	2  , 0, 1, 0
};
#else

INTOS  OSUnMapTbl[16]= {
	255, 0, 1, 0,
	2  , 0, 1, 0,
	3  , 0, 1, 0,
	2  , 0, 1, 0
};
#endif

extern void	OSStartHighRdy(void);

/* 
*************************************************************************************************
*                                           FIND THE HIGHEST READY PRIO				*
* Description: 	This function is used to FIND THE HIGHEST READY PRIORITY			*
*												*
* Parameters  : 	NONE										*
*												*
* Returns    : 	THE HIGHEST READY PRIORITY							*
*************************************************************************************************/

INTOS	OSFindHighPrio(void)
{
	if(OSRdyMap&0x000F)
	{
		return(OSUnMapTbl[OSRdyMap&0x000F]);
	}
	else if(OSRdyMap&0x00F0)
	{
		return(OSUnMapTbl[(OSRdyMap>>4)&0x000F]+4);
	}
	else if(OSRdyMap&0x0F00)
	{
		return(OSUnMapTbl[(OSRdyMap>>8)&0x000F]+8);
	}
	else if(OSRdyMap&0xF000)
	{
		return(OSUnMapTbl[(OSRdyMap>>12)&0x000F]+12);
	}
	else
	{
		return(0); 	//OS_LOWEST_PRIO
	}
}


/*
*************************************************************************************************
*                                              SCHEDULER					*
*												*
* Description: This function is called by other OS services to determine whether a new, high	*
*              level code priority task has been made ready to run.  This function is invoked 	* 
*              by TASK and is not used to reschedule tasks from ISRs 				*
*		(see OSIntExit() for ISR rescheduling).						*
*												*
* Parameters  : none										*
*												*
* Returns    : none										*
*												*
* Notes      : 1) This function is INTERNAL to OS and your application should not call it.	*
*              2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock())	*
*************************************************************************************************
*/	

void OSSched (void)
{

	OS_ENTER_CRITICAL();


	OSPrioHighRdy = OSFindHighPrio();   		/* Get the highest priority from table	*/

	OSPrioCur=OSPrioHighRdy;
	OSTCBHighRdy=&OSTCBTbl[OSPrioHighRdy];

	OS_TASK_SW();                			/* Perform a context switch            	*/

	OS_EXIT_CRITICAL();
}


/*
*************************************************************************************************
*                                             INITIALIZATION					*
*                                    INITIALIZE MISCELLANEOUS VARIABLES				*
*												*
* Description: This function is called by OSInit() to initialize miscellaneous variables.	*
*												*
* Parameters  : none										*
*												*
* Returns    : none										*
*************************************************************************************************
*/

void  OSInitSys (void)
{

#if	OS_STK_CHK_EN>0
	int	i;

	for(i=0;i<OS_MAX_STK_SIZE;i++)
	{
		OSSystemStk[i]=0;
	}
#endif


#if	OS_TASK_IDLE_EN>0
    OSIdleCtr     	= 0;                            /* Clear the 32-bit idle counter       	*/
	OSTime        	= 0;                            /* Clear the 32-bit system clock      	*/
#endif

#if	OS_STK_GROWTH_UP>0
	OSStkPtr	= OSSystemStk;
#else
	OSStkPtr	= OSSystemStk+OS_MAX_STK_SIZE-1;
#endif

}




/*
*************************************************************************************************
*                                             INITIALIZATION					*
*                                       INITIALIZE THE READY LIST				*
*												*
* Description: This function is called by OSInit() to initialize the Ready List.		*
*												*
* Parameters  : none										*
*												*
* Returns    : none										*
*************************************************************************************************
*/

void  OSInitRdyList (void)
{
//	INTOS     i;
   
	OSRdyMap      = 0x00;				/* Clear the ready list			*/

	OSPrioCur     = 0;
	OSPrioHighRdy = 0;

	OSTCBHighRdy  = (OS_TCB *)0;
	OSTCBCur      = (OS_TCB *)0;
}


/*
*************************************************************************************************
*                                             INITIALIZATION					*
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS			*
*												*
* Description: This function is called by OSInit() to initialize the free list of OS_TCBs.	*
*												*
* Parameters  : none										*
*												*
* Returns    : none										*
*************************************************************************************************
*/

void  OSInitTCBList (void)
{
	INTOS i;
	OS_TCB	*ptcb;

	ptcb=&OSTCBTbl[0];

	for (i = 0; i < OS_LOWEST_PRIO + 1; i++)
	{            					/* Clear the priority table        	*/
		ptcb->OSTCBStkPtr=(void*)0;
		ptcb->TimerPeriod=0;
		ptcb->TimerCnt=0;
		ptcb->OSEvent=0;
		ptcb->OSEventBitMask=~0;
		ptcb++;
	}

}


/*
*************************************************************************************************	
*                                              IDLE TASK					*
*												*
* Description: This task is internal to RTOS and executes whenever no other higher priority 	*
*              tasks executes because they are ALL waiting for event(s) to occur.		*
*												*
* Parameters : none										*
*												*
* Returns    : none										*
*************************************************************************************************
*/

#if 	OS_TASK_IDLE_EN>0
void  OSTaskIdle ()
{

    for (;;)
    {
        OS_ENTER_CRITICAL();
        OSIdleCtr++;
        OS_EXIT_CRITICAL();
    }
}
#endif


/*
*************************************************************************************************
*                                             INITIALIZATION					*
*                                         CREATING THE IDLE TASK				*
*												*
* Description: This function creates the Idle Task.						*	
*												*
* Parameters : none										*
*												*
* Returns    : none										*
*************************************************************************************************
*/
#if	OS_TASK_IDLE_EN>0
void  OSInitTaskIdle (void)
{
	OSTaskCreate(OSTaskIdle,OS_TASK_IDLE_STK_SIZE,OS_IDLE_PRIO);
}
#endif
