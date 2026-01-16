#ifndef VM_H
#define VM_H

#include <stdint.h>

#define VM_STACK_SIZE 256
#define VM_MEM_SIZE 1024

typedef enum {
    OP_HALT = 0,
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PRINT,
    OP_JMP,
    OP_JZ,
    OP_LOAD,
    OP_STORE
} opcode_t;

typedef struct {
    uint8_t code[VM_MEM_SIZE];
    int stack[VM_STACK_SIZE];
    int sp;
    int pc;
    int mem[256];
} vm_t;

void vm_init(vm_t *vm);
void vm_load(vm_t *vm, uint8_t *code, int size);
void vm_run(vm_t *vm);

#endif
