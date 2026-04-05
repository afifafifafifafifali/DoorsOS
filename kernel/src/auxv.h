#pragma once

#include <stdint.h>
#include <stddef.h>

/* Auxiliary Vector Types (System V ABI) */
#define AT_NULL     0   /* End of vector */
#define AT_IGNORE   1   /* Entry should be ignored */
#define AT_EXECFD   2   /* File descriptor of program */
#define AT_PHDR     3   /* Program headers of program */
#define AT_PHENT    4   /* Size of program header entry */
#define AT_PHNUM    5   /* Number of program headers */
#define AT_PAGESZ   6   /* System page size */
#define AT_BASE     7   /* Base address of interpreter */
#define AT_FLAGS    8   /* Flags */
#define AT_ENTRY    9   /* Entry point of program */
#define AT_NOTELF   10  /* Program is not ELF */
#define AT_UID      11  /* Real uid */
#define AT_EUID     12  /* Effective uid */
#define AT_GID      13  /* Real gid */
#define AT_EGID     14  /* Effective gid */
#define AT_PLATFORM 15  /* String identifying platform */
#define AT_HWCAP    16  /* Machine dependent hints */
#define AT_CLKTCK   17  /* Frequency of times() */
#define AT_FPUCW    18  /* Used FPU initialization */
#define AT_SECURE   23  /* Secure mode flag */
#define AT_BASE_PLATFORM 24 /* String identifying real platform */
#define AT_RANDOM   25  /* Pointer to 16 random bytes */
#define AT_HWCAP2   26  /* More machine dependent hints */
#define AT_EXECFN   31  /* Filename of executable */
#define AT_SYSINFO  32  /* Pointer to vDSO */
#define AT_SYSINFO_EHDR 33 /* Pointer to vDSO ELF header */

/* Auxiliary vector entry (must match what userland expects) */
typedef struct {
    uint64_t a_type;
    uint64_t a_val;
} auxv_t;

