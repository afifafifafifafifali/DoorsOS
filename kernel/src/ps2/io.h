#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//Inb
uint8_t inb(uint16_t port) ;
// outb
void outb(uint16_t port, uint8_t value) ;
// Inw
uint16_t inw(uint16_t port) ;

// Outw
void outw(uint16_t port, uint16_t value);
// Inl
uint32_t inl(uint16_t port);
// Outl
void outl(uint16_t port, uint32_t value);
// io wait
void io_wait(void);
void insw(uint16_t port, void *addr, size_t count);
void outsw(uint16_t port, const void *addr, size_t count);

 uint64_t rdmsr(uint32_t msr);
 void wrmsr(uint32_t msr, uint64_t value);