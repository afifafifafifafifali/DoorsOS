#include "syscall.h"
#include "../gfx/printf.h"
#include "../tasks/task.h"
#include "../ps2/kbio.h"
#include "../mem/paging.h"
#include "../gfx/serial_io.h"
#include "../so_loader.h"
#include "../interrupts/fd.h"
#include "../interrupts/pipe.h"
#include "../mmap.h"
#include "../mem/heap.h"
#include "../fs/fat32.h"
#include "../libs/string.h"
#include "../elf.h"
#include "../vmm.h"


static  int task_errno = 0;

static inline int64_t set_errno_and_return(int err) {
    task_errno = err;
    return -err;
}

uint64_t syscall_handler_c(uint64_t arg1, uint64_t arg2, uint64_t arg3,
                           uint64_t arg4, uint64_t arg5, uint64_t arg6,
                           uint64_t num) {
    int64_t ret = 0;

    switch (num) {
        
        
        case SYS_READ: {
            
            int fd = (int)arg1;
            void* buf = (void*)arg2;
            size_t count = (size_t)arg3;
            
            if (!buf || count == 0) {
                ret = 0;
                break;
            }
            
            if (fd == STDIN_FILENO) {
              
                char* char_buf = (char*)buf;
                size_t bytes_read = 0;
                
                while (bytes_read < count) {
                    char c = ps2_kbio_getchar_nb();
                    if (c == 0) break; 
                    char_buf[bytes_read++] = c;
                }
                
                if (bytes_read == 0) {
                    ret = -EAGAIN;  
                } else {
                    ret = bytes_read;
                }
                break;
            }
            
           
            ret = fd_read(fd, buf, count);
            if (ret < 0) {
                ret = set_errno_and_return(EBADF);
            }
            break;
        }
        
        case SYS_WRITE: {
            
            int fd = (int)arg1;
            const void* buf = (const void*)arg2;
            size_t count = (size_t)arg3;
            
            if (!buf || count == 0) {
                ret = 0;
                break;
            }
            
            if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
                const char* str = (const char*)buf;
                for (size_t i = 0; i < count; i++) {
                    serial_io_putchar(str[i]);
                }
                ret = count;
                break;
            }
            
            ret = fd_write(fd, buf, count);
            if (ret < 0) {
                ret = set_errno_and_return(EBADF);
            }
            break;
        }
        
        case SYS_OPEN: {
            
            const char* path = (const char*)arg1;
            int flags = (int)arg2;
            int mode = (int)arg3;  
            
            if (!path) {
                ret = set_errno_and_return(EFAULT);
                break;
            }
            
            ret = fd_open(path, flags);
            if (ret < 0) {
                
                ret = set_errno_and_return(ENOENT);
            }
            break;
        }
        
        case SYS_CLOSE: {
          
            int fd = (int)arg1;
            
            ret = fd_close(fd);
            if (ret < 0) {
                ret = set_errno_and_return(EBADF);
            } else {
                ret = 0;
            }
            break;
        }
        
    
        
        case SYS_EXIT: {
            // sys_exit(status) - set task exit code and kill
            int status = (int)arg1;
            serial_io_printf("[SYSCALL exit] Task %d exiting with status %d\n", 
                           runningTask->id, status);
            taskDie(status);
            
            ret = 0;
            break;
        }
        
        case SYS_GETPID: {
            
            ret = runningTask->id;
            break;
        }
        
        case SYS_FORK: {
           
            Task* child = task_fork();
            
            if (!child) {
                ret = set_errno_and_return(EAGAIN);
            } else {
                // Parent returns child PID
                ret = child->id;
            }
            break;
        }
        
        case SYS_EXECVE: {
           
            const char* path = (const char*)arg1;
            char** argv = (char**)arg2;
            char** envp = (char**)arg3;
            
            if (!path) {
                ret = set_errno_and_return(EFAULT);
                break;
            }
            
            serial_io_printf("[SYSCALL execve] Loading ELF: %s\n", path);
            
            // Load ELF binary
            elf64_program_t prog;
            elf_error_t err = elf64_load_file(path, &prog);
            
            if (err != ELF_OK) {
                serial_io_printf("[SYSCALL execve] Failed to load ELF: %s\n", 
                               elf64_strerror(err));
                ret = set_errno_and_return(ENOEXEC);
                break;
            }
            
            serial_io_printf("[SYSCALL execve] ELF loaded at 0x%lx, entry 0x%lx\n", 
                           prog.base, prog.entry);
            
            
            uint8_t* elf_stack = (uint8_t*)vmm_alloc_pages(2);  // 2 pages = 8KB
            uint64_t stack_top = (uint64_t)elf_stack + 0x2000;
            
            serial_io_printf("[SYSCALL execve] Stack at 0x%lx (top: 0x%lx)\n",
                           (uint64_t)elf_stack, stack_top);
            
            // Count argc
            uint64_t argc = 0;
            if (argv) {
                while (argv[argc]) argc++;
            }
            
            serial_io_printf("[SYSCALL execve] argc=%lu\n", argc);
            
            // Execute ELF - call entry point with argc, argv, envp
            void (*entry)(uint64_t, char**, char**) = (void (*)(uint64_t, char**, char**))prog.entry;
            
            
            runningTask->regs.rsp = stack_top;
            runningTask->regs.rip = prog.entry;
            
            
            asm volatile(
                "mov %0, %%rdi\n\t"
                "mov %1, %%rsi\n\t"  
                "mov %2, %%rdx\n\t"
                "call *%3\n\t"
                :
                : "r"(argc), "r"(argv), "r"(envp), "r"(entry)
                : "rdi", "rsi", "rdx", "rax", "rcx", "memory"
            );
            
            
            elf64_unload(&prog);
            
            ret = 0;
            break;
        }
        
        case SYS_WAITPID: {
            // sys_waitpid(pid, *status, options)
            int pid = (int)arg1;
            int* status = (int*)arg2;
            int options = (int)arg3;
            
            // SHIT MANAGE
            if (status) {
                *status = 0;
            }
            ret = pid;  // Return child PID
            break;
        }
        
       
        
        case SYS_MMAP: {
            
            void* addr = (void*)arg1;
            size_t length = (size_t)arg2;
            int prot = (int)arg3;
            int flags = (int)arg4;
            int fd = (int)arg5;
            off_t offset = (off_t)arg6;
            
            if (flags & MAP_ANONYMOUS) {
                
                void* result = kmmap(length, prot, flags);
                if (!result) {
                    ret = set_errno_and_return(ENOMEM);
                } else {
                    ret = (uint64_t)result;
                }
            } else if (fd >= 0) {
                
                void* result = fd_mmap(fd, addr, length, prot, flags, offset);
                if (result == MAP_FAILED) {
                    ret = set_errno_and_return(ENOMEM);
                } else {
                    ret = (uint64_t)result;
                }
            } else {
                ret = set_errno_and_return(EINVAL);
            }
            break;
        }
        
        case SYS_MUNMAP: {
            
            void* addr = (void*)arg1;
            size_t length = (size_t)arg2;
            
            kmunmap(addr, length);
            ret = 0;
            break;
        }
        
        case SYS_BRK: {
            
            void* new_brk = (void*)arg1;
            
            if (new_brk == NULL) {
                // Return current break
                ret = (uint64_t)program_break;
            } else {
                // Set new break
                int result = brk(new_brk);
                if (result < 0) {
                    ret = set_errno_and_return(ENOMEM);
                } else {
                    ret = (uint64_t)program_break;
                }
            }
            break;
        }
        
        
        
        case SYS_LSEEK: {
            
            int fd = (int)arg1;
            off_t offset = (off_t)arg2;
            int whence = (int)arg3;
            
            ret = fd_seek(fd, offset, whence);
            if (ret < 0) {
                ret = set_errno_and_return(EINVAL);
            }
            break;
        }
        
        case SYS_DUP: {
            
            int oldfd = (int)arg1;
            
            ret = fd_dup(oldfd);
            if (ret < 0) {
                ret = set_errno_and_return(EBADF);
            }
            break;
        }
        
        case SYS_DUP2: {
            
            int oldfd = (int)arg1;
            int newfd = (int)arg2;
            
            // @todo
            ret = fd_dup2(oldfd,newfd);
            break;
        }
        
        case SYS_PIPE: {
            
            int* pipefd = (int*)arg1;
            
            ret = fd_pipe(pipefd);
            if (ret < 0) {
                ret = set_errno_and_return(EMFILE);
            } else {
                ret = 0;
            }
            break;
        }
        
        
        
        case SYS_GETTIMEOFDAY: {
            
            ret = 0;
            break;
        }
        
        case SYS_CLOCK_GETTIME: {
            //@todo : lazy
            ret = 0;
            break;
        }
        
        
        
        case SYS_UNAME: {
            // sysinfo
            struct utsname* buf = (struct utsname*)arg1;
            
            if (!buf) {
                ret = set_errno_and_return(EFAULT);
                break;
            }
            
            // Fill in DoorsOS info
            strcpy(buf->sysname, "DoorsOS");
            strcpy(buf->nodename, "localhost");
            strcpy(buf->release, "0.1.0");
            strcpy(buf->version, "Shit edition");
            strcpy(buf->machine, "x86_64");
            serial_io_printf("Hi niggas,it me mario the uname\n");
            ret = 0;
            break;
        }
        
        case SYS_ARCH_PRCTL: {
            // @todo
            ret = 0;
            break;
        }
        
        
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
           
            const char* path = (const char*)arg1;
            so_module_t* mod = (so_module_t*)malloc(sizeof(so_module_t));
            if (!mod) {
                ret = set_errno_and_return(ENOMEM);
                break;
            }
            so_error_t err = so_load_file(path, get_kernel_exports(), mod);
            if (err != SO_OK) {
                free(mod);
                ret = set_errno_and_return(ENOEXEC);
            } else {
                ret = (uint64_t)mod;
            }
            break;
        }
        
        case SYS_FUCK_YOU: {
            
            serial_io_printf("[sysfuckyou] arg1: %ld ", arg1);
            serial_io_printf("[sysfuckyou] arg2: %ld ", arg2);
            serial_io_printf("[sysfuckyou] arg3: %ld ", arg3);
            serial_io_printf("[sysfuckyou] arg4: %ld ", arg4);
            serial_io_printf("[sysfuckyou] arg5: %ld ", arg5);
            serial_io_printf("[sysfuckyou] arg6: %ld \n", arg6);
            ret = 1;
            break;
        }
        
      
        
        default: {
            serial_io_printf("[SYSCALL] Unknown syscall %ld. \n", num);
            ret = set_errno_and_return(ENOSYS);
            break;
        }
    }

    return (uint64_t)ret;
}

void syscall_init(void) {
    extern void syscall_stub;
    printf("[SYSCALL] int 0x80 handler registered at %p\n", &syscall_stub);
    serial_io_printf("[SYSCALL] Unix syscall table initialized\n");
    
    // Initialize fd system
    fd_init();
    
    // Initialize mmap system
    mmap_init();
    
    serial_io_printf("[SYSCALL] FD and MMAP subsystems ready\n");
}
