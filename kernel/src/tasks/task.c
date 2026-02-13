#include "task.h"
#include "../mem/paging.h"
#include "../mem/new/pmm.h"
#include "../gfx/serial_io.h"
#include "../shell/shell_enhanced.h"
#include "../interrupts/timer.h"

#define term_write serial_io_printf
#define TASK_SLICE_DEFAULT 50  // 50 ticks

Task *runningTask;
static Task mainTask;
static Task otherTask;
static Task shellTask;
static Task random;

extern void switchTask(Registers* from, Registers* to); // task.asm

void yield() 
{
    Task *prev = runningTask;

    do {
        runningTask = runningTask->next;
    } while (runningTask->state != TASK_READY);

    prev->state = TASK_READY;
    runningTask->state = TASK_RUNNING;
    runningTask->slice = TASK_SLICE_DEFAULT;

    switchTask(&prev->regs, &runningTask->regs);
}

void preempt_check() 
{
    if (!runningTask) return;

    if (runningTask->slice == 0) {
        yield();
    } else {
        runningTask->slice--;
    }
}

void createTask(Task *task, void (*main)(), uint64_t flags, uint64_t cr3)
{
    task->regs.rax = 0;
    task->regs.rbx = 0;
    task->regs.rcx = 0;
    task->regs.rdx = 0;
    task->regs.rsi = 0;
    task->regs.rdi = 0;
    task->regs.r8  = 0;
    task->regs.r9  = 0;
    task->regs.r10 = 0;
    task->regs.r11 = 0;
    task->regs.r12 = 0;
    task->regs.r13 = 0;
    task->regs.r14 = 0;
    task->regs.r15 = 0;

    task->regs.rflags = flags;
    task->regs.rip    = (uint64_t) main;
    task->regs.cr3    = cr3;
    task->regs.rsp    = ((uint64_t) k_malloc(0xAf1f) + 0xFFF) & ~0xF;

    task->next       = NULL;
    task->prev       = NULL;
    task->state      = TASK_READY;
    task->slice      = TASK_SLICE_DEFAULT;
}

void taskKill(Task *task)
{
    if (!task) return;
    task->state = TASK_DEAD;

    if (task->prev) task->prev->next = task->next;
    if (task->next) task->next->prev = task->prev;

    if (runningTask == task) {
        yield(); 
    }
}

void taskCreate(Task *task, void (*main)())
{
    createTask(task, main, mainTask.regs.rflags, mainTask.regs.cr3);

    Task *tail = &mainTask;
    while (tail->next && tail->next != &mainTask) tail = tail->next;

    tail->next = task;
    task->prev = tail;
    task->next = &random; 
}


static void otherMain() 
{
    while (1) {
        serial_io_printf("Hello from otherTask!\n");
        serial_io_printf("Back to mainTask...\n");
        yield();
    }
}

static void shellMain()
{
    serial_io_printf("Starting shell...\n");
    while (1) {
        shell_run();
        yield();
    }
}

static void idle_task()
{
    while (1) yield();
}

void initTasking()
{
    __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(mainTask.regs.cr3)::"%rax");
    __asm__ volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(mainTask.regs.rflags)::"%rax");

    createTask(&otherTask, otherMain, mainTask.regs.rflags, mainTask.regs.cr3);
    createTask(&random, idle_task, mainTask.regs.rflags, mainTask.regs.cr3);
    createTask(&shellTask, shellMain, mainTask.regs.rflags, mainTask.regs.cr3);

    mainTask.next     = &otherTask;
    otherTask.next    = &random;
    random.next       = &shellTask;
    shellTask.next    = &otherTask;

    otherTask.prev    = &mainTask;
    random.prev       = &otherTask;
    shellTask.prev    = &random;

    runningTask = &mainTask;
    runningTask->state = TASK_RUNNING;

    yield();
}
