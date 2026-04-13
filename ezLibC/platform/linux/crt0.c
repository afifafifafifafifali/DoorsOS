#include <_ezLibC_deps.h>

extern int main(int argc, char* argv[], char* envp[]);

_Noreturn void _start(void) {
    char **sp;

    __asm__ volatile("mov %%rsp, %0" : "=r"(sp));

    int argc = (int)(long)sp[0];
    char **argv = &sp[1];
    char **envp = argv + argc + 1;

    // Align stack to 16 bytes before calling main
    __asm__ volatile (
        "andq $-16, %%rsp\n"
        :
        :
        : "rsp"
    );

    int ret = main(argc, argv, envp);

    sys_exit(ret);

    __builtin_unreachable();
}