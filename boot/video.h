#ifndef VIDEO_H

#define VIDEO_H

#define VIDEO_RAM_START_ADDR	0xb8000
#define VIDEO_RAM_SIZE			0x10000
#define SCREEN_LINE_LEN			(80 * 2)
#define SCREEN_HEIGHT			25

int	tw_printf(const char *fmt, ...);

#endif /* end of include guard: VIDEO_H */
