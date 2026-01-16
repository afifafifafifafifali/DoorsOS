#include "vm/vm.h"
#include "gfx/printf.h"
#include "libs/string.h"
#include "mem/heap.h"
#include "fs/fat32.h"

typedef struct {
    const char *name;
    uint8_t opcode;
    int has_arg;
} instruction_t;

static instruction_t instructions[] = {
    {"HALT", OP_HALT, 0},
    {"PUSH", OP_PUSH, 1},
    {"POP", OP_POP, 0},
    {"ADD", OP_ADD, 0},
    {"SUB", OP_SUB, 0},
    {"MUL", OP_MUL, 0},
    {"DIV", OP_DIV, 0},
    {"PRINT", OP_PRINT, 0},
    {"JMP", OP_JMP, 1},
    {"JZ", OP_JZ, 1},
    {"LOAD", OP_LOAD, 1},
    {"STORE", OP_STORE, 1},
    {NULL, 0, 0}
};

static int parse_number(const char *str) {
    int val = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        val = val * 10 + (str[i] - '0');
        i++;
    }
    return val;
}

static int assemble_line(const char *line, uint8_t *output, int *out_len) {
    // Skip whitespace
    while (*line == ' ' || *line == '\t') line++;
    
    // Empty line or comment
    if (*line == '\0' || *line == '\n' || *line == '\r' || *line == ';') {
        *out_len = 0;
        return 1;
    }
    
    // Find instruction
    for (int i = 0; instructions[i].name; i++) {
        int len = strlen(instructions[i].name);
        if (strncmp(line, instructions[i].name, len) == 0) {
            output[0] = instructions[i].opcode;
            *out_len = 1;
            
            if (instructions[i].has_arg) {
                const char *arg = line + len;
                while (*arg == ' ' || *arg == '\t') arg++;
                output[1] = parse_number(arg);
                *out_len = 2;
            }
            return 1;
        }
    }
    
    return 0;
}

void assemble_program(const char *source) {
    printf("=== Assembling Program ===\n");
    
    uint8_t bytecode[1024];
    int bc_len = 0;
    
    const char *line = source;
    while (*line && bc_len < 1024) {
        uint8_t out[2];
        int out_len;
        
        if (assemble_line(line, out, &out_len)) {
            for (int i = 0; i < out_len; i++) {
                bytecode[bc_len++] = out[i];
            }
        }
        
        // Next line
        while (*line && *line != '\n') line++;
        if (*line == '\n') line++;
    }
    
    printf("Assembled %d bytes\n", bc_len);
    
    // Run
    vm_t vm;
    vm_init(&vm);
    vm_load(&vm, bytecode, bc_len);
    vm_run(&vm);
}

void assemble_file(const char *filename) {
    printf("Assembling: %s\n", filename);
    
    uint8_t *source = malloc(4096);
    if (!source) {
        printf("Memory allocation failed\n");
        return;
    }
    
    uint32_t size;
    if (!fat32_read_file(filename, source, &size)) {
        printf("Failed to read file\n");
        free(source);
        return;
    }
    
    source[size] = '\0';
    assemble_program((char *)source);
    free(source);
}
