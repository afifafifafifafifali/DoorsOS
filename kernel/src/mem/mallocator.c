#include "heap.h"
#include "../bootloader.h"
#include <stdint.h>
#include "../libs/string.h"

extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

#define HEAP_SIZE (16 * 1024 * 1024) // 16 MiB heap size

static uint8_t* heap_start = NULL;
static uint8_t* heap_end = NULL;
static FreeBlock* free_list = NULL;

// Converts physical address to kernel virtual address using Limine HHDM base
static inline void* phys_to_virt(uint64_t phys_addr) {
    return (void*)(hhdm_request.response->offset + phys_addr);
}

void allocator_init(void) {
    if (memmap_request.response == NULL || hhdm_request.response == NULL) {
        // Critical error - memmap or hhdm not available
        while (1);
    }

    // Find a usable memory region big enough for heap
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= HEAP_SIZE) {
            // Map physical base to virtual via HHDM
            heap_start = phys_to_virt(entry->base);
            heap_end = heap_start + HEAP_SIZE;

            free_list = (FreeBlock*)heap_start;
            free_list->size = HEAP_SIZE - sizeof(FreeBlock);
            free_list->next = NULL;
            return;
        }
    }

    // If no suitable region found, halt
    while (1);
}

// --- allocator implementation ---

static inline size_t align8(size_t size) {
    return (size + 7) & ~7;
}

void* allocator_malloc(size_t size) {
    size = align8(size);
    FreeBlock** current = &free_list;

    while (*current) {
        if ((*current)->size >= size) {
            FreeBlock* allocated = *current;

            if (allocated->size >= size + sizeof(FreeBlock) + 8) {
                FreeBlock* new_block = (FreeBlock*)((uint8_t*)allocated + sizeof(FreeBlock) + size);
                new_block->size = allocated->size - size - sizeof(FreeBlock);
                new_block->next = allocated->next;

                allocated->size = size;
                *current = new_block;
            } else {
                *current = allocated->next;
            }
            return (void*)((uint8_t*)allocated + sizeof(FreeBlock));
        }
        current = &(*current)->next;
    }

    return NULL;
}

void allocator_free(void* ptr) {
    if (!ptr) return;

    FreeBlock* block = (FreeBlock*)((uint8_t*)ptr - sizeof(FreeBlock));
    FreeBlock** current = &free_list;

    while (*current && *current < block) {
        current = &(*current)->next;
    }

    block->next = *current;
    *current = block;

    if (block->next && (uint8_t*)block + sizeof(FreeBlock) + block->size == (uint8_t*)block->next) {
        block->size += sizeof(FreeBlock) + block->next->size;
        block->next = block->next->next;
    }

    if (current != &free_list) {
        FreeBlock* prev = free_list;
        while (prev->next != block) {
            prev = prev->next;
        }
        if ((uint8_t*)prev + sizeof(FreeBlock) + prev->size == (uint8_t*)block) {
            prev->size += sizeof(FreeBlock) + block->size;
            prev->next = block->next;
        }
    }
}

void* allocator_calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = allocator_malloc(total);
    if (ptr) {
        for (size_t i = 0; i < total; i++) {
            ((uint8_t*)ptr)[i] = 0;
        }
    }
    return ptr;
}

void* allocator_realloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return allocator_malloc(new_size);
    }
    if (new_size == 0) {
        allocator_free(ptr);
        return NULL;
    }

    FreeBlock* block = (FreeBlock*)((uint8_t*)ptr - sizeof(FreeBlock));
    size_t old_size = block->size;

    if (new_size <= old_size) {
        return ptr;
    }

    void* new_ptr = allocator_malloc(new_size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, old_size);
    allocator_free(ptr);
    return new_ptr;
}