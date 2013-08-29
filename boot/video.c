#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "video.h"
#include "mem.h"

static int g_video_offset = 0;

#define putc(c) \
	do { \
		__asm__(\
			"addl %0, %%edi\n\t"\
			"cld\n\t"\
			"andb %%al, %%al\n\t"\
			"jz 2f\n\t"\
			"movb $0xc, %%ah\n\t"\
			"stosw\n\t"\
			"addl $2, %0\n\t"\
			"2:\n\t" \
			: "=r"(g_video_offset)\
			: "a"((c)), "D"(VIDEO_RAM_START_ADDR), "0"(g_video_offset));\
	} while (0)

#define putnl() \
	do {\
		g_video_offset = (g_video_offset/SCREEN_LINE_LEN + 1) * SCREEN_LINE_LEN; \
	} while (0)


static void	tw_video_init(void);
static int	tw_vprintf(const char *fmt, va_list arg_list);
static char*	tw_itoa(u32 num, char *buf, char c);

static void
tw_video_init()
{
	static int has_inited = 0;
	
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
	char num_str[64] = {0};
	int shown_char_cnt;
	char c;

	for (shown_char_cnt = 0; *fmt != '\0';) {
		c = *fmt++;
		if (c != '%') {
			if (c == '\n' || c == '\r') {
				putnl();
			} else {
				putc(c);
			}
			shown_char_cnt++;
			continue;
		}
		// check the directive
		c = *fmt++;
		if (c == '\0') {
			break;
		}
		char *s = NULL;
		switch (c) {
			case 'd':
			case 'b':
			case 'x':
				s = tw_itoa(va_arg(arg_list, u32), num_str, c);
				break;
			case 's':
				s = va_arg(arg_list, char*);
				break;
			case '%':
				putc('%');
				shown_char_cnt++;
				// fall through
			default:
				// others not support yet
				continue;
		}
		// show string
		if (s) {
			while ((c = *s++) != '\0') {
				putc(c);
				shown_char_cnt++;
			}
		}
	}

	return shown_char_cnt;
}

static char*
tw_itoa(u32 num, char *buf, char c)
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

	if (num == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}

	if (base == 10) {
		int d = (int)num;
		if (d < 0) {
			buf[0] = '-';
			num = -d;
		}
	}
	for (i = 1; num > 0; i++, num /= base) {
		v = num % base;
		if (v > 9) {
			buf[i] = (v-10) + 'a';
		} else {
			buf[i] = v + '0';
		}
	}
	buf[i] = '\0';
	// revert
	for (j = i-1, i = 1; j-i >= 1 ; j--, i++) {
		buf[i] = buf[i] ^ buf[j];
		buf[j] = buf[i] ^ buf[j];
		buf[i] = buf[i] ^ buf[j];
	}
	// left shift 1 bit if positive
	if (buf[0] != '-') {
		for (i = 0; buf[i+1] != '\0'; i++) {
			buf[i] = buf[i+1];
		}
		buf[i] = '\0';
	}
	return buf;
}
