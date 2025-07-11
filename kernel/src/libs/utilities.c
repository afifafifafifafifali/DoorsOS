#include "utilities.h"
#include <stdint.h>
#include <stdbool.h>
// --- Random Number Generator ---

static uint32_t rand_state = 1;

void srand(uint32_t seed) {
    rand_state = seed ? seed : 1;  // Avoid zero seed
}

int rand(void) {
    // Simple LCG parameters from Numerical Recipes
    rand_state = rand_state * 1103515245 + 12345;
    return (int)((rand_state >> 16) & RAND_MAX);
}

// --- Integer Math Utilities ---

int abs(int x) {
    return (x < 0) ? -x : x;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int clamp(int val, int min_val, int max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

// --- Bitwise Utilities ---

bool is_power_of_two(uint64_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

uint64_t align_up(uint64_t value, uint64_t alignment) {
    if (alignment == 0) return value;  // Avoid division/modulo by zero
    uint64_t remainder = value % alignment;
    if (remainder == 0) return value;
    return value + (alignment - remainder);
}

uint64_t align_down(uint64_t value, uint64_t alignment) {
    if (alignment == 0) return value;
    return value - (value % alignment);
}

// --- Misc ---

void udelay(volatile uint64_t cycles) {
    while (cycles--) {
        __asm__ __volatile__("nop");
    }
}

void udelay_ms(uint64_t ms) {
    // Rough approximation: CPU_FREQ_MHZ = CPU frequency in MHz
    const uint64_t CPU_FREQ_MHZ = 1000; // 1 GHz example
    udelay(ms * CPU_FREQ_MHZ * 1000);
}