#include "fd.h"
#include "../gfx/serial_io.h"
#include "../mem/heap.h"
#include "../libs/string.h"

#define serial_io_serial_io_printf serial_io_printf
void fd_test_basic(void) {
    serial_io_printf("\n=== FD Basic Test ===\n");
    serial_io_printf("\n=== FD Basic Test ===\n");
    
    // Test 1: Create and write a file
    int fd = fd_open("/testfile.txt", O_CREAT | O_RDWR);
    if (fd < 0) {
        serial_io_printf("FAILED: fd_open\n");
        return;
    }
    serial_io_printf("SUCCESS: Opened file, fd=%d\n", fd);
    
    const char* msg = "Hello, File Descriptors!";
    int written = fd_write(fd, msg, strlen(msg));
    if (written != (int)strlen(msg)) {
        serial_io_printf("FAILED: fd_write (wrote %d, expected %d)\n", written, (int)strlen(msg));
        fd_close(fd);
        return;
    }
    serial_io_printf("SUCCESS: Wrote %d bytes\n", written);
    
    // Seek back to start
    fd_seek(fd, 0, SEEK_SET);
    serial_io_printf("SUCCESS: Seeked to start\n");
    
    // Read back
    char buffer[128];
    memset(buffer, 0, 128);
    int read_bytes = fd_read(fd, buffer, strlen(msg));
    if (read_bytes != (int)strlen(msg)) {
        serial_io_printf("FAILED: fd_read (read %d, expected %d)\n", read_bytes, (int)strlen(msg));
        fd_close(fd);
        return;
    }
    
    if (strcmp(buffer, msg) != 0) {
        serial_io_printf("FAILED: Data mismatch\n");
        serial_io_printf("  Expected: %s\n", msg);
        serial_io_printf("  Got:      %s\n", buffer);
        fd_close(fd);
        return;
    }
    serial_io_printf("SUCCESS: Read back data correctly: '%s'\n", buffer);
    
    // Close (should flush to disk)
    if (fd_close(fd) < 0) {
        serial_io_printf("FAILED: fd_close\n");
        return;
    }
    serial_io_printf("SUCCESS: Closed file\n");
    
    serial_io_printf("=== Basic Test PASSED ===\n\n");
}

void fd_test_large_file(void) {
    serial_io_printf("\n=== FD Large File Test (4MB) ===\n");
    serial_io_serial_io_printf("\n=== FD Large File Test (4MB) ===\n");
    
    // Test with 4MB file
    const uint32_t test_size = 4 * 1024 * 1024; // 4MB
    
    serial_io_printf("Allocating %u MB buffer...\n", test_size / (1024 * 1024));
    uint8_t* test_data = malloc(test_size);
    if (!test_data) {
        serial_io_printf("FAILED: malloc for test data\n");
        return;
    }
    
    // Fill with pattern
    serial_io_printf("Filling buffer with test pattern...\n");
    for (uint32_t i = 0; i < test_size; i++) {
        test_data[i] = (uint8_t)(i & 0xFF);
    }
    serial_io_printf("SUCCESS: Buffer filled\n");
    
    // Open file for writing
    int fd = fd_open("/largefile.bin", O_CREAT | O_RDWR | O_TRUNC);
    if (fd < 0) {
        serial_io_printf("FAILED: fd_open\n");
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: Opened file, fd=%d\n", fd);
    
    // Write in chunks to simulate real usage
    serial_io_printf("Writing 4MB in 256KB chunks...\n");
    uint32_t chunk_size = 256 * 1024; // 256KB chunks
    uint32_t written_total = 0;
    
    for (uint32_t offset = 0; offset < test_size; offset += chunk_size) {
        uint32_t to_write = chunk_size;
        if (offset + to_write > test_size) {
            to_write = test_size - offset;
        }
        
        int written = fd_write(fd, test_data + offset, to_write);
        if (written != (int)to_write) {
            serial_io_printf("FAILED: fd_write at offset %u (wrote %d, expected %u)\n", 
                   offset, written, to_write);
            fd_close(fd);
            free(test_data);
            return;
        }
        written_total += written;
        
        if ((offset / chunk_size) % 4 == 0) {
            serial_io_printf("  Written %u KB / %u KB\n", 
                   written_total / 1024, test_size / 1024);
        }
    }
    serial_io_printf("SUCCESS: Wrote %u bytes total\n", written_total);
    
    // Check file size
    int32_t file_size = fd_size(fd);
    if (file_size != (int32_t)test_size) {
        serial_io_printf("FAILED: File size mismatch (got %d, expected %u)\n", 
               file_size, test_size);
        fd_close(fd);
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: File size correct: %d bytes\n", file_size);
    
    // Seek to beginning
    fd_seek(fd, 0, SEEK_SET);
    
    // Read back and verify in chunks
    serial_io_printf("Reading back and verifying...\n");
    uint8_t* verify_buffer = malloc(chunk_size);
    if (!verify_buffer) {
        serial_io_printf("FAILED: malloc for verify buffer\n");
        fd_close(fd);
        free(test_data);
        return;
    }
    
    uint32_t verified_total = 0;
    uint32_t error_count = 0;
    
    for (uint32_t offset = 0; offset < test_size; offset += chunk_size) {
        uint32_t to_read = chunk_size;
        if (offset + to_read > test_size) {
            to_read = test_size - offset;
        }
        
        int read_bytes = fd_read(fd, verify_buffer, to_read);
        if (read_bytes != (int)to_read) {
            serial_io_printf("FAILED: fd_read at offset %u (read %d, expected %u)\n", 
                   offset, read_bytes, to_read);
            error_count++;
            break;
        }
        
        // Verify data
        for (uint32_t i = 0; i < to_read; i++) {
            if (verify_buffer[i] != test_data[offset + i]) {
                if (error_count < 10) {
                    serial_io_printf("ERROR: Mismatch at offset %u: got 0x%02x, expected 0x%02x\n",
                           offset + i, verify_buffer[i], test_data[offset + i]);
                }
                error_count++;
            }
        }
        
        verified_total += read_bytes;
        
        if ((offset / chunk_size) % 4 == 0) {
            serial_io_printf("  Verified %u KB / %u KB (errors: %u)\n", 
                   verified_total / 1024, test_size / 1024, error_count);
        }
    }
    
    free(verify_buffer);
    
    if (error_count > 0) {
        serial_io_printf("FAILED: %u verification errors\n", error_count);
        fd_close(fd);
        free(test_data);
        return;
    }
    
    serial_io_printf("SUCCESS: All data verified correctly\n");
    
    // Test seek operations
    serial_io_printf("Testing seek operations...\n");
    
    // Seek to middle
    fd_seek(fd, test_size / 2, SEEK_SET);
    int32_t pos = fd_tell(fd);
    if (pos != (int32_t)(test_size / 2)) {
        serial_io_printf("FAILED: Seek to middle (pos=%d, expected %u)\n", 
               pos, test_size / 2);
        fd_close(fd);
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: Seek to middle: %d\n", pos);
    
    // Seek to end
    fd_seek(fd, 0, SEEK_END);
    pos = fd_tell(fd);
    if (pos != (int32_t)test_size) {
        serial_io_printf("FAILED: Seek to end (pos=%d, expected %u)\n", pos, test_size);
        fd_close(fd);
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: Seek to end: %d\n", pos);
    
    // Close and flush
    serial_io_printf("Closing file (this will flush to FAT32)...\n");
    if (fd_close(fd) < 0) {
        serial_io_printf("FAILED: fd_close\n");
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: File closed and flushed\n");
    
    // Reopen and verify persistence
    serial_io_printf("Reopening file to verify persistence...\n");
    fd = fd_open("/largefile.bin", O_RDONLY);
    if (fd < 0) {
        serial_io_printf("FAILED: fd_open for verification\n");
        free(test_data);
        return;
    }
    
    file_size = fd_size(fd);
    if (file_size != (int32_t)test_size) {
        serial_io_printf("FAILED: Reopened file size mismatch (got %d, expected %u)\n",
               file_size, test_size);
        fd_close(fd);
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: Reopened file has correct size: %d\n", file_size);
    
    // Quick spot check - read first 1KB
    uint8_t spot_check[1024];
    fd_read(fd, spot_check, 1024);
    bool spot_ok = true;
    for (int i = 0; i < 1024; i++) {
        if (spot_check[i] != test_data[i]) {
            spot_ok = false;
            break;
        }
    }
    
    if (!spot_ok) {
        serial_io_printf("FAILED: Spot check of reopened file\n");
        fd_close(fd);
        free(test_data);
        return;
    }
    serial_io_printf("SUCCESS: Spot check passed\n");
    
    fd_close(fd);
    free(test_data);
    
    serial_io_printf("=== Large File Test PASSED ===\n\n");
}

void fd_test_pipe(void) {
    serial_io_printf("\n=== FD Pipe Test ===\n");
    serial_io_serial_io_printf("\n=== FD Pipe Test ===\n");
    
    int pipefd[2];
    if (fd_pipe(pipefd) < 0) {
        serial_io_printf("FAILED: fd_pipe\n");
        return;
    }
    serial_io_printf("SUCCESS: Created pipe: read_fd=%d, write_fd=%d\n", pipefd[0], pipefd[1]);
    
    // Write to pipe
    const char* msg = "Hello through pipe!";
    int written = fd_write(pipefd[1], msg, strlen(msg));
    if (written != (int)strlen(msg)) {
        serial_io_printf("FAILED: pipe write\n");
        fd_close(pipefd[0]);
        fd_close(pipefd[1]);
        return;
    }
    serial_io_printf("SUCCESS: Wrote %d bytes to pipe\n", written);
    
    // Read from pipe
    char buffer[128];
    memset(buffer, 0, 128);
    int read_bytes = fd_read(pipefd[0], buffer, strlen(msg));
    if (read_bytes != (int)strlen(msg)) {
        serial_io_printf("FAILED: pipe read (read %d, expected %d)\n", 
               read_bytes, (int)strlen(msg));
        fd_close(pipefd[0]);
        fd_close(pipefd[1]);
        return;
    }
    
    if (strcmp(buffer, msg) != 0) {
        serial_io_printf("FAILED: Pipe data mismatch\n");
        fd_close(pipefd[0]);
        fd_close(pipefd[1]);
        return;
    }
    serial_io_printf("SUCCESS: Read from pipe: '%s'\n", buffer);
    
    // Close pipe
    fd_close(pipefd[0]);
    fd_close(pipefd[1]);
    serial_io_printf("SUCCESS: Closed pipe\n");
    
    serial_io_printf("=== Pipe Test PASSED ===\n\n");
}

void fd_test_dup(void) {
    serial_io_printf("\n=== FD Dup Test ===\n");

    // Open a file
    int fd = fd_open("/duptest.txt", O_CREAT | O_RDWR);
    if (fd < 0) {
        serial_io_printf("FAILED: fd_open\n");
        return;
    }

    // Write via original fd
    const char* msg = "dup test data";
    int written = fd_write(fd, msg, strlen(msg));
    if (written != (int)strlen(msg)) {
        serial_io_printf("FAILED: fd_write (wrote %d, expected %d)\n", written, (int)strlen(msg));
        fd_close(fd);
        return;
    }
    serial_io_printf("SUCCESS: Wrote %d bytes via original fd\n", written);

    // Duplicate the fd
    int fd2 = fd_dup(fd);
    if (fd2 < 0) {
        serial_io_printf("FAILED: fd_dup\n");
        fd_close(fd);
        return;
    }
    serial_io_printf("SUCCESS: Duplicated fd=%d to fd=%d\n", fd, fd2);

    // Seek fd2 to start and read back
    fd_seek(fd2, 0, SEEK_SET);
    char buffer[128];
    memset(buffer, 0, 128);
    int read_bytes = fd_read(fd2, buffer, strlen(msg));
    if (read_bytes != (int)strlen(msg)) {
        serial_io_printf("FAILED: fd_read via dup (read %d, expected %d)\n", read_bytes, (int)strlen(msg));
        fd_close(fd);
        fd_close(fd2);
        return;
    }
    if (strcmp(buffer, msg) != 0) {
        serial_io_printf("FAILED: Data mismatch via dup\n");
        fd_close(fd);
        fd_close(fd2);
        return;
    }
    serial_io_printf("SUCCESS: Read via dup fd: '%s'\n", buffer);

    // Write more via fd2
const char* msg2 = "!";
fd_seek(fd2, 0, SEEK_END);
int written2 = fd_write(fd2, msg2, strlen(msg2));
if (written2 != (int)strlen(msg2)) {
    serial_io_printf("FAILED: fd_write via dup\n");
    fd_close(fd);
    fd_close(fd2);
    return;
}
serial_io_printf("SUCCESS: Wrote via dup fd\n");

    // Close original, verify dup still works
    fd_close(fd);
    serial_io_printf("SUCCESS: Closed original fd\n");

    fd_seek(fd2, 0, SEEK_SET);
    memset(buffer, 0, 128);
    fd_read(fd2, buffer, strlen(msg));
    if (strncmp(buffer, msg, strlen(msg)) != 0) {
        serial_io_printf("FAILED: dup fd unusable after original closed\n");
        fd_close(fd2);
        return;
    }
    serial_io_printf("SUCCESS: dup fd still readable after original closed\n");

    fd_close(fd2);
    serial_io_printf("=== Dup Test PASSED ===\n\n");
}

void fd_test_complete(void) {
    serial_io_printf("\n");
    serial_io_printf("\n");
    serial_io_printf("    FILE DESCRIPTOR SYSTEM TEST SUITE        \n");
    serial_io_printf("\n");
    
    fd_init();
    
    fd_test_basic();
    fd_test_pipe();
    fd_test_large_file();
    
    fd_test_dup();
    serial_io_printf("\n");
    serial_io_printf("\n");
    serial_io_printf("    ALL FD TESTS COMPLETED                   \n");
    serial_io_printf("\n");
    serial_io_printf("\n");
}