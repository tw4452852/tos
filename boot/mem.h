#ifndef MEM_H

#define MEM_H

#define MEM_DESC_CNT_ADDR	0x9000
#define MEM_MAP_ADDR		0x9004

#define tw_memset(d, v, l) \
		do { \
			__asm__ ( \
					"rep stosb\n\t" \
					: \
					: "a"((v)), "D"((d)), "c"((l)) \
					); \
		} while (0)

#define tw_memcpy(d, s, l) \
		do { \
			__asm__ ( \
					"rep movsb\n\t" \
					: \
					: "D"((d)), "S"((s)), "c"((l)) \
					); \
		} while (0)

typedef struct {
	long	base_low;
	long	base_high;
	long	length_low;
	long	length_high;
	long	type;
} mem_desc_s;

void	show_mem_map(void);
#endif /* end of include guard: MEM_H */
