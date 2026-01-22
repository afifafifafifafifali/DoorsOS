#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_init(uint64_t cpu_hz);
void timer_sleep_ms(uint64_t ms);
uint64_t timer_get_ticks(void);
void pit_init(uint32_t freq);
void pit_test(void);

#endif
