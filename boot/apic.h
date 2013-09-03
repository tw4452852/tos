#ifndef APIC_H

#define APIC_H

void	apic_init(void);
void	apic_eoi(u32 irq);

#define _LAPIC_ID_OFFSET	0x20
#define _LAPIC_VER_OFFSET	0x30
#define _LAPIC_TPR_OFFSET	0x80
#define _LAPIC_APR_OFFSET	0x90
#define _LAPIC_PPR_OFFSET	0xa0
#define _LAPIC_EOI_OFFSET	0xb0
#define _LAPIC_RRD_OFFSET	0xc0
#define _LAPIC_LD_OFFSET	0xd0
#define _LAPIC_DF_OFFSET	0xe0
#define _LAPIC_SIV_OFFSET	0xf0
#define _LAPIC_ISR_OFFSET	0x100
#define _LAPIC_TMR_OFFSET	0x180
#define _LAPIC_IRR_OFFSET	0x200
#define _LAPIC_ERR_OFFSET	0x280
#define _LAPIC_CMCI_OFFSET	0x2f0
#define _LAPIC_ICR_OFFSET	0x300
#define _LAPIC_TIMER_OFFSET	0x320
#define _LAPIC_SENOR_OFFSET	0x330
#define _LAPIC_PC_OFFSET	0x340
#define _LAPIC_LINT0_OFFSET	0x350
#define _LAPIC_LINT1_OFFSET	0x360
#define _LAPIC_LERR_OFFSET	0x370
#define _LAPIC_IC_OFFSET	0x380
#define _LAPIC_CC_OFFSET	0x390
#define _LAPIC_DC_OFFSET	0x3e0

#define _LAPIC_READ_REG(base, offset) (*(u32 *)((base) + (offset)))
#define _LAPIC_WRITE_REG(base, offset, v) (*(u32 *)((base) + (offset)) = (v))

#define _RSDP_STR	"RSD PTR "
#define _APIC_STR	"APIC"
#endif /* end of include guard: APIC_H */
