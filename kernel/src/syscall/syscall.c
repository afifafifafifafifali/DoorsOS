#include "syscall.h"
#include "../gfx/printf.h"
#include "../tasks/task.h"
#include "../ps2/kbio.h"
#include "../mem/paging.h"
#include "../gfx/serial_io.h"
#include "../so_loader.h"

// Syscall handler - called from assembly with args in registers
// Args: arg1 (rdi), arg2 (rsi), arg3 (rdx), syscall_num (rcx)
uint64_t syscall_handler_c(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t num) {
    //serial_io_printf("SYSCALL IRQ IS CALLED--inside the handler_C");
    uint64_t ret = 0;

    switch (num) {
        case SYS_PRINT_WRITE: {
            const char* str = (const char*)arg2;
            uint64_t len = arg3;
            for (uint64_t i = 0; i < len; i++) {
                serial_io_putchar(str[i]);
            }
            ret = len;
            break;
        }
        case SYS_LOAD_SO: {
            const char* path = (const char*) arg1;
            so_module_t mod;
            so_error_t  err = so_load_file("/test_lib.so", kernel_exports, &mod);
            return (uint64_t) &mod;
            break;
        }

        

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
