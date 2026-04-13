/* Minimal string.h for ezLibC */

#ifndef __EZLIBC_STRING_H
#define __EZLIBC_STRING_H 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Implemented in src/string.c */

size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);
char* strcpy(char* restrict dest, const char* restrict src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
size_t strspn(const char* s, const char* accept);
size_t strcspn(const char* s, const char* reject);
char* strtok(char* s, const char* delim);
char* strtok_r(char* s, const char* delim, char** saveptr);
int strcoll(const char* a, const char* b);
size_t strxfrm(char* dest, const char* src, size_t n);
char* strdup(const char* s);
char* strndup(const char* s, size_t n);

/* Memory functions */

void* memcpy(void* restrict dest, const void* restrict src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memchr(const void* s, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif
