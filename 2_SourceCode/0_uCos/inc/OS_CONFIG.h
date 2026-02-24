/************************************************************************************************
SANTAK ELECTRONIC Ltd										*
RTOS kernel for ups ,All rights reserved							*
VERSION:		V00									*
File name:		OS_config.h								*
Description:		TASK SCHEDULE FUNCTIONS							*
Update Date:		2003/07/22								*
Writer:			ZD									*
*************************************************************************************************/


/************************************************************************************************/
/*                                        CONTROL SWITCH					                    */
/************************************************************************************************/

#define	OS_TASK_IDLE_EN			0		/*	Argument check enable switch	*/
#define	OS_TABLE_IN_FLASH		1		/*	constant variable in ram or rom	*/
#define	OS_STK_GROWTH_UP		1		/*	STACK GROWTH DIRECTION		*/
#define	OS_STK_CHK_EN			0	//lg/*	STACK CHECK ENABLE		*/ 


/************************************************************************************************/
/*                                          STACK CONFIG						                */
/************************************************************************************************/

#define	OS_MAX_STK_SIZE			2000  /*	MAX SYSTEM STACK SIZE		*/

/************************************************************************************************/
/*					                    TASK CONFIG			                     			    */
/************************************************************************************************/
#define	OS_LOWEST_PRIO			7 	/*	MAX TASK NUMBER-1		*/

#if	OS_TASK_IDLE_EN>0
#define OS_IDLE_PRIO			7		/*	LOWEST PRIORITY			*/
#define OS_TASK_IDLE_STK_SIZE		50		/*	IDLE TASK STACK SIZE*/
#define	OS_CPUFREE_FACTOR		34		/*CALCULATION THE CPU USEAGE*/
#endif

/************************************************************************************************
*					EVENT NUMNER DEFINITION				*
*************************************************************************************************/

#define	TASK_EVENT			INT16U


/************************************************************************************************
*					TIMER CONFIG						*
*************************************************************************************************/
#define OS_TICKS_PER_SEC		500		/*	THE TIMER ISR NUMBER IN 1S	*/
