#include "mmap.h"
#include "interrupts/fd.h"
#include "gfx/serial_io.h"
#include "libs/string.h"
#include "mem/heap.h"
#include "mem/paging.h"
#include "vmm.h"

/* -----------------------------------------------------------------------
 * Global mapping table
 * -----------------------------------------------------------------------*/
static mmap_region_t mapping_table[MMAP_MAX_MAPPINGS];
static bool mmap_initialized = false;

/* -----------------------------------------------------------------------
 * Helper: align `value` up to the next multiple of MMAP_PAGE_SIZE
 * -----------------------------------------------------------------------*/
static inline size_t page_align_up(size_t value)
{
    return (value + MMAP_PAGE_SIZE - 1) & ~(MMAP_PAGE_SIZE - 1);
}

/* -----------------------------------------------------------------------
 * Helper: return PAGE_* flags that correspond to the given PROT_* flags
 *
 * mapPage() takes a uint8_t, so we use the low-byte page flags.
 * -----------------------------------------------------------------------*/
static inline uint8_t prot_to_page_flags(int prot)
{
    uint8_t flags = 0x01 | 0x04;          /* PAGE_PRESENT | PAGE_USER */
    if (prot & PROT_WRITE)
        flags |= 0x02;                     /* PAGE_WRITE */
    return flags;
}

/* -----------------------------------------------------------------------
 * Helper: allocate a free slot in the mapping table.
 * Returns NULL if the table is full.
 * -----------------------------------------------------------------------*/
static mmap_region_t *alloc_slot(void)
{
    for (int i = 0; i < MMAP_MAX_MAPPINGS; i++) {
        if (!mapping_table[i].used)
            return &mapping_table[i];
    }
    return NULL;
}

/* -----------------------------------------------------------------------
 * Helper: find the slot that contains `addr` within its range.
 * -----------------------------------------------------------------------*/
static mmap_region_t *find_slot(uintptr_t addr, size_t length)
{
    for (int i = 0; i < MMAP_MAX_MAPPINGS; i++) {
        mmap_region_t *r = &mapping_table[i];
        if (r->used && r->virt_addr == addr && r->length == length)
            return r;
    }
    return NULL;
}

/* -----------------------------------------------------------------------
 * mmap_init()
 * -----------------------------------------------------------------------*/
void mmap_init(void)
{
    memset(mapping_table, 0, sizeof(mapping_table));
    mmap_initialized = true;
}

/* -----------------------------------------------------------------------
 * mmap()  —  eager, no-fault
 * -----------------------------------------------------------------------*/
void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset)
{
    if (!mmap_initialized)
        return MAP_FAILED;

    /* Basic validation --------------------------------------------------*/
    if (length == 0)
        return MAP_FAILED;

    if ((flags & MAP_ANONYMOUS) && fd != MMAP_ANON_FD)
        return MAP_FAILED;

    if (!(flags & (MAP_PRIVATE | MAP_SHARED | MAP_ANONYMOUS)))
        return MAP_FAILED;

    if (offset & (MMAP_PAGE_SIZE - 1))   /* offset must be page-aligned */
        return MAP_FAILED;

    size_t aligned_len = page_align_up(length);
    size_t page_count  = aligned_len / MMAP_PAGE_SIZE;

    /* Allocate a slot ---------------------------------------------------*/
    mmap_region_t *slot = alloc_slot();
    if (!slot)
        return MAP_FAILED;

    /* Choose a virtual address -----------------------------------------*/
    uintptr_t base_virt;

    if (flags & MAP_FIXED) {
        base_virt = (uintptr_t)addr;
    } else {
        void *heap_block = malloc(aligned_len);
        if (!heap_block) {
            slot->used = false;
            return MAP_FAILED;
        }
        base_virt = (uintptr_t)heap_block;
    }

    /* Allocate page-aligned physical backing storage.
     * malloc is NOT physically contiguous, so we allocate each page
     * individually via vmm_alloc_page() and map them one by one. -----*/
    uintptr_t *phys_pages = malloc(page_count * sizeof(uintptr_t));
    if (!phys_pages) {
        if (!(flags & MAP_FIXED))
            free((void *)base_virt);
        slot->used = false;
        return MAP_FAILED;
    }
    memset(phys_pages, 0, page_count * sizeof(uintptr_t));

    void *backing_virt = NULL;  /* HHDM alias of page 0, for msync */

    for (size_t i = 0; i < page_count; i++) {
        void *virt_page = vmm_alloc_page();
        if (!virt_page) {
            /* Roll back already-allocated pages */
            for (size_t j = 0; j < i; j++)
                vmm_free_page((void *)phys_pages[j]);
            free(phys_pages);
            if (!(flags & MAP_FIXED))
                free((void *)base_virt);
            slot->used = false;
            return MAP_FAILED;
        }
        phys_pages[i] = vmm_virt_to_phys(virt_page);
        if (i == 0)
            backing_virt = virt_page;
    }

    /* Zero all backing pages — do it through the mapped virtual address --*/
    for (size_t i = 0; i < page_count; i++) {
        void *vpage = (void *)(base_virt + i * MMAP_PAGE_SIZE);
        mapPage(vpage, (void *)phys_pages[i], prot_to_page_flags(PROT_READ | PROT_WRITE));
    }

    /* Now zero through the mapped virtual address (guaranteed accessible) */
    memset((void *)base_virt, 0, aligned_len);

    /* File-backed: read content from the file into backing storage -----*/
    if (!(flags & MAP_ANONYMOUS)) {
        if (fd < 0 || fd >= MAX_FDS || fd_table[fd].type == FD_TYPE_NONE) {
            for (size_t i = 0; i < page_count; i++)
                vmm_free_page((void *)phys_pages[i]);
            free(phys_pages);
            if (!(flags & MAP_FIXED))
                free((void *)base_virt);
            slot->used = false;
            return MAP_FAILED;
        }

        /* Read directly from the fd's file_buffer at the given offset */
        size_t file_size   = fd_table[fd].file_size;
        size_t to_read     = aligned_len;
        if (offset < file_size) {
            size_t avail = file_size - offset;
            if (to_read > avail)
                to_read = avail;
        } else {
            to_read = 0;
        }

        if (to_read > 0) {
            memcpy((void *)base_virt,
                   fd_table[fd].file_buffer + offset,
                   to_read);
        }
    }

    /* Re-map with the actual requested protection ---------------------*/
    if (prot != (PROT_READ | PROT_WRITE)) {
        uint8_t pf = prot_to_page_flags(prot);
        for (size_t i = 0; i < page_count; i++) {
            void *vpage = (void *)(base_virt + i * MMAP_PAGE_SIZE);
            mapPage(vpage, (void *)phys_pages[i], pf);
        }
    }

    /* Populate the slot ------------------------------------------------*/
    slot->used        = true;
    slot->virt_addr   = base_virt;
    slot->phys_addr   = phys_pages[0];   /* first physical page */
    slot->phys_pages  = phys_pages;      /* array of per-page phys addrs */
    slot->backing_virt = (uintptr_t)backing_virt;  /* HHDM alias, msync */
    slot->backing_raw  = 0;
    slot->length      = aligned_len;
    slot->page_count  = page_count;
    slot->prot        = prot;
    slot->flags       = flags;
    slot->fd          = (flags & MAP_ANONYMOUS) ? -1 : fd;
    slot->file_offset = offset;
    slot->file_length = (flags & MAP_ANONYMOUS) ? 0 : aligned_len;

    return (void *)base_virt;
}

/* -----------------------------------------------------------------------
 * munmap()
 * -----------------------------------------------------------------------*/
int munmap(void *addr, size_t length)
{
    if (!mmap_initialized || !addr || length == 0)
        return -1;

    size_t aligned_len = page_align_up(length);
    mmap_region_t *slot = find_slot((uintptr_t)addr, aligned_len);
    if (!slot)
        return -1;

    /* Tear down page-table entries */
    unmapPages(addr, aligned_len);

    /* Free backing pages (one by one) ---------------------------------*/
    if (slot->phys_pages) {
        for (size_t i = 0; i < slot->page_count; i++) {
            if (slot->phys_pages[i])
                vmm_free_page((void *)slot->phys_pages[i]);
        }
        free(slot->phys_pages);
    }

    /* If the virtual address came from malloc, free it too -------------*/
    if (!(slot->flags & MAP_FIXED))
        free(addr);

    /* Clear the slot */
    memset(slot, 0, sizeof(mmap_region_t));

    return 0;
}

/* -----------------------------------------------------------------------
 * msync()
 * -----------------------------------------------------------------------*/
int msync(void *addr, size_t length)
{
    if (!mmap_initialized || !addr || length == 0)
        return -1;

    size_t aligned_len = page_align_up(length);
    mmap_region_t *slot = find_slot((uintptr_t)addr, aligned_len);
    if (!slot)
        return -1;

    /* Only MAP_SHARED file-backed mappings need a flush ----------------*/
    if ((slot->flags & MAP_SHARED) && slot->fd >= 0 && slot->fd < MAX_FDS) {
        file_descriptor_t *desc = &fd_table[slot->fd];

        size_t sync_len = slot->file_length;
        if (sync_len > desc->file_size) {
            desc->file_size = (uint32_t)sync_len;
        }

        /* Ensure fd buffer is large enough ----------------------------*/
        uint32_t need = (uint32_t)sync_len;
        if (!desc->file_buffer || need > desc->file_size) {
            uint8_t *new_buf = malloc(need);
            if (!new_buf)
                return -1;
            if (desc->file_buffer && desc->file_size > 0)
                memcpy(new_buf, desc->file_buffer, desc->file_size);
            if (desc->file_buffer)
                free(desc->file_buffer);
            desc->file_buffer = new_buf;
        }

        /* Copy from the mapped virtual address (fully kernel-mode) ----*/
        uint8_t *src = (uint8_t *)slot->virt_addr;
        memcpy(desc->file_buffer, src, sync_len);
        desc->modified = true;

        return fd_flush(slot->fd);
    }

    /* MAP_PRIVATE or MAP_ANONYMOUS — nothing to flush */
    return 0;
}

/* -----------------------------------------------------------------------
 * mprotect()
 * -----------------------------------------------------------------------*/
int mprotect(void *addr, size_t length, int prot)
{
    if (!mmap_initialized || !addr || length == 0)
        return -1;

    size_t aligned_len = page_align_up(length);
    mmap_region_t *slot = find_slot((uintptr_t)addr, aligned_len);
    if (!slot)
        return -1;

    slot->prot = prot;
    uint8_t pf = prot_to_page_flags(prot);

    /* Re-map every page with the new flags */
    for (size_t i = 0; i < slot->page_count; i++) {
        void *vpage = (void *)(slot->virt_addr + i * MMAP_PAGE_SIZE);
        void *ppage = (void *)slot->phys_pages[i];
        mapPage(vpage, ppage, pf);
    }

    return 0;
}

/* -----------------------------------------------------------------------
 * mmap_fork_copy()
 * -----------------------------------------------------------------------*/
int mmap_fork_copy(void)
{
    return 0;
}

/* -----------------------------------------------------------------------
 * mmap_dump()
 * -----------------------------------------------------------------------*/
void mmap_dump(void)
{
    serial_io_printf("[mmap_dump] active mappings:\n");
    int count = 0;
    for (int i = 0; i < MMAP_MAX_MAPPINGS; i++) {
        mmap_region_t *r = &mapping_table[i];
        if (r->used) {
            serial_io_printf("  [%d] virt=0x%lx phys=0x%lx len=%zu "
                             "prot=%d flags=%d fd=%d off=%zu\n",
                             i,
                             (unsigned long)r->virt_addr,
                             (unsigned long)r->phys_addr,
                             r->length,
                             r->prot,
                             r->flags,
                             r->fd,
                             r->file_offset);
            count++;
        }
    }
    serial_io_printf("[mmap_dump] total: %d\n", count);
}
