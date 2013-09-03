#include "keyboard.h"
#include "interrupt.h"
#include "io.h"
#include "video.h"

static void	keyboard_callback(void *arg);

void
keyboard_init()
{
	intr_handler_s keyboard_handler;
	keyboard_handler.callback = keyboard_callback;

	register_irq(KEYBOARD_IRQ, &keyboard_handler);
}

static void
keyboard_callback(void *arg)
{
	tw_printf("a key(0x%x) pressed\n", (u32)inb(0x60));
}
