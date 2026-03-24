#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "elf.h"



/* A single entry in the kernel export table.
 * Terminate the table with { NULL, NULL }. */
typedef struct {
    const char *name;
    void       *addr;
} kernel_export_t;

/* Loaded shared object handle */
typedef struct {
    uint64_t  base_addr;      /* Runtime load base chosen by loader */
    uint64_t  size;           /* Total span of mapped memory        */
    void     *image;          /* Raw file bytes (heap-allocated)     */
    uint32_t  image_size;

    /* Dynamic symbol table (points inside image after parse) */
    struct elf64_sym *dynsym;
    uint32_t          dynsym_count;
    const char       *dynstr;

    char name[64];            /* Human-readable label / path         */
} so_module_t;

typedef enum {
    SO_OK = 0,
    SO_ERR_NOT_DYN,           /* ELF type is not ET_DYN              */
    SO_ERR_NO_LOAD_SEG,       /* No PT_LOAD segments found           */
    SO_ERR_MAP_FAILED,        /* Memory mapping / alloc failed       */
    SO_ERR_RELOC_FAILED,      /* Relocation could not be applied     */
    SO_ERR_UNDEF_SYM,         /* Unresolved undefined symbol         */
    SO_ERR_INVALID,           /* Generic validation error            */
    SO_ERR_OOM,               /* Out of memory                       */
} so_error_t;

/* -----------------------------------------------------------------------
 * API
 * -----------------------------------------------------------------------*/

/*
 * so_load()
 *   Load a .so from a raw memory buffer.
 *   `exports` is your NULL-terminated kernel export table.
 *   On success SO_OK, *out is filled.
 */
so_error_t so_load(const void *file, uint32_t file_size,
                   const kernel_export_t *exports,
                   so_module_t *out);

/*
 * so_load_file()
 *   Convenience wrapper that reads from the FAT32 filesystem.
 */
so_error_t so_load_file(const char *path,
                        const kernel_export_t *exports,
                        so_module_t *out);

/*
 * so_get_symbol()
 *   Look up a symbol by name in the loaded module's dynsym.
 *   Returns the runtime address (base + st_value), or NULL.
 */
void *so_get_symbol(const so_module_t *mod, const char *name);

/*
 * so_unload()
 *   Free all resources associated with a loaded module.
 */
void so_unload(so_module_t *mod);

/*
 * so_strerror()
 */
const char *so_strerror(so_error_t err);