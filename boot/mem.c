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

int
tw_memcmp(char *dst, char *src, int len)
{
	int ret;

	__asm__(
			"movl $0, %0\n\t"
			"cld\n\t"
			"repe cmpsb\n\t"
			"je 1f\n\t"
			"movl $1, %0\n\t"
			"1:\n\t"
			: "=r"(ret)
			: "S"(src), "D"(dst), "c"(len)
	);
	return ret;
}

void
show_mem_map()
{
	long cnt = *(long *)MEM_DESC_CNT_ADDR, i;
	mem_desc_s *descs = (mem_desc_s *)MEM_MAP_ADDR;

	tw_printf("there are %d mem descriptors:\n", cnt);
	for (i = 0; i < cnt; i++) {
		tw_printf("start: 0x%x%x, lenth: 0x%x%x, type: %x\n",
				descs[i].base_high, descs[i].base_low,
				descs[i].length_high, descs[i].length_low,
				descs[i].type);
	}
}
