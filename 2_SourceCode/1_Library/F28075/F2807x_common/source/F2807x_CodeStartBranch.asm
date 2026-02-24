;//###########################################################################
;//
;// FILE:  F2807x_CodeStartBranch.asm
;//
;// TITLE: Branch for redirecting code execution after boot.
;//
;// For these examples, code_start is the first code that is executed after
;// exiting the boot ROM code.
;//
;// The codestart section in the linker cmd file is used to physically place
;// this code at the correct memory location.  This section should be placed
;// at the location the BOOT ROM will re-direct the code to.  For example,
;// for boot to FLASH this code will be located at 0x3f7ff6.
;//
;// In addition, the example F2807x projects are setup such that the codegen
;// entry point is also set to the code_start label.  This is done by linker
;// option -e in the project build options.  When the debugger loads the code,
;// it will automatically set the PC to the "entry point" address indicated by
;// the -e linker option.  In this case the debugger is simply assigning the PC,
;// it is not the same as a full reset of the device.
;//
;// The compiler may warn that the entry point for the project is other then
;//  _c_init00.  _c_init00 is the C environment setup and is run before
;// main() is entered. The code_start code will re-direct the execution
;// to _c_init00 and thus there is no worry and this warning can be ignored.
;//
;//###########################################################################
;// $TI Release: F2807x Support Library v200 $
;// $Release Date: Tue Jun 21 14:11:45 CDT 2016 $
;// $Copyright: Copyright (C) 2014-2016 Texas Instruments Incorporated -
;//             http://www.ti.com/ ALL RIGHTS RESERVED $
;//###########################################################################

***********************************************************************

ramm0	.set	0000H
ramm0l	.set	400H

ramm1	.set	400H
ramm1l	.set	400H

ramls01d  .set    8000H
ramls01dl .set    1000H

ramls23d  .set    9000H
ramls23dl .set    1000H

ramls45d  .set    0A000H
ramls45dl .set    1000H

ramld01d  .set    0B000H
ramld01dl .set    1000H

ramlgs0d  .set    0C000H
ramlgs0dl .set    1000H

ramlgs1d  .set    0D000H
ramlgs1dl .set    1000H

ramlgs2d  .set    0E000H
ramlgs2dl .set    1000H

ramlgs3d  .set    0F000H
ramlgs3dl .set    1000H

ramlgs4d  .set    10000H
ramlgs4dl .set    1000H

ramlgs5d  .set    11000H
ramlgs5dl .set    1000H

ramlgs6d  .set    12000H
ramlgs6dl .set    1000H

ramlgs7d  .set    13000H
ramlgs7dl .set    1000H


	.ref _c_int00
	.ref _BootLoader
	.global	code_start

	.sect "codestart"

code_start:

	 LB	 _UpdateCheck



	.sect	"codeupdate"
_UpdateCheck:

        ADDB      SP,#4

        MOVL      XAR4,#9FFFFH; #0x9FFFF
        MOV       AL,*+XAR4[0]
        MOV       *-SP[3],AL

        MOVL      XAR4,#9FFFEH; #0x9FFFE
        MOV       AL,*+XAR4[0]
        MOV       *-SP[2],AL

        MOVL      XAR4,#82000H ;#0x82002
        MOV       AL,*+XAR4[0]
        MOV       *-SP[1],AL

        MOVL      XAR4,#82001H ;#0x82003
        MOV       AL,*+XAR4[0]
        MOV       *-SP[0],AL

        MOV       AL,*-SP[3]
        CMP       AL,#0AAAAH	; compared 0xAAAA
        BF        L1  ,NEQ

        MOV       AL,*-SP[2]
        CMP       AL,#0AAAAH	; compared 0xAAAA
        BF        L1  ,NEQ

        MOV       AL,*-SP[1]
        CMP       AL,#0AAAAH	; compared 0xAAAA
        BF        L1  ,NEQ

        MOV       AL,*-SP[0]
        CMP       AL,#0AAAAH	; compared 0xAAAA
        BF        L1  ,NEQ
        LB        _clearram

L1:
        LB        _disable_wg


	.sect	"codeupdate"
_disable_wg:

  SETC OBJMODE
  SETC INTM
  MOVL      XAR1,#0x7029	;watchdog register address
  EALLOW
  MOV      *XAR1,#0x0068
  EDIS

  LB  _BootLoader

	.sect "clearram"
_clearram:
	;diable watch dog
	movl	XAR0,#0x7029
	EALLOW
	mov	*XAR0,#0x0068
	EDIS

	;first the ram M0
	mov	AL,#0

	movl	XAR5,#ramm0
	mov	AR4,#ramm0l-1
	rpt	@AR4
||	mov	*XAR5++,AL

	;then the ram M1
	movl	XAR5,#ramm1
	mov	AR4,#ramm1l-1
	rpt	@AR4
||	mov	*XAR5++,AL

	;then the ram LS01
	movl	XAR5,#ramls01d
	mov	AR4,#ramls01dl-1
	rpt	@AR4
||	mov	*XAR5++,AL

	;then the ram LS23
	movl	XAR5,#ramls23d
	mov	AR4,#ramls23dl-1
	rpt	@AR4
||	mov	*XAR5++,AL

	;then the ram LS45
	movl	XAR5,#ramls45d
	mov	AR4,#ramls45dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram D01
	movl	XAR5,#ramld01d
	mov	AR4,#ramld01dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS0
	movl	XAR5,#ramlgs0d
	mov	AR4,#ramlgs0dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS1
	movl	XAR5,#ramlgs1d
	mov	AR4,#ramlgs1dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS2
	movl	XAR5,#ramlgs2d
	mov	AR4,#ramlgs2dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS3
	movl	XAR5,#ramlgs3d
	mov	AR4,#ramlgs3dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS4
	movl	XAR5,#ramlgs4d
	mov	AR4,#ramlgs4dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS5
	movl	XAR5,#ramlgs5d
	mov	AR4,#ramlgs5dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS6
	movl	XAR5,#ramlgs6d
	mov	AR4,#ramlgs6dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	;then the ram GS7
	movl	XAR5,#ramlgs7d
	mov	AR4,#ramlgs7dl-1
	rpt	AR4
||	mov	*XAR5++,AL

	movz	AR0,AL
	movz	AR5,AL
	movz	AR4,AL
	MOV     *SP++,#0
	MOV     *SP++,#0x0A0B
	POP     ST1
	POP     ST0
	LB	_c_int00

	.end


;//
;// End of file.
;//
