#include "io.h"

void inline
outb(u16 port, u8 val)
{
	__asm__ __volatile__("outb %0, %1": : "a"(val), "Nd"(port));
}

u8 inline
inb(u16 port)
{
	u8 ret;
	__asm__ __volatile__("inb %1, %0": "=a"(ret) : "Nd"(port));
	return ret;
}
