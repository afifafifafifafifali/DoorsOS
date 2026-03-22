#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t* buffer;
    uint32_t size;

    uint32_t read_pos;
    uint32_t write_pos;

    uint32_t used;

    bool read_open;
    bool write_open;
} pipe_t;

pipe_t* pipe_create(uint32_t size);
void pipe_destroy(pipe_t* p);

int pipe_write(pipe_t* p, const uint8_t* data, uint32_t size);
int pipe_read(pipe_t* p, uint8_t* buffer, uint32_t size);

void pipe_close_read(pipe_t* p);
void pipe_close_write(pipe_t* p);

#endif