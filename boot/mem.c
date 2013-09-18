#include "common.h"
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

static void
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

static void
add_a_page(u32 addr, u32 *page_dir)
{
	int dir_index, tbl_index;
	u32 *page_tbl;

	dir_index = (addr>>22)&0x3ff;
	tbl_index = (addr>>12)&0x3ff;
	page_dir[dir_index] |= 1;
	page_tbl = (u32 *)((page_dir[dir_index])&0xfffff000);
	page_tbl[tbl_index] |= 1;
}

extern char* _cmain_end;
static void
setup_page()
{
	int i;
	u32 *page_dir = (u32 *)(((u32)(&_cmain_end) & 0xfffff000) + 0x1000);
	u32 *page_tbl = page_dir + 0x1000;
	u32 addr = 0;
	long cnt = *(long *)MEM_DESC_CNT_ADDR;
	mem_desc_s *descs = (mem_desc_s *)MEM_MAP_ADDR;

	// setup blank page directory
	for (i = 0; i < 1024; i++) {
		page_dir[i] = (u32)(&page_tbl[i*1024]) | 2;
	}
	for (i = 0; i < 1024*1024; i++) {
		page_tbl[i] = addr | 2;
		addr += 0x1000;
	}
	for (i = 0; i < cnt; i++) {
		for (addr = 0; addr < descs[i].length_low; addr += 0x1000) {
			add_a_page(descs[i].base_low + addr, page_dir);
		}
	}
	// add video memory map
	for (i = 0; i < VIDEO_RAM_SIZE; i += 0x1000) {
		add_a_page(VIDEO_RAM_START_ADDR + i, page_dir);
	}
	// add local apic memory map
	for (i = 0; i < 0x1000; i += 0x1000) {
		add_a_page(0xfee00000+i, page_dir);
	}

	// tell cpu the page directory address
	__asm__ __volatile__("movl %0, %%cr3"::"b"(page_dir));
	// enable paging
	__asm__ __volatile__(
			"movl %cr0, %eax\n\t"
			"orl $0x80000000, %eax\n\t"
			"movl %eax, %cr0\n\t"
	);
}

void
mem_init()
{
	show_mem_map();
	setup_page();
}
