#include "common.h"
#include "apic.h"
#include "video.h"

static u32 lapic_base_addr;

#define lapic_read_reg(offset) _LAPIC_READ_REG(lapic_base_addr, offset)
#define lapic_write_reg(offset, v) _LAPIC_WRITE_REG(lapic_base_addr, offset, v)

static void get_lapic_base_addr(void);
static void	show_all_lapic_regs(void);
static void	lapic_init(void);
static void	ioapic_init(void);
static void	disable_8259(void);

void
apic_init()
{
	lapic_init();
	ioapic_init();
	__asm__("sti");
}

void
lapic_init()
{
	get_lapic_base_addr();
	// inhibite the softints
	lapic_write_reg(_LAPIC_TPR_OFFSET, 0x20);
	// local interrupt 0 is a external interrupts
	lapic_write_reg(_LAPIC_LINT0_OFFSET, 0x8700);
	// local interrupt 1 is a nmi
	lapic_write_reg(_LAPIC_LINT1_OFFSET, 0x400);
	show_all_lapic_regs();
}

static inline void
get_lapic_base_addr()
{
	__asm__(
			"rdmsr"
			:"=a"(lapic_base_addr)
			:"c"(0x1b)
	);
	// clear lowest 12 bit
	lapic_base_addr &= 0xfffff000;
}

static inline void
show_all_lapic_regs()
{
	tw_printf("show local apic regs:\n");
	tw_printf(">>> base addr: 0x%x\n", lapic_base_addr);
	tw_printf(">>> task priority: 0x%x\n", lapic_read_reg(_LAPIC_TPR_OFFSET));
	tw_printf(">>> timer interrupt vector: 0x%x\n", lapic_read_reg(_LAPIC_TIMER_OFFSET));
	tw_printf(">>> performance counter: 0x%x\n", lapic_read_reg(_LAPIC_PC_OFFSET));
	tw_printf(">>> local interrupt 0: 0x%x\n", lapic_read_reg(_LAPIC_LINT0_OFFSET));
	tw_printf(">>> local interrupt 1: 0x%x\n", lapic_read_reg(_LAPIC_LINT1_OFFSET));
	tw_printf(">>> error interrupt: 0x%x\n", lapic_read_reg(_LAPIC_LERR_OFFSET));
	tw_printf(">>> spurious interrupt: 0x%x\n", lapic_read_reg(_LAPIC_SIV_OFFSET));
}

static int
find_rsdp(char *start, char *end)
{
	
}

static void
ioapic_init()
{
	// disable pic(8259a)
	disable_8259();
	find_rsdp((char *)0xe0000, (char *)0xfffff);
}

static inline void
disable_8259()
{
	__asm__(
			"outb $0x21\n\t"
			"outb $0xa1\n\t"
			: :"a"(0xff)
	);
}
