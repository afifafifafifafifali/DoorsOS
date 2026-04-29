#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "gfx/serial_io.h"
#include "fs/fat32.h"
#include "PhysAlloc.h"
#include "mem/heap.h"

#define ELF64_R_SYM(i)    ((i) >> 32)
#define ELF64_R_TYPE(i)   ((i) & 0xffffffffL)
#define FIXED_HIGHER_HALF_OFFSET_64 ((uint64_t)0xffffffff80000000)

/* ELF Magic */
#define ELF_MAGIC_0 0x7F
#define ELF_MAGIC_1 'E'
#define ELF_MAGIC_2 'L'
#define ELF_MAGIC_3 'F'

/* ELF Class */
#define ELFCLASS32 1
#define ELFCLASS64 2

/* ELF Data encoding */
#define ELFDATA2LSB 1  /* Little endian */
#define ELFDATA2MSB 2  /* Big endian */

/* ELF Type */
#define ET_NONE 0      /* No file type */
#define ET_REL  1      /* Relocatable file */
#define ET_EXEC 2      /* Executable file */
#define ET_DYN  3      /* Shared object file */
#define ET_CORE 4      /* Core file */

/* ELF Machine */
#define EM_386    3    /* Intel 80386 */
#define EM_X86_64 62   /* AMD x86-64 */

/* ELF Version */
#define EV_CURRENT 1

/* Program header types */
#define PT_NULL    0  /* Unused entry */
#define PT_LOAD    1  /* Loadable segment */
#define PT_DYNAMIC 2  /* Dynamic linking info */
#define PT_INTERP  3  /* Interpreter path */
#define PT_NOTE    4  /* Note section */
#define PT_SHLIB   5  /* Shared library */
#define PT_PHDR    6  /* Program header table */
#define PT_TLS     7  /* Thread-local storage */

/* Program header flags */
#define PF_X 1  /* Execute */
#define PF_W 2  /* Write */
#define PF_R 4  /* Read */

/* Section header types */
#define SHT_NULL     0  /* Inactive */
#define SHT_PROGBITS 1  /* Program data */
#define SHT_SYMTAB   2  /* Symbol table */
#define SHT_STRTAB   3  /* String table */
#define SHT_RELA     4  /* Relocation entries with addends */
#define SHT_HASH     5  /* Symbol hash table */
#define SHT_DYNAMIC  6  /* Dynamic linking info */
#define SHT_NOTE     7  /* Notes */
#define SHT_NOBITS   8  /* BSS */
#define SHT_REL      9  /* Relocation entries */
#define SHT_DYNSYM   11 /* Dynamic symbol table */

/* Section header flags */
#define SHF_WRITE      0x1  /* Writable section */
#define SHF_ALLOC      0x2  /* Occupies memory */
#define SHF_EXECINSTR  0x4  /* Executable section */

/* Symbol binding */
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

/* Symbol type */
#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4

/* Symbol binding macros */
#define ELF64_ST_BIND(info) ((info) >> 4)
#define ELF64_ST_TYPE(info) ((info) & 0xF)
#define ELF64_ST_INFO(bind, type) (((bind) << 4) | ((type) & 0xF))

/* Relocation types for x86-64 */
#define R_X86_64_NONE      0
#define R_X86_64_64        1
#define R_X86_64_PC32      2
#define R_X86_64_32        10
#define R_X86_64_32S       11
#define R_X86_64_16        12
#define R_X86_64_PC16      13
#define R_X86_64_8         14
#define R_X86_64_RELATIVE  8
#define R_X86_64_PC8       15
#define R_X86_64_PLT32     4
#define R_X86_64_GOTPCREL  9
#define R_X86_64_GLOB_DAT  6   
#define R_X86_64_JUMP_SLOT 7  
#define R_X86_64_COPY      5   

/*DYNAMIK TAGS*/

/* Dynamic tags (elf64_dyn.tag values) */
#define DT_NULL     0
#define DT_NEEDED   1
#define DT_PLTRELSZ 2
#define DT_PLTGOT   3
#define DT_HASH     4
#define DT_STRTAB   5
#define DT_SYMTAB   6
#define DT_RELA     7
#define DT_RELASZ   8
#define DT_RELAENT  9
#define DT_STRSZ    10
#define DT_SYMENT   11
#define DT_INIT     12
#define DT_FINI     13
#define DT_SONAME   14
#define DT_RPATH    15
#define DT_SYMBOLIC 16
#define DT_REL      17
#define DT_RELSZ    18
#define DT_RELENT   19
#define DT_PLTREL   20
#define DT_DEBUG    21
#define DT_TEXTREL  22
#define DT_JMPREL   23
#define DT_BIND_NOW 24

/* ELF errors */
typedef enum {
    ELF_OK = 0,
    ELF_ERR_INVALID_MAGIC,
    ELF_ERR_INVALID_CLASS,
    ELF_ERR_INVALID_DATA,
    ELF_ERR_INVALID_TYPE,
    ELF_ERR_INVALID_MACHINE,
    ELF_ERR_INVALID_VERSION,
    ELF_ERR_NO_ENTRY,
    ELF_ERR_LOAD_FAILED,
    ELF_ERR_OUT_OF_MEMORY
} elf_error_t;

/* ELF64 header */
struct elf64_hdr {
    uint8_t  ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t hdr_size;
    uint16_t phdr_size;
    uint16_t ph_num;
    uint16_t shdr_size;
    uint16_t sh_num;
    uint16_t shstrndx;
};

/* ELF64 program header */
struct elf64_phdr {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
};

/* ELF64 section header */
struct elf64_shdr {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

/* ELF64 symbol */
struct elf64_sym {
    uint32_t st_name;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

/* ELF64 relocation entry (with addend) */
struct elf64_rela {
    uint64_t offset;
    uint64_t info;
    int64_t  addend;
};

/* ELF64 relocation entry (without addend) */
struct elf64_rel {
    uint64_t offset;
    uint64_t info;
};

/* ELF64 dynamic entry */
struct elf64_dyn {
    int64_t  tag;
    uint64_t val;
};

/* Special section indices */
#define SHN_UNDEF     0
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2

/* Max value for uint64_t */
#ifndef UINT64_MAX
#define UINT64_MAX  ((uint64_t)0xFFFFFFFFFFFFFFFF)
#endif

/* ELF64 file context */
typedef struct {
    struct elf64_hdr *hdr;
    struct elf64_phdr *phdrs;
    struct elf64_shdr *shdrs;
    struct elf64_sym *symtab;
    uint32_t symtab_count;
    const char *strtab;
    uint64_t strtab_size;
    const char *shstrtab;
    uint64_t base_vaddr;
    uint64_t entry;
    void *image;
    uint32_t image_size;
} elf64_t;

/* ELF64 loaded program context */
typedef struct {
    uint64_t entry;
    uint64_t base;
    uint64_t size;
    elf64_t elf;
} elf64_program_t;

/* API Functions */

/* Validate ELF64 file */
elf_error_t elf64_validate(const void *file, uint32_t file_size);

/* Load ELF64 from memory buffer */
elf_error_t elf64_load(const void *file, uint32_t file_size, elf64_program_t *prog);

/* Load ELF64 from filesystem */
elf_error_t elf64_load_file(const char *path, elf64_program_t *prog);

/* Get symbol by name */
struct elf64_sym *elf64_get_symbol(elf64_t *elf, const char *name);

/* Apply relocations */
/* NOTE: This is now internal and called from elf64_load.
   The function signature changed to accept segment mappings. */

/* Unload ELF64 program */
void elf64_unload(elf64_program_t *prog);

/* Get error string */
const char *elf64_strerror(elf_error_t err);