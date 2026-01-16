#include "../interrupts/scheduler.h"
#include "../interrupts/pipe.h"
#include "../gfx/printf.h"
#include "../interrupts/timer.h"
#include "../libs/string.h"

static int stdout_pipe;
static int counter_val = 0;

void counter_task(void) {
    char buf[32];
    for (int i = 0; i < 10; i++) {
        counter_val++;
        snprintf(buf, sizeof(buf), "Count: %d\n", counter_val);
        pipe_write(stdout_pipe, buf, strlen(buf));
        timer_sleep_ms(200);
    }
}

void display_task(void) {
    char buf[128];
    for (int i = 0; i < 10; i++) {
        int n = pipe_read(stdout_pipe, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);
        }
        timer_sleep_ms(100);
    }
}

void test_counter(void) {
    printf("=== Counter Test ===\n");
    
    stdout_pipe = pipe_create();
    if (stdout_pipe < 0) {
        printf("Failed to create pipe\n");
        return;
    }
    
    int t1 = sched_task_create(counter_task, 1);
    int t2 = sched_task_create(display_task, 1);
    
    if (t1 < 0 || t2 < 0) {
        printf("Failed to create tasks\n");
        return;
    }
    
    printf("Counter and display tasks running...\n");
    
    for (int i = 0; i < 300; i++) {
        scheduler_tick();
        timer_sleep_ms(5);
    }
    
    pipe_close(stdout_pipe);
    printf("=== Counter Complete ===\n");
}
