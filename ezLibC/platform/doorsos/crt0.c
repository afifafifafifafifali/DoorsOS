#include <_ezLibC_deps.h>
#include <unistd.h>

static void printkk(const char* s) {
    write_full(1, s, strlen(s));
}


extern int main(int argc, char* argv[], char* envp[]);

void _start(int argc, char **argv, char **envp){
    printkk("jumpingpong ");
    int ret = main(argc, argv, envp);

    sys_exit(ret);
}