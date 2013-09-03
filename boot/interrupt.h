#ifndef INTERRUPT_H

#define INTERRUPT_H

#include "common.h"

#define IDT_MAX_EXCEPTION		32
#define IDT_MAX_DESCS			256
#define EXCEPTION_ERRCODE_MASK  0x00027d00

#ifndef __ASSEMBLY__
void	interrupt_init(void);
void	default_handler(u32 irq, u32 error_code, u32 eip, u32 cs, u32 eflag);

typedef struct {
	void (*callback)(void *);
	void *arg;
} intr_handler_s;
int		register_irq(u8 irq, intr_handler_s *handler);
void	unregister_irq(u8 irq);
#endif

#endif /* end of include guard: INTERRUPT_H */
