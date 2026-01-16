#include "scheduler.h"
#include "../mem/heap.h"
#include "../gfx/printf.h"
#include <stddef.h>
#include <stdbool.h>

static task_control_block_t tasks[MAX_TASKS];
static int current_task = -1;
static int task_count = 0;
bool scheduler_enabled = false;

extern void context_switch(task_regs_t *old, task_regs_t *new);

void scheduler_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_DEAD;
    }
    current_task = -1;
    task_count = 0;
}

int sched_task_create(void (*entry)(void), int priority) {
    if (task_count >= MAX_TASKS) return -1;
    
    int tid = task_count++;
    task_control_block_t *t = &tasks[tid];
    
    t->tid = tid;
    t->state = TASK_READY;
    t->priority = priority;
    t->quantum = priority + 1;
    t->stack_base = malloc(TASK_STACK_SIZE);
    
    if (!t->stack_base) return -1;
    
    uint64_t *stack = (uint64_t *)(t->stack_base + TASK_STACK_SIZE);
    *(--stack) = (uint64_t)sched_task_exit;
    
    t->regs.rax = 0;
    t->regs.rbx = 0;
    t->regs.rcx = 0;
    t->regs.rdx = 0;
    t->regs.rsi = 0;
    t->regs.rdi = 0;
    t->regs.rbp = 0;
    t->regs.rsp = (uint64_t)stack;
    t->regs.r8 = 0;
    t->regs.r9 = 0;
    t->regs.r10 = 0;
    t->regs.r11 = 0;
    t->regs.r12 = 0;
    t->regs.r13 = 0;
    t->regs.r14 = 0;
    t->regs.r15 = 0;
    t->regs.rip = (uint64_t)entry;
    t->regs.rflags = 0x202;
    
    return tid;
}

void scheduler_tick(void) {
    if (task_count == 0 || current_task < 0) return;
    
    int next = (current_task + 1) % task_count;
    while (tasks[next].state != TASK_READY && next != current_task) {
        next = (next + 1) % task_count;
    }
    
    if (tasks[next].state != TASK_READY) return;
    
    if (current_task >= 0 && tasks[current_task].state == TASK_RUNNING) {
        tasks[current_task].state = TASK_READY;
    }
    
    tasks[next].state = TASK_RUNNING;
    int prev = current_task;
    current_task = next;
    
    if (prev >= 0) {
        context_switch(&tasks[prev].regs, &tasks[next].regs);
    }
}

void sched_task_exit(void) {
    if (current_task >= 0) {
        tasks[current_task].state = TASK_DEAD;
        free(tasks[current_task].stack_base);
    }
    scheduler_tick();
    while(1);
}

int get_current_tid(void) {
    return current_task;
}

void scheduler_enable(void) {
    scheduler_enabled = true;
}

void scheduler_disable(void) {
    scheduler_enabled = false;
}
