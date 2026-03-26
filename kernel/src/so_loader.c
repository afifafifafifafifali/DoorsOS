#include "so_loader.h"
#include "gfx/serial_io.h"
#include "fs/fat32.h"
#include "mem/heap.h"
#include "libs/string.h"

#include "uacpi/kernel_api.h"



// Mostly bullshit from osdev.org
/* -----------------------------------------------------------------------
 * Internal helpers
 * -----------------------------------------------------------------------*/

static const char *so_error_table[] = {
    "Success",
    "ELF is not ET_DYN (not a shared object)",
    "No PT_LOAD segments found",
    "Failed to map segment memory",
    "Relocation failed",
    "Unresolved undefined symbol",
    "Invalid ELF",
    "Out of memory",
};

const char *so_strerror(so_error_t err) {
    if ((unsigned)err >= sizeof(so_error_table)/sizeof(*so_error_table))
        return "Unknown error";
    return so_error_table[err];
}

/* Resolve a symbol name against the kernel export table.
 * Returns the function/data pointer, or NULL if not found. */
static void *resolve_export(const kernel_export_t *exports, const char *name) {
    if (!exports || !name) return NULL;
    for (const kernel_export_t *e = exports; e->name != NULL; e++) {
        if (strcmp(e->name, name) == 0)
            return e->addr;
    }
    return NULL;
}

/* Parse SHT_DYNSYM and its associated SHT_STRTAB out of the image.
 * Falls back to SHT_SYMTAB if DYNSYM is absent (e.g. unstripped test .so). */
static void so_parse_dynsym(so_module_t *mod, struct elf64_hdr *hdr) {
    struct elf64_shdr *shdrs = (struct elf64_shdr *)((uint8_t *)mod->image + hdr->shoff);

    for (uint16_t i = 0; i < hdr->sh_num; i++) {
        struct elf64_shdr *sh = &shdrs[i];

        if (sh->sh_type == SHT_DYNSYM || sh->sh_type == SHT_SYMTAB) {
            mod->dynsym       = (struct elf64_sym *)((uint8_t *)mod->image + sh->sh_offset);
            mod->dynsym_count = sh->sh_size / sizeof(struct elf64_sym);

            if (sh->sh_link < hdr->sh_num) {
                struct elf64_shdr *str_sh = &shdrs[sh->sh_link];
                mod->dynstr = (const char *)((uint8_t *)mod->image + str_sh->sh_offset);
            }

            /* Prefer DYNSYM; if we found it stop looking */
            if (sh->sh_type == SHT_DYNSYM) break;
        }
    }
}

/* -----------------------------------------------------------------------
 * Relocation
 *
 * Key difference from elf64_apply_relocation():
 *   - `load_base`  is added to every symbol value and every absolute patch
 *   - R_X86_64_RELATIVE is handled (the most common reloc in PIE/DSO)
 *   - SHN_UNDEF symbols are resolved from the kernel export table
 *   - We patch into the *mapped* memory (base + vaddr), not the raw image
 * -----------------------------------------------------------------------*/
static so_error_t so_apply_rela(so_module_t *mod,
                                 struct elf64_rela *rela,
                                 struct elf64_sym  *symtab,
                                 uint32_t           symtab_count,
                                 const char        *strtab,
                                 const kernel_export_t *exports)
{
    uint32_t sym_idx  = (uint32_t)(rela->info >> 32);
    uint32_t rel_type = (uint32_t)(rela->info & 0xFFFFFFFF);

    /* The offset field in ET_DYN RELA is ALWAYS a vaddr offset from 0.
     * Add the load base to get the runtime address to patch. */
    uint64_t patch_addr = mod->base_addr + rela->offset;
    uint64_t *patch64   = (uint64_t *)patch_addr;
    int32_t  *patch32s  = (int32_t  *)patch_addr;
    uint32_t *patch32   = (uint32_t *)patch_addr;

    uint64_t sym_runtime_addr = 0;
    const char *sym_name_str  = "(none)";

    if (sym_idx > 0 && symtab && sym_idx < symtab_count) {
        struct elf64_sym *sym = &symtab[sym_idx];

        if (strtab)
            sym_name_str = strtab + sym->st_name;

        if (sym->st_shndx == SHN_UNDEF) {
            void *resolved = resolve_export(exports, sym_name_str);
            if (!resolved) {
                serial_io_printf("SO: UNRESOLVED '%s'\n", sym_name_str);
                return SO_ERR_UNDEF_SYM;
            }
            sym_runtime_addr = (uint64_t)resolved;
            serial_io_printf("SO: RESOLVED '%s' -> 0x%lx\n", sym_name_str, sym_runtime_addr);
        } else if (sym->st_shndx == SHN_ABS) {
            sym_runtime_addr = sym->st_value;
        } else {
            sym_runtime_addr = mod->base_addr + sym->st_value;
        }
    }

    serial_io_printf("SO: reloc type=%-2u offset=0x%lx patch_addr=0x%lx sym='%s' sym_val=0x%lx addend=%ld\n",
                     rel_type, rela->offset, patch_addr, sym_name_str,
                     sym_runtime_addr, (int64_t)rela->addend);

    switch (rel_type) {
        case R_X86_64_NONE:
            break;

        case R_X86_64_64:
            *patch64 = sym_runtime_addr + (int64_t)rela->addend;
            break;

        case R_X86_64_RELATIVE:
            /* B + A — base-relative, no symbol */
            *patch64 = mod->base_addr + (int64_t)rela->addend;
            break;

        case R_X86_64_GLOB_DAT:
            /* S — fill GOT entry with symbol address, no addend */
            *patch64 = sym_runtime_addr;
            break;

        case R_X86_64_JUMP_SLOT:
            /* S — fill PLT GOT slot with symbol address, no addend */
            *patch64 = sym_runtime_addr;
            serial_io_printf("SO: JUMP_SLOT patched GOT at 0x%lx -> 0x%lx\n",
                             patch_addr, sym_runtime_addr);
            break;

        case R_X86_64_PC32:
        case R_X86_64_PLT32:
            *patch32s = (int32_t)(sym_runtime_addr + (int64_t)rela->addend - patch_addr);
            break;

        case R_X86_64_32:
        case R_X86_64_32S:
            *patch32 = (uint32_t)(sym_runtime_addr + (int64_t)rela->addend);
            break;

        case R_X86_64_GOTPCREL:
            *patch32s = (int32_t)(sym_runtime_addr + (int64_t)rela->addend - patch_addr);
            break;

        case R_X86_64_COPY:
            if (sym_runtime_addr && sym_idx < symtab_count) {
                struct elf64_sym *sym = &symtab[sym_idx];
                memcpy((void *)patch_addr, (void *)sym_runtime_addr, sym->st_size);
            }
            break;

        default:
            serial_io_printf("SO: Unknown reloc type %u at offset 0x%lx\n",
                             rel_type, rela->offset);
            return SO_ERR_RELOC_FAILED;
    }

    return SO_OK;
}

static so_error_t so_relocate(so_module_t *mod,
                               struct elf64_hdr *hdr,
                               const kernel_export_t *exports)
{
    struct elf64_phdr *phdrs = (struct elf64_phdr *)((uint8_t *)mod->image + hdr->phoff);

    /* ----------------------------------------------------------------
     * Find the PT_DYNAMIC segment and extract all relocation tables.
     * We read everything from the dynamic segment rather than section
     * headers because stripped .so files may have no section headers.
     * ---------------------------------------------------------------- */
    uint64_t dt_rela     = 0, dt_relasz   = 0, dt_relaent = sizeof(struct elf64_rela);
    uint64_t dt_jmprel   = 0, dt_pltrelsz = 0, dt_pltrel  = 0;
    uint64_t dt_symtab   = 0, dt_strtab   = 0;

    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        if (phdrs[i].type != PT_DYNAMIC) continue;

        /* PT_DYNAMIC vaddr is relative to load base in ET_DYN */
        struct elf64_dyn *dyn = (struct elf64_dyn *)(mod->base_addr + phdrs[i].vaddr);

        for (; dyn->tag != DT_NULL; dyn++) {
            switch (dyn->tag) {
                case DT_RELA:     dt_rela     = dyn->val; break;
                case DT_RELASZ:   dt_relasz   = dyn->val; break;
                case DT_RELAENT:  dt_relaent  = dyn->val; break;
                case DT_JMPREL:   dt_jmprel   = dyn->val; break;
                case DT_PLTRELSZ: dt_pltrelsz = dyn->val; break;
                case DT_PLTREL:   dt_pltrel   = dyn->val; break;
                case DT_SYMTAB:   dt_symtab   = dyn->val; break;
                case DT_STRTAB:   dt_strtab   = dyn->val; break;
                default: break;
            }
        }
        break; /* only one PT_DYNAMIC */
    }

    /* Symbol table and string table (base-relative addresses) */
    struct elf64_sym *symtab    = dt_symtab ? (struct elf64_sym *)(mod->base_addr + dt_symtab) : NULL;
    const char       *strtab    = dt_strtab ? (const char       *)(mod->base_addr + dt_strtab) : NULL;
    uint32_t          sym_count = mod->dynsym_count ? mod->dynsym_count : 0;

    /* ----------------------------------------------------------------
     * Apply DT_RELA — general relocations (RELATIVE, GLOB_DAT, etc.)
     * ---------------------------------------------------------------- */
    if (dt_rela && dt_relasz) {
        struct elf64_rela *relas     = (struct elf64_rela *)(mod->base_addr + dt_rela);
        uint32_t           rel_count = dt_relasz / dt_relaent;

        serial_io_printf("SO: DT_RELA: %u entries at 0x%lx\n", rel_count, dt_rela);

        for (uint32_t j = 0; j < rel_count; j++) {
            so_error_t err = so_apply_rela(mod, &relas[j],
                                            symtab, sym_count,
                                            strtab, exports);
            if (err != SO_OK) return err;
        }
    }

    if (dt_jmprel && dt_pltrelsz) {
        serial_io_printf("SO: DT_JMPREL: %lu bytes at 0x%lx\n", dt_pltrelsz, dt_jmprel);

        if (dt_pltrel == DT_REL) {
            /* REL format — no addend, almost never happens on x86-64 */
            serial_io_printf("SO: DT_PLTREL=REL not supported\n");
            return SO_ERR_RELOC_FAILED;
        }

        /* RELA format (default for x86-64) */
        struct elf64_rela *relas     = (struct elf64_rela *)(mod->base_addr + dt_jmprel);
        uint32_t           rel_count = dt_pltrelsz / sizeof(struct elf64_rela);

        for (uint32_t j = 0; j < rel_count; j++) {
            so_error_t err = so_apply_rela(mod, &relas[j],
                                            symtab, sym_count,
                                            strtab, exports);
            if (err != SO_OK) return err;
        }
    }

    /* ----------------------------------------------------------------
     * Fallback: scan section headers for any RELA sections not covered
     * above (e.g. unstripped debug builds that have extra rela sections).
     * ---------------------------------------------------------------- */
    if (hdr->shoff && hdr->sh_num) {
        struct elf64_shdr *shdrs = (struct elf64_shdr *)((uint8_t *)mod->image + hdr->shoff);

        for (uint16_t i = 0; i < hdr->sh_num; i++) {
            struct elf64_shdr *sh = &shdrs[i];
            if (sh->sh_type != SHT_RELA) continue;
            if (sh->sh_link >= hdr->sh_num) continue;
            if (sh->sh_info >= hdr->sh_num) continue;

            struct elf64_shdr *sym_sh  = &shdrs[sh->sh_link];
            struct elf64_sym  *sh_syms = (struct elf64_sym *)((uint8_t *)mod->image + sym_sh->sh_offset);
            uint32_t           sh_sym_count = sym_sh->sh_size / sizeof(struct elf64_sym);
            const char        *sh_strtab = NULL;
            if (sym_sh->sh_link < hdr->sh_num)
                sh_strtab = (const char *)((uint8_t *)mod->image + shdrs[sym_sh->sh_link].sh_offset);

            struct elf64_rela *relas     = (struct elf64_rela *)((uint8_t *)mod->image + sh->sh_offset);
            uint32_t           rel_count = sh->sh_size / sizeof(struct elf64_rela);

            for (uint32_t j = 0; j < rel_count; j++) {
                so_error_t err = so_apply_rela(mod, &relas[j],
                                                sh_syms, sh_sym_count,
                                                sh_strtab, exports);
                if (err != SO_OK) return err;
            }
        }
    }

    return SO_OK;
}

/* -----------------------------------------------------------------------
 * so_load() — main entry point
 * -----------------------------------------------------------------------*/
so_error_t so_load(const void *file, uint32_t file_size,
                   const kernel_export_t *exports,
                   so_module_t *out)
{
    if (!file || !out || file_size < sizeof(struct elf64_hdr))
        return SO_ERR_INVALID;

    /* Re-use your existing ELF validator — it already accepts ET_DYN */
    elf_error_t verr = elf64_validate(file, file_size);
    if (verr != ELF_OK) {
        serial_io_printf("SO: ELF validation failed: %s\n", elf64_strerror(verr));
        return SO_ERR_INVALID;
    }

    struct elf64_hdr *hdr = (struct elf64_hdr *)file;
    if (hdr->type != ET_DYN) {
        serial_io_printf("SO: Not a shared object (type=%u)\n", hdr->type);
        return SO_ERR_NOT_DYN;
    }

    memset(out, 0, sizeof(so_module_t));
    out->image      = (void *)file;
    out->image_size = file_size;

    struct elf64_phdr *phdrs = (struct elf64_phdr *)((uint8_t *)file + hdr->phoff);

    /* ---- Pass 1: compute total virtual span ---- */
    uint64_t min_vaddr = UINT64_MAX;
    uint64_t max_vaddr = 0;
    bool     has_load  = false;

    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        if (phdrs[i].type != PT_LOAD) continue;
        has_load = true;
        if (phdrs[i].vaddr < min_vaddr) min_vaddr = phdrs[i].vaddr;
        uint64_t end = phdrs[i].vaddr + phdrs[i].memsz;
        if (end > max_vaddr) max_vaddr = end;
    }

    if (!has_load) return SO_ERR_NO_LOAD_SEG;

  
    uint64_t span = max_vaddr - min_vaddr;
    out->size = span;

  
    void *mapped = uacpi_kernel_map(0 /* physical: let kernel decide */, span);
    if (!mapped) {
        serial_io_printf("SO: Failed to map %lu bytes\n", span);
        return SO_ERR_MAP_FAILED;
    }

    out->base_addr = (uint64_t)mapped;
    serial_io_printf("SO: Mapped %lu bytes at 0x%lx\n", span, out->base_addr);

    /* ---- Pass 2: copy segments into mapped memory ---- */
    for (uint16_t i = 0; i < hdr->ph_num; i++) {
        struct elf64_phdr *ph = &phdrs[i];
        if (ph->type != PT_LOAD || ph->memsz == 0) continue;

        uint8_t *dst = (uint8_t *)out->base_addr + (ph->vaddr - min_vaddr);
        uint8_t *src = (uint8_t *)file + ph->offset;

        if (ph->filesz > 0)
            memcpy(dst, src, ph->filesz);
        if (ph->memsz > ph->filesz)
            memset(dst + ph->filesz, 0, ph->memsz - ph->filesz);
    }

    /* ---- Parse dynsym ---- */
    so_parse_dynsym(out, hdr);
    serial_io_printf("SO: dynsym has %u entries\n", out->dynsym_count);

    /* ---- Relocate ---- */
    so_error_t err = so_relocate(out, hdr, exports);
    if (err != SO_OK) {
        serial_io_printf("SO: Relocation failed: %s\n", so_strerror(err));
        uacpi_kernel_unmap(mapped, span);
        return err;
    }

    serial_io_printf("SO: Loaded '%s' at base 0x%lx size %lu\n",
                     out->name[0] ? out->name : "(unnamed)", out->base_addr, out->size);
    return SO_OK;
}

so_error_t so_load_file(const char *path,
                         const kernel_export_t *exports,
                         so_module_t *out)
{
    if (!path || !out) return SO_ERR_INVALID;

    serial_io_printf("SO: Loading '%s'\n", path);

    uint32_t buf_size = 131072; /* 128 KB initial buffer */
    void *buf = malloc(buf_size);
    if (!buf) return SO_ERR_OOM;

    uint32_t file_size = buf_size;
    if (!fat32_read_file(path, buf, &file_size)) {
        serial_io_printf("SO: Failed to read '%s'\n", path);
        free(buf);
        return SO_ERR_INVALID;
    }

    /* Copy path into name field */
    strncpy(out->name, path, sizeof(out->name) - 1);
    out->name[sizeof(out->name) - 1] = '\0';

    so_error_t err = so_load(buf, file_size, exports, out);
    if (err != SO_OK) {
        free(buf);
        /* name was set before so_load, clear it */
        return err;
    }

    /* buf is now the backing image; kept alive until so_unload() */
    return SO_OK;
}

/* -----------------------------------------------------------------------
 * so_get_symbol
 * -----------------------------------------------------------------------*/
void *so_get_symbol(const so_module_t *mod, const char *name) {
    if (!mod || !name || !mod->dynsym || !mod->dynstr) return NULL;

    for (uint32_t i = 0; i < mod->dynsym_count; i++) {
        const struct elf64_sym *sym = &mod->dynsym[i];
        if (sym->st_shndx == SHN_UNDEF) continue; /* imported, not defined here */
        const char *sym_name = mod->dynstr + sym->st_name;
        if (strcmp(sym_name, name) == 0) {
            /* sym->st_value is a vaddr offset from 0 in ET_DYN */
            return (void *)(mod->base_addr + sym->st_value);
        }
    }
    return NULL;
}

/* -----------------------------------------------------------------------
 * so_unload
 * -----------------------------------------------------------------------*/
void so_unload(so_module_t *mod) {
    serial_io_printf("Beginning unload\n");
    if (!mod) return;
    if (!mod->base_addr && !mod->image) {
        serial_io_printf("SO: so_unload called on already-unloaded module\n");
        return;
    }

    if (mod->base_addr && mod->size)
        uacpi_kernel_unmap((void *)mod->base_addr, mod->size);

    if (mod->image)
        free(mod->image);

    serial_io_printf("Memsetting the final touches of unload\n");
    memset(mod, 0, sizeof(so_module_t));
}