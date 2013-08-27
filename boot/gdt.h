#ifndef GDT_H

#define GDT_H

#include "common.h"

typedef union {
	u64 u;
	struct {
		u64	limit_low	: 16;
		u64	base_low	: 16;
		u64	base_mid	: 8;
		u64	attr_low	: 8;
		u64	limit_high	: 4;
		u64 attr_high	: 4;
		u64	base_high	: 8;
	} s;
} gdt_desc_u;

#define _DA_G			(1 << 11)
#define _DA_SEG32		(1 << 10)
#define _DA_SEG64		(1 << 9)
#define _DA_P			(1 << 7)
#define _DA_DPL1		(0x1 << 5)
#define _DA_DPL2		(0x2 << 5)
#define _DA_DPL3		(0x3 << 5)
#define _DA_TYPE		(1 << 4)
#define _DA_DATA_R		0X0
#define _DA_DATA_RA		0x1
#define _DA_DATA_RW		0x2
#define _DA_DATA_RWA	0x3
#define _DA_DATA_RED	0x4
#define _DA_DATA_REDA	0x5
#define _DA_DATA_RWED	0x6
#define _DA_DATA_RWEDA	0x7
#define _DA_CODE_X		0x8
#define _DA_CODE_XA		0x9
#define _DA_CODE_XR		0xa
#define _DA_CODE_XRA	0xb
#define _DA_CODE_XC		0xc
#define _DA_CODE_XCA	0xd
#define _DA_CODE_XRC	0xe
#define _DA_CODE_XRCA	0xf
#define GDT_CODE_PL0	(_DA_G | _DA_SEG32 | _DA_P | _DA_TYPE | _DA_CODE_XR)
#define GDT_DATA_PL0	(_DA_G | _DA_SEG32 | _DA_P | _DA_TYPE | _DA_DATA_RW)

#define GDT_MAX_DESCS	16
#define CODE_PL0_INDEX	1
#define DATA_PL0_INDEX	2

#define _SEL_IDT	(1 << 2)
#define _SEL_RPL1	0x1
#define _SEL_RPL2	0x2
#define _SEL_RPL3	0x3

#define SEL_CODE_PL0	((CODE_PL0_INDEX) << 3)
#define SEL_DATA_PL0	((DATA_PL0_INDEX) << 3)

void	gdt_init(void);
void	gdt_set_desc(u8 index, u32 base, u32 limit, u16 attr);

#endif /* end of include guard: GDT_H */
