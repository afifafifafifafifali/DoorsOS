#include "fd.h"
#include "../mem/heap.h"
#include "../libs/string.h"
#include "../fs/fat32.h"
#include "../interrupts/pipe.h"
#include "../gfx/serial_io.h"

// File descriptor table
file_descriptor_t fd_table[MAX_FDS];

// Initialize file descriptor system
void fd_init(void) {
    for (int i = 0; i < MAX_FDS; i++) {
        fd_table[i].type = FD_TYPE_NONE;
        fd_table[i].flags = 0;
        fd_table[i].position = 0;
        fd_table[i].file_buffer = NULL;
        fd_table[i].file_size = 0;
        fd_table[i].modified = false;
        fd_table[i].pipe = NULL;
        memset(fd_table[i].path, 0, 256);
    }
    serial_io_printf("FD system initialized\n");
}

// Find a free file descriptor slot
static int find_free_fd(void) {
    for (int i = 0; i < MAX_FDS; i++) {
        if (fd_table[i].type == FD_TYPE_NONE) {
            return i;
        }
    }
    return -1; // No free slots
}

// Check if fd is valid
bool fd_is_valid(int fd) {
    if (fd < 0 || fd >= MAX_FDS) return false;
    return fd_table[fd].type != FD_TYPE_NONE;
}

// Open a file and return file descriptor
int fd_open(const char* path, int flags) {
    if (!path) return -1;
    
    int fd = find_free_fd();
    if (fd < 0) {
        serial_io_printf("fd_open: no free descriptors\n");
        return -1;
    }
    serial_io_printf("#1 ");
    
    file_descriptor_t* desc = &fd_table[fd];
    
    // Initialize descriptor
    desc->type = FD_TYPE_FILE;
    desc->flags = flags;
    desc->position = 0;
    desc->modified = false;
    strncpy(desc->path, path, 255);
    desc->path[255] = 0;
    
    // Try to read existing file
    uint32_t file_size = 0;
    bool file_exists = false;
    
    serial_io_printf("#2");
    // Allocate a temporary buffer to check if file exists
    uint8_t* temp_buf = malloc(16 * 1024 * 1024); // 16MB max file size
    if (!temp_buf) {
        serial_io_printf("fd_open: failed to allocate temp buffer\n");
        desc->type = FD_TYPE_NONE;
        return -1;
    }
    
    serial_io_printf("#3 ");
    if (fat32_read_file(path, temp_buf, &file_size)) {
        file_exists = true;
        serial_io_printf("fd_open: file exists, size=%u\n", file_size);
    }
    
    // Handle file opening modes
    serial_io_printf("#4 ");
    if (file_exists) {
        if (flags & O_TRUNC) {
            // Truncate file
            file_size = 0;
            desc->file_size = 0;
            serial_io_printf("#5 ");
            desc->file_buffer = malloc(4096); // Start with 4KB
            if (!desc->file_buffer) {
                free(temp_buf);
                desc->type = FD_TYPE_NONE;
                return -1;
            }
            serial_io_printf("#6 ");
            memset(desc->file_buffer, 0, 4096);
            desc->modified = true;
        } else {
            // Keep existing content
            serial_io_printf("#7 ");
            desc->file_size = file_size;
            desc->file_buffer = malloc(file_size > 0 ? file_size : 1);
            if (!desc->file_buffer) {
                free(temp_buf);
                desc->type = FD_TYPE_NONE;
                return -1;
            }
            serial_io_printf("#8 ");
            memcpy(desc->file_buffer, temp_buf, file_size);
            
            if (flags & O_APPEND) {
                desc->position = file_size;
            }
            serial_io_printf("#9 ");
        }
    } else {
        // File doesn't exist
        if (flags & O_CREAT) {
            serial_io_printf("#9 ");
            // Create new file
            desc->file_size = 0;
            serial_io_printf("#10 ");
            desc->file_buffer = malloc(4096); // Start with 4KB
            if (!desc->file_buffer) {
                free(temp_buf);
                desc->type = FD_TYPE_NONE;
                return -1;
            }
            serial_io_printf("#11 ");
            memset(desc->file_buffer, 0, 4096);
            desc->modified = true;
        } else {
            // File doesn't exist and O_CREAT not set
            free(temp_buf);
            desc->type = FD_TYPE_NONE;
            serial_io_printf("#12 ");
            serial_io_printf("fd_open: file not found and O_CREAT not set\n");
            return -1;
        }
    }
    
    serial_io_printf("#13 ");
    free(temp_buf);
    serial_io_printf("fd_open: opened %s as fd=%d\n", path, fd);
    return fd;
}

// Flush modified file to disk
int fd_flush(int fd) {
    if (!fd_is_valid(fd)) return -1;

    file_descriptor_t* desc = &fd_table[fd];

    if (desc->type != FD_TYPE_FILE) return -1;
    if (!desc->modified) return 0; // Nothing to flush

    serial_io_printf("fd_flush: flushing fd=%d, size=%u\n", fd, desc->file_size);

    // Write file back to FAT32
    if (desc->file_size > 0) {
        if (!fat32_write_file(desc->path, desc->file_buffer, desc->file_size)) {
            serial_io_printf("fd_flush: write failed\n");
            return -1;
        }
    }
    // Empty files: skip write (FAT32 layer doesn't support 0-byte writes)

    desc->modified = false;
    serial_io_printf("fd_flush: success\n");
    return 0;
}

// Close a file descriptor
int fd_close(int fd) {
    if (!fd_is_valid(fd)) return -1;
    
    file_descriptor_t* desc = &fd_table[fd];
    
    serial_io_printf("fd_close: closing fd=%d\n", fd);
    
    if (desc->type == FD_TYPE_FILE) {
        // Flush changes before closing
        if (desc->modified) {
            if (fd_flush(fd) < 0) {
                serial_io_printf("fd_close: flush failed\n");
                // Continue with close anyway
            }
        }
        
        // Free file buffer
        if (desc->file_buffer) {
            free(desc->file_buffer);
            desc->file_buffer = NULL;
        }
    } else if (desc->type == FD_TYPE_PIPE_READ) {
        pipe_t* p = (pipe_t*)desc->pipe;
        if (p) {
            pipe_close_read(p);
        }
    } else if (desc->type == FD_TYPE_PIPE_WRITE) {
        pipe_t* p = (pipe_t*)desc->pipe;
        if (p) {
            pipe_close_write(p);
        }
    }
    
    // Clear descriptor
    desc->type = FD_TYPE_NONE;
    desc->flags = 0;
    desc->position = 0;
    desc->file_size = 0;
    desc->modified = false;
    desc->pipe = NULL;
    memset(desc->path, 0, 256);
    
    return 0;
}

// Read from file descriptor
int fd_read(int fd, void* buffer, uint32_t size) {
    if (!fd_is_valid(fd)) return -1;
    if (!buffer || size == 0) return 0;
    
    file_descriptor_t* desc = &fd_table[fd];
    
    if (desc->type == FD_TYPE_FILE) {
        // Check read permission
        if ((desc->flags & O_WRONLY) && !(desc->flags & O_RDWR)) {
            serial_io_printf("fd_read: no read permission\n");
            return -1;
        }
        
        // Calculate how much we can read
        uint32_t available = 0;
        if (desc->position < desc->file_size) {
            available = desc->file_size - desc->position;
        }
        
        uint32_t to_read = (size < available) ? size : available;
        
        if (to_read > 0) {
            memcpy(buffer, desc->file_buffer + desc->position, to_read);
            desc->position += to_read;
        }
        
        return to_read;
    } else if (desc->type == FD_TYPE_PIPE_READ) {
        pipe_t* p = (pipe_t*)desc->pipe;
        if (!p) return -1;
        return pipe_read(p, (uint8_t*)buffer, size);
    }
    
    return -1;
}

// Write to file descriptor
int fd_write(int fd, const void* buffer, uint32_t size) {
    if (!fd_is_valid(fd)) return -1;
    if (!buffer || size == 0) return 0;
    
    file_descriptor_t* desc = &fd_table[fd];
    
    if (desc->type == FD_TYPE_FILE) {
        // Check write permission
        if ((desc->flags & O_RDONLY) && !(desc->flags & O_RDWR)) {
            serial_io_printf("fd_write: no write permission\n");
            return -1;
        }
        
        // Calculate new size after write
        uint32_t end_pos = desc->position + size;
        
        // Resize buffer if needed
        if (end_pos > desc->file_size) {
            // Allocate new larger buffer
            uint32_t new_size = end_pos;
            // Round up to next 4KB boundary for efficiency
            if (new_size % 4096 != 0) {
                new_size = ((new_size / 4096) + 1) * 4096;
            }
            
            uint8_t* new_buffer = malloc(new_size);
            if (!new_buffer) {
                serial_io_printf("fd_write: realloc failed\n");
                return -1;
            }
            
            // Copy existing data
            if (desc->file_buffer && desc->file_size > 0) {
                memcpy(new_buffer, desc->file_buffer, desc->file_size);
            }
            
            // Zero fill gap if seeking past end
            if (desc->position > desc->file_size) {
                memset(new_buffer + desc->file_size, 0, desc->position - desc->file_size);
            }
            
            // Free old buffer
            if (desc->file_buffer) {
                free(desc->file_buffer);
            }
            
            desc->file_buffer = new_buffer;
        }
        
        // Write data
        memcpy(desc->file_buffer + desc->position, buffer, size);
        desc->position += size;
        
        // Update file size
        if (desc->position > desc->file_size) {
            desc->file_size = desc->position;
        }
        
        desc->modified = true;
        
        return size;
    } else if (desc->type == FD_TYPE_PIPE_WRITE) {
        pipe_t* p = (pipe_t*)desc->pipe;
        if (!p) return -1;
        return pipe_write(p, (const uint8_t*)buffer, size);
    }
    
    return -1;
}

// Seek within file
int fd_seek(int fd, int32_t offset, int whence) {
    if (!fd_is_valid(fd)) return -1;
    
    file_descriptor_t* desc = &fd_table[fd];
    
    if (desc->type != FD_TYPE_FILE) {
        return -1; // Can't seek pipes
    }
    
    int32_t new_pos = 0;
    
    switch (whence) {
        case SEEK_SET:
            new_pos = offset;
            break;
        case SEEK_CUR:
            new_pos = desc->position + offset;
            break;
        case SEEK_END:
            new_pos = desc->file_size + offset;
            break;
        default:
            return -1;
    }
    
    // Allow seeking past end (will be zero-filled on write)
    if (new_pos < 0) {
        new_pos = 0;
    }
    
    desc->position = new_pos;
    return new_pos;
}

// Get current position
int32_t fd_tell(int fd) {
    if (!fd_is_valid(fd)) return -1;
    
    file_descriptor_t* desc = &fd_table[fd];
    
    if (desc->type != FD_TYPE_FILE) {
        return -1;
    }
    
    return desc->position;
}

// Get file size
int32_t fd_size(int fd) {
    if (!fd_is_valid(fd)) return -1;
    
    file_descriptor_t* desc = &fd_table[fd];
    
    if (desc->type != FD_TYPE_FILE) {
        return -1;
    }
    
    return desc->file_size;
}

// Create a pipe and return two file descriptors (read and write)
int fd_pipe(int pipefd[2]) {
    if (!pipefd) return -1;
    
    int read_fd = find_free_fd();
    if (read_fd < 0) return -1;
    
    // *** Reserve the read slot before searching for write slot ***
    fd_table[read_fd].type = FD_TYPE_PIPE_READ;
    
    int write_fd = find_free_fd();
    if (write_fd < 0) {
        fd_table[read_fd].type = FD_TYPE_NONE; // undo reservation
        return -1;
    }
    
    pipe_t* p = pipe_create(64 * 1024);
    if (!p) {
        fd_table[read_fd].type = FD_TYPE_NONE;
        return -1;
    }
    
    fd_table[read_fd].flags = O_RDONLY;
    fd_table[read_fd].pipe = p;
    
    fd_table[write_fd].type = FD_TYPE_PIPE_WRITE;
    fd_table[write_fd].flags = O_WRONLY;
    fd_table[write_fd].pipe = p;
    
    pipefd[0] = read_fd;
    pipefd[1] = write_fd;
    
    serial_io_printf("fd_pipe: created pipe read_fd=%d write_fd=%d\n", read_fd, write_fd);
    return 0;
}

// Duplicate file descriptor
int fd_dup(int oldfd) {
    if (!fd_is_valid(oldfd)) return -1;
    
    int newfd = find_free_fd();
    if (newfd < 0) return -1;
    
    file_descriptor_t* old_desc = &fd_table[oldfd];
    file_descriptor_t* new_desc = &fd_table[newfd];
    
    // Copy descriptor
    new_desc->type = old_desc->type;
    new_desc->flags = old_desc->flags;
    new_desc->position = old_desc->position;
    new_desc->file_size = old_desc->file_size;
    new_desc->modified = old_desc->modified;
    new_desc->pipe = old_desc->pipe;
    memcpy(new_desc->path, old_desc->path, 256);
    
    // For files, allocate a separate buffer and copy data
    // This avoids double-free when both fds are closed
    if (old_desc->type == FD_TYPE_FILE) {
        if (old_desc->file_buffer && old_desc->file_size > 0) {
            new_desc->file_buffer = malloc(old_desc->file_size);
            if (new_desc->file_buffer) {
                memcpy(new_desc->file_buffer, old_desc->file_buffer, old_desc->file_size);
            }
        } else {
            new_desc->file_buffer = NULL;
        }
    }

    serial_io_printf("fd_dup: duplicated fd=%d to fd=%d\n", oldfd, newfd);

    return newfd;
}

int fd_dup2(int oldfd, int newfd) {
    if (!fd_is_valid(oldfd)) return -1;
    if (newfd < 0 || newfd >= MAX_FDS) return -1;
    
    // If oldfd == newfd, just return newfd (no-op, but validate oldfd)
    if (oldfd == newfd) {
        return newfd;
    }
    
    // Close newfd if it's already open
    if (fd_is_valid(newfd)) {
        fd_close(newfd);
    }
    
    file_descriptor_t* old_desc = &fd_table[oldfd];
    file_descriptor_t* new_desc = &fd_table[newfd];
    
    // Copy descriptor
    new_desc->type = old_desc->type;
    new_desc->flags = old_desc->flags;
    new_desc->position = old_desc->position;
    new_desc->file_size = old_desc->file_size;
    new_desc->modified = old_desc->modified;
    new_desc->pipe = old_desc->pipe;
    memcpy(new_desc->path, old_desc->path, 256);

    // For files, allocate a separate buffer and copy data
    // This avoids double-free when both fds are closed
    if (old_desc->type == FD_TYPE_FILE) {
        if (old_desc->file_buffer && old_desc->file_size > 0) {
            new_desc->file_buffer = malloc(old_desc->file_size);
            if (new_desc->file_buffer) {
                memcpy(new_desc->file_buffer, old_desc->file_buffer, old_desc->file_size);
            }
        } else {
            new_desc->file_buffer = NULL;
        }
    }

    serial_io_printf("fd_dup2: duplicated fd=%d to fd=%d\n", oldfd, newfd);

    return newfd;
}