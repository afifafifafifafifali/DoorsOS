#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>

#define MAX_TASKS 16
#define STACK_SIZE 0x4000

typedef struct {
    uint8_t stack[STACK_SIZE];
    uint64_t rsp;
    int active;
} task_t;

void multitasking_init(void);
void task_create(void (*entry_point)(void));
void yield(void);
void task_kill(int id);
void task_exit(void);
int multitasking_get_current_task(void);

// Save kernel main context, needed for returning after tasks end
void multitasking_save_kernel_context(uint64_t rsp);

#endif
