#include "string.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../mem/heap.h"

size_t strlen(const char *s) {
    const char *p = s;

    while (*p) {
        p++;
    }

    return p - s;
}

char *strcpy(char *restrict dest, const char *restrict src) {
    char *p = dest;

    while ((*p++ = *src++) != '\0') {
        // Copying characters until we hit the null terminator.
    }

    return dest;
}


char *strncpy(char *restrict dest, const char *restrict src, size_t n) {
    char *p = dest;

    while (n-- && (*p++ = *src++) != '\0') {
        // Copying characters until we hit the null terminator or n reaches 0.
    }

    while (n-- > 0) {
        *p++ = '\0'; // Fill remaining space with null characters.
    }

    return dest;
}

char *strcat(char *restrict dest, const char *restrict src) {
    char *p = dest;

    while (*p) {
        p++; // Move to the end of dest.
    }

    while ((*p++ = *src++) != '\0') {
        // Copying characters from src to the end of dest.
    }

    return dest;
}

char *strncat(char *restrict dest, const char *restrict src, size_t n) {
    char *p = dest;

    while (*p) {
        p++; // Move to the end of dest.
    }

    while (n-- && (*p++ = *src++) != '\0') {
        // Copying characters from src to the end of dest until n reaches 0 or null terminator.
    }

    if (n > 0) {
        *p = '\0'; // Ensure the string is null-terminated.
    }

    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    if (n == (size_t)-1) {
        return 0; // If n is 0, strings are considered equal.
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}



bool strEql(string_t str1, string_t str2) {
    if (str1 == NULL || str2 == NULL) {
        return false; // If either string is NULL, they are not equal.
    }

    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return false; // Characters differ, strings are not equal.
        }
        str1++;
        str2++;
    }

    return *str1 == *str2; // Both strings must reach the null terminator at the same time.
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == c) {
            return (char *)s;
        }
        s++;
    }
    return (c == '\0') ? (char *)s : NULL;
}

char *strdup(const char *s) {
    if (!s) return NULL;
    
    size_t len = strlen(s) + 1;
    char *dup = (char *)malloc(len);
    if (!dup) return NULL;
    
    memcpy(dup, s, len);
    return dup;
}

char k_toupper(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}

char k_tolower(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    return (char*)last;
}
