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
static void detect_ioapic(void);

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

static void
ioapic_init()
{
	// disable pic(8259a)
	disable_8259();
	detect_ioapic();
}

static u32
find_rsdp(char *start, char *end)
{
	tw_printf("try to find rsdp in [0x%x - 0x%x]\n", (u32)start, (u32)end);
	while (start < end) {
		if (tw_memcmp(start, _RSDP_STR, 8) == 0) {
			tw_printf("find rsdp at 0x%x\n", (u32)start);
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
	} __attribute__ ((packed)) rsdt_header_s;
	rsdt_header_s *rsdt_header = (rsdt_header_s *)rsdt_base;
	tw_printf("rsdt header(0x%x): len(0x%x), revision(0x%x), oemid(%s), oem_tbl_id(%s), oem_revision(0x%x)\n",
			(u32)rsdt_header, rsdt_header->len, rsdt_header->revision, rsdt_header->oemid,
			rsdt_header->oem_tbl_id, rsdt_header->oem_revision);
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
