#pragma once 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef char *string_t; // For convenience, can be used as a string type

// Implemented in main.c


void *memcpy(void *restrict dest, const void *restrict src, size_t n);

void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// Included in string.c
size_t strlen(const char *s);
char *strcpy(char *restrict dest, const char *restrict src);
char *strncpy(char *restrict dest, const char *restrict src, size_t n);
char *strcat(char *restrict dest, const char *restrict src);
char *strncat(char *restrict dest, const char *restrict src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

bool strEql(string_t str1, string_t str2);