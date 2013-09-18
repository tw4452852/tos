#include "gdt.h"

static u64 gdt[GDT_MAX_DESCS];

void
gdt_init()
{
	int i;

	for (i = 0; i < GDT_MAX_DESCS; i++) {
		gdt[i] = 0;
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
	gdtr.limit = GDT_MAX_DESCS * sizeof(u64);
	__asm__ __volatile__("lgdt %0"::"m"(gdtr));
}

inline void
gdt_set_desc(u8 index, u32 base, u32 limit, u16 attr)
{
	gdt[index] = GDT_DESC(base, limit, attr);
}
