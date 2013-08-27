#include "gdt.h"
#include "video.h"
#include "mem.h"
#include "interrupt.h"

void
cmain()
{
	gdt_init();
	tw_printf("%%%s: (%d=0x%x=0b%b)\n", "cmain", 10, 10, 10);
	show_mem_map();
	interrupt_init();
}
