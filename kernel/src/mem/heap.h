#pragma once
#include <stddef.h>
#include <stdint.h>
#include "new/pmm.h"

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;


// Initialize allocator on given heap region
void allocator_init(void);

// Allocate `size` bytes
void* allocator_malloc(size_t size);

// Free memory pointed by ptr
void allocator_free(void* ptr);

// Allocate zero-initialized memory for num * size bytes
void* allocator_calloc(size_t num, size_t size);

// Change size of allocated block
void* allocator_realloc(void* ptr, size_t new_size);


#define malloc k_malloc
#define free k_free

#define calloc allocator_calloc
#define realloc allocator_realloc
