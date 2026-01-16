#include "../interrupts/scheduler.h"
#include "../interrupts/pipe.h"
#include "../gfx/printf.h"
#include "../interrupts/timer.h"
#include "../libs/string.h"

static int test_pipe;

void producer_task(void) {
    for (int i = 0; i < 5; i++) {
        char msg[32];
        snprintf(msg, sizeof(msg), "MSG_%d\n", i);
        pipe_write(test_pipe, msg, strlen(msg));
        printf("[Producer] Sent: %s", msg);
        timer_sleep_ms(100);
    }
}

void consumer_task(void) {
    char buf[64];
    for (int i = 0; i < 5; i++) {
        int n = pipe_read(test_pipe, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("[Consumer] Received: %s", buf);
        }
        timer_sleep_ms(150);
    }
}

void test_multitasking_pipes(void) {
    printf("=== Multitasking & Pipe Test ===\n");
    
    scheduler_init();
    test_pipe = pipe_create();
    
    if (test_pipe < 0) {
        printf("Failed to create pipe\n");
        return;
    }
    
    int t1 = sched_task_create(producer_task, 1);
    int t2 = sched_task_create(consumer_task, 1);
    
    if (t1 < 0 || t2 < 0) {
        printf("Failed to create tasks\n");
        return;
    }
    
    printf("Tasks created: Producer=%d, Consumer=%d\n", t1, t2);
    
    for (int i = 0; i < 100; i++) {
        scheduler_tick();
        timer_sleep_ms(10);
    }
    
    pipe_close(test_pipe);
    printf("=== Test Complete ===\n");
}
