#ifndef FD_H
#define FD_H

#include <stdint.h>
#include <stdbool.h>

// File descriptor types
typedef enum {
    FD_TYPE_NONE = 0,
    FD_TYPE_FILE,      // Regular FAT32 file
    FD_TYPE_PIPE_READ, // Pipe read end
    FD_TYPE_PIPE_WRITE // Pipe write end
} fd_type_t;

// File opening flags
#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT     0100
#define O_TRUNC     01000
#define O_APPEND    02000


// Seek whence values
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

// File descriptor structure
typedef struct {
    fd_type_t type;
    int flags;
    uint32_t position;      // Current read/write position
    
    // For file type
    char path[256];         // Full path to file
    uint8_t* file_buffer;   // Cached file content
    uint32_t file_size;     // Size of file
    bool modified;          // Has buffer been modified?
    
    // For pipe type
    void* pipe;             // Pointer to pipe_t structure
} file_descriptor_t;

// Maximum number of file descriptors
#define MAX_FDS 256

// File descriptor table (extern for mmap access)
extern file_descriptor_t fd_table[MAX_FDS];

// Initialize file descriptor system
void fd_init(void);

// Open a file and return file descriptor
int fd_open(const char* path, int flags);

// Close a file descriptor
int fd_close(int fd);

// Read from file descriptor
int fd_read(int fd, void* buffer, uint32_t size);

// Write to file descriptor
int fd_write(int fd, const void* buffer, uint32_t size);

// Seek within file
int fd_seek(int fd, int32_t offset, int whence);

// Get current position
int32_t fd_tell(int fd);

// Create a pipe and return two file descriptors (read and write)
int fd_pipe(int pipefd[2]);

// Flush modified file to disk
int fd_flush(int fd);

// Duplicate file descriptor
int fd_dup(int oldfd);

// Get file size
int32_t fd_size(int fd);

// Check if fd is valid
bool fd_is_valid(int fd);

int fd_dup2(int oldfd, int newfd);
#endif