#include "fat32.h"
#include "../gfx/printf.h"
#include "../storage/storage.h"
#include "../libs/string.h"
#include "../mem/new/pmm.h"

void fat32_test_complete(void) {
    printf("=== FAT32 Complete Test ===\n");
    
    // Test 1: List files
    printf("\n[Test 1] Listing root directory:\n");
    fat32_list_root();
    
    // Test 2: Write file
    printf("\n[Test 2] Writing test file...\n");
    const char* test_data = "Hello from DoorsOS!\nThis is a test file.\nAHCI + FAT32 working!\n";
    if (fat32_write_file("test.txt", (uint8_t*)test_data, strlen(test_data))) {
        printf("Write: SUCCESS\n");
    } else {
        printf("Write: FAILED\n");
        return;
    }
    
    // Test 3: Read file back
    printf("\n[Test 3] Reading test file...\n");
    uint8_t* read_buf = k_malloc(512);
    uint32_t size = 0;
    
    if (fat32_read_file_simple("test.txt", read_buf, &size)) {
        printf("Read: SUCCESS (%u bytes)\n", size);
        printf("Content:\n");
        for (uint32_t i = 0; i < size; i++) {
            printf("%c", read_buf[i]);
        }
        printf("\n");
    } else {
        printf("Read: FAILED\n");
    }
    
    k_free(read_buf);
    
    // Test 4: Create directory
    printf("\n[Test 4] Creating directory...\n");
    if (fat32_create_directory("testdir")) {
        printf("mkdir: SUCCESS\n");
    } else {
        printf("mkdir: FAILED\n");
    }
    
    // Test 5: List again
    printf("\n[Test 5] Listing after changes:\n");
    fat32_list_root();
    
    // Test 6: Delete file
    printf("\n[Test 6] Deleting test file...\n");
    if (fat32_delete_file("test.txt")) {
        printf("Delete: SUCCESS\n");
    } else {
        printf("Delete: FAILED\n");
    }
    
    // Test 7: Final list
    printf("\n[Test 7] Final listing:\n");
    fat32_list_root();
    
    printf("\n=== FAT32 Test Complete ===\n");
}
