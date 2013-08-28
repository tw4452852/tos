#include "interrupt.h"
#include "video.h"
#include "gdt.h"

static idt_desc_u idt[IDT_MAX_DESCS] __attribute__ ((aligned(8)));

static void	init_idt(void);
static void	general_handler(void);

void
interrupt_init()
{
	init_idt();
}

static void
init_idt()
{
	int i;
	idt_desc_u desc;

	desc.u = 0;
	desc.s.offset_low = (u32)(general_handler) & 0xffff;
	desc.s.sel = SEL_CODE_PL0;
	desc.s.attr = IDT_ATTR;
	desc.s.offset_high = ((u32)general_handler>>16) & 0xffff;

	for (i = 0; i < IDT_MAX_DESCS; i++) {
		idt[i] = desc;
	}

	struct {
		u16 limit;
		u32 addr;
	} __attribute__ ((packed)) idtr;
	idtr.addr = (u32)idt;
	idtr.limit = IDT_MAX_DESCS * sizeof(idt_desc_u);
	__asm__(
			"cli\n\t"
			"lidt %0\n\t"
			"sti\n\t"
			::"m"(idtr)
	);
}

static void
general_handler()
{
	tw_printf("recv a interrupt!\n");
	__asm__("iret");
}
