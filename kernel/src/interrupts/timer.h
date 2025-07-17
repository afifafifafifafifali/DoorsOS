#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Initialize timer frequency (in Hz)
void timer_init(uint64_t cpu_hz);

// Busy wait sleep in milliseconds
void timer_sleep_ms(uint64_t ms);

#endif
