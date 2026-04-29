#include <unistd.h>
#include <_ezLibC_deps.h>
#include "kbio.h"

int kbhit(void) {
    char c;
    return read(FD_KBIO_EVENTS, &c, 1);
}

int getch(void) {
    char c;
    while (read(FD_KBIO_EVENTS, &c, 1) != 1) {}
    return (int)(unsigned char)c;
}

int getche(void) {
    int c = getch();
    if (c >= 32 || c == '\n' || c == '\r' || c == '\b') {
        write(1, &c, 1);
    }
    return c;
}

int _kbio_get_arrow_nb(void) {
    char c = getch();
    if (c == '\033') {
        char seq = getch();
        if (seq == '[') {
            char arrow = getch();
            switch (arrow) {
                case 'A': return KBIO_KEY_UP;
                case 'B': return KBIO_KEY_DOWN;
                case 'C': return KBIO_KEY_RIGHT;
                case 'D': return KBIO_KEY_LEFT;
            }
        }
    }
    return c;
}