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

static elf_error_t elf64_apply_relocation(elf64_t *elf, struct elf64_rela *rela,
                                           struct elf64_shdr *symtab_shdr,
                                           struct elf64_shdr *target_shdr) {
    uint32_t sym_idx = ELF64_ST_BIND(rela->info >> 32);
    uint32_t rel_type = rela->info & 0xFFFFFFFF;
    
    /* Get symbol if present */
    struct elf64_sym *sym = NULL;
    if (sym_idx > 0 && sym_idx < (symtab_shdr->sh_size / sizeof(struct elf64_sym))) {
        sym = &((struct elf64_sym *)((uint8_t *)elf->image + symtab_shdr->sh_offset))[sym_idx];
    }

    /* Calculate target address */
    uint64_t *target = (uint64_t *)((uint8_t *)elf->image + target_shdr->sh_offset + rela->offset);
    uint64_t sym_val = sym ? sym->st_value : 0;

    switch (rel_type) {
        case R_X86_64_NONE:
            /* No relocation needed */
            break;

        case R_X86_64_64:
            *target = sym_val + rela->addend;
            break;

        case R_X86_64_PC32: {
            int32_t *target32 = (int32_t *)target;
            *target32 = (int32_t)(sym_val + rela->addend - (uint64_t)target32);
            break;
        }

        case R_X86_64_32:
        case R_X86_64_32S: {
            uint32_t *target32 = (uint32_t *)target;
            *target32 = (uint32_t)(sym_val + rela->addend);
            break;
        }

        case R_X86_64_PLT32: {
            int32_t *target32 = (int32_t *)target;
            *target32 = (int32_t)(sym_val + rela->addend - (uint64_t)target32);
            break;
        }

        case R_X86_64_GOTPCREL: {
            int32_t *target32 = (int32_t *)target;
            /* For now, treat as PC-relative */
            *target32 = (int32_t)(sym_val + rela->addend - (uint64_t)target32);
            break;
        }

        default:
            serial_io_printf("ELF: Unknown relocation type %u\n", rel_type);
            return ELF_ERR_LOAD_FAILED;
    }

    return ELF_OK;
}

elf_error_t elf64_relocate(elf64_t *elf) {
    struct elf64_hdr *hdr = elf->hdr;

    if (!elf->shdrs) {
        return ELF_OK;  /* No sections, no relocations needed */
    }

    /* Find relocation sections */
    for (uint16_t i = 0; i < hdr->sh_num; i++) {
        struct elf64_shdr *shdr = &elf->shdrs[i];

        if (shdr->sh_type == SHT_RELA) {
            /* Get the section this relocation applies to */
            if (shdr->sh_info >= hdr->sh_num) {
                continue;
            }
            struct elf64_shdr *target_shdr = &elf->shdrs[shdr->sh_info];

            /* Find symbol table */
            if (shdr->sh_link >= hdr->sh_num) {
                continue;
            }
            struct elf64_shdr *symtab_shdr = &elf->shdrs[shdr->sh_link];

            /* Process relocations */
            struct elf64_rela *rela = (struct elf64_rela *)((uint8_t *)elf->image + shdr->sh_offset);
            uint32_t rel_count = shdr->sh_size / sizeof(struct elf64_rela);

            for (uint32_t j = 0; j < rel_count; j++) {
                elf_error_t err = elf64_apply_relocation(elf, &rela[j], symtab_shdr, target_shdr);
                if (err != ELF_OK) {
                    return err;
                }
            }
        } else if (shdr->sh_type == SHT_REL) {
            /* REL relocations (without addend) - less common for x86-64 */
            serial_io_printf("ELF: SHT_REL not supported on x86-64\n");
            return ELF_ERR_LOAD_FAILED;
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

    /* Calculate base virtual address and total size */
    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        struct elf64_phdr *phdr = &prog->elf.phdrs[i];

        if (phdr->type == PT_LOAD) {
            if (phdr->vaddr < prog->elf.base_vaddr) {
                prog->elf.base_vaddr = phdr->vaddr;
            }
        }
    }

    /* Load program headers (segments) */
    uint64_t hhdm_mapped_base = 0;
    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        struct elf64_phdr *phdr = &prog->elf.phdrs[i];

        if (phdr->type == PT_LOAD && phdr->memsz > 0) {
            /* Map the physical memory for this segment */
            uint8_t *src = (uint8_t *)file + phdr->offset;
            uint8_t *dst = (uint8_t *)uacpi_kernel_map(phdr->vaddr, phdr->memsz);

            if (!dst) {
                serial_io_printf("ELF: Failed to map memory at 0x%lx\n", phdr->vaddr);
                return ELF_ERR_LOAD_FAILED;
            }

            /* Store the HHDM mapped base address */
            if (phdr->vaddr == prog->elf.base_vaddr) {
                hhdm_mapped_base = (uint64_t)dst;
            }

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

    /* Calculate total size */
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
    prog->base = hhdm_mapped_base;  /* Use HHDM mapped base */

    /* Calculate entry point using HHDM mapped address */
    uint64_t entry_offset = hdr->entry - prog->elf.base_vaddr;
    prog->elf.entry = hhdm_mapped_base + entry_offset;
    prog->entry = prog->elf.entry;

    serial_io_printf("ELF: HHDM mapped base: 0x%lx, entry offset: 0x%lx, final entry: 0x%lx\n",
                     hhdm_mapped_base, entry_offset, prog->entry);

    /* Apply relocations for relocatable binaries */
    if (hdr->type == ET_REL || hdr->type == ET_DYN) {
        err = elf64_relocate(&prog->elf);
        if (err != ELF_OK) {
            return err;
        }
    }

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
