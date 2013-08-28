#ifndef GDT_H

#define GDT_H

#include "common.h"

void	gdt_init(void);
void	gdt_set_desc(u8 index, u32 base, u32 limit, u16 attr);

#endif /* end of include guard: GDT_H */
