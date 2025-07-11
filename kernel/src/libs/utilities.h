#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
// --- Random Number Generator ---
void srand(uint32_t seed);
int rand(void); // Returns a pseudo-random number between 0 and RAND_MAX

#define RAND_MAX 32767


// --- Integer Math Utilities ---
int abs(int x);
int min(int a, int b);
int max(int a, int b);
int clamp(int val, int min_val, int max_val);


// --- Bitwise Utilities ---
bool is_power_of_two(uint64_t x);
uint64_t align_up(uint64_t value, uint64_t alignment);
uint64_t align_down(uint64_t value, uint64_t alignment);

// --- Misc ---
void udelay(volatile uint64_t cycles); // crude delay loop
void udelay_ms(uint64_t ms);