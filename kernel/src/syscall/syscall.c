#include "syscall.h"
#include "../gfx/printf.h"
#include "../tasks/task.h"
#include "../ps2/kbio.h"
#include "../mem/paging.h"

extern void* k_malloc(uint64_t size);

// Syscall entry point
__attribute__((section(".syscall_entry")))
uint64_t syscall_entry(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (num) {
        case SYS_WRITE: {
            char *str = (char *)arg1;
            uint64_t len = arg2;
            for (uint64_t i = 0; i < len; i++) {
                printf("%c", str[i]);
            }
            return len;
        }
        
        case SYS_READ: {
            char *buf = (char *)arg1;
            uint64_t max = arg2;
            char temp[256];
            char *result = ps2_kbio_read(temp, max < 256 ? max : 256);
            if (result) {
                uint64_t len = 0;
                while (result[len] && len < max - 1) {
                    buf[len] = result[len];
                    len++;
                }
                buf[len] = '\0';
                return len;
            }
            return 0;
        }
        
        case SYS_EXIT:
            printf("Program exited with code %ld\n", arg1);
            
            return 0;
        
        case SYS_YIELD:
            yield();
            return 0;
        
        default:
            printf("Unknown syscall: %ld\n", num);
            return -1;
    }
}

void syscall_init() {
    uint64_t *ptr = (uint64_t *)SYSCALL_PTR_ADDR;
    serial_io_printf("SYSCALL DBG: KMALLICING");
    void* virt = k_malloc(0x1000);
uint64_t phys = virt_to_phys(virt);

    serial_io_printf("SYSCALL DBG:MAPPAGING");
    mapPage(SYSCALL_PTR_ADDR, phys, 0x3);
    serial_io_printf("SYSCAL::PTRING");
    *ptr = (uint64_t)syscall_entry;
    
    printf("Syscalls initialized at 0x%lx (ptr at 0xCAFE000)\n", (uint64_t)syscall_entry);
    serial_io_printf("SYSCALL PTR VALUE: 0x%lx\n", *(uint64_t*)0xCAFE000);



}
