#include "video.h"
#include "util.h"

#define VIDEO_RAM_START_ADDR 0xb8000
#define VIDEO_RAM_SIZE 0x10000

static int g_video_offset = 0;

static void
tw_video_init(void)
{
	static has_inited = 0;
	
	if (!has_inited) {
		tw_memset((char *)VIDEO_RAM_START_ADDR, 0, VIDEO_RAM_SIZE);
		has_inited = 1;
	}
}

int
tw_puts(const char *str)
{
	int before = g_video_offset;
	tw_video_init();
	__asm__ (
			"addl %0, %%edi\n\t"
			"cld\n\t"
			"1:\n\t"
			"lodsb\n\t"
			"andb %%al, %%al\n\t"
			"jz 2f\n\t"
			"movb $0xc, %%ah\n\t"
			"stosw\n\t"
			"inc %0\n\t"
			"jmp 1b\n\t"
			"2:\n\t"
			: "=r"(g_video_offset)
			: "S"(str), "D"(VIDEO_RAM_START_ADDR), "0"(g_video_offset)
			: "%eax");
	return g_video_offset - before;
}
