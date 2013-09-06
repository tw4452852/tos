#ifndef TIMER_H

#define TIMER_H

#include "common.h"

typedef enum {
	TIMER_MODE_PERIODIC = 0,
	TIMER_MODE_ONESHOT,
	TIMER_MODE_TSC,
} timer_mode_e;

typedef struct {
	timer_mode_e	mode;
	u32				time_ms;
} timer_config_s;

void	timer_init(void);
void	timer_enable(void);
void	timer_disable(void);
void	timer_stop(void);
int		timer_config(timer_config_s *conf);

#define TIMER_IRQ	0x20

#endif /* end of include guard: TIMER_H */
