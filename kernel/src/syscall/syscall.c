#include "syscall.h"
#include "../gfx/printf.h"
#include "../tasks/task.h"
#include "../ps2/kbio.h"
#include "../ps2/mouse.h"
#include "../mem/paging.h"
#include "../gfx/serial_io.h"
#include "../so_loader.h"
#include "../interrupts/fd.h"
#include "../interrupts/pipe.h"
#include "../bootloader.h"

#include "../mem/heap.h"
#include "../fs/fat32.h"
#include "../libs/string.h"
#include "../elf.h"
#include "../auxv.h"
#include "../vmm.h"
#include "../mmap.h"


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

            /* Fake VFS: kbio events */
            if (fd == FD_KBIO_EVENTS) {
                if (count < sizeof(kbio_event_t)) {
                    ret = set_errno_and_return(EINVAL);
                    break;
                }
                kbio_event_t evt;
                if (!kbio_get_event(&evt)) {
                    ret = -EAGAIN;
                    break;
                }
                memcpy(buf, &evt, sizeof(kbio_event_t));
                ret = sizeof(kbio_event_t);
                break;
            }

            /* Fake VFS: mouse events */
            if (fd == FD_MOUSE_EVENTS) {
                if (count < sizeof(mouse_state_t)) {
                    ret = set_errno_and_return(EINVAL);
                    break;
                }
                /* Snapshot current state, clear edge flags */
                mouse_state_t snap = mstate;
                /* Clear edge-detection flags so next read sees new edges */
                mstate.left_pressed   = 0;
                mstate.left_released  = 0;
                mstate.right_pressed  = 0;
                mstate.right_released = 0;
                mstate.mid_pressed    = 0;
                mstate.mid_released   = 0;
                memcpy(buf, &snap, sizeof(mouse_state_t));
                ret = sizeof(mouse_state_t);
                break;
            }

            /* Fake VFS: framebuffer — read returns pixel bytes (linear buffer) */
            if (fd == FD_FRAMEBUFFER) {
                if (!framebuffer_request.response ||
                    framebuffer_request.response->framebuffer_count < 1) {
                    ret = set_errno_and_return(ENODEV);
                    break;
                }
                struct limine_framebuffer *fb =
                    framebuffer_request.response->framebuffers[0];
                uint64_t fb_size = fb->height * fb->pitch;
                if (count > fb_size)
                    count = fb_size;
                memcpy(buf, fb->address, count);
                ret = count;
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

            /* Fake VFS: framebuffer — write pixel bytes */
            if (fd == FD_FRAMEBUFFER) {
                if (!framebuffer_request.response ||
                    framebuffer_request.response->framebuffer_count < 1) {
                    ret = set_errno_and_return(ENODEV);
                    break;
                }
                struct limine_framebuffer *fb =
                    framebuffer_request.response->framebuffers[0];
                uint64_t fb_size = fb->height * fb->pitch;
                if (count > fb_size)
                    count = fb_size;
                memcpy(fb->address, (void*)buf, count);
                ret = count;
                break;
            }

            if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
                const char* str = (const char*)buf;
                for (size_t i = 0; i < count; i++) {
                    _putchar(str[i]);
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

            /* Fake VFS — intercept special paths */
            if (strncmp(path, "/dev/fb0", 9) == 0) {
                /* Verify framebuffer exists */
                if (!framebuffer_request.response ||
                    framebuffer_request.response->framebuffer_count < 1) {
                    ret = set_errno_and_return(ENODEV);
                    break;
                }
                ret = FD_FRAMEBUFFER;
                break;
            }
            if (strncmp(path, "/dev/kbio", 10) == 0) {
                ret = FD_KBIO_EVENTS;
                break;
            }
            if (strncmp(path, "/dev/mouse", 11) == 0) {
                ret = FD_MOUSE_EVENTS;
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

            /* Fake VFS FDs — nothing to close */
            if (fd == FD_KBIO_EVENTS || fd == FD_FRAMEBUFFER ||
                fd == FD_MOUSE_EVENTS) {
                ret = 0;
                break;
            }

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
            
            
            uint8_t* elf_stack = (uint8_t*)vmm_alloc_pages(32);  // 2 pages = 8KB
            uint64_t stack_top = (uint64_t)elf_stack + 0x2000;
            
            serial_io_printf("[SYSCALL execve] Stack at 0x%lx (top: 0x%lx)\n",
                           (uint64_t)elf_stack, stack_top);
            
            // Count argc
            uint64_t argc = 0;
            if (argv) {
                while (argv[argc]) argc++;
            }
            
            serial_io_printf("[SYSCALL execve] argc=%lu\n", argc);

            /* Build auxiliary vector */
            struct elf64_hdr *ehdr = prog.elf.hdr;
            auxv_t auxv[] = {
                { AT_PAGESZ,  4096 },
                { AT_CLKTCK,  100 },
                { AT_PHDR,    prog.base + ehdr->phoff },
                { AT_PHENT,   ehdr->phdr_size },
                { AT_PHNUM,   ehdr->ph_num },
                { AT_ENTRY,   prog.entry },
                { AT_BASE,    prog.base },
                { AT_NULL,    0 }
            };

            serial_io_printf("[SYSCALL execve] auxv at %p\n", auxv);

            // Execute ELF - call entry point with argc, argv, envp, auxv
            void (*entry)(uint64_t, char**, char**, auxv_t*) = (void (*)(uint64_t, char**, char**, auxv_t*))prog.entry;


            runningTask->regs.rsp = stack_top;
            runningTask->regs.rip = prog.entry;


            asm volatile(
                "mov %0, %%rdi\n\t"
                "mov %1, %%rsi\n\t"
                "mov %2, %%rdx\n\t"
                "mov %3, %%rcx\n\t"
                "call *%4\n\t"
                :
                : "r"(argc), "r"(argv), "r"(envp), "r"(auxv), "r"(entry)
                : "rdi", "rsi", "rdx", "rcx", "rax", "memory"
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

        /* ------------------------------------------------------------------
         * Memory mapping syscalls
         * ------------------------------------------------------------------*/
        case SYS_MMAP: {
            void *addr   = (void *)arg1;
            size_t len   = (size_t)arg2;
            int prot     = (int)arg3;
            int flags    = (int)arg4;
            int fd       = (int)arg5;
            size_t off   = (size_t)arg6;

            void *map = mmap(addr, len, prot, flags, fd, off);
            if (map == MAP_FAILED) {
                ret = set_errno_and_return(ENOMEM);
            } else {
                ret = (uint64_t)map;
            }
            break;
        }

        case SYS_MUNMAP: {
            void *addr = (void *)arg1;
            size_t len = (size_t)arg2;

            ret = munmap(addr, len);
            if (ret < 0) {
                ret = set_errno_and_return(EINVAL);
            }
            break;
        }

        case SYS_MSYNC: {
            void *addr = (void *)arg1;
            size_t len = (size_t)arg2;

            ret = msync(addr, len);
            if (ret < 0) {
                ret = set_errno_and_return(EINVAL);
            }
            break;
        }

        case SYS_MPROTECT: {
            void *addr = (void *)arg1;
            size_t len = (size_t)arg2;
            int prot   = (int)arg3;

            ret = mprotect(addr, len, prot);
            if (ret < 0) {
                ret = set_errno_and_return(EACCES);
            }
            break;
        }

        /* ------------------------------------------------------------------
         * ioctl — fake VFS device control
         * ------------------------------------------------------------------*/
        case SYS_IOCTL: {
        int fd  = (int)arg1;
        uint64_t req = (uint64_t)arg2;
        uint64_t arg = (uint64_t)arg3;

        serial_io_printf("[IOCTL ENTRY] fd=%d req=0x%lx arg=0x%lx\n", fd, req, arg);
        serial_io_printf("[IOCTL] framebuffer_request.response = %p\n", framebuffer_request.response);
    
    /* Fake VFS: framebuffer ioctl — FBIOGET_INFO */
    if (fd == FD_FRAMEBUFFER && req == FBIOGET_INFO) {
        serial_io_printf("[IOCTL] Matched FD_FRAMEBUFFER && FBIOGET_INFO\n");
        
        if (!framebuffer_request.response ||
            framebuffer_request.response->framebuffer_count < 1) {
            serial_io_printf("[IOCTL] No framebuffer available\n");
            ret = set_errno_and_return(ENODEV);
            break;
        }
        
        serial_io_printf("[IOCTL] Getting framebuffer pointer...\n");
        struct limine_framebuffer *fb =
            framebuffer_request.response->framebuffers[0];
        serial_io_printf("[IOCTL] fb = %p\n", fb);
        serial_io_printf("[IOCTL] fb->address = %p\n", fb->address);
        
        serial_io_printf("[IOCTL] Building info struct...\n");
        struct fb_info info;
        info.addr             = (uint64_t)fb->address;
        serial_io_printf("[IOCTL] Set addr\n");
        info.width            = fb->width;
        info.height           = fb->height;
        info.pitch            = fb->pitch;
        info.bpp              = fb->bpp;
        info.red_mask_size    = fb->red_mask_size;
        info.red_mask_shift   = fb->red_mask_shift;
        info.green_mask_size  = fb->green_mask_size;
        info.green_mask_shift = fb->green_mask_shift;
        info.blue_mask_size   = fb->blue_mask_size;
        info.blue_mask_shift  = fb->blue_mask_shift;

        serial_io_printf("[IOCTL] Info struct ready, arg pointer = 0x%lx\n", arg);
        serial_io_printf("[IOCTL] About to memcpy %zu bytes\n", sizeof(info));
        
        if (arg)
            memcpy((void*)arg, &info, sizeof(info));
            
        serial_io_printf("[IOCTL] memcpy complete\n");
        ret = 0;
        break;
    }

            serial_io_printf("[SYSCALL ioctl] fd=%d req=0x%lx unhandled\n", fd, req);
            ret = set_errno_and_return(ENOTTY);
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
    
    
    serial_io_printf("[SYSCALL] FD and MMAP subsystems ready\n");
}
