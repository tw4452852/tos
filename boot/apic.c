#include "common.h"
#include "apic.h"
#include "video.h"
#include "string.h"
#include "io.h"

static u32 lapic_base_addr;
static u32 ioapic_base_addr = 0;

static void get_lapic_base_addr(void);
static void	show_all_lapic_regs(void);
static void	lapic_init(void);
static void	ioapic_init(void);
static void	disable_8259(void);
static void detect_ioapic(void);
static u32	ioapic_read(u32 index);
static void	ioapic_write(u32 index, u32 value);
static void	switch_to_apic(void);

#define _LAPIC_READ_REG(base, offset) (*(u32 *)((base) + (offset)))
#define _LAPIC_WRITE_REG(base, offset, v) (*(u32 *)((base) + (offset)) = (v))
u32 inline
lapic_read_reg(u32 offset)
{
	return _LAPIC_READ_REG(lapic_base_addr, offset);
}

void
lapic_write_reg(u32 offset, u32 v)
{
	_LAPIC_WRITE_REG(lapic_base_addr, offset, v);
}

void
apic_eoi(u32 irq)
{
	lapic_write_reg(_LAPIC_EOI_OFFSET, 0);
}

void
apic_init()
{
	__asm__("cli");
	lapic_init();
	ioapic_init();
	__asm__("sti");
}

void
lapic_init()
{
	get_lapic_base_addr();
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
	tw_printf("show local apic(%d) regs:\n", lapic_read_reg(_LAPIC_ID_OFFSET));
	tw_printf(">>> version: 0x%x\n", lapic_read_reg(_LAPIC_VER_OFFSET));
	tw_printf(">>> task priority: 0x%x\n", lapic_read_reg(_LAPIC_TPR_OFFSET));
	tw_printf(">>> timer interrupt vector: 0x%x\n", lapic_read_reg(_LAPIC_TIMER_OFFSET));
	tw_printf(">>> performance counter: 0x%x\n", lapic_read_reg(_LAPIC_PC_OFFSET));
	tw_printf(">>> local interrupt 0: 0x%x\n", lapic_read_reg(_LAPIC_LINT0_OFFSET));
	tw_printf(">>> local interrupt 1: 0x%x\n", lapic_read_reg(_LAPIC_LINT1_OFFSET));
	tw_printf(">>> error interrupt: 0x%x\n", lapic_read_reg(_LAPIC_LERR_OFFSET));
	tw_printf(">>> spurious interrupt: 0x%x\n", lapic_read_reg(_LAPIC_SIV_OFFSET));
}

static void
ioapic_init()
{
	// disable pic(8259a)
	disable_8259();
	detect_ioapic();
	if (ioapic_base_addr == 0) {
		return;
	}
	// ioapic irq mapping
	// ioapic irq : cpu vector
	// 0-15: 0x20-0x2f
	int irq;
	for (irq = 0; irq < 16; irq++) {
		if (irq == 2) {
			//disable irq2
			continue;
		}
		ioapic_write(0x10 + irq*2, 0x20 + irq);
		//tw_printf("irq %d: 0x%x%x\n", irq, ioapic_read(0x10+irq*2+1), ioapic_read(0x10+irq*2));
	}

	switch_to_apic();
}

static inline void
switch_to_apic()
{
	// for recently motherborad, there is no need to do these
	outb(0x22, 0x70);
	outb(0x23, 0x01);
}

static inline u32
ioapic_read(u32 index)
{
	*(u32 *)ioapic_base_addr = index;
	return *(u32 *)(ioapic_base_addr + 0x10);
}

static inline void
ioapic_write(u32 index, u32 value)
{
	*(u32 *)ioapic_base_addr = index;
	*(u32 *)(ioapic_base_addr + 0x10) = value;
}


static inline void
disable_8259()
{
	outb(0x21, 0xff);
	outb(0xa1, 0xff);
}

static u32
find_rsdp(char *start, char *end)
{
	while (start < end) {
		if (tw_memcmp(start, _RSDP_STR, strlen(_RSDP_STR)) == 0) {
			return (u32)start;
		}
		start += 16;
	}
	return 0;
}


static void
detect_ioapic()
{
	u32 ebda_base = (*(u16 *)0x40e) << 4;
	u32 rsdp_base, rsdt_base;
	int i, entries_cnt;

	// try to find rsdp within 1Kb of ebda and [0xe0000-0xfffff]
	rsdp_base = find_rsdp((char *)ebda_base, (char *)(ebda_base + 0x400));
	if (rsdp_base == 0) {
		rsdp_base = find_rsdp((char *)0xe0000, (char *)0xfffff);
	}
	if (rsdp_base == 0) {
		tw_printf("can't find rsdp!!\n");
		return;
	}
	typedef struct {
		char	sig[8];
		u8		checksum;
		char	oemid[6];
		u8		revision;
		u32		rsdt_addr;
	} __attribute__ ((packed)) rsdp_s;
	rsdp_s *rsdp = (rsdp_s *)rsdp_base;
	tw_printf("rsdp(0x%x): oemid(%s), revision(0x%x), rsdt_addr(0x%x)\n",
			(u32)rsdp, rsdp->oemid, rsdp->revision, rsdp->rsdt_addr);

	// parse rsdt
	rsdt_base = rsdp->rsdt_addr;
	typedef struct {
		char	sig[4];
		u32		len;
		u8		revision;
		u8		checksum;
		char	oemid[6];
		char	oem_tbl_id[8];
		u32		oem_revision;
		u32		creator_id;
		u32		creator_revison;
		char*	entries[];
	} __attribute__ ((packed)) rsdt_header_s;
	rsdt_header_s *rsdt_header = (rsdt_header_s *)rsdt_base;
	tw_printf("rsdt header(0x%x): len(0x%x), revision(0x%x), oemid(%s), oem_tbl_id(%s), oem_revision(0x%x)\n",
			(u32)rsdt_header, rsdt_header->len, rsdt_header->revision, rsdt_header->oemid,
			rsdt_header->oem_tbl_id, rsdt_header->oem_revision);

	// find madt
	entries_cnt = (rsdt_header->len - sizeof(*rsdt_header))/sizeof(char*);
	for (i = 0; i < entries_cnt; i++) {
		if (tw_memcmp(rsdt_header->entries[i], _APIC_STR, strlen(_APIC_STR)) == 0) {
			break;
		}
	}
	if (i == entries_cnt) {
		tw_printf("can't find madt!!\n");
		return;
	}

	// find io-apic
	struct ioapic_head {
		u8 type;
		u8 len;
		u8 ioapic_id;
		u8 reverse;
		u32 ioapic_addr;
		u32 globl_sys_int_base;
	} __attribute__ ((packed)) *p = (struct ioapic_head *)(rsdt_header->entries[i] + 44);
	while ((u32)p < (u32)(rsdt_header->entries[i] + ((rsdt_header_s *)(rsdt_header->entries[i]))->len)) {
		if (p->type == 1) {
			break;
		}
		p = (struct ioapic_head *)((char *)p + p->len);
	}
	if (p->type != 1) {
		tw_printf("can't find ioapic entry!!\n");
		return;
	}
	tw_printf("ioapic: id(%d), addr(0x%x), global_sys_interrupt_base(%d)\n",
			p->ioapic_id, p->ioapic_addr, p->globl_sys_int_base);

	ioapic_base_addr = p->ioapic_addr;
}
