#include "gdt.h"
#include "interrupt.h"
#include "video.h"
#include "apic.h"

static u64 idt[IDT_MAX_DESCS] __attribute__ ((aligned(8)));

static void	init_idt(void);

void
interrupt_handler(u32 irq, u32 error_code, u32 eip, u32 cs, u32 eflag)
{
	tw_printf("recv irq(%d): error_code(0x%x), eip(0x%x), cs(0x%x), eflag(0x%x)\n",
			irq, error_code, eip, cs, eflag);
}

void
interrupt_init()
{
	init_idt();
	apic_init();
}

// definition in isr.S
extern void interrupt_handler_start(void);
static void
init_idt()
{
	int i;

	for (i = 0; i < IDT_MAX_DESCS; i++) {
		idt[i] = IDT_DESC((u32)(&interrupt_handler_start + i*0x09));
	}

	struct {
		u16 limit;
		u32 addr;
	} __attribute__ ((packed)) idtr;
	idtr.addr = (u32)idt;
	idtr.limit = IDT_MAX_DESCS * sizeof(u64);
	__asm__(
			"cli\n\t"
			"lidt %0\n\t"
			::"m"(idtr)
	);
}

