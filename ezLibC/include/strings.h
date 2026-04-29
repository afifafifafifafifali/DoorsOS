#ifndef _STRINGS_H
#define _STRINGS_H

#include <stddef.h>

static char tolower_ascii(char c) {
    if (c >= 'A' && c <= 'Z')
        return c + 32;
    return c;
}

static inline int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = tolower_ascii(*s1);
        char c2 = tolower_ascii(*s2);

        if (c1 != c2)
            return (int)(unsigned char)c1 - (int)(unsigned char)c2;

        s1++;
        s2++;
    }

    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

static inline int strncasecmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && *s2) {
        char c1 = tolower_ascii(*s1);
        char c2 = tolower_ascii(*s2);

        if (c1 != c2)
            return (int)(unsigned char)c1 - (int)(unsigned char)c2;

        s1++;
        s2++;
        n--;
    }

    if (n == 0)
        return 0;

    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

#endif