#pragma once
#include <stddef.h>
#include <stdint.h>
#include "new/pmm.h"

// nigga nigga nigga

// Global program break for brk syscall
extern uint8_t* program_break;

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

// FUCK LINKED LIST
void allocator_init(void);


void* allocator_malloc(size_t size);


void allocator_free(void* ptr);


void* allocator_calloc(size_t num, size_t size);


void* allocator_realloc(void* ptr, size_t new_size);


void* allocator_malloc(size_t size);
void allocator_free(void* ptr);
void* _sbrk(ptrdiff_t increment);
int brk(void* addr);

#define malloc allocator_malloc
#define free allocator_free

#define calloc allocator_calloc
#define realloc allocator_realloc
