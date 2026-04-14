

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define ALIGNMENT   16
#define ALIGN_UP(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct block_header {
    size_t size;          
    int    is_free;
} block_header_t;

typedef struct free_block {
    struct free_block* next;
} free_block_t;

static block_header_t* heap_tail = NULL;  
static free_block_t*   free_list = NULL; 

static void* header_to_data(block_header_t* hdr) {
    return (void*)((uint8_t*)hdr + sizeof(block_header_t));
}

static block_header_t* data_to_header(void* ptr) {
    return (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
}

static block_header_t* extend_heap(size_t extra) {
    size_t total = sizeof(block_header_t) + extra;

    void* p = sbrk((intptr_t)total);
    if (p == (void*)-1) return NULL;

    block_header_t* hdr = (block_header_t*)p;
    hdr->size     = extra;
    hdr->is_free  = 0;

    heap_tail = hdr;
    return hdr;
}

static void split_block(block_header_t* hdr, size_t needed) {
    size_t leftover = hdr->size - needed;
    size_t min_split = sizeof(free_block_t) + ALIGNMENT; // minimum useful free block 

    if (leftover >= min_split) {
        hdr->size = needed;

        block_header_t* new_hdr = (block_header_t*)((uint8_t*)hdr + sizeof(block_header_t) + needed);
        new_hdr->size    = leftover - sizeof(block_header_t);
        new_hdr->is_free = 1;

        free_block_t* fb = (free_block_t*)header_to_data(new_hdr);
        fb->next   = free_list;
        free_list  = fb;
    }
}


void* malloc(size_t size) {
    if (size == 0) return NULL;

    size_t aligned = ALIGN_UP(size);

    // walking
    free_block_t* prev = NULL;
    free_block_t* cur  = free_list;

    while (cur) {
        block_header_t* hdr = data_to_header((void*)cur);

        if (hdr->size >= aligned) {
           
            if (prev) prev->next = cur->next;
            else      free_list  = cur->next;

            hdr->is_free = 0;
            split_block(hdr, aligned);

            memset(header_to_data(hdr), 0, hdr->size);
            return header_to_data(hdr);
        }

        prev = cur;
        cur  = cur->next;
    }

    block_header_t* hdr = extend_heap(aligned);
    if (!hdr) return NULL;

    return header_to_data(hdr);
}

void free(void* ptr) {
    if (!ptr) return;

    block_header_t* hdr = data_to_header(ptr);
    if (hdr->is_free) return;   

    hdr->is_free = 1;

    block_header_t* next_hdr = (block_header_t*)((uint8_t*)hdr + sizeof(block_header_t) + hdr->size);

    if (hdr != heap_tail) {
        if (!next_hdr->is_free) {
            
            free_block_t* fb = (free_block_t*)header_to_data(hdr);
            fb->next   = free_list;
            free_list  = fb;
        } else {
            // coalece the shit
            free_block_t* prev = NULL;
            free_block_t* cur  = free_list;
            while (cur) {
                if (cur == header_to_data(next_hdr)) {
                    if (prev) prev->next = cur->next;
                    else      free_list  = cur->next;
                    break;
                }
                prev = cur;
                cur  = cur->next;
            }

            hdr->size += sizeof(block_header_t) + next_hdr->size;
            
            free_block_t* fb = (free_block_t*)header_to_data(hdr);
            fb->next   = free_list;
            free_list  = fb;
        }
    } else {
        free_block_t* fb = (free_block_t*)header_to_data(hdr);
        fb->next   = free_list;
        free_list  = fb;
    }
}

void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* p = malloc(total);
    if (p) memset(p, 0, total);
    return p;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) { free(ptr); return NULL; }

    block_header_t* hdr = data_to_header(ptr);
    size_t old_size = hdr->size;

    if (size <= old_size) return ptr;  // fits in existing block 

    void* new_ptr = malloc(size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
}
