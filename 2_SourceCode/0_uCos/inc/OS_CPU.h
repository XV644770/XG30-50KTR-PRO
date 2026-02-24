/************************************************************************************************
SANTAK ELECTRONIC Ltd										*
RTOS kernel for ups ,All rights reserved							*
VERSION:		V00									*
File name:		OS_CPU.h								*
Description:		DEFINITION OF STRUCTURE RELATED TO CPU					*
Update Date:		2003/07/22								*
Writer:			ZD									*
*************************************************************************************************/
#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

/************************************************************************************************/
/*					                 Basic variables type definition				            */
/************************************************************************************************/
#define BOOLEAN 	unsigned char
#define INT8U   	unsigned char
#define INT8S   	  signed char
#define INTOS		unsigned int				/* the smallest unit int the cpu	*/
#define INT16U  	unsigned int
#define INT16S  	  signed int
#define INT32U  	unsigned long
#define INT32S  	  signed long
#define OS_STK		INTOS



/************************************************************************************************/
/*					                        Macro definitions					                */
/************************************************************************************************/

#define	OS_ENTER_CRITICAL()		asm("	SETC	INTM")
#define	OS_EXIT_CRITICAL()		asm("	CLRC	INTM")
#define OS_TASK_SW()			asm("	TRAP	#31")




