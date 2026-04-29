#include "elf.h"
#include "mem/paging.h"
#include "libs/string.h"
#include "uacpi/kernel_api.h"

static const char *error_strings[] = {
    "Success",
    "Invalid ELF magic number",
    "Invalid ELF class (not ELF64)",
    "Invalid ELF data encoding",
    "Invalid ELF type",
    "Invalid machine type (not x86-64)",
    "Invalid ELF version",
    "No entry point",
    "Failed to load segments",
    "Out of memory"
};

const char *elf64_strerror(elf_error_t err) {
    if (err < 0 || err > ELF_ERR_OUT_OF_MEMORY) {
        return "Unknown error";
    }
    return error_strings[err];
}

elf_error_t elf64_validate(const void *file, uint32_t file_size) {
    if (file_size < sizeof(struct elf64_hdr)) {
        return ELF_ERR_INVALID_MAGIC;
    }

    const uint8_t *ident = (const uint8_t *)file;

    /* Check ELF magic */
    if (ident[0] != ELF_MAGIC_0 || ident[1] != ELF_MAGIC_1 ||
        ident[2] != ELF_MAGIC_2 || ident[3] != ELF_MAGIC_3) {
        return ELF_ERR_INVALID_MAGIC;
    }

    /* Check class (must be 64-bit) */
    if (ident[4] != ELFCLASS64) {
        return ELF_ERR_INVALID_CLASS;
    }

    /* Check data encoding (we only support little endian) */
    if (ident[5] != ELFDATA2LSB) {
        return ELF_ERR_INVALID_DATA;
    }

    /* Check version */
    if (ident[6] != EV_CURRENT) {
        return ELF_ERR_INVALID_VERSION;
    }

    const struct elf64_hdr *hdr = (const struct elf64_hdr *)file;

    /* Check type (we support EXEC and DYN) */
    if (hdr->type != ET_EXEC && hdr->type != ET_DYN) {
        return ELF_ERR_INVALID_TYPE;
    }

    /* Check machine (must be x86-64) */
    if (hdr->machine != EM_X86_64) {
        return ELF_ERR_INVALID_MACHINE;
    }

    /* Check version */
    if (hdr->version != EV_CURRENT) {
        return ELF_ERR_INVALID_VERSION;
    }

    return ELF_OK;
}

static struct elf64_sym *elf64_find_symbol(elf64_t *elf, const char *name) {
    if (!elf->symtab || !elf->strtab) {
        return NULL;
    }

    for (uint32_t i = 0; i < elf->symtab_count; i++) {
        struct elf64_sym *sym = &elf->symtab[i];
        const char *sym_name = elf->strtab + sym->st_name;

        if (strcmp(sym_name, name) == 0) {
            return sym;
        }
    }

    return NULL;
}

struct elf64_sym *elf64_get_symbol(elf64_t *elf, const char *name) {
    return elf64_find_symbol(elf, name);
}

static elf_error_t elf64_parse(elf64_t *elf) {
    struct elf64_hdr *hdr = elf->hdr;

    /* Parse program headers */
    if (hdr->phoff > 0 && hdr->ph_num > 0) {
        elf->phdrs = (struct elf64_phdr *)((uint8_t *)elf->image + hdr->phoff);
    }

    /* Parse section headers */
    if (hdr->shoff > 0 && hdr->sh_num > 0) {
        elf->shdrs = (struct elf64_shdr *)((uint8_t *)elf->image + hdr->shoff);

        /* Get section header string table */
        if (hdr->shstrndx != SHN_UNDEF && hdr->shstrndx < hdr->sh_num) {
            struct elf64_shdr *shstrtab_shdr = &elf->shdrs[hdr->shstrndx];
            elf->shstrtab = (const char *)((uint8_t *)elf->image + shstrtab_shdr->sh_offset);
        }

        /* Find symbol table and string table */
        for (uint16_t i = 0; i < hdr->sh_num; i++) {
            struct elf64_shdr *shdr = &elf->shdrs[i];

            if (shdr->sh_type == SHT_SYMTAB) {
                elf->symtab = (struct elf64_sym *)((uint8_t *)elf->image + shdr->sh_offset);
                elf->symtab_count = shdr->sh_size / sizeof(struct elf64_sym);

                /* Get associated string table */
                if (shdr->sh_link < hdr->sh_num) {
                    struct elf64_shdr *strtab_shdr = &elf->shdrs[shdr->sh_link];
                    elf->strtab = (const char *)((uint8_t *)elf->image + strtab_shdr->sh_offset);
                    elf->strtab_size = strtab_shdr->sh_size;
                }
            }
        }
    }

    return ELF_OK;
}

/*
 * FIXED: Properly compute the loaded address of a virtual address
 * in the kernel's memory space after HHDM mapping.
 * 
 * The key insight: we're using uacpi_kernel_map which returns a pointer
 * to the actual kernel virtual address where the segment was mapped.
 * We need to track the mapping offset for each segment.
 */
struct segment_mapping {
    uint64_t vaddr_start;
    uint64_t vaddr_end;
    uint64_t loaded_addr;
};

static elf_error_t elf64_apply_relocation(elf64_t *elf, 
                                          struct elf64_rela *rela,
                                          struct elf64_shdr *symtab_shdr,
                                          struct elf64_shdr *target_shdr,
                                          uint64_t load_base,
                                          const struct segment_mapping *mappings,
                                          uint32_t num_mappings) {
    uint32_t sym_idx = ELF64_R_SYM(rela->info);
    uint32_t rel_type = ELF64_R_TYPE(rela->info);
    
    /* Get symbol if present */
    struct elf64_sym *sym = NULL;
    if (sym_idx > 0 && sym_idx < (symtab_shdr->sh_size / sizeof(struct elf64_sym))) {
        sym = &((struct elf64_sym *)((uint8_t *)elf->image + symtab_shdr->sh_offset))[sym_idx];
    }

    /* Find the loaded address of the relocation target */
    uint64_t target_addr = 0;
    
    /* The relocation offset is relative to the target section */
    uint64_t target_vaddr = target_shdr->sh_addr + rela->offset;
    
    /* Find which segment this address is in */
    for (uint32_t i = 0; i < num_mappings; i++) {
        if (target_vaddr >= mappings[i].vaddr_start && 
            target_vaddr < mappings[i].vaddr_end) {
            uint64_t offset_in_segment = target_vaddr - mappings[i].vaddr_start;
            target_addr = mappings[i].loaded_addr + offset_in_segment;
            break;
        }
    }
    
    if (target_addr == 0) {
        serial_io_printf("ELF: Relocation target 0x%lx not in any mapped segment\n", target_vaddr);
        return ELF_ERR_LOAD_FAILED;
    }
    
    uint64_t *target = (uint64_t *)target_addr;
    
    /* Get symbol value - also needs to be resolved to loaded address */
    uint64_t sym_val = 0;
    if (sym) {
        uint64_t sym_vaddr = sym->st_value;
        
        if (sym_vaddr != 0) {
            /* Find which segment the symbol is in */
            for (uint32_t i = 0; i < num_mappings; i++) {
                if (sym_vaddr >= mappings[i].vaddr_start && 
                    sym_vaddr < mappings[i].vaddr_end) {
                    uint64_t offset_in_segment = sym_vaddr - mappings[i].vaddr_start;
                    sym_val = mappings[i].loaded_addr + offset_in_segment;
                    break;
                }
            }
            
            if (sym_val == 0) {
                serial_io_printf("ELF: Symbol value 0x%lx not in any mapped segment\n", sym_vaddr);
                return ELF_ERR_LOAD_FAILED;
            }
        }
    }

    switch (rel_type) {
        case R_X86_64_NONE:
            break;

        case R_X86_64_64:
            *target = sym_val + rela->addend;
            break;

        case R_X86_64_PC32: {
            int32_t *target32 = (int32_t *)target;
            int64_t value = sym_val + rela->addend - target_addr;
            if (value < INT32_MIN || value > INT32_MAX) {
                serial_io_printf("ELF: PC32 relocation overflow at 0x%lx\n", target_addr);
                return ELF_ERR_LOAD_FAILED;
            }
            *target32 = (int32_t)value;
            break;
        }

        case R_X86_64_32: {
            uint32_t *target32 = (uint32_t *)target;
            *target32 = (uint32_t)(sym_val + rela->addend);
            break;
        }
        
        case R_X86_64_32S: {
            int32_t *target32 = (int32_t *)target;
            *target32 = (int32_t)(sym_val + rela->addend);
            break;
        }
        
        case R_X86_64_RELATIVE: {
            /* RELATIVE: addend is relative to base, or just an offset */
            uint64_t reloc_value = load_base + rela->addend;
            
            /* If addend looks like a vaddr, try to resolve it properly */
            if (rela->addend >= 0x400000) {
                for (uint32_t i = 0; i < num_mappings; i++) {
                    if (rela->addend >= mappings[i].vaddr_start && 
                        rela->addend < mappings[i].vaddr_end) {
                        uint64_t offset = rela->addend - mappings[i].vaddr_start;
                        reloc_value = mappings[i].loaded_addr + offset;
                        break;
                    }
                }
            }
            
            *target = reloc_value;
            break;
        }
        
        case R_X86_64_PLT32: {
            int32_t *target32 = (int32_t *)target;
            int64_t value = sym_val + rela->addend - target_addr;
            if (value < INT32_MIN || value > INT32_MAX) {
                serial_io_printf("ELF: PLT32 relocation overflow at 0x%lx\n", target_addr);
                return ELF_ERR_LOAD_FAILED;
            }
            *target32 = (int32_t)value;
            break;
        }

        case R_X86_64_GOTPCREL: {
            int32_t *target32 = (int32_t *)target;
            int64_t value = sym_val + rela->addend - target_addr;
            if (value < INT32_MIN || value > INT32_MAX) {
                serial_io_printf("ELF: GOTPCREL relocation overflow at 0x%lx\n", target_addr);
                return ELF_ERR_LOAD_FAILED;
            }
            *target32 = (int32_t)value;
            break;
        }

        default:
            serial_io_printf("ELF: Unknown relocation type %u at offset 0x%lx\n", 
                           rel_type, rela->offset);
            return ELF_ERR_LOAD_FAILED;
    }

    return ELF_OK;
}

elf_error_t elf64_relocate(elf64_t *elf, uint64_t load_base,
                           const struct segment_mapping *mappings,
                           uint32_t num_mappings) {
    struct elf64_hdr *hdr = elf->hdr;

    if (!elf->shdrs) {
        return ELF_OK;
    }

    for (uint16_t i = 0; i < hdr->sh_num; i++) {
        struct elf64_shdr *shdr = &elf->shdrs[i];

        if (shdr->sh_type == SHT_RELA) {
            if (shdr->sh_info >= hdr->sh_num) {
                continue;
            }
            struct elf64_shdr *target_shdr = &elf->shdrs[shdr->sh_info];

            if (shdr->sh_link >= hdr->sh_num) {
                continue;
            }
            struct elf64_shdr *symtab_shdr = &elf->shdrs[shdr->sh_link];

            struct elf64_rela *rela = (struct elf64_rela *)((uint8_t *)elf->image + shdr->sh_offset);
            uint32_t rel_count = shdr->sh_size / sizeof(struct elf64_rela);

            serial_io_printf("ELF: Processing %u relocations for section %d\n", rel_count, i);

            for (uint32_t j = 0; j < rel_count; j++) {
                elf_error_t err = elf64_apply_relocation(elf, &rela[j], symtab_shdr, 
                                                        target_shdr, load_base,
                                                        mappings, num_mappings);
                if (err != ELF_OK) {
                    return err;
                }
            }
        }
    }

    return ELF_OK;
}

elf_error_t elf64_load(const void *file, uint32_t file_size, elf64_program_t *prog) {
    if (!file || !prog || file_size < sizeof(struct elf64_hdr)) {
        return ELF_ERR_INVALID_MAGIC;
    }

    /* Validate ELF file */
    elf_error_t err = elf64_validate(file, file_size);
    if (err != ELF_OK) {
        return err;
    }

    /* Initialize program structure */
    memset(prog, 0, sizeof(elf64_program_t));
    memset(&prog->elf, 0, sizeof(elf64_t));

    prog->elf.image = (void *)file;
    prog->elf.image_size = file_size;
    prog->elf.hdr = (struct elf64_hdr *)file;

    /* Parse ELF structures */
    err = elf64_parse(&prog->elf);
    if (err != ELF_OK) {
        return err;
    }

    struct elf64_hdr *hdr = prog->elf.hdr;
    prog->elf.base_vaddr = UINT64_MAX;

    /* Calculate base virtual address */
    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        struct elf64_phdr *phdr = &prog->elf.phdrs[i];
        if (phdr->type == PT_LOAD) {
            if (phdr->vaddr < prog->elf.base_vaddr) {
                prog->elf.base_vaddr = phdr->vaddr;
            }
        }
    }

    serial_io_printf("ELF: Base vaddr: 0x%lx\n", prog->elf.base_vaddr);

    /* Allocate space for segment mappings */
    struct segment_mapping *mappings = malloc(sizeof(struct segment_mapping) * hdr->ph_num);
    if (!mappings) {
        return ELF_ERR_OUT_OF_MEMORY;
    }
    uint32_t mapping_count = 0;
    uint64_t hhdm_mapped_base = 0;

    /* Load program headers (segments) */
    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        struct elf64_phdr *phdr = &prog->elf.phdrs[i];

        if (phdr->type == PT_LOAD && phdr->memsz > 0) {
            /* Map the physical memory for this segment via uacpi */
            uint8_t *src = (uint8_t *)file + phdr->offset;
            uint8_t *dst = (uint8_t *)uacpi_kernel_map(phdr->vaddr, phdr->memsz);

            if (!dst) {
                serial_io_printf("ELF: Failed to map memory at 0x%lx for segment %d\n", 
                               phdr->vaddr, i);
                free(mappings);
                return ELF_ERR_LOAD_FAILED;
            }

            /* Store the HHDM mapped base for the first segment */
            if (phdr->vaddr == prog->elf.base_vaddr) {
                hhdm_mapped_base = (uint64_t)dst;
            }

            serial_io_printf("ELF: Loading segment %d: vaddr=0x%lx -> hhdm=0x%lx, size=%lu\n",
                           i, phdr->vaddr, (uint64_t)dst, phdr->memsz);

            /* Record this segment's mapping for relocation use */
            mappings[mapping_count].vaddr_start = phdr->vaddr;
            mappings[mapping_count].vaddr_end = phdr->vaddr + phdr->memsz;
            mappings[mapping_count].loaded_addr = (uint64_t)dst;
            mapping_count++;

            /* Copy data segment */
            if (phdr->filesz > 0) {
                memcpy(dst, src, phdr->filesz);
            }

            /* Zero out BSS portion */
            if (phdr->memsz > phdr->filesz) {
                memset(dst + phdr->filesz, 0, phdr->memsz - phdr->filesz);
            }
        }
    }

    /* Calculate total size and entry point */
    uint64_t max_vaddr = 0;
    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        struct elf64_phdr *phdr = &prog->elf.phdrs[i];
        if (phdr->type == PT_LOAD) {
            uint64_t end_vaddr = phdr->vaddr + phdr->memsz;
            if (end_vaddr > max_vaddr) {
                max_vaddr = end_vaddr;
            }
        }
    }
    prog->size = max_vaddr - prog->elf.base_vaddr;
    prog->base = hhdm_mapped_base;

    /* Calculate entry point using the mappings */
    uint64_t entry_vaddr = hdr->entry;
    uint64_t prog_entry = 0;
    
    for (uint32_t i = 0; i < mapping_count; i++) {
        if (entry_vaddr >= mappings[i].vaddr_start && 
            entry_vaddr < mappings[i].vaddr_end) {
            uint64_t offset = entry_vaddr - mappings[i].vaddr_start;
            prog_entry = mappings[i].loaded_addr + offset;
            break;
        }
    }
    
    if (prog_entry == 0) {
        serial_io_printf("ELF: Warning - entry point 0x%lx not in any loaded segment\n", entry_vaddr);
        prog_entry = hhdm_mapped_base;  /* Fallback */
    }
    
    prog->elf.entry = prog_entry;
    prog->entry = prog_entry;

    serial_io_printf("ELF: HHDM mapped base: 0x%lx, entry: 0x%lx\n",
                     hhdm_mapped_base, prog->entry);

    /* Apply relocations for PIE/relocatable binaries */
    if (hdr->type == ET_DYN || hdr->type == ET_REL) {
        serial_io_printf("ELF: Applying relocations (type=%d, base=0x%lx)...\n", 
                       hdr->type, hhdm_mapped_base);
        err = elf64_relocate(&prog->elf, hhdm_mapped_base, mappings, mapping_count);
        if (err != ELF_OK) {
            serial_io_printf("ELF: Relocation failed: %s\n", elf64_strerror(err));
            free(mappings);
            return err;
        }
        serial_io_printf("ELF: Relocations applied successfully\n");
    }

    free(mappings);

    serial_io_printf("ELF: Loaded at 0x%lx, entry 0x%lx, size %lu bytes\n",
                     prog->base, prog->entry, prog->size);

    return ELF_OK;
}

elf_error_t elf64_load_file(const char *path, elf64_program_t *prog) {
    if (!path || !prog) {
        return ELF_ERR_INVALID_MAGIC;
    }

    serial_io_printf("ELF: Loading file '%s'...\n", path);

    /* Allocate a buffer - start with 64KB, expand if needed */
    uint32_t buffer_size = 65536;
    void *file_buffer = malloc(buffer_size);
    if (!file_buffer) {
        serial_io_printf("ELF: Failed to allocate buffer\n");
        return ELF_ERR_OUT_OF_MEMORY;
    }

    /* Read the file - fat32_read_file updates file_size with actual size */
    uint32_t file_size = buffer_size;
    if (!fat32_read_file(path, file_buffer, &file_size)) {
        serial_io_printf("ELF: Failed to read file '%s'\n", path);
        free(file_buffer);
        return ELF_ERR_LOAD_FAILED;
    }

    serial_io_printf("ELF: Read %u bytes from '%s'\n", file_size, path);

    if (file_size == 0) {
        serial_io_printf("ELF: File '%s' is empty\n", path);
        free(file_buffer);
        return ELF_ERR_INVALID_MAGIC;
    }

    /* Load ELF from buffer */
    elf_error_t err = elf64_load(file_buffer, file_size, prog);
    if (err != ELF_OK) {
        serial_io_printf("ELF: Failed to load '%s': %s\n", path, elf64_strerror(err));
        free(file_buffer);
        return err;
    }

    /* Keep the buffer allocated - it's now part of the loaded program */
    /* The caller should call elf64_unload() to free it */
    return ELF_OK;
}

void elf64_unload(elf64_program_t *prog) {
    if (!prog) return;

    if (prog->base && prog->size)
        uacpi_kernel_unmap((void *)prog->base, prog->size);

    if (prog->elf.image)
        free(prog->elf.image);

    memset(prog, 0, sizeof(elf64_program_t));
}