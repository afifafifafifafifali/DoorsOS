#include "multitasking.h"
#include "../gfx/printf.h"

extern void task_switch(task_t *old, task_t *new);

// Task array and counters
task_t tasks[MAX_TASKS];
static int current_task = -1;
static int task_count = 0;

// Kernel context stack pointer saved here:
static uint64_t kernel_rsp = 0;

// Save kernel context RSP for returning later
void multitasking_save_kernel_context(uint64_t rsp) {
    kernel_rsp = rsp;
}

int multitasking_get_current_task(void) {
    return current_task;
}

void multitasking_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].active = 0;
    }
    current_task = -1;
    task_count = 0;
}

void task_create(void (*entry_point)(void)) {
    if (task_count >= MAX_TASKS) {
        printf("[ERR] Too many tasks!\n");
        return;
    }

    task_t *t = &tasks[task_count];

    uint8_t *stack_top = t->stack + STACK_SIZE;

    // Push fake return address = entry point
    stack_top -= 8;
    *(uint64_t *)stack_top = (uint64_t)entry_point;

    // Push dummy callee-saved registers (rbp, rbx, r12-r15)
    for (int i = 0; i < 6; i++) {
        stack_top -= 8;
        *(uint64_t *)stack_top = 0xCAFEBABECAFEBABE;
    }

    t->rsp = (uint64_t)stack_top;
    t->active = 1;

    printf("[INFO] Task %d created at entry 0x%llx\n", task_count, (uint64_t)entry_point);
    task_count++;
}

void task_kill(int id) {
    if (id < 0 || id >= task_count) {
        printf("[ERR] Invalid task ID: %d\n", id);
        return;
    }
    if (!tasks[id].active) {
        printf("[WARN] Task %d already inactive\n", id);
        return;
    }

    tasks[id].active = 0;
    printf("[INFO] Task %d killed\n", id);
}

// This yield switches to next active task
void yield(void) {
    if (task_count == 0) return;

    int prev = current_task;
    int next = (current_task + 1) % task_count;

    // Find next active task
    while (!tasks[next].active) {
        next = (next + 1) % task_count;
        if (next == current_task) {
            // No other active task, return to kernel context if running tasks
            if (current_task != -1) {
                // Switch back to kernel context (main)
                current_task = -1;
                extern task_t kernel_task;
                task_switch(&tasks[prev], &kernel_task);
            }
            return;
        }
    }

    current_task = next;

    if (prev == -1) {
        // First time start, switch from kernel context to task
        extern task_t kernel_task;
        task_switch(&kernel_task, &tasks[current_task]);
    } else {
        task_switch(&tasks[prev], &tasks[current_task]);
    }
}

// This is a dummy kernel task context that holds kernel's RSP and fake stack for switching back
task_t kernel_task;

void multitasking_setup_kernel_task(uint64_t kernel_rsp_val) {
    kernel_task.rsp = kernel_rsp_val;
    kernel_task.active = 1; // Mark active for switching from tasks back
}

// Call this from kmain before first yield()
// Pass current RSP of kernel main (inline asm)
void multitasking_save_kernel_rsp(void) {
    uint64_t rsp_val;
    __asm__ volatile("mov %%rsp, %0" : "=r"(rsp_val));
    multitasking_setup_kernel_task(rsp_val);
}

void task_exit(void) {
    int id = multitasking_get_current_task();
    printf("[INFO] Task %d exiting...\n", id);
    task_kill(id);

    // Try switch to next active task
    int start = (current_task + 1) % task_count;
    int next = start;

    while (!tasks[next].active) {
        next = (next + 1) % task_count;
        if (next == start) {
            // No tasks left, switch back to kernel
            current_task = -1;
            task_switch(&tasks[id], &kernel_task);
            // never returns here
            for (;;) asm volatile("hlt");
        }
    }

    int prev = current_task;
    current_task = next;
    task_switch(&tasks[prev], &tasks[current_task]);
    for (;;) asm volatile("hlt");
}
