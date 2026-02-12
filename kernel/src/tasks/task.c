#include "task.h"
#include "../mem/paging.h"
#include "../mem/new/pmm.h"
#include "../gfx/serial_io.h"
#include "../shell/shell_enhanced.h"
#include "../interrupts/timer.h"

#define term_write serial_io_printf

static Task *runningTask;
static Task mainTask;
static Task otherTask;
static Task shellTask;
static Task random;

extern void switchTask(Registers* from, Registers* to);

// Voluntary yield
void yield() 
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

// Call in long loops to allow cooperative multitasking
void preempt_check() 
{
    // Nothing needed in pure volunteer mode
}

void createTask(Task *task, void (*main)(), uint64_t flags, uint64_t cr3)
{
    task->regs.rax = 0;
    task->regs.rbx = 0;
    task->regs.rcx = 0;
    task->regs.rdx = 0;
    task->regs.rsi = 0;
    task->regs.rdi = 0;
    task->regs.rflags = flags;
    task->regs.rip = (uint64_t) main;
    task->regs.cr3 = cr3;
    task->regs.rsp = ((uint64_t) k_malloc(0xAf1f) + 0xFFF) & ~0xF; 
    task->next = NULL;
}

static void otherMain() 
{
    while(1){
        serial_io_printf("Hello from otherTask!\n");
        serial_io_printf("Back to mainTask...\n");
        yield(); // voluntary yield
    }
    
}

static void shellMain()
{
    serial_io_printf("Starting shell...\n");
    while (1) {
        shell_run(); // your shell
        yield();     // voluntary yield
    }
}

static void idle_task()
{
    while(1) {
        // Idle task - just keep yielding
        yield();
    }
}

void initTasking() 
{
    // Get RFLAGS and CR3
    
    __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %0;":"=m"(mainTask.regs.cr3)::"%rax");
    __asm__ volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=m"(mainTask.regs.rflags)::"%rax");
 
    createTask(&otherTask, otherMain, mainTask.regs.rflags, mainTask.regs.cr3);
    createTask(&random,idle_task,mainTask.regs.rflags,mainTask.regs.cr3);
    createTask(&shellTask,shellMain,mainTask.regs.rflags,mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &random;
    random.next = &shellTask;
    shellTask.next = &otherTask;

 
    runningTask = &mainTask;
    // Proper way,do not edit.
    yield();
}