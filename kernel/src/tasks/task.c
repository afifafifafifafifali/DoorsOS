#include "task.h"
#include "../mem/paging.h"
#include "../mem/new/pmm.h"
#include "../gfx/serial_io.h"
#include "../shell/shell_enhanced.h"
#include "../interrupts/timer.h"
#include "../libs/utilities.h"

#define term_write serial_io_printf
#define TASK_SLICE_DEFAULT 50  // now each task gets 50 ticks

#define MAX_TASKS 32600 // lower than RAND_MAX




Task *runningTask;
static Task mainTask;
static Task otherTask;
static Task shellTask;
static Task random;



extern void switchTask(Registers* from, Registers* to);

// --- Cooperative yield ---
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

// --- Preemptive check (call from timer/loops) ---
void preempt_check() 
{
    if (!runningTask) return;

    if (runningTask->slice == 0) {
        yield();
    } else {
        runningTask->slice--;
    }
}

// --- Core task creation ---
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

// --- Kill a task (kernel only) ---
void taskKill(Task *task)
{
    if (!task) return;
    task->state = TASK_DEAD;

    // Unlink from run queue
    if (task->prev) task->prev->next = task->next;
    if (task->next) task->next->prev = task->prev;

    if (runningTask == task) {
        yield(); // switch immediately
    }
}

// --- Create a task and add to run queue ---
void taskCreate(Task *task, void (*main)())
{
    createTask(task, main, mainTask.regs.rflags, mainTask.regs.cr3);

    
    Task *tail = &random;
    tail->next = task;
    task->prev = tail;
    task->next = &shellTask; // circular
    task->id = rand();
}

// --- Example Tasks ---
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

Task blll;
static int ligma = 0;
static void blllM(){
    while (1){
        serial_io_printf("PISS\n");
        ligma++;
        if(ligma == 10){
        taskKill(&blll);
    }
        yield();  

          }
}


typedef uint64_t (*syscall_t)(uint64_t, uint64_t, uint64_t, uint64_t);
#define SYSCALL_PTR ((syscall_t*)0xD00F5AF1F)

#include "../syscall/syscall.h"
Task syscallTask;
int sigma = 0;
static void syscall_test_task()
{
    char buf[64];
    syscall_t sc = *SYSCALL_PTR; // fetch syscall pointer

    char *msg = "Hello via 0xD00F5AF1F!\n";
    sc(SYS_WRITE, (uint64_t)msg, strlen(msg), 0);

    sigma++;
    if(sigma == 1){
        taskKill(&syscallTask);
    }
    yield();

    
}


void getCurrentTaskPID(void){
    return runningTask->id;
}
// --- Tasking Init ---
void initTasking()
{
    // Save RFLAGS & CR3 from current context
    __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(mainTask.regs.cr3)::"%rax");
    __asm__ volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(mainTask.regs.rflags)::"%rax");

    createTask(&otherTask, otherMain, mainTask.regs.rflags, mainTask.regs.cr3);
    otherTask.id = 1;
    random.id = 2; shellTask.id = 3;
    createTask(&random, idle_task, mainTask.regs.rflags, mainTask.regs.cr3);
    createTask(&shellTask, shellMain, mainTask.regs.rflags, mainTask.regs.cr3);

    // Setup circular run queue
    mainTask.next     = &otherTask;
    otherTask.next    = &random;
    random.next       = &shellTask;
    shellTask.next    = &otherTask;

    otherTask.prev    = &mainTask;
    random.prev       = &otherTask;
    shellTask.prev    = &random;

    runningTask = &mainTask;
    runningTask->state = TASK_RUNNING;


    taskCreate(&blll,blllM);
    taskCreate(&syscallTask, syscall_test_task);
    // Enter multitasking
    yield();

    
}