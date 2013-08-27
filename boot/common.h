#ifndef COMMON_H

#define COMMON_H

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

#endif /* end of include guard: COMMON_H */
