#include "interrupt.h"

.section	.text
.globl interrupt_handler_start
interrupt_handler_start:
i = 0
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

exception_handler:
	# 0(sp): irq number
	# 4(sp): error code
	# 8(sp): eip
	# 12(sp): cs
	# 16(sp): eflag
	call interrupt_handler
	addl $8, %esp # skip irq number and error code
	iret
