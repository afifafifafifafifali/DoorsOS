#include "timer.h"
#include "../gfx/printf.h"
#include "../ps2/io.h"
#include "isr.h"
#include "pic.h"
#include <stdbool.h>
#include "../tasks/task.h"

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
/*
void timer_sleep_ms(uint64_t ms) {
    if (cpu_frequency_hz == 0) return;
    uint64_t start = rdtsc();
    uint64_t wait_cycles = (cpu_frequency_hz / 1000) * ms;
    while ((rdtsc() - start) < wait_cycles) {
        asm volatile("pause");
    }
}*/

void timer_sleep_ms(uint64_t ms) {
    //uint64_t target_ticks = timer_ticks + (ms * 100 / 1000); //  100Hz PIT
    uint64_t target_ticks = timer_ticks + (ms + 9) / 10;
    while (timer_ticks < target_ticks) {
        asm volatile("hlt"); 
    }
}

uint64_t timer_get_ticks(void) {
    return timer_ticks;
}

static void timer_irq_handler(interrupt_frame_t* frame) {
    (void)frame;
    timer_ticks++;

    if ((timer_get_ticks() - last_switch_tick) >= 5) {
        needs_yield = true;
    }

    send_eoi_to_irq(0);

    // TODO : use the need_yield flag.

}

void pit_init(uint32_t freq) {
    uint32_t divisor = 1193180 / freq;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    register_irq_handler(32, timer_irq_handler);
    clear_mask_for_irq(0);
    printf("PIT initialized at %u Hz (divisor: %u)\n", freq, divisor);
}

void pit_test(void) {
    printf("PIT Test: Starting 3-second timer test...\n");
    uint64_t start_ticks = timer_ticks;

    while ((timer_ticks - start_ticks) < 300) {
        asm volatile("pause");
    }

    uint64_t elapsed_ticks = timer_ticks - start_ticks;
    printf("PIT Test: Expected 300 ticks, got %llu ticks\n", elapsed_ticks);
    printf("PIT Test: %s\n", (elapsed_ticks >= 290 && elapsed_ticks <= 310) ? "PASS" : "FAIL");
}

// HERTZ ONLY 
uint64_t measure_cpu_frequency_with_pit(void) {
    uint64_t start_ticks = timer_ticks;
    uint64_t start_tsc = rdtsc();

    while ((timer_ticks - start_ticks) < 100) {
        asm volatile("pause");
    }

    uint64_t end_tsc = rdtsc();
    uint64_t tsc_delta = end_tsc - start_tsc;

    uint64_t measured_freq = tsc_delta; 

    

    return measured_freq;
}

