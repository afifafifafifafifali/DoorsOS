#include "timer.h"
#include "../gfx/printf.h"
#include "../ps2/io.h"
#include "isr.h"
#include "pic.h"
#include "scheduler.h"
#include <stdbool.h>

extern bool scheduler_enabled;

static uint64_t cpu_frequency_hz = 0;
static uint64_t timer_ticks = 0;

static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void timer_init(uint64_t cpu_hz) {
    cpu_frequency_hz = cpu_hz;
    printf("Timer initialized with CPU frequency: %llu Hz\n", cpu_frequency_hz);
}

void timer_sleep_ms(uint64_t ms) {
    if (cpu_frequency_hz == 0) return;
    uint64_t start = rdtsc();
    uint64_t wait_cycles = (cpu_frequency_hz / 1000) * ms;
    while ((rdtsc() - start) < wait_cycles) {
        asm volatile("pause");
    }
}

uint64_t timer_get_ticks(void) {
    return timer_ticks;
}

static void timer_irq_handler(interrupt_frame_t* frame) {
    (void)frame;
    timer_ticks++;
    if (scheduler_enabled) {
        scheduler_tick();
    }
    send_eoi_to_irq(0);
}

void pit_init(uint32_t freq) {
    uint32_t divisor = 1193180 / freq;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    register_irq_handler(32, timer_irq_handler);
    clear_mask_for_irq(0);
}
