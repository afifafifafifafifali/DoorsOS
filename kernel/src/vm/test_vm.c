#include "vm/vm.h"
#include "gfx/printf.h"

void test_vm(void) {
    printf("=== VM Test: Calculate 5 + 3 * 2 ===\n");
    
    vm_t vm;
    vm_init(&vm);
    
    // Program: push 5, push 3, push 2, mul, add, print, halt
    uint8_t program[] = {
        OP_PUSH, 5,
        OP_PUSH, 3,
        OP_PUSH, 2,
        OP_MUL,
        OP_ADD,
        OP_PRINT,
        OP_HALT
    };
    
    vm_load(&vm, program, sizeof(program));
    vm_run(&vm);
    
    printf("=== VM Test Complete ===\n");
}

void test_vm_loop(void) {
    printf("=== VM Test: Count 1 to 5 ===\n");
    
    vm_t vm;
    vm_init(&vm);
    
    // Program: counter loop
    // mem[0] = counter
    // push 1, store 0 (counter = 1)
    // loop: load 0, print, load 0, push 1, add, store 0, load 0, push 6, sub, jz end, jmp loop
    uint8_t program[] = {
        OP_PUSH, 1,
        OP_STORE, 0,      // counter = 1
        // loop (pc=4):
        OP_LOAD, 0,
        OP_PRINT,         // print counter
        OP_LOAD, 0,
        OP_PUSH, 1,
        OP_ADD,
        OP_STORE, 0,      // counter++
        OP_LOAD, 0,
        OP_PUSH, 6,
        OP_SUB,
        OP_JZ, 28,        // if counter == 6, jump to halt
        OP_PUSH, 4,
        OP_JMP, 4,        // jump to loop
        OP_HALT
    };
    
    vm_load(&vm, program, sizeof(program));
    vm_run(&vm);
    
    printf("=== VM Loop Complete ===\n");
}
