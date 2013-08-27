#ifndef INTERRUPT_H

#define INTERRUPT_H

#include "common.h"

typedef union {
	u64 u;
	struct {
		u64	offset_low	: 16;
		u64	sel			: 16;
		#define IDT_ATTR	(0x8e00 & 0xffff)
		u64	attr		: 16;
		u64	offset_high	: 16;
	} s;
} idt_desc_u;

#define IDT_MAX_DESCS	10

void	interrupt_init(void);

#endif /* end of include guard: INTERRUPT_H */
