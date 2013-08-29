#ifndef COMMON_H

#define COMMON_H

// type
#define u8	unsigned char
#define s8	signed char
#define u16	unsigned short
#define s16 signed short
#ifdef __i386__
#define u32 unsigned long
#define s32 signed long
#define u64 unsigned long long
#define s64 signed long long
#else
#define u32 unsigned int
#define s32 signed int
#define u64 unsigned long
#define s64 signed long
#endif

#ifdef __ASSEMBLY__
	#define _AC(v, t) v
	#define _AT(v, t) v
#else
	#define __AC(v, t) (v##t)
	#define _AC(v, t) __AC(v, t)
	#define _AT(v, t) ((t)(v))
#endif

// gdt
#define GDT_DESC(base, limit, attr) \
	((((base)	& _AC(0xff000000, ULL)) << (56-24)) | \
	 (((attr)	& _AC(0X0000f0ff, ULL)) << 40) | \
	 (((limit)	& _AC(0x000f0000, ULL)) << (48-16)) | \
	 (((base)	& _AC(0x00ffffff, ULL)) << 16) | \
	 (((limit)	& _AC(0x0000ffff, ULL))))

#define _DA_G			(1 << 15)
#define _DA_SEG32		(1 << 14)
#define _DA_SEG64		(1 << 13)
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

#define _SEL_IDT	(1 << 2)
#define _SEL_RPL1	0x1
#define _SEL_RPL2	0x2
#define _SEL_RPL3	0x3

#define GDT_MAX_DESCS	16
#define CODE_PL0_INDEX	1
#define DATA_PL0_INDEX	2

#define SEL_CODE_PL0	((CODE_PL0_INDEX) << 3)
#define SEL_DATA_PL0	((DATA_PL0_INDEX) << 3)

// idt
#define _IDT_ATTR	0x8e00
#define IDT_DESC(offset) \
	((((offset)			& _AC(0xffff0000, ULL)) << (48 - 16)) | \
	 (((_IDT_ATTR)		& _AC(0x0000ffff, ULL)) << 32) | \
	 (((SEL_CODE_PL0)	& _AC(0x0000ffff, ULL)) << 16) | \
	 (((offset)			& _AC(0x0000ffff, ULL))))

// kernel is located at 0x1000 * 16 + 0
#define KERNEL_ADDR_SEG		0x1000
#define KERNEL_ADDR_OFFSET	0x0
#define KERNEL_ADDR (KERNEL_ADDR_SEG*16 + KERNEL_ADDR_OFFSET)

#define STACK_TOP (KERNEL_ADDR)

#endif /* end of include guard: COMMON_H */
