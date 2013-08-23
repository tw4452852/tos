#include "video.h"
#include "mem.h"

void
cmain()
{
	tw_printf("%%%s: (%d=0x%x=0b%b)\n", "cmain", 10, 10, 10);
	show_mem_map();
}
