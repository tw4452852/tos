#include "gdt.h"
#include "video.h"
#include "mem.h"
#include "interrupt.h"
#include "keyboard.h"

#define reload_segs(data) \
	__asm__( \
			"movl %0, %%ds\n\t" \
			"movl %0, %%es\n\t" \
			"movl %0, %%fs\n\t" \
			"movl %0, %%gs\n\t" \
			"movl %0, %%ss\n\t" \
			: :"r"((data)) \
	)

void
cmain()
{
	gdt_init();
	reload_segs(SEL_DATA_PL0);
	tw_printf("%%%s: (%d=0x%x=0b%b)\n", "cmain", 10, 10, 10);
	show_mem_map();
	interrupt_init();
	keyboard_init();
}
