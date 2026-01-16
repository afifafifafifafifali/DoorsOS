#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <stdbool.h>

#define PIPE_BUF_SIZE 4096
#define MAX_PIPES 16

typedef struct {
    char buffer[PIPE_BUF_SIZE];
    int read_pos;
    int write_pos;
    int count;
    bool active;
} pipe_t;

int pipe_create(void);
int pipe_write(int pipe_id, const char *data, int len);
int pipe_read(int pipe_id, char *buf, int len);
void pipe_close(int pipe_id);

#endif
