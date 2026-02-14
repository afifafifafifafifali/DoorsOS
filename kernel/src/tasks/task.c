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

//#define STACK_SIZE 0xB000   // round number, page-ish
const uint64_t STACK_SIZE = 0xAf1f;
void createTask(Task *task, void (*main)(), uint64_t flags, uint64_t cr3)
{
    memset(task, 0, sizeof(Task));

    uint8_t *stack = k_malloc(STACK_SIZE);
    if (!stack) {
        serial_io_printf("STACK FAIL\n");
        while (1);
    }

    uint64_t top = (uint64_t)(stack + STACK_SIZE);

    // 16-byte align
    top &= ~0xF;

    task->regs.rip = (uint64_t)main;
    task->regs.rsp = top;
    task->regs.rflags = flags;
    task->regs.cr3 = cr3;

    task->state = TASK_READY;
    task->slice = TASK_SLICE_DEFAULT;
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
    serial_io_printf("Inside taskCreate\n");

    // Allocate stack
     // 44 KB
    uint8_t *stack_virt = (uint8_t*)0xFFFF800000900000; // pick safe high virtual memory

    for (uint64_t addr = (uint64_t)stack_virt - STACK_SIZE; addr < (uint64_t)stack_virt; addr += 0x1000)
    {
        void *page = k_malloc(0x1000);
        mapPage(addr, virt_to_phys(page), PAGE_PRESENT | PAGE_WRITE);
    }

    createTask(task, main, mainTask.regs.rflags, mainTask.regs.cr3);
    task->regs.rsp = (uint64_t)stack_virt; // top of stack

    // Directly insert before random
    Task *prev = random.prev;
    if (!prev) prev = &mainTask; // fallback if random.prev not set

    prev->next = task;
    task->prev = prev;
    task->next = &random;
    random.prev = task;

    serial_io_printf("Task safely linked before random\n");

    // No yield here
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
