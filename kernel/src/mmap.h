#ifndef MMAP_H
#define MMAP_H

/*
 * mmap.h — POSIX-style memory mapping for doorsos
 *
 * Eager implementation: all pages are allocated and populated at mmap() time.
 * No page faults. No lazy loading. Pages are wired immediately.
 *
 * Supports:
 *   MAP_ANONYMOUS  — zero-filled pages, not backed by any file
 *   MAP_PRIVATE    — file-backed, copy-on-mmap (COW at map time, not fault time)
 *   MAP_SHARED     — file-backed, writes go back to fd buffer (msync flushes to FAT32)
 *   MAP_FIXED      — use addr hint exactly (no alignment rounding of the hint)
 *
 * Integration points:
 *   - Physical memory : malloc() / free()  (heap.h)
 *   - Virtual wiring  : mapPage()          (paging.h)
 *   - File I/O        : fd_read/seek/size  (fd.h)
 *   - Unmap           : unmapPages()       (paging.h)
 *   - Flush           : fd_flush()         (fd.h)
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------
 * PROT flags  (match POSIX values so userspace headers won't clash)
 * -----------------------------------------------------------------------*/
#define PROT_NONE   0x00   /* pages cannot be accessed              */
#define PROT_READ   0x01   /* pages can be read                     */
#define PROT_WRITE  0x02   /* pages can be written                  */
#define PROT_EXEC   0x04   /* pages can be executed                 */

/* -----------------------------------------------------------------------
 * MAP flags
 * -----------------------------------------------------------------------*/
#define MAP_SHARED      0x01   /* share changes (msync writes back)     */
#define MAP_PRIVATE     0x02   /* private copy, changes not written back */
#define MAP_ANONYMOUS   0x20   /* not backed by a file, fd must be -1   */
#define MAP_ANON        MAP_ANONYMOUS
#define MAP_FIXED       0x10   /* place mapping at exact addr           */

/* -----------------------------------------------------------------------
 * Sentinel for failure (like (void*)-1 on Linux)
 * -----------------------------------------------------------------------*/
#define MAP_FAILED  ((void *)(-1ULL))

/* -----------------------------------------------------------------------
 * fd value for anonymous mappings
 * -----------------------------------------------------------------------*/
#define MMAP_ANON_FD   (-1)

/* -----------------------------------------------------------------------
 * Internal: one entry in the global mapping table
 * -----------------------------------------------------------------------*/
#define MMAP_MAX_MAPPINGS   256
#define MMAP_PAGE_SIZE      0x1000   /* 4 KiB */

typedef struct {
    bool        used;

    uintptr_t   virt_addr;    /* base virtual address of the mapping    */
    uintptr_t   phys_addr;    /* physical address of first page         */
    uintptr_t  *phys_pages;   /* malloc'd array of per-page phys addrs  */
    uintptr_t   backing_virt; /* HHDM alias of page 0 (for msync)       */
    uintptr_t   backing_raw;  /* unused (kept for compat)               */
    size_t      length;       /* mapping length in bytes (page-aligned) */
    size_t      page_count;   /* number of 4 KiB pages                  */

    int         prot;         /* PROT_* flags                           */
    int         flags;        /* MAP_* flags                            */



    /* File-backed fields (ignored for MAP_ANONYMOUS) */
    int         fd;           /* file descriptor (-1 for anon)          */
    size_t      file_offset;  /* byte offset into the file              */
    size_t      file_length;  /* how many file bytes were mapped        */
} mmap_region_t;

/* -----------------------------------------------------------------------
 * Public API
 * -----------------------------------------------------------------------*/

/*
 * mmap_init()
 *   Must be called once during kernel init before any mmap() call.
 */
void mmap_init(void);

/*
 * mmap()
 *   Create a new memory mapping.
 *
 *   addr   - hint for virtual address (0 = let allocator choose)
 *   length - number of bytes to map (rounded up to page boundary)
 *   prot   - PROT_READ | PROT_WRITE | PROT_EXEC | PROT_NONE
 *   flags  - MAP_SHARED | MAP_PRIVATE | MAP_ANONYMOUS [| MAP_FIXED]
 *   fd     - open file descriptor, or -1 for MAP_ANONYMOUS
 *   offset - byte offset into the file (must be page-aligned)
 *
 *   Returns: virtual address of mapping, or MAP_FAILED on error.
 */
void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset);

/*
 * munmap()
 *   Remove a mapping created by mmap().
 *   addr + length must exactly match a previously returned mapping.
 *
 *   Returns 0 on success, -1 on error.
 */
int munmap(void *addr, size_t length);

/*
 * msync()
 *   For MAP_SHARED file-backed mappings: copy the mapped memory back into
 *   the fd buffer and flush it to FAT32.
 *   For MAP_PRIVATE or MAP_ANONYMOUS: no-op (returns 0).
 *
 *   Returns 0 on success, -1 on error.
 */
int msync(void *addr, size_t length);

/*
 * mprotect()
 *   Change the protection flags on an existing mapping.
 *   Remaps each page with the new PAGE_* attributes.
 *
 *   Returns 0 on success, -1 on error.
 */
int mprotect(void *addr, size_t length, int prot);

/*
 * mmap_fork_copy()
 *   Called by fork: eagerly duplicates every mapping owned by the parent
 *   into fresh physical pages.  The child's mappings start as independent
 *   copies regardless of MAP_SHARED / MAP_PRIVATE — matching your "eager,
 *   no-fault" philosophy.
 *
 *   Returns 0 on success, -1 if any allocation fails.
 */
int mmap_fork_copy(void);

/*
 * mmap_dump()
 *   Debug: print all active mappings to serial.
 */
void mmap_dump(void);

#endif /* MMAP_H */