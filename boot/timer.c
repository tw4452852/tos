#include "apic.h"
#include "timer.h"
#include "video.h"
#include "io.h"
#include "interrupt.h"

static u32 hz = 0;

static u32	get_hz(void);
static void	timer_cb(void *arg);

void
timer_init()
{
	intr_handler_s th;
	th.callback = timer_cb;
	register_irq(TIMER_IRQ, &th);

	hz = get_hz();
	tw_printf("hz is 0x%x\n", hz);

	// timer use ivt 0x20
	lapic_write_reg(_LAPIC_TIMER_OFFSET, TIMER_IRQ);
}

static void
timer_cb(void *arg)
{
	tw_printf("in timer interrupt callback\n");
}

static u32
get_hz()
{
	// divided counter by 16
	lapic_write_reg(_LAPIC_DC_OFFSET, 0x3);
	// enable timer interrupt(vector 255)
	lapic_write_reg(_LAPIC_TIMER_OFFSET, 0xff);
	timer_enable();

	// initialize PIT channel 2 in one-shot mode
	// waiting 1/100 sec
	outb(0x61, (inb(0x61)&0xfd)|1);
	outb(0x43, 0xb2);
	// 1193182/100 = 11932 = 0x2e9c
	outb(0x42, 0x9c); // lsb
	inb(0x60); // short delay
	outb(0x42, 0x2e); // msb

	// reload counter
	u8 v = inb(0x61) & 0xfe;
	outb(0x61, v);
	outb(0x61, v|1);

	// reset apic timer init counter to -1
	lapic_write_reg(_LAPIC_IC_OFFSET, 0xffffffff);

	// wait until the PIT counter reaches zero
	while (!(inb(0x61) & 0x20));

	// disable timer interrupt
	timer_disable();

	return (0xffffffff - lapic_read_reg(_LAPIC_CC_OFFSET) + 1) * 100 * 16;
}

void
timer_stop(void)
{
	lapic_write_reg(_LAPIC_IC_OFFSET, 0);
}

void
timer_enable()
{
	u32 v = lapic_read_reg(_LAPIC_TIMER_OFFSET);
	v &= ~(1 << 16);
	lapic_write_reg(_LAPIC_TIMER_OFFSET, v);
}

void
timer_disable()
{
	u32 v = lapic_read_reg(_LAPIC_TIMER_OFFSET);
	v |= (1 << 16);
	lapic_write_reg(_LAPIC_TIMER_OFFSET, v);
}

int
timer_config(timer_config_s *config)
{
	u32 init_counter;

	if (config == 0) {
		tw_printf("config timer: config is NULL\n");
		return -1;
	}
	if (config->mode == TIMER_MODE_TSC) {
		tw_printf("config timer: mode tsc not supported yet\n");
		return -1;
	}

	u32 v = lapic_read_reg(_LAPIC_TIMER_OFFSET);
	v &= ~(3 << 17); // default is one-shot mode
	if (config->mode == TIMER_MODE_PERIODIC) {
		v |= (1 << 17);
	}
	lapic_write_reg(_LAPIC_TIMER_OFFSET, v);

	init_counter = (hz/1000) * (config->time_ms);
	lapic_write_reg(_LAPIC_IC_OFFSET, init_counter);

	return 0;
}
