#include <string.h>
#include <unistd.h>

/* --- String functions --- */

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len] != '\0') len++;
    return len;
}

size_t strnlen(const char* s, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && s[len] != '\0') len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;
    while ((*d++ = *src++) != '\0');
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (*d) d++;
    while (n-- && (*d = *src++)) d++;
    *d = '\0';
    return dest;
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char* a, const char* b, size_t n) {
    while (n && *a && *a == *b) { a++; b++; n--; }
    return n ? (unsigned char)*a - (unsigned char)*b : 0;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return (char)c == '\0' ? (char*)s : NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    return (char)c == '\0' ? (char*)s : (char*)last;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) { h++; n++; }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return NULL;
}

size_t strspn(const char* s, const char* accept) {
    size_t len = 0;
    while (*s) {
        const char* a = accept;
        while (*a && *a != *s) a++;
        if (!*a) return len;
        len++; s++;
    }
    return len;
}

size_t strcspn(const char* s, const char* reject) {
    size_t len = 0;
    while (*s) {
        const char* r = reject;
        while (*r) {
            if (*r == *s) return len;
            r++;
        }
        len++; s++;
    }
    return len;
}

int strcoll(const char* a, const char* b) {
    return strcmp(a, b);
}

size_t strxfrm(char* dest, const char* src, size_t n) {
    size_t len = strlen(src);
    if (dest) strncpy(dest, src, n);
    return len;
}

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* d = (char*)brk(0);
    if (!d) return NULL;
    brk((void*)((char*)d + len));
    memcpy(d, s, len);
    return d;
}

char* strndup(const char* s, size_t n) {
    size_t len = strnlen(s, n) + 1;
    char* d = (char*)brk(0);
    if (!d) return NULL;
    brk((void*)((char*)d + len));
    memcpy(d, s, len);
    d[len - 1] = '\0';
    return d;
}

/* --- Memory functions --- */

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t* restrict pdest = (uint8_t* restrict)dest;
    const uint8_t* restrict psrc = (const uint8_t* restrict)src;
    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
    unsigned char* dst = (unsigned char*)dstptr;
    const unsigned char* src = (const unsigned char*)srcptr;
    if (dst < src) {
        for (size_t i = 0; i < size; i++)
            dst[i] = src[i];
    } else {
        for (size_t i = size; i != 0; i--)
            dst[i - 1] = src[i - 1];
    }
    return dstptr;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
    return 0;
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = (const unsigned char*)s;
    while (n--) {
        if (*p == (unsigned char)c) return (void*)p;
        p++;
    }
    return NULL;
}
