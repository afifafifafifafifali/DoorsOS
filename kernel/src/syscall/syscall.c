#include "syscall.h"
#include "../gfx/printf.h"
#include "../tasks/task.h"
#include "../ps2/kbio.h"
#include "../mem/paging.h"
#include "../gfx/serial_io.h"

// Syscall handler - called from assembly with args in registers
// Args: arg1 (rdi), arg2 (rsi), arg3 (rdx), syscall_num (rcx)
uint64_t syscall_handler_c(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t num) {
    //serial_io_printf("SYSCALL IRQ IS CALLED--inside the handler_C");
    uint64_t ret = 0;

    switch (num) {
        case SYS_WRITE: {
            const char* str = (const char*)arg2;
            uint64_t len = arg3;
            for (uint64_t i = 0; i < len; i++) {
                serial_io_putchar(str[i]);
            }
            ret = len;
            break;
        }

        case SYS_READ: {
            char* buf = (char*)arg1;
            uint64_t max = arg2;
            char temp[256];
            char* result = ps2_kbio_read(temp, max < 256 ? max : 256);
            if (result) {
                uint64_t len = 0;
                while (result[len] && len < max - 1) {
                    buf[len] = result[len];
                    len++;
                }
                buf[len] = '\0';
                ret = len;
            } else {
                ret = 0;
            }
            break;
        }

        case SYS_EXIT:
            printf("[SYSCALL] Process exited with code %ld\n", arg1);
            ret = 0;
            break;

        case SYS_YIELD:
            yield();
            ret = 0;
            break;

        case SYS_GETPID:
            ret = 1; // TODO: Return actual PID from current task
            break;

        default:
            printf("[SYSCALL] Unknown syscall %ld\n", num);
            ret = (uint64_t)-1;
            break;
    }
    
    return ret;
}

void syscall_init() {
    extern void syscall_stub;
    printf("[SYSCALL] int 0x80 handler registered at %p\n", &syscall_stub);
    serial_io_printf("[SYSCALL] Initialized via int 0x80\n");
}
