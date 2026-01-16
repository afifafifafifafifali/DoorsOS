#include "pipe.h"
#include <stddef.h>

static pipe_t pipes[MAX_PIPES];

int pipe_create(void) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipes[i].active) {
            pipes[i].read_pos = 0;
            pipes[i].write_pos = 0;
            pipes[i].count = 0;
            pipes[i].active = true;
            return i;
        }
    }
    return -1;
}

int pipe_write(int pipe_id, const char *data, int len) {
    if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipes[pipe_id].active) return -1;
    
    pipe_t *p = &pipes[pipe_id];
    int written = 0;
    
    while (written < len && p->count < PIPE_BUF_SIZE) {
        p->buffer[p->write_pos] = data[written++];
        p->write_pos = (p->write_pos + 1) % PIPE_BUF_SIZE;
        p->count++;
    }
    
    return written;
}

int pipe_read(int pipe_id, char *buf, int len) {
    if (pipe_id < 0 || pipe_id >= MAX_PIPES || !pipes[pipe_id].active) return -1;
    
    pipe_t *p = &pipes[pipe_id];
    int read = 0;
    
    while (read < len && p->count > 0) {
        buf[read++] = p->buffer[p->read_pos];
        p->read_pos = (p->read_pos + 1) % PIPE_BUF_SIZE;
        p->count--;
    }
    
    return read;
}

void pipe_close(int pipe_id) {
    if (pipe_id >= 0 && pipe_id < MAX_PIPES) {
        pipes[pipe_id].active = false;
    }
}
