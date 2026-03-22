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

    //prev->state = TASK_READY;
    if (prev->state != TASK_DEAD) {
        prev->state = TASK_READY;
    }
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
    uint64_t stack = (uint64_t) k_malloc(0x1000); // 4KB stack
    task->regs.rsp = (stack + 0x1000) & ~0xF;

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


extern Task* task_fork(void); // the fucking trampoline

Task* task_fork_impl(uint64_t live_rsp, uint64_t return_addr)
{
    Task *child = (Task*) k_malloc(sizeof(Task));
    if (!child) return NULL;

    child->regs = runningTask->regs;

    uint64_t child_stack_bottom = (uint64_t) k_malloc(0x1000);
    if (!child_stack_bottom) return NULL;

    uint64_t child_stack_top = (child_stack_bottom + 0x1000) & ~0xFULL;

    uint64_t parent_stack_base = (live_rsp & ~0xFFFULL) + 0x1000;
    uint64_t used              = parent_stack_base - live_rsp;
    uint64_t child_rsp         = child_stack_top - used;

    memcpy((void*) child_rsp, (void*) live_rsp, (size_t) used);

    // live_rsp points at the return address word, child resumes
    // after task_fork() in the caller — skip that word
    child->regs.rsp = child_rsp + 8;
    child->regs.rip = return_addr;
    child->regs.rax = 0;

    // zero caller-saved regs
    child->regs.rcx = 0; child->regs.rdx = 0;
    child->regs.rsi = 0; child->regs.rdi = 0;
    child->regs.r8  = 0; child->regs.r9  = 0;
    child->regs.r10 = 0; child->regs.r11 = 0;

    child->state = TASK_READY;
    child->slice = TASK_SLICE_DEFAULT;
    child->id    = rand();

    Task *next        = runningTask->next;
    runningTask->next = child;
    child->prev       = runningTask;
    child->next       = next;
    if (next) next->prev = child;

    return child;
}


int getCurrentTaskPID(void){
    return runningTask->id;
}

// In task.c, add this test task and call taskCreate(&forkTest, forkTestMain) from initTasking()

static Task forkTest;

static void forkTestMain()
{
    serial_io_printf("[fork test] Task starting, PID=%d\n", getCurrentTaskPID());

    Task *child = task_fork();  // calls the asm trampoline

    /*
     After fork returns:
    - In PARENT: child != NULL (pointer to child Task)
    - In CHILD: child == NULL (because rax was set to 0)
    */
    if (child) {
        // PARENT PATH
        serial_io_printf("[fork test] PARENT PID=%d: child born PID=%d\n",
                         getCurrentTaskPID(), child->id);
        while (1) {
            serial_io_printf("[fork test] PARENT PID=%d heartbeat\n", 
                           getCurrentTaskPID());
            taskDie(0);
            yield();
        }
    } else {
        // CHILD PATH
        serial_io_printf("[fork test] CHILD PID=%d: I was just born!\n",
                         getCurrentTaskPID());
        while (1) {
            serial_io_printf("[fork test] CHILD PID=%d heartbeat\n",
                             getCurrentTaskPID());
            taskDie(0);
            yield();
        }
    }
}

void taskDie(int err_code)
{
    serial_io_printf("[task %d] Exiting with code %d\n", runningTask->id, err_code);
    
    runningTask->state = TASK_DEAD;
// FUCK OFF FROM RUN QUEUEUE
    if (runningTask->prev) runningTask->prev->next = runningTask->next;
    if (runningTask->next) runningTask->next->prev = runningTask->prev;

    Task *prev = runningTask;
    
    do {
        runningTask = runningTask->next;
    } while (runningTask->state != TASK_READY);

    runningTask->state = TASK_RUNNING;
    runningTask->slice = TASK_SLICE_DEFAULT;

    // THIS SWITCH WILL NEVER WORKS
    switchTask(&prev->regs, &runningTask->regs);
    
    
    __builtin_unreachable(); // gnu moo moo majic
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
    taskCreate(&forkTest,forkTestMain);
    // Enter multitasking
    yield();

    
}