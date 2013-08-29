#include "interrupt.h"
.section	.data
recursion_flag: .long 0
.section	.text
.globl interrupt_handler_start
interrupt_handler_start:
i = 0
# intel reserve exception or fault
.rept IDT_MAX_EXCEPTION
	.if (EXCEPTION_ERRCODE_MASK >> i) & 1
		nop
		nop
	.else
		pushl $0
	.endif
	pushl $i
	jmp exception_handler
i = i + 1
.endr
# user defined interrupt
.rept IDT_MAX_DESCS - IDT_MAX_EXCEPTION
	pushl $0
	pushl $i
	jmp exception_handler
i = i + 1
.endr

exception_handler:
	cmpl $2, %ss:recursion_flag
	je hlt_loop
	incl %ss:recursion_flag

	# 0(sp): irq number
	# 4(sp): error code
	# 8(sp): eip
	# 12(sp): cs
	# 16(sp): eflag
	call interrupt_handler
	addl $8, %esp # skip irq number and error code

	decl %ss:recursion_flag
	iret

hlt_loop:
	hlt
	jmp hlt_loop
