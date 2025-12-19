#pragma once 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef char *string_t; // For convenience, can be used as a string type

// Implemented in main.c


void *memcpy(void *dest, const void *src, size_t n);

void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// Included in string.c
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strdup(const char *s);

bool strEql(string_t str1, string_t str2);char k_toupper(char c);
char k_tolower(char c);
char* strrchr(const char* s, int c);
