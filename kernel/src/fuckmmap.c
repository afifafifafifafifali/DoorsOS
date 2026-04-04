#include "mmap.h"
#include "interrupts/fd.h"
#include "gfx/serial_io.h"
#include "libs/string.h"
#include "mem/heap.h" //for virt-to-phys

/* -----------------------------------------------------------------------
 * Test 1: Anonymous mapping — zero filled, read/write
 * -----------------------------------------------------------------------*/
static void mmap_test_anonymous(void)
{
    serial_io_printf("\n--- mmap_test_anonymous ---\n");

    void *p = mmap(NULL, 8192, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (p == MAP_FAILED) {
        serial_io_printf("FAILED: mmap returned MAP_FAILED\n");
        return;
    }
    serial_io_printf("OK: got mapping at 0x%lx\n", (unsigned long)p);

    /* Should be zeroed */
    uint8_t *b = (uint8_t *)p;
    for (int i = 0; i < 8192; i++) {
        if (b[i] != 0) {
            serial_io_printf("FAILED: byte %d is 0x%02x, expected 0\n", i, b[i]);
            munmap(p, 8192);
            return;
        }
    }
    serial_io_printf("OK: all bytes zeroed\n");

    /* Write a pattern */
    for (int i = 0; i < 8192; i++)
        b[i] = (uint8_t)(i & 0xFF);

    /* Read it back */
    for (int i = 0; i < 8192; i++) {
        if (b[i] != (uint8_t)(i & 0xFF)) {
            serial_io_printf("FAILED: pattern mismatch at %d\n", i);
            munmap(p, 8192);
            return;
        }
    }
    serial_io_printf("OK: pattern write/read-back passed\n");

    if (munmap(p, 8192) != 0) {
        serial_io_printf("FAILED: munmap\n");
        return;
    }
    serial_io_printf("OK: munmap succeeded\n");
    serial_io_printf("PASS: anonymous mapping\n");
}

/* -----------------------------------------------------------------------
 * Test 2: File-backed MAP_PRIVATE — reads file into memory, changes
 *          are NOT written back to disk.
 * -----------------------------------------------------------------------*/
static void mmap_test_file_private(void)
{
    serial_io_printf("\n--- mmap_test_file_private ---\n");

    /* Create and populate a test file via fd */
    int fd = fd_open("/mmap_test.bin", O_CREAT | O_RDWR | O_TRUNC);
    if (fd < 0) {
        serial_io_printf("FAILED: fd_open\n");
        return;
    }

    /* Write 4096 bytes of ascending pattern */
    uint8_t src[4096];
    for (int i = 0; i < 4096; i++)
        src[i] = (uint8_t)(i & 0xFF);

    if (fd_write(fd, src, 4096) != 4096) {
        serial_io_printf("FAILED: fd_write\n");
        fd_close(fd);
        return;
    }
    serial_io_printf("OK: wrote 4096 bytes to /mmap_test.bin\n");

    /* mmap the file MAP_PRIVATE */
    void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE, fd, 0);

    if (p == MAP_FAILED) {
        serial_io_printf("FAILED: mmap MAP_PRIVATE\n");
        fd_close(fd);
        return;
    }
    serial_io_printf("OK: MAP_PRIVATE mapping at 0x%lx\n", (unsigned long)p);

    /* Verify file data visible in mapping */
    uint8_t *b = (uint8_t *)p;
    for (int i = 0; i < 4096; i++) {
        if (b[i] != src[i]) {
            serial_io_printf("FAILED: byte %d = 0x%02x, expected 0x%02x\n",
                             i, b[i], src[i]);
            munmap(p, 4096);
            fd_close(fd);
            return;
        }
    }
    serial_io_printf("OK: file data correctly visible in mapping\n");

    /* Modify mapped memory — should NOT affect the file (MAP_PRIVATE) */
    for (int i = 0; i < 4096; i++)
        b[i] = 0xAB;

    /* Read file directly — should still have original content */
    fd_seek(fd, 0, SEEK_SET);
    uint8_t check[4096];
    fd_read(fd, check, 4096);

    for (int i = 0; i < 4096; i++) {
        if (check[i] != src[i]) {
            serial_io_printf("FAILED: MAP_PRIVATE write leaked to file at byte %d\n", i);
            munmap(p, 4096);
            fd_close(fd);
            return;
        }
    }
    serial_io_printf("OK: MAP_PRIVATE write did NOT affect file\n");

    munmap(p, 4096);
    fd_close(fd);
    serial_io_printf("PASS: file-backed MAP_PRIVATE\n");
}

/* -----------------------------------------------------------------------
 * Test 3: File-backed MAP_SHARED + msync — writes go back to disk.
 * -----------------------------------------------------------------------*/
static void mmap_test_file_shared(void)
{
    serial_io_printf("\n--- mmap_test_file_shared ---\n");

    /* Create a 4096-byte file of 0xCC */
    int fd = fd_open("/mmap_shared.bin", O_CREAT | O_RDWR | O_TRUNC);
    if (fd < 0) {
        serial_io_printf("FAILED: fd_open\n");
        return;
    }

    uint8_t fill[4096];
    memset(fill, 0xCC, 4096);
    fd_write(fd, fill, 4096);
    serial_io_printf("OK: wrote 0xCC pattern to /mmap_shared.bin\n");

    /* mmap MAP_SHARED */
    void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_SHARED, fd, 0);

    if (p == MAP_FAILED) {
        serial_io_printf("FAILED: mmap MAP_SHARED\n");
        fd_close(fd);
        return;
    }
    serial_io_printf("OK: MAP_SHARED mapping at 0x%lx\n", (unsigned long)p);

    /* Overwrite the mapping */
    memset(p, 0x42, 4096);
    serial_io_printf("OK: wrote 0x42 into shared mapping\n");

    /* msync — should push changes into fd buffer + flush to FAT32 */
    if (msync(p, 4096) != 0) {
        serial_io_printf("FAILED: msync\n");
        munmap(p, 4096);
        fd_close(fd);
        return;
    }
    serial_io_printf("OK: msync succeeded\n");

    /* Read back from fd to confirm sync */
    fd_seek(fd, 0, SEEK_SET);
    uint8_t check[4096];
    fd_read(fd, check, 4096);

    for (int i = 0; i < 4096; i++) {
        if (check[i] != 0x42) {
            serial_io_printf("FAILED: msync did not update file byte %d "
                             "(got 0x%02x, expected 0x42)\n", i, check[i]);
            munmap(p, 4096);
            fd_close(fd);
            return;
        }
    }
    serial_io_printf("OK: file contents updated by msync\n");

    munmap(p, 4096);
    fd_close(fd);
    serial_io_printf("PASS: MAP_SHARED + msync\n");
}

/* -----------------------------------------------------------------------
 * Test 4: mprotect — change protection on a live mapping
 * -----------------------------------------------------------------------*/
static void mmap_test_mprotect(void)
{
    serial_io_printf("\n--- mmap_test_mprotect ---\n");

    void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (p == MAP_FAILED) {
        serial_io_printf("FAILED: mmap\n");
        return;
    }

    /* Write something */
    ((uint8_t *)p)[0] = 0xBE;

    /* Change to read-only */
    if (mprotect(p, 4096, PROT_READ) != 0) {
        serial_io_printf("FAILED: mprotect PROT_READ\n");
        munmap(p, 4096);
        return;
    }
    serial_io_printf("OK: mprotect PROT_READ applied\n");

    /* Data still readable */
    if (((uint8_t *)p)[0] != 0xBE) {
        serial_io_printf("FAILED: data not readable after mprotect PROT_READ\n");
        munmap(p, 4096);
        return;
    }
    serial_io_printf("OK: data still readable after mprotect PROT_READ\n");

    /* Restore write */
    if (mprotect(p, 4096, PROT_READ | PROT_WRITE) != 0) {
        serial_io_printf("FAILED: mprotect PROT_READ|PROT_WRITE\n");
        munmap(p, 4096);
        return;
    }
    serial_io_printf("OK: mprotect PROT_READ|PROT_WRITE applied\n");

    ((uint8_t *)p)[0] = 0xEF;
    if (((uint8_t *)p)[0] != 0xEF) {
        serial_io_printf("FAILED: write after mprotect restore\n");
        munmap(p, 4096);
        return;
    }

    munmap(p, 4096);
    serial_io_printf("PASS: mprotect\n");
}

/* -----------------------------------------------------------------------
 * Test 5: Multiple mappings and mmap_dump
 * -----------------------------------------------------------------------*/
static void mmap_test_multi(void)
{
    serial_io_printf("\n--- mmap_test_multi ---\n");

    void *a = mmap(NULL, 4096,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *b = mmap(NULL, 8192,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *c = mmap(NULL, 16384, PROT_READ,               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (a == MAP_FAILED || b == MAP_FAILED || c == MAP_FAILED) {
        serial_io_printf("FAILED: one of the multi-mappings failed\n");
        if (a != MAP_FAILED) munmap(a, 4096);
        if (b != MAP_FAILED) munmap(b, 8192);
        if (c != MAP_FAILED) munmap(c, 16384);
        return;
    }

    serial_io_printf("OK: 3 mappings created\n");
    mmap_dump();

    /* Make sure they don't overlap */
    if (a == b || b == c || a == c) {
        serial_io_printf("FAILED: mappings share the same address\n");
    } else {
        serial_io_printf("OK: all mappings at distinct addresses\n");
    }

    munmap(a, 4096);
    munmap(b, 8192);
    munmap(c, 16384);

    mmap_dump();   /* should show 0 active mappings */
    serial_io_printf("PASS: multiple mappings\n");
}

/* -----------------------------------------------------------------------
 * Entry point
 * -----------------------------------------------------------------------*/
void mmap_test_complete(void)
{
    serial_io_printf("\n");
    serial_io_printf("========================================\n");
    serial_io_printf("         MMAP TEST SUITE                \n");
    serial_io_printf("========================================\n");

    /* fd system must be ready before file-backed tests */
    

    mmap_test_anonymous();
    mmap_test_file_private();
    mmap_test_file_shared();
    mmap_test_mprotect();
    mmap_test_multi();

    serial_io_printf("\n========================================\n");
    serial_io_printf("         MMAP TESTS DONE                \n");
    serial_io_printf("========================================\n\n");
}