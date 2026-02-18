#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "io.h"
//Inb
uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
// outb
void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
// Inw
uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
// Outw
void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}
// Inl
uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
// Outl
void outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}
// io wait
void io_wait(void) {
    __asm__ volatile ("outb %0, $0x80" : : "a"((uint8_t)0));
}

// insw
void insw(uint16_t port, void *addr, size_t count) {
    __asm__ volatile ("rep insw" : "=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count) : "memory");
}
// outsw
void outsw(uint16_t port, const void *addr, size_t count) {
    __asm__ volatile ("rep outsw" : "=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count) : "memory");
}

 uint64_t rdmsr(uint32_t msr) {
    uint32_t lo, hi;
    __asm__ volatile (
        "rdmsr"
        : "=a"(lo), "=d"(hi)
        : "c"(msr)
    );
    return ((uint64_t)hi << 32) | lo;
}

 void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t lo = (uint32_t)value;
    uint32_t hi = (uint32_t)(value >> 32);

    __asm__ volatile (
        "wrmsr"
        :
        : "c"(msr), "a"(lo), "d"(hi)
    );
}