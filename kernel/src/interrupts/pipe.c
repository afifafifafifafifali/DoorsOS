#include "pipe.h"
#include "../mem/heap.h"
#include "../libs/string.h"

pipe_t* pipe_create(uint32_t size) {
    pipe_t* p = (pipe_t*)malloc(sizeof(pipe_t));
    if (!p) return NULL;

    p->buffer = (uint8_t*)malloc(size);
    if (!p->buffer) {
        free(p);
        return NULL;
    }

    p->size = size;
    p->read_pos = 0;
    p->write_pos = 0;
    p->used = 0;

    p->read_open = true;
    p->write_open = true;

    return p;
}

void pipe_destroy(pipe_t* p) {
    if (!p) return;
    if (p->buffer) free(p->buffer);
    free(p);
}

int pipe_write(pipe_t* p, const uint8_t* data, uint32_t size) {
    if (!p || !p->write_open) return -1;

    uint32_t written = 0;

    while (written < size && p->used < p->size) {
        p->buffer[p->write_pos] = data[written];

        p->write_pos = (p->write_pos + 1) % p->size;
        p->used++;
        written++;
    }

    return written;
}

int pipe_read(pipe_t* p, uint8_t* buffer, uint32_t size) {
    if (!p || !p->read_open) return -1;

    uint32_t read = 0;

    while (read < size && p->used > 0) {
        buffer[read] = p->buffer[p->read_pos];

        p->read_pos = (p->read_pos + 1) % p->size;
        p->used--;
        read++;
    }

    return read;
}

void pipe_close_read(pipe_t* p) {
    if (!p) return;
    p->read_open = false;
}

void pipe_close_write(pipe_t* p) {
    if (!p) return;
    p->write_open = false;
}