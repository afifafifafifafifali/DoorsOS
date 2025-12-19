#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#define HHDM_BASE   0xFFFF800000000000ULL

typedef struct Node {
    void* base;
    size_t length;
    struct Node* next;
} Node;

typedef struct alloc_entry {
    size_t size;
    void* base;
} alloc_entry;

void printMemoryMaps();
void setMemoryMap(uint8_t selection);
void* printHeader(void* start);
void* getMemoryMapBase();
uint64_t getMemoryMapLength();
void* phys_to_virt(uint64_t phys_addr);
uint64_t virt_to_phys(void* virt_addr);
void* k_malloc(size_t size);
void k_free(void* ptr);
void init_pmm(void);

#endif