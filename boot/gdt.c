#include "gdt.h"
#include "video.h"

static gdt_desc_u gdt[GDT_MAX_DESCS];

void
gdt_init()
{
	int i;

	for (i = 0; i < GDT_MAX_DESCS; i++) {
		gdt[i].u = 0;
	}

	/*
	 * set default code&data descs:
	 * code32: 0-4G
	 * data32: 0-4G
	 */
	gdt_set_desc(CODE_PL0_INDEX, 0, 0xfffff, GDT_CODE_PL0);
	gdt_set_desc(DATA_PL0_INDEX, 0, 0xfffff, GDT_DATA_PL0);

	struct {
		u16				limit;
		unsigned long	addr;
	} __attribute__ ((packed)) gdtr;
	gdtr.addr = (unsigned long)gdt;
	gdtr.limit = GDT_MAX_DESCS * sizeof(gdt_desc_u);
	__asm__("lgdt %0"::"m"(gdtr));
}

void
gdt_set_desc(u8 index, u32 base, u32 limit, u16 attr)
{
	gdt_desc_u desc;

	desc.u = 0;
	desc.s.limit_low = limit & 0xffff;
	desc.s.base_low = base & 0xffff;
	desc.s.base_mid = (base>>16) & 0xff;
	desc.s.attr_low = attr & 0xff;
	desc.s.limit_high = (limit>>16) & 0xf;
	desc.s.attr_high = (attr>>8) & 0xf;
	desc.s.base_high = (base>>24) & 0xff;

	gdt[index] = desc;
}
