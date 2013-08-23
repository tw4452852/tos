#include "mem.h"
#include "video.h"

__asm__(".code16\n\t");
void	load_pmem_map() __attribute__((section(".btext")));
void
load_pmem_map()
{
	long cnt;
	__asm__(
			"xorl %0, %0\n\t"
			"1:\n\t"
			"int $0x15\n\t"
			"jc 2f\n\t"
			"andl %%ebx, %%ebx\n\t"
			"jz 2f\n\t"
			"addw $20, %%di\n\t"
			"movl $0xe820, %%eax\n\t"
			"inc %0\n\t"
			"jmp 1b\n\t"
			"2:\n\t"
			: "=S"(cnt)
			: "D"(MEM_MAP_ADDR), "b"(0), "a"(0x0000e820), "c"(20), "d"(0x534d4150)
		   );
	*(long *)MEM_DESC_CNT_ADDR = cnt;
}

__asm__(".code32\n\t");

void
show_mem_map()
{
	tw_printf("there are %d mem descriptors\n", *(long *)MEM_DESC_CNT_ADDR);
}
