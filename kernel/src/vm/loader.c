#include "vm/vm.h"
#include "fs/fat32.h"
#include "gfx/printf.h"
#include "libs/string.h"
#include "mem/heap.h"

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

static uint8_t parse_hex_byte(const char *str) {
    // Parse "0xNN" format
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        return (hex_to_int(str[2]) << 4) | hex_to_int(str[3]);
    }
    return 0;
}

void run_program(const char *filename) {
    printf("Loading program: %s\n", filename);
    
    uint8_t *file_data = malloc(4096);
    if (!file_data) {
        printf("Memory allocation failed\n");
        return;
    }
    
    uint32_t size;
    if (!fat32_read_file(filename, file_data, &size)) {
        printf("Failed to read file\n");
        free(file_data);
        return;
    }
    
    // Parse hex bytecode
    uint8_t *bytecode = malloc(1024);
    if (!bytecode) {
        printf("Memory allocation failed\n");
        free(file_data);
        return;
    }
    
    int bc_idx = 0;
    char *line = (char *)file_data;
    char *end = (char *)(file_data + size);
    
    while (line < end && bc_idx < 1024) {
        // Skip whitespace
        while (line < end && (*line == ' ' || *line == '\t' || *line == '\r')) line++;
        
        if (line >= end) break;
        
        // Parse hex byte
        if (line[0] == '0' && (line[1] == 'x' || line[1] == 'X')) {
            bytecode[bc_idx++] = parse_hex_byte(line);
            line += 4;
        }
        
        // Skip to next line
        while (line < end && *line != '\n') line++;
        if (line < end) line++;
    }
    
    printf("Loaded %d bytes of bytecode\n", bc_idx);
    
    // Run VM
    vm_t vm;
    vm_init(&vm);
    vm_load(&vm, bytecode, bc_idx);
    vm_run(&vm);
    
    free(bytecode);
    free(file_data);
}
