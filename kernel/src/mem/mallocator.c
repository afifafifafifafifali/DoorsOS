#include "heap.h"
#include "../bootloader.h"
#include <stdint.h>
#include "../libs/string.h"
#include "../gfx/serial_io.h"
#include <stddef.h>
#include "../physalloc.h"

extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;



#define HEAP_SIZE (856 * 1024 * 1024) // 856 MiB heap size

static uint8_t* heap_start = NULL;
static uint8_t* heap_end = NULL;
static FreeBlock* free_list = NULL;

// Global program break for brk syscall
uint8_t* program_break = NULL;


void* phys_to_virt(uint64_t phys_addr) {
    return (void*)(hhdm_request.response->offset + phys_addr);
}


void allocator_init(void) {
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= HEAP_SIZE) {
            // map physical base to virtual via HHDM
            heap_start = phys_to_virt(entry->base);
            heap_end = heap_start + HEAP_SIZE;

            // Reserve this physical region in PhysAlloc so PMM doesn't touch it
            phys_alloc_reserve(entry->base, HEAP_SIZE);

            // Initialize program break to heap start
            program_break = heap_start;

            free_list = (FreeBlock*)heap_start;
            free_list->size = HEAP_SIZE - sizeof(FreeBlock);
            free_list->next = NULL;
            serial_io_printf("Heap allocated at %p size %lx\n", heap_start, HEAP_SIZE);
            return;
        }
    }

    while (1);
}

// --- allocator impl ---

static inline size_t align8(size_t size) {
    return (size + 7) & ~7;
}

void* allocator_malloc(size_t size) {
    if (size == 0) return NULL;
    size = align8(size);
    FreeBlock** current = &free_list;

    while (*current) {
        
        if ((*current)->size >= size) {
           // serial_io_printf("Found");
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
    if ((uint8_t*)ptr < heap_start || (uint8_t*)ptr >= heap_end) return; 

    FreeBlock* block = (FreeBlock*)((uint8_t*)ptr - sizeof(FreeBlock));
    FreeBlock** current = &free_list;

    while (*current && *current < block) {
        current = &(*current)->next;
    }

    block->next = *current;
    *current = block;

    // Coalesce with next
    if (block->next && (uint8_t*)block + sizeof(FreeBlock) + block->size == (uint8_t*)block->next) {
        block->size += sizeof(FreeBlock) + block->next->size;
        block->next = block->next->next;
    }

    // Coalesce with prev disabled for now
    // if (current != &free_list) {
    //     FreeBlock* prev = free_list;
    //     while (prev && prev->next != block) {
    //         prev = prev->next;
    //     }
    //     if (prev && (uint8_t*)prev + sizeof(FreeBlock) + prev->size == (uint8_t*)block) {
    //         prev->size += sizeof(FreeBlock) + block->size;
    //         prev->next = block->next;
    //     }
    // }
}

void* allocator_calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = allocator_malloc(total);
    if (ptr) {
        memset(ptr, 0, total);
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

void* _sbrk(ptrdiff_t increment) {
    if (!program_break) program_break = heap_start;

    void* prev = program_break;
    void* next = program_break + increment;

    if (next >= heap_end) return (void*)-1;  // out of memory
    program_break = next;
    return prev;
}

int brk(void* addr) {
    if (!program_break) {
        program_break = heap_start;  // initialize if not done
    }

    ptrdiff_t increment = (uint8_t*)addr - program_break;
    void* result = _sbrk(increment);
    if (result == (void*)-1) {
        return -1;  // failure, can't extend heap
    }

    program_break = (uint8_t*)addr;
    return 0;  // success
}