#include "user_enhanced.h"
#include "../gfx/printf.h"
#include "../interrupts/multitasking.h"
#include "../mem/new/pmm.h"
#include "../libs/string.h"
#include "../interrupts/timer.h"

#define MAX_USER_TASKS 16

typedef struct {
    uint32_t id;
    char name[32];
    void (*func)(void);
    int active;
} user_task_t;

static user_task_t user_tasks[MAX_USER_TASKS];
static int task_count = 0;

void user_init(void) {
    memset(user_tasks, 0, sizeof(user_tasks));
    task_count = 0;
}

int user_create_task(const char* name, void (*func)(void)) {
    if (task_count >= MAX_USER_TASKS) {
        return -1;
    }
    
    user_task_t* task = &user_tasks[task_count];
    strncpy(task->name, name, 31);
    task->name[31] = 0;
    task->func = func;
    task->active = 1;
    
    task_create(func);
    task->id = task_count + 1;
    
    task_count++;
    return task->id;
}

void user_list_tasks(void) {
    int active = 0;
    for (int i = 0; i < task_count; i++) {
        if (user_tasks[i].active) {
            printf("  [%d] %s\n", user_tasks[i].id, user_tasks[i].name);
            active++;
        }
    }
    if (active == 0) {
        printf("  No active tasks\n");
    }
}

void task_counter(void) {
    for (int i = 0; i < 10; i++) {
        printf("Counter: %d\n", i);
        timer_sleep_ms(500);
        yield();
    }
    task_kill(multitasking_get_current_task());
}

void task_hello(void) {
    for (int i = 0; i < 5; i++) {
        printf("Hello from task!\n");
        timer_sleep_ms(1000);
        yield();
    }
    task_kill(multitasking_get_current_task());
}

void task_stress(void) {
    printf("Stress test starting...\n");
    for (int i = 0; i < 100; i++) {
        void* ptr = k_malloc(1024);
        if (ptr) k_free(ptr);
        yield();
    }
    printf("Stress test complete\n");
    task_kill(multitasking_get_current_task());
}

bool user_run_task(const char* name) {
    void (*func)(void) = NULL;
    
    if (strcmp(name, "counter") == 0) {
        func = task_counter;
    } else if (strcmp(name, "hello") == 0) {
        func = task_hello;
    } else if (strcmp(name, "stress") == 0) {
        func = task_stress;
    } else {
        return false;
    }
    
    return user_create_task(name, func) >= 0;
}
