#include "vm.h"
#include "../gfx/printf.h"
#include <stddef.h>

void vm_init(vm_t *vm) {
    vm->sp = 0;
    vm->pc = 0;
    for (int i = 0; i < VM_MEM_SIZE; i++) vm->code[i] = 0;
    for (int i = 0; i < VM_STACK_SIZE; i++) vm->stack[i] = 0;
    for (int i = 0; i < 256; i++) vm->mem[i] = 0;
}

void vm_load(vm_t *vm, uint8_t *code, int size) {
    for (int i = 0; i < size && i < VM_MEM_SIZE; i++) {
        vm->code[i] = code[i];
    }
}

void vm_run(vm_t *vm) {
    while (vm->pc < VM_MEM_SIZE) {
        uint8_t op = vm->code[vm->pc++];
        
        switch (op) {
            case OP_HALT:
                return;
            
            case OP_PUSH: {
                int val = vm->code[vm->pc++];
                vm->stack[vm->sp++] = val;
                break;
            }
            
            case OP_POP:
                if (vm->sp > 0) vm->sp--;
                break;
            
            case OP_ADD: {
                int b = vm->stack[--vm->sp];
                int a = vm->stack[--vm->sp];
                vm->stack[vm->sp++] = a + b;
                break;
            }
            
            case OP_SUB: {
                int b = vm->stack[--vm->sp];
                int a = vm->stack[--vm->sp];
                vm->stack[vm->sp++] = a - b;
                break;
            }
            
            case OP_MUL: {
                int b = vm->stack[--vm->sp];
                int a = vm->stack[--vm->sp];
                vm->stack[vm->sp++] = a * b;
                break;
            }
            
            case OP_DIV: {
                int b = vm->stack[--vm->sp];
                int a = vm->stack[--vm->sp];
                if (b != 0) vm->stack[vm->sp++] = a / b;
                break;
            }
            
            case OP_PRINT:
                if (vm->sp > 0) {
                    printf("%d\n", vm->stack[vm->sp - 1]);
                }
                break;
            
            case OP_JMP:
                vm->pc = vm->code[vm->pc];
                break;
            
            case OP_JZ: {
                int addr = vm->code[vm->pc++];
                if (vm->sp > 0 && vm->stack[--vm->sp] == 0) {
                    vm->pc = addr;
                }
                break;
            }
            
            case OP_LOAD: {
                int addr = vm->code[vm->pc++];
                vm->stack[vm->sp++] = vm->mem[addr];
                break;
            }
            
            case OP_STORE: {
                int addr = vm->code[vm->pc++];
                vm->mem[addr] = vm->stack[--vm->sp];
                break;
            }
            
            default:
                printf("Unknown opcode: %d\n", op);
                return;
        }
    }
}
