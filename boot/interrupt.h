#ifndef INTERRUPT_H

#define INTERRUPT_H

#include "common.h"

#define IDT_MAX_EXCEPTION	32
#define IDT_MAX_DESCS 256
#define EXCEPTION_ERRCODE_MASK  0x00027d00

#ifndef __ASSEMBLY__
void	interrupt_init(void);
void	interrupt_handler(u32 irq, u32 error_code, u32 eip, u32 cs, u32 eflag);
#endif

#endif /* end of include guard: INTERRUPT_H */
