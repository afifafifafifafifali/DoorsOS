#include "mmap.h"
#include "physalloc.h"
#include "mem/paging.h"
#include "bootloader.h"
#include "libs/string.h"
#include "gfx/serial_io.h"
#include "fs/fat32.h"
#include "interrupts/fd.h"
#include "mem/heap.h"
#include "interrupts/pipe.h"
#include "uacpi/types.h"
extern volatile struct limine_hhdm_request hhdm_request;

// Page size constant
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

// Round up to page boundary
#define PAGE_ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))


#define MAX_MMAP_REGIONS 256

typedef struct {
    void* virt_addr;      
    uint64_t phys_base;   
    size_t length;        
    int prot;             
    int flags;            
    bool in_use;          
    bool file_backed;     
    bool fd_backed;       
    int fd;               
    void* file_data;      
    uint32_t file_size;  
} mmap_region_t;

static mmap_region_t mmap_regions[MAX_MMAP_REGIONS];
static bool mmap_initialized = false;
static uint64_t mmap_virt_next = 0x0000000080000000;  // Global mmap virtual address allocator

// Convert protection flags to page table flags
static uint8_t prot_to_page_flags(int prot) {
    uint8_t flags = PAGE_PRESENT;

    if (prot & PROT_WRITE) {
        flags |= PAGE_WRITE;
    }

    // User accessible by default for mmap regions
    flags |= PAGE_USER;

    if (!(prot & PROT_EXEC)) {
        flags |= PAGE_NO_EXECUTE;
    }

    return flags;
}


static void mmap_map_pages(void* virt, uacpi_phys_addr phys, size_t len, uint8_t flags) {
    uintptr_t hhdm = hhdm_request.response->offset;
    uintptr_t virt_addr = (uintptr_t)virt & ~0xFFFULL;  // Align down
    uintptr_t phys_addr = phys & ~0xFFFULL;              // Align down
    
    size_t num_pages = (len + PAGE_SIZE - 1) / PAGE_SIZE;

    serial_io_printf("mmap_map_pages: virt=0x%llx phys=0x%llx len=%zu pages=%zu\n",
                     virt_addr, phys_addr, len, num_pages);

    for (size_t i = 0; i < num_pages; i++) {
        void* vaddr = (void*)(virt_addr + (i * PAGE_SIZE));
        void* paddr = (void*)(phys_addr + (i * PAGE_SIZE));

        uint64_t pml4_index = ((uintptr_t)vaddr >> 39) & 0x1FF;
        uint64_t pdpt_index = ((uintptr_t)vaddr >> 30) & 0x1FF;
        uint64_t pd_index = ((uintptr_t)vaddr >> 21) & 0x1FF;
        uint64_t pt_index = ((uintptr_t)vaddr >> 12) & 0x1FF;

        serial_io_printf("  page %zu: vaddr=%p paddr=%p indices PML4=%lu PDPT=%lu PD=%lu PT=%lu\n",
                         i, vaddr, paddr, pml4_index, pdpt_index, pd_index, pt_index);

        // Check/create PML4 entry
        if (!pml4->entries[pml4_index].present) {
            uint64_t pdpt_phys = phys_alloc_page();
            if (!pdpt_phys) {
                serial_io_printf("mmap_map_pages: Failed to allocate PDPT\n");
                return;
            }
            memset((void*)(pdpt_phys + hhdm), 0, PAGE_SIZE);
            pml4->entries[pml4_index].present = 1;
            pml4->entries[pml4_index].writable = 1;
            pml4->entries[pml4_index].user_accessible = 1;
            pml4->entries[pml4_index].physical_address = pdpt_phys >> 12;
            serial_io_printf("  Allocated new PDPT at phys 0x%llx\n", pdpt_phys);
        }

        PageTable* pdpt = (PageTable*)((pml4->entries[pml4_index].physical_address << 12) | hhdm);

        // Check/create PDPT entry
        if (!pdpt->entries[pdpt_index].present) {
            uint64_t pd_phys = phys_alloc_page();
            if (!pd_phys) {
                serial_io_printf("mmap_map_pages: Failed to allocate PD\n");
                return;
            }
            memset((void*)(pd_phys + hhdm), 0, PAGE_SIZE);
            pdpt->entries[pdpt_index].present = 1;
            pdpt->entries[pdpt_index].writable = 1;
            pdpt->entries[pdpt_index].user_accessible = 1;
            pdpt->entries[pdpt_index].physical_address = pd_phys >> 12;
            serial_io_printf("  Allocated new PD at phys 0x%llx\n", pd_phys);
        }

        PageTable* pd = (PageTable*)((pdpt->entries[pdpt_index].physical_address << 12) | hhdm);

        // Check/create PD entry
        if (!pd->entries[pd_index].present) {
            uint64_t pt_phys = phys_alloc_page();
            if (!pt_phys) {
                serial_io_printf("mmap_map_pages: Failed to allocate PT\n");
                return;
            }
            memset((void*)(pt_phys + hhdm), 0, PAGE_SIZE);
            pd->entries[pd_index].present = 1;
            pd->entries[pd_index].writable = 1;
            pd->entries[pd_index].user_accessible = 1;
            pd->entries[pd_index].physical_address = pt_phys >> 12;
            serial_io_printf("  Allocated new PT at phys 0x%llx\n", pt_phys);
        }

        PageTable* pt = (PageTable*)((pd->entries[pd_index].physical_address << 12) | hhdm);

        // Set PT entry
        pt->entries[pt_index].present = (flags >> 0) & 1;
        pt->entries[pt_index].writable = (flags >> 1) & 1;
        pt->entries[pt_index].user_accessible = (flags >> 2) & 1;
        pt->entries[pt_index].physical_address = ((uintptr_t)paddr) >> 12;
        
        serial_io_printf("  Mapped PT entry: present=%d writable=%d user=%d phys=%llx\n",
                         pt->entries[pt_index].present,
                         pt->entries[pt_index].writable,
                         pt->entries[pt_index].user_accessible,
                         pt->entries[pt_index].physical_address);
    }

    // Flush TLB for entire range
    for (size_t i = 0; i < num_pages; i++) {
        __asm__ volatile ("invlpg (%0)" :: "r" (virt_addr + (i * PAGE_SIZE)) : "memory");
    }
    
    serial_io_printf("mmap_map_pages: done\n");
}

// Unmap a page in the mmap region
static void mmap_unmap_page(void* virt) {
    uintptr_t hhdm = hhdm_request.response->offset;
    uintptr_t virt_addr = (uintptr_t)virt;

    uint64_t pml4_index = (virt_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = (virt_addr >> 30) & 0x1FF;
    uint64_t pd_index = (virt_addr >> 21) & 0x1FF;
    uint64_t pt_index = (virt_addr >> 12) & 0x1FF;

    if (!pml4->entries[pml4_index].present) return;
    PageTable* pdpt = (PageTable*)((pml4->entries[pml4_index].physical_address << 12) | hhdm);

    if (!pdpt->entries[pdpt_index].present) return;
    PageTable* pd = (PageTable*)((pdpt->entries[pdpt_index].physical_address << 12) | hhdm);

    if (!pd->entries[pd_index].present) return;
    PageTable* pt = (PageTable*)((pd->entries[pd_index].physical_address << 12) | hhdm);

    // Clear PT entry
    pt->entries[pt_index].present = 0;
    pt->entries[pt_index].writable = 0;
    pt->entries[pt_index].user_accessible = 0;
    pt->entries[pt_index].physical_address = 0;

    // Flush TLB
    __asm__ volatile ("invlpg (%0)" :: "r" (virt) : "memory");
}


static void mmap_unmap_pages(void* virt, size_t length) {
    size_t page_count = (length + PAGE_SIZE - 1) / PAGE_SIZE;
    for (size_t i = 0; i < page_count; i++) {
        mmap_unmap_page((void*)((uintptr_t)virt + i * PAGE_SIZE));
    }
}


void mmap_init(void) {
    if (mmap_initialized) {
        return;
    }
    
    memset(mmap_regions, 0, sizeof(mmap_regions));
    mmap_initialized = true;
    
    serial_io_printf("MMAP: Initialized with %d region slots\n", MAX_MMAP_REGIONS);
}

// Find a free region slot
static int find_free_region(void) {
    for (int i = 0; i < MAX_MMAP_REGIONS; i++) {
        if (!mmap_regions[i].in_use) {
            return i;
        }
    }
    return -1;
}

// Find region by virtual address
static int find_region_by_addr(void* addr) {
    for (int i = 0; i < MAX_MMAP_REGIONS; i++) {
        if (mmap_regions[i].in_use && mmap_regions[i].virt_addr == addr) {
            return i;
        }
    }
    return -1;
}


static void* mmap_anonymous(size_t length, int prot, int flags) {
  
    size_t aligned_length = PAGE_ALIGN_UP(length);
    size_t page_count = aligned_length / PAGE_SIZE;

    serial_io_printf("MMAP anonymous: requesting %zu bytes (%zu pages)\n",
                     aligned_length, page_count);

    // Allocate physical pages directly from PMM (not HHDM-mapped)
    uint64_t phys_base = phys_alloc_pages(page_count);
    if (phys_base == 0) {
        serial_io_printf("MMAP: Failed to allocate %zu pages\n", page_count);
        return NULL;
    }

    serial_io_printf("MMAP anonymous: physical base 0x%llx\n", phys_base);

   
    uint64_t virt_base = mmap_virt_next;
    mmap_virt_next += aligned_length;

  
    uint8_t page_flags = prot_to_page_flags(prot);
    mmap_map_pages((void*)virt_base, phys_base, aligned_length, page_flags);

    // Zero the memory (anonymous mappings should be zeroed)
    memset((void*)virt_base, 0, aligned_length);

    // Find a free region slot
    int region_idx = find_free_region();
    if (region_idx < 0) {
        serial_io_printf("MMAP: No free region slots!\n");
        // Free the allocation
        phys_free_pages(phys_base, page_count);
        return NULL;
    }

    // Record the region
    mmap_regions[region_idx].virt_addr = (void*)virt_base;
    mmap_regions[region_idx].phys_base = phys_base;
    mmap_regions[region_idx].length = aligned_length;
    mmap_regions[region_idx].prot = prot;
    mmap_regions[region_idx].flags = flags | MAP_ANONYMOUS;
    mmap_regions[region_idx].in_use = true;
    mmap_regions[region_idx].file_backed = false;
    mmap_regions[region_idx].fd_backed = false;
    mmap_regions[region_idx].fd = -1;
    mmap_regions[region_idx].file_data = NULL;

    serial_io_printf("MMAP anonymous: recorded in region %d at %p\n", region_idx, (void*)virt_base);

    return (void*)virt_base;
}


void* kmmap(size_t length, int prot, int flags) {
    if (!mmap_initialized) {
        mmap_init();
    }
    
    if (length == 0) {
        serial_io_printf("MMAP: Invalid length (0)\n");
        return NULL;
    }
    
    // Check if this is an anonymous mapping
    if (flags & MAP_ANONYMOUS) {
        return mmap_anonymous(length, prot, flags);
    }
    
    // For file-backed mappings, user should use kmmap_file()
    serial_io_printf("MMAP: File-backed mapping requires kmmap_file()\n");
    return NULL;
}

// Unmap memory region
void kmunmap(void* addr, size_t length) {
    if (!addr || length == 0) {
        return;
    }

    int region_idx = find_region_by_addr(addr);
    if (region_idx < 0) {
        serial_io_printf("MMAP: Region not found for addr %p\n", addr);
        return;
    }

    mmap_region_t* region = &mmap_regions[region_idx];

    serial_io_printf("MMAP unmapping: %p, length %zu\n", addr, region->length);

    // Calculate page count
    size_t page_count = region->length / PAGE_SIZE;
    uint64_t phys_to_free = region->phys_base;
    // Unmap pages from page tables
    mmap_unmap_pages(addr, region->length);

    // For fd-backed mappings, we don't free the underlying data
    // (it's managed by the fd system)
    if (!region->fd_backed) {
        // Free physical pages only for non-fd-backed mappings
        phys_free_pages(region->phys_base, page_count);
    }

    // Clear region
    region->in_use = false;
    region->virt_addr = NULL;
    region->phys_base = 0;
    region->length = 0;
    region->prot = 0;
    region->flags = 0;
    region->file_backed = false;
    region->fd_backed = false;
    region->file_data = NULL;
    region->fd = -1;

    serial_io_printf("MMAP unmapped: freed %zu pages at phys 0x%llx\n", page_count, phys_to_free);
}

// Map a file into memory
void* kmmap_file(const char* filename, int prot, int flags, uint32_t* out_size) {
    if (!mmap_initialized) {
        mmap_init();
    }

    if (!filename) {
        serial_io_printf("MMAP file: NULL filename\n");
        return NULL;
    }

    serial_io_printf("MMAP file: Opening '%s'\n", filename);

    // Find a free region slot first
    int region_idx = find_free_region();
    if (region_idx < 0) {
        serial_io_printf("MMAP file: No free region slots\n");
        return NULL;
    }

    // Use fat32_read_file to get size
    uint8_t* size_buffer = malloc(69699);  // Dummy buffer to get size
    uint32_t size = 0;

    if (!fat32_read_file(filename, size_buffer, &size)) {
        serial_io_printf("MMAP file: Failed to read file '%s'\n", filename);
        return NULL;
    }

    uint32_t file_size = size;
    serial_io_printf("MMAP file: File size is %u bytes\n", file_size);

    if (file_size == 0) {
        serial_io_printf("MMAP file: Empty file\n");
        if (out_size) *out_size = 0;
        return NULL;
    }

    // Align file size to page boundary for mapping
    size_t aligned_size = PAGE_ALIGN_UP(file_size);
    size_t page_count = aligned_size / PAGE_SIZE;

    serial_io_printf("MMAP file: Need %zu bytes (%zu pages)\n",
                     aligned_size, page_count);

    // Allocate physical pages directly from PMM
    uint64_t phys_base = phys_alloc_pages(page_count);
    if (phys_base == 0) {
        serial_io_printf("MMAP file: Failed to allocate pages\n");
        return NULL;
    }

    // Allocate virtual address from global pool
    uint64_t virt_base = mmap_virt_next;
    mmap_virt_next += aligned_size;

    serial_io_printf("MMAP file: Allocated phys 0x%llx, mapping at virt 0x%llx\n",
                     phys_base, virt_base);

    // Map pages with protection flags
    uint8_t page_flags = prot_to_page_flags(prot);
    mmap_map_pages((void*)virt_base, phys_base, aligned_size, page_flags);

    // Read file content into mapped memory
    serial_io_printf("MMAP file: Reading file content...\n");

    uint32_t bytes_read = file_size;
    if (!fat32_read_file(filename, (void*)virt_base, &bytes_read)) {
        serial_io_printf("MMAP file: Failed to read file content\n");
        // Free allocation
        phys_free_pages(phys_base, page_count);
        return NULL;
    }

    serial_io_printf("MMAP file: Read %u bytes\n", bytes_read);

    // Zero-pad remaining bytes in last page
    if (bytes_read < aligned_size) {
        memset((void*)(virt_base + bytes_read), 0, aligned_size - bytes_read);
    }

    // Record the region
    mmap_regions[region_idx].virt_addr = (void*)virt_base;
    mmap_regions[region_idx].phys_base = phys_base;
    mmap_regions[region_idx].length = aligned_size;
    mmap_regions[region_idx].prot = prot;
    mmap_regions[region_idx].flags = flags;
    mmap_regions[region_idx].in_use = true;
    mmap_regions[region_idx].file_backed = true;
    mmap_regions[region_idx].fd_backed = false;
    mmap_regions[region_idx].fd = -1;
    mmap_regions[region_idx].file_data = (void*)virt_base;
    mmap_regions[region_idx].file_size = file_size;

    serial_io_printf("MMAP file: Mapped to %p (region %d)\n", (void*)virt_base, region_idx);

    if (out_size) {
        *out_size = file_size;
    }

    return (void*)virt_base;
}

// Get the actual file size from a file-backed mapping
uint32_t kmmap_get_file_size(void* addr) {
    int region_idx = find_region_by_addr(addr);
    if (region_idx < 0 || !mmap_regions[region_idx].file_backed) {
        return 0;
    }
    return mmap_regions[region_idx].file_size;
}

// Map a file descriptor into memory (Linux-style mmap)
void* fd_mmap(int fd, void* addr, size_t length, int prot, int flags, off_t offset) {
    (void)addr;  // Address hint not supported yet
    
    if (!mmap_initialized) {
        mmap_init();
    }

    if (!fd_is_valid(fd)) {
        serial_io_printf("fd_mmap: invalid fd=%d\n", fd);
        return MAP_FAILED;
    }

    file_descriptor_t* desc = &fd_table[fd];
    
    serial_io_printf("fd_mmap: mapping fd=%d, type=%d, length=%zu\n", fd, desc->type, length);

    // Find a free region slot
    int region_idx = find_free_region();
    if (region_idx < 0) {
        serial_io_printf("fd_mmap: no free region slots\n");
        return MAP_FAILED;
    }

    // Handle different fd types
    if (desc->type == FD_TYPE_FILE) {
        // For regular files, map the file buffer
        if (desc->file_buffer == NULL || desc->file_size == 0) {
            serial_io_printf("fd_mmap: file has no content\n");
            return MAP_FAILED;
        }

        // Adjust length if not specified or if it exceeds file size
        if (length == 0) {
            length = desc->file_size;
        }

        // Align to page boundary
        size_t aligned_length = PAGE_ALIGN_UP(length);
        size_t page_count = aligned_length / PAGE_SIZE;

        serial_io_printf("fd_mmap: file size=%u, mapping %zu bytes (%zu pages)\n", 
                         desc->file_size, aligned_length, page_count);

        // Allocate physical pages directly from PMM
        uint64_t phys_base = phys_alloc_pages(page_count);
        if (phys_base == 0) {
            serial_io_printf("fd_mmap: failed to allocate pages\n");
            return MAP_FAILED;
        }

        // Allocate virtual address from global pool
        uint64_t virt_base = mmap_virt_next;
        mmap_virt_next += aligned_length;

        serial_io_printf("fd_mmap: allocated phys 0x%llx, mapping at virt 0x%llx\n",
                         phys_base, virt_base);

        // Map pages with protection flags
        uint8_t page_flags = prot_to_page_flags(prot);
        mmap_map_pages((void*)virt_base, phys_base, aligned_length, page_flags);

        // Copy file content to mapped memory
        size_t copy_size = (length < desc->file_size) ? length : desc->file_size;
        memcpy((void*)virt_base, desc->file_buffer + offset, copy_size);

        // Zero-pad if needed
        if (copy_size < aligned_length) {
            memset((void*)(virt_base + copy_size), 0, aligned_length - copy_size);
        }

        // Record the region
        mmap_regions[region_idx].virt_addr = (void*)virt_base;
        mmap_regions[region_idx].phys_base = phys_base;
        mmap_regions[region_idx].length = aligned_length;
        mmap_regions[region_idx].prot = prot;
        mmap_regions[region_idx].flags = flags;
        mmap_regions[region_idx].in_use = true;
        mmap_regions[region_idx].file_backed = false;
        mmap_regions[region_idx].fd_backed = true;
        mmap_regions[region_idx].fd = fd;
        mmap_regions[region_idx].file_data = desc->file_buffer;  // Reference to fd's buffer
        mmap_regions[region_idx].file_size = desc->file_size;

        serial_io_printf("fd_mmap: mapped to %p (region %d)\n", (void*)virt_base, region_idx);
        return (void*)virt_base;

    } else if (desc->type == FD_TYPE_PIPE_READ || desc->type == FD_TYPE_PIPE_WRITE) {
        // Pipes cannot be memory mapped in the traditional sense
        // They are streaming interfaces, not seekable storage
        serial_io_printf("fd_mmap: pipes cannot be memory mapped\n");
        return MAP_FAILED;
    }

    serial_io_printf("fd_mmap: unsupported fd type %d\n", desc->type);
    return MAP_FAILED;
}

// Synchronize mapped region with underlying file (msync equivalent)
int fd_msync(void* addr, size_t length, int flags) {
    (void)length;
    (void)flags;
    
    int region_idx = find_region_by_addr(addr);
    if (region_idx < 0) {
        return -1;
    }

    mmap_region_t* region = &mmap_regions[region_idx];
    
    if (!region->fd_backed) {
        return 0;  // Nothing to sync for anonymous mappings
    }

    // For fd-backed mappings, copy data back to the fd's buffer
    if (region->prot & PROT_WRITE) {
        file_descriptor_t* desc = &fd_table[region->fd];
        if (desc && desc->type == FD_TYPE_FILE) {
            size_t copy_size = (region->file_size < region->length) ? 
                               region->file_size : region->length;
            memcpy(desc->file_buffer, addr, copy_size);
            desc->modified = true;
            serial_io_printf("fd_msync: synced %zu bytes to fd=%d\n", copy_size, region->fd);
        }
    }

    return 0;
}

// Test function for mmap
void test_mmap_basic(void) {
    serial_io_printf("\n=== MMAP Basic Test ===\n");

    mmap_init();

    // Test 1: Anonymous mapping
    serial_io_printf("\nTest 1: Anonymous mapping\n");
    void* anon_map = kmmap(PAGE_SIZE * 2, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE);
    if (anon_map) {
        serial_io_printf("  Allocated anonymous mapping at %p\n", anon_map);

        // Write some data
        uint64_t* ptr = (uint64_t*)anon_map;
        ptr[0] = 0xDEADBEEFCAFEBABEULL;
        ptr[1] = 0x1234567890ABCDEFULL;

        serial_io_printf("  Wrote test data: 0x%llx, 0x%llx\n", ptr[0], ptr[1]);

        // Verify
        if (ptr[0] == 0xDEADBEEFCAFEBABEULL && ptr[1] == 0x1234567890ABCDEFULL) {
            serial_io_printf("  PASS: Data verification\n");
        } else {
            serial_io_printf("  FAIL: Data verification\n");
        }

        // Unmap
        kmunmap(anon_map, PAGE_SIZE * 2);
        serial_io_printf("  Unmapped anonymous mapping\n");
    } else {
        serial_io_printf("  FAIL: Anonymous mapping allocation\n");
    }

    // Test 2: File mapping (if filesystem is available)
    serial_io_printf("\nTest 2: File mapping\n");
    uint32_t file_size = 0;
    void* file_map = kmmap_file("/test.txt", PROT_READ | PROT_WRITE, MAP_PRIVATE, &file_size);
    if (file_map) {
        serial_io_printf("  Mapped file at %p, size %u bytes\n", file_map, file_size);

        // Print first few bytes as string (if text file)
        char* str = (char*)file_map;
        serial_io_printf("  File content (first 64 bytes): ");
        for (int i = 0; i < 64 && i < file_size; i++) {
            if (str[i] >= 32 && str[i] <= 126) {
                serial_io_printf("%c", str[i]);
            } else {
                serial_io_printf(".");
            }
        }
        serial_io_printf("\n");

        // Unmap
        kmunmap(file_map, PAGE_ALIGN_UP(file_size));
        serial_io_printf("  Unmapped file mapping\n");
    } else {
        serial_io_printf("  INFO: File mapping not available (file not found or FS not ready)\n");
    }

    // Test 3: fd_mmap - map file via file descriptor
    serial_io_printf("\nTest 3: fd_mmap (file descriptor mapping)\n");
    int fd = fd_open("/test.txt", O_RDONLY);
    if (fd >= 0) {
        serial_io_printf("  Opened /test.txt as fd=%d\n", fd);

        void* fd_map = fd_mmap(fd, NULL, 0, PROT_READ | PROT_WRITE, MAP_PRIVATE, 0);
        if (fd_map != MAP_FAILED) {
            serial_io_printf("  fd_mmap succeeded at %p\n", fd_map);

            // Print content
            char* str = (char*)fd_map;
            serial_io_printf("  File content: ");
            for (int i = 0; i < 64 && str[i] != 0; i++) {
                if (str[i] >= 32 && str[i] <= 126) {
                    serial_io_printf("%c", str[i]);
                } else {
                    serial_io_printf(".");
                }
            }
            serial_io_printf("\n");

            // Unmap
            kmunmap(fd_map, PAGE_SIZE);
            serial_io_printf("  Unmapped fd mapping\n");
        } else {
            serial_io_printf("  FAIL: fd_mmap returned MAP_FAILED\n");
        }

        fd_close(fd);
        serial_io_printf("  Closed fd\n");
    } else {
        serial_io_printf("  INFO: Could not open file for fd_mmap test\n");
    }

    // Test 4: Pipe cannot be mmap'd
    serial_io_printf("\nTest 4: Pipe mmap (should fail)\n");
    int pipefd[2];
    if (fd_pipe(pipefd) == 0) {
        serial_io_printf("  Created pipe: read_fd=%d, write_fd=%d\n", pipefd[0], pipefd[1]);
        
        void* pipe_map = fd_mmap(pipefd[0], NULL, 0, PROT_READ, MAP_PRIVATE, 0);
        if (pipe_map == MAP_FAILED) {
            serial_io_printf("  PASS: Pipe mmap correctly failed\n");
        } else {
            serial_io_printf("  FAIL: Pipe mmap should have failed\n");
            if (pipe_map) kmunmap(pipe_map, PAGE_SIZE);
        }
        
        fd_close(pipefd[0]);
        fd_close(pipefd[1]);
    } else {
        serial_io_printf("  INFO: Could not create pipe\n");
    }

    serial_io_printf("=== MMAP Test End ===\n\n");
}

// Read entire file into newly allocated memory (convenience wrapper)
void* kread_file(const char* filename, uint32_t* out_size) {
    return kmmap_file(filename, PROT_READ | PROT_WRITE, MAP_PRIVATE, out_size);
}

// Free memory allocated by kmmap_file or kread_file
void kfree_file(void* addr) {
    if (!addr) {
        return;
    }

    int region_idx = find_region_by_addr(addr);
    if (region_idx < 0) {
        serial_io_printf("MMAP free: Region not found for %p\n", addr);
        return;
    }

    mmap_region_t* region = &mmap_regions[region_idx];
    size_t page_count = region->length / PAGE_SIZE;

    // Unmap pages
    mmap_unmap_pages(addr, region->length);

    // Free physical pages
    phys_free_pages(region->phys_base, page_count);

    // Clear region
    region->in_use = false;
    region->virt_addr = NULL;
    region->phys_base = 0;
    region->length = 0;
    region->prot = 0;
    region->flags = 0;
    region->file_backed = false;
    region->fd_backed = false;
    region->file_data = NULL;
    region->file_size = 0;
}

// Get information about a mapped region
bool kmmap_get_info(void* addr, mmap_info_t* info) {
    if (!addr || !info) {
        return false;
    }
    
    int region_idx = find_region_by_addr(addr);
    if (region_idx < 0) {
        return false;
    }
    
    mmap_region_t* region = &mmap_regions[region_idx];
    
    info->addr = region->virt_addr;
    info->length = region->length;
    info->prot = region->prot;
    info->flags = region->flags;
    info->file_backed = region->file_backed;
    info->file_size = region->file_size;
    
    return true;
}
