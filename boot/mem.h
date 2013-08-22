#ifndef MEM_H

#define MEM_H

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

#endif /* end of include guard: MEM_H */
