#include <sys/mman.h>
#include <_ezLibC_deps.h>

void* mmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset) {
    return sys_mmap(addr, length, prot, flags, fd, offset);
}

int munmap(void* addr, size_t length) {
    return (int)sys_munmap(addr, length);
}
