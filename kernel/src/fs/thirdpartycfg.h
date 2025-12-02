/*#pragma once

// My third-party code was added to the DoorsOS kernel.
#include <stdint.h>
#include "../mem/heap.h"

#define kmalloc malloc
#define kfree free
#define kcalloc calloc
#define krealloc realloc

// The kmalloc_ap function
static inline void *kmalloc_ap(uint32_t size, uint8_t page_align, uint32_t *phys){
    void *ptr = kmalloc(size);
    if (ptr && page_align) {
        // Align the pointer to the next page boundary
        uint32_t addr = (uint32_t)ptr;
        addr = (addr + 0xFFF) & ~0xFFF; // Align to 4KB
        ptr = (void *)addr;
    }
    if (phys) {
        *phys = (uint32_t)ptr; // Return physical address
    }
    return ptr;
}

static inline int k_toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');  // or simply c - 32
    }
    return c;
}

static inline int k_tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');  // or simply c + 32
    }
    return c;
}

// Note: itos cannot be inline static if you want to return a pointer into a caller buffer.
// But you can mark it inline for potential optimization.
static inline char* itos(uint32_t myint, char buffer[], int bufflen) {
    int i = bufflen - 2;
    buffer[bufflen - 1] = '\0';

    if (myint == 0) {
        buffer[i--] = '0';
    }

    while (myint > 0 && i >= 0) {
        buffer[i--] = (myint % 10) + '0';
        myint /= 10;
    }

    return &buffer[i + 1];
}
*/