# DoorsOS VM Assembly Language

## Instruction Set

### Stack Operations
- `PUSH <value>` - Push value onto stack (0x01)
- `POP` - Pop value from stack (0x02)

### Arithmetic Operations
- `ADD` - Pop two values, push sum (0x03)
- `SUB` - Pop two values, push difference (0x04)
- `MUL` - Pop two values, push product (0x05)
- `DIV` - Pop two values, push quotient (0x06)

### I/O Operations
- `PRINT` - Print top of stack (0x07)

### Control Flow
- `JMP <addr>` - Jump to address (0x08)
- `JZ <addr>` - Jump if top of stack is zero (0x09)

### Memory Operations
- `LOAD <addr>` - Load from memory address to stack (0x0A)
- `STORE <addr>` - Store top of stack to memory address (0x0B)

### System
- `HALT` - Stop execution (0x00)

## Example Programs

### Hello Math (5 + 3 * 2)
```
PUSH 5
PUSH 3
PUSH 2
MUL
ADD
PRINT
HALT
```

### Counter Loop (1 to 5)
```
PUSH 1
STORE 0
LOAD 0
PRINT
LOAD 0
PUSH 1
ADD
STORE 0
LOAD 0
PUSH 6
SUB
JZ 28
PUSH 4
JMP 4
HALT
```

## Bytecode Format

Programs are stored as hex bytes, one per line:
```
0x01
0x05
0x01
0x03
0x07
0x00
```

## Usage

1. Create program file: `mkfile program.bc`
2. Enter bytecode (one hex byte per line)
3. Execute: `exec program.bc`
