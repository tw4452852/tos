#include "gdt.h"
#include "interrupt.h"
#include "video.h"
#include "apic.h"
#include "mem.h"

static u64 idt[IDT_MAX_DESCS] __attribute__ ((aligned(8)));
static intr_handler_s handlers[IDT_MAX_DESCS] = {{0}};

static void	init_idt(void);

void
default_handler(u32 irq, u32 error_code, u32 eip, u32 cs, u32 eflag)
{
	tw_printf("recv irq(%d): error_code(0x%x), eip(0x%x), cs(0x%x), eflag(0x%x)\n",
			irq, error_code, eip, cs, eflag);
	if (handlers[irq].callback != 0) {
		handlers[irq].callback(handlers[irq].arg);
	}
	if (irq >= 0x20) {
		apic_eoi(irq);
	}
}

void
interrupt_init()
{
	int i;

	init_idt();
	apic_init();
	for (i = 0; i < IDT_MAX_DESCS; i++) {
		tw_memset(&(handlers[i]), 0, sizeof(intr_handler_s));
	}
}

// definition in isr.S
extern void interrupt_handler_start(void);
extern void exception_handler_start(void);
static void
init_idt()
{
	int i;

	for (i = 0; i < IDT_MAX_DESCS; i++) {
		if (i < IDT_MAX_EXCEPTION) {
			idt[i] = IDT_DESC((u32)(&exception_handler_start + i*0x09));
		} else {
			idt[i] = IDT_DESC((u32)(&interrupt_handler_start + (i-IDT_MAX_EXCEPTION)*0x0c));
		}
	}

	struct {
		u16 limit;
		u32 addr;
	} __attribute__ ((packed)) idtr;
	idtr.addr = (u32)idt;
	idtr.limit = IDT_MAX_DESCS * sizeof(u64);
	__asm__ __volatile__(
			"cli\n\t"
			"lidt %0\n\t"
			::"m"(idtr)
	);
}

int
register_irq(u8 irq, intr_handler_s *handler)
{
	if (handler == 0) {
		tw_printf("register irq(%d) failed: handler is NULL\n", irq);
		return -1;
	}
	if (handler->callback == 0) {
		tw_printf("register irq(%d) failed: callback is NULL\n", irq);
		return -1;
	}
	if (handlers[irq].callback != 0) {
		tw_printf("register irq(%d) failed: already registered\n", irq);
		return -1;
	}

	tw_memcpy(&(handlers[irq]), handler, sizeof(intr_handler_s));
	return 0;
}

void
unregister_irq(u8 irq)
{
	handlers[irq].callback = 0;
}
