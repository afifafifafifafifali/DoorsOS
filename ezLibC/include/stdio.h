/* Minimal stdio.h for ezLibC */

#ifndef __EZLIBC_STDIO_H
#define __EZLIBC_STDIO_H 1

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------
 * Standard file descriptors (also in unistd.h)
 * ------------------------------------------------------------------ */
#ifndef STDIN_FILENO
#define STDIN_FILENO    0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO   1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO   2
#endif

/* ------------------------------------------------------------------
 * EOF
 * ------------------------------------------------------------------ */
#define EOF (-1)

/* ------------------------------------------------------------------
 * FILE structure
 * ------------------------------------------------------------------ */
#define _IONBF  0
#define _IOLBF  1
#define _IOFBF  2

typedef struct {
    int    fd;
    int    mode;       /* "r", "w", "a", etc — simplified */
    int    buffering;  /* _IONBF, _IOLBF, _IOFBF */
    char*  buf;        /* user-supplied or malloc'd buffer */
    size_t buf_size;
    size_t buf_pos;
    size_t buf_len;
    int    error;
    int    eof;
} FILE;

/* Standard streams */
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

/* ------------------------------------------------------------------
 * Formatted output
 * ------------------------------------------------------------------ */
int printf(const char* fmt, ...);
int fprintf(FILE* stream, const char* fmt, ...);
int sprintf(char* buf, const char* fmt, ...);
int snprintf(char* buf, size_t n, const char* fmt, ...);

int vprintf(const char* fmt, va_list ap);
int vfprintf(FILE* stream, const char* fmt, va_list ap);
int vsprintf(char* buf, const char* fmt, va_list ap);
int vsnprintf(char* buf, size_t n, const char* fmt, va_list ap);

/* ------------------------------------------------------------------
 * Formatted input
 * ------------------------------------------------------------------ */
int scanf(const char* fmt, ...);
int fscanf(FILE* stream, const char* fmt, ...);
int sscanf(const char* buf, const char* fmt, ...);

int vscanf(const char* fmt, va_list ap);
int vfscanf(FILE* stream, const char* fmt, va_list ap);
int vsscanf(const char* buf, const char* fmt, va_list ap);

/* ------------------------------------------------------------------
 * File operations
 * ------------------------------------------------------------------ */
FILE* fopen(const char* pathname, const char* mode);
int   fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int    fputc(int c, FILE* stream);
int    putc(int c, FILE* stream);
int    putchar(int c);
int    fgetc(FILE* stream);
int    getc(FILE* stream);
int    getchar(void);
char*  fgets(char* s, int n, FILE* stream);
int    puts(const char* s);
int    fflush(FILE* stream);

long   ftell(FILE* stream);
int    fseek(FILE* stream, long offset, int whence);
void   rewind(FILE* stream);

int    feof(FILE* stream);
int    ferror(FILE* stream);
void   clearerr(FILE* stream);

void   setbuf(FILE* stream, char* buf);
void   setvbuf(FILE* stream, char* buf, int mode, size_t size);

#ifdef __cplusplus
}
#endif

#endif
