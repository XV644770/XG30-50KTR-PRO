;************************************************************************************************
;SANTAK ELECTRONIC Ltd										*
;RTOS kernel for PV ,All rights reserved							*
;version:		V00									*
;File name:		OS_CPU_A.asm								*
;Description:		some function need to be writen int asnm				*				*
*************************************************************************************************
; Ver | dd mmm yyyy | Who  | Description of changes
; V01   01  08 2003   L.G    writen for DSP2810
;
    .global	_OSTCBCur
	.global	_OSTCBHighRdy
	.global	_OSStartHighRdy
	.global	_OSCtxSw
	.global	_OSIntCtxSw
	.global	_OS_ENTER_CRITICAL
	.global	_OS_EXIT_CRITICAL

	.def	_OSStartHighRdy
	.def	_OSCtxSw
	.def	_OSIntCtxSw

    .sect "ramfuncs"	; Run in ram
;************************************************************************************************
;OSStartHighRdy()										*
;start high ready task										*
;************************************************************************************************
_OSStartHighRdy:			;							*
	MOVW	DP,#_OSTCBCur		;

	MOVL    XAR6,@_OSTCBCur
	MOVL	ACC,*XAR6
	MOV	SP,AL


	POP RPC
	POP	XT
	POP	XAR7
	POP	XAR6
	POP	XAR5
	POP	XAR4
	POP	XAR3
	POP	XAR2
	POP	AR1H:AR0H

	POP	DP:ST1
	IRET				;							*

;************************************************************************************************
;OSCtxSw											*
;task level sw											*
;************************************************************************************************
_OSCtxSw:				;
	ASP
	PUSH	DP:ST1
	PUSH	AR1H:AR0H
	PUSH	XAR2
	PUSH	XAR3
	PUSH	XAR4
	PUSH	XAR5
	PUSH	XAR6
	PUSH	XAR7
	PUSH	XT
	PUSH	RPC


_OSCtxSw_0:
	MOVW	DP,#_OSTCBCur		;
	MOVL	XAR6,@_OSTCBCur
	MOV	AL,SP
	MOVL	*XAR6,ACC

	MOVW	DP,#_OSTCBHighRdy	;
	MOVL	XAR7,@_OSTCBHighRdy
	MOVL	ACC,*XAR7
	MOV	SP,AL

	MOVL	ACC,@_OSTCBHighRdy
	MOVL	@_OSTCBCur,ACC


	POP	RPC
	POP	XT
	POP	XAR7
	POP	XAR6
	POP	XAR5
	POP	XAR4
	POP	XAR3
	POP	XAR2
	POP	AR1H:AR0H

	POP	DP:ST1
	NASP
	IRET
;************************************************************************************************
;OSCtxSw											*
;int level sw											*
;************************************************************************************************									*
_OSIntCtxSw:				;							*
   	POP	RPC			; pop-off the return address from call to this routine	*
    					; never to be used...					*
	POP	RPC			; clean-up the sw stack's pushed			*
    					; frame pointer and return address from			*
                                        ; the call to "OSIntExit."				*
	LB	_OSCtxSw_0		; the same as task level				*
;************************************************************************************************
