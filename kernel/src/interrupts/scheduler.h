#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define MAX_TASKS 32
#define TASK_STACK_SIZE 0x8000

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_DEAD
} task_state_t;

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags;
} task_regs_t;

typedef struct {
    int tid;
    task_state_t state;
    int priority;
    uint64_t quantum;
    uint8_t *stack_base;
    task_regs_t regs;
} task_control_block_t;

void scheduler_init(void);
int sched_task_create(void (*entry)(void), int priority);
void scheduler_tick(void);
void sched_task_exit(void);
int get_current_tid(void);
void scheduler_enable(void);
void scheduler_disable(void);

#endif
