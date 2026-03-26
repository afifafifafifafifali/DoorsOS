#ifndef MMAP_H
#define MMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// THIS IS MOSTLY CODE STOLEN FROM https://github.com/64/ByteOS and Managarm
typedef uint64_t uacpi_size;
typedef int64_t off_t;

// Page size
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

// Protection flags
#define PROT_NONE   0x0
#define PROT_READ   0x1
#define PROT_WRITE  0x2
#define PROT_EXEC   0x4

// Mapping flags
#define MAP_PRIVATE     0x0
#define MAP_SHARED      0x1
#define MAP_ANONYMOUS   0x20
#define MAP_FIXED       0x10
#define MAP_POPULATE    0x80

// Return value for failed mmap
#define MAP_FAILED  ((void*)-1)

// msync flags
#define MS_ASYNC    0x1
#define MS_SYNC     0x4
#define MS_INVALIDATE 0x2

// Mapped region information
typedef struct {
    void* addr;         // Mapping address
    size_t length;      // Length in bytes
    int prot;           // Protection flags
    int flags;          // Mapping flags
    bool file_backed;   // Is this a file-backed mapping
    uint32_t file_size; // Original file size (for file-backed)
} mmap_info_t;

// Initialize mmap subsystem
void mmap_init(void);

// Allocate anonymous memory mapping
// Returns: Virtual address of mapping, or NULL on failure
void* kmmap(size_t length, int prot, int flags);

// Unmap memory region
void kmunmap(void* addr, size_t length);

// Map a file into memory
// Parameters:
//   filename  - Path to the file (e.g., "/boot/file.bin")
//   prot      - Protection flags (PROT_READ, PROT_WRITE, etc.)
//   flags     - Mapping flags (MAP_PRIVATE, MAP_SHARED)
//   out_size  - Optional: receives the actual file size
// Returns: Virtual address of mapping, or NULL on failure
void* kmmap_file(const char* filename, int prot, int flags, uint32_t* out_size);

// Map a file descriptor into memory (Linux-style mmap)
// Parameters:
//   fd        - File descriptor to map
//   addr      - Hint for address (NULL for kernel choice)
//   length    - Length to map
//   prot      - Protection flags
//   flags     - Mapping flags (MAP_SHARED, MAP_PRIVATE)
//   offset    - Offset within file
// Returns: Mapped address, or MAP_FAILED on error
void* fd_mmap(int fd, void* addr, size_t length, int prot, int flags, off_t offset);

// Synchronize mapped region with underlying file (msync)
// Parameters:
//   addr      - Start address of mapped region
//   length    - Length to sync
//   flags     - Sync flags (MS_SYNC, MS_ASYNC, MS_INVALIDATE)
// Returns: 0 on success, -1 on failure
int fd_msync(void* addr, size_t length, int flags);

// Get the actual file size from a file-backed mapping
// Returns: File size in bytes, or 0 if not a file-backed mapping
uint32_t kmmap_get_file_size(void* addr);

// Read entire file into newly allocated memory
// This is a convenience wrapper around kmmap_file
// Parameters:
//   filename  - Path to the file
//   out_size  - Receives the file size
// Returns: Pointer to file content, or NULL on failure
// Note: Caller must free using kfree_file() when done
void* kread_file(const char* filename, uint32_t* out_size);

// Free memory allocated by kread_file or kmmap_file
// Parameters:
//   addr      - Address returned by kmmap_file or kread_file
void kfree_file(void* addr);

// Get information about a mapped region
// Returns: true if region found, false otherwise
bool kmmap_get_info(void* addr, mmap_info_t* info);

// Test function
void test_mmap_basic(void);

#endif // MMAP_H
