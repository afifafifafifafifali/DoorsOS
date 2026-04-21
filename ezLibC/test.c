#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdio.h>
#include <_ezLibC_deps.h>

int kgetc(void) {
    char c;
    while (1) {
        int n = read(0, &c, 1);
        if (n > 0) return (int)c;
    }
}

void kputc(char c) {
    write(1, &c, 1);
}
int read_line(char* buf, int max_len) {
    int total = 0;

    while (total < max_len - 1) {
        char c = (char)kgetc();

        if (c == '\r') continue;

        if (c == '\n') {
            kputc('\n');
            buf[total++] = '\n';
            break;
        }

        if (c == 0x08 || c == 127) {
            if (total > 0) {
                total--;
                kputc('\b');
                kputc(' ');
                kputc('\b');
            }
            continue;
        }

        if (c >= 32 && c <= 126) {
            buf[total++] = c;
            kputc(c);
        }
    }

    buf[total] = '\0';
    return total;
}
int main(){
    double rizz = 0.123456789123657 / 2.00;
    printf("FUCK YOURSELF NIGGA! %f\n",rizz);
    char c[1000]; read_line(c,999);
    printf("%s\n",c);

    
    return 0;
}