#pragma once

#include <stdint.h>

/* Global CPU info */
extern char vendor[100];
extern int detected;
extern uint64_t memory_amount;

/* Functions */
void print_cpu_info(void);
void return_cpu(void);
