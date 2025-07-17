
#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void init_pit(uint32_t frequency);
uint32_t pit_sleep(uint64_t milliseconds);

#endif
