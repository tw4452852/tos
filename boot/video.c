#include <stdarg.h>
#include <string.h>

#include "video.h"
#include "mem.h"

#define VIDEO_RAM_START_ADDR 0xb8000
#define VIDEO_RAM_SIZE 0x10000
static int g_video_offset = 0;

static void	tw_video_init(void);
static int	tw_vprintf(const char *fmt, va_list arg_list);
static char*	tw_itoa(int num, char *buf, char c);

static void
tw_video_init()
{
	static has_inited = 0;
	
	if (!has_inited) {
		tw_memset((char *)VIDEO_RAM_START_ADDR, 0, VIDEO_RAM_SIZE);
		has_inited = 1;
	}
}

int
tw_printf(const char *fmt, ...)
{
	va_list arg_list;
	int ret;

	tw_video_init();

	va_start(arg_list, fmt);
	ret = tw_vprintf(fmt, arg_list);
	va_end(arg_list);

	return ret;
}

static int
tw_vprintf(const char *fmt, va_list arg_list)
{
	int before = g_video_offset;
	char str[64] = {0};
	int i;
	char c;

	for (i = 0; i < 64 && *fmt != '\0';) {
		c = *fmt++;
		if (c != '%') {
			str[i++] = c;
			continue;
		}
		// check the directive
		c = *fmt++;
		int num;
		char *s;
		switch (c) {
			case 'd':
			case 'b':
			case 'x':
				num = va_arg(arg_list, int);
				tw_itoa(num, &str[i], c);
				break;
			case 's':
				s = va_arg(arg_list, char*);
				tw_memcpy(&str[i], s, strlen(s));
				break;
			case '%':
				str[i++] = '%';
				// fall through
			default:
				// others not support yet
				continue;
		}
		i += strlen(&str[i]);
	}

	// out of buffer
	if (i == 64 && *fmt != '\0') {
		return -1;
	}
	
	/*show the string*/
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

char*
tw_itoa(int num, char *buf, char c)
{
	int i, j, v;
	int base;
	switch (c) {
		case 'b':
			base = 2;
			break;
		case 'd':
			base = 10;
			break;
		case 'x':
			base = 16;
			break;
		default:
			// others not support yet
			return NULL;
	}

	if (num < 0) {
		buf[0] = '-';
		num = -num;
	}
	for (i = 1; num > 0; i++, num /= base) {
		v = num % base;
		if (v > 9) {
			buf[i] = (v-10) + 'a';
		} else {
			buf[i] = v + '0';
		}
	}
	for (j = i-1, i = 1; j-i >= 1 ; j--, i++) {
		buf[i] = buf[i] ^ buf[j];
		buf[j] = buf[i] ^ buf[j];
		buf[i] = buf[i] ^ buf[j];
	}
	if (buf[0] != '-') {
		for (i = 0; buf[i+1] != '\0'; i++) {
			buf[i] = buf[i+1];
		}
		buf[i] = '\0';
	}
	return buf;
}
