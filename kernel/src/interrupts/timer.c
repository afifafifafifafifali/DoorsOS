#include "timer.h"
#include "../gfx/printf.h"

static uint64_t cpu_frequency_hz = 0;

static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void timer_init(uint64_t cpu_hz) {
    cpu_frequency_hz = cpu_hz;
    printf("Timer initialized with CPU frequency: %llu Hz\n", cpu_frequency_hz);
}

// Busy wait for approximately ms milliseconds using TSC
void timer_sleep_ms(uint64_t ms) {
    if (cpu_frequency_hz == 0) {
        // frequency not set, can't sleep accurately
        return;
    }

    uint64_t start = rdtsc();
    uint64_t wait_cycles = (cpu_frequency_hz / 1000) * ms;

    while ((rdtsc() - start) < wait_cycles) {
        asm volatile("pause"); // Yield CPU briefly
    }
}
