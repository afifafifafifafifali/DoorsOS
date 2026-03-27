#include <stdint.h>

#define SYS_PRINT_WRITE 67671

static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, 
                                uint64_t arg3, uint64_t arg4, uint64_t arg5, 
                                uint64_t arg6) {
    uint64_t ret;
    register uint64_t r10 asm("r10") = arg4;
    register uint64_t r8  asm("r8")  = arg5;
    register uint64_t r9  asm("r9")  = arg6;
    
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory", "cc"
    );
    return ret;
}

static inline uint64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return syscall(num, arg1, arg2, arg3, 0, 0, 0);
}

static inline uint64_t sys_print(const char* buf, uint64_t count) {
    return syscall3(SYS_PRINT_WRITE, 1, (uint64_t)buf, count);
}

static inline int64_t syscall1(uint64_t num, uint64_t arg1) {
    return syscall(num, arg1, 0, 0, 0, 0, 0);
}

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};

#define SYS_UNAME            63

static inline int64_t sys_uname(struct utsname* buf) {
    return syscall1(SYS_UNAME, (uint64_t)buf);
}

#define SYS_FUCK_YOU      67673
static inline uint64_t sys_fuck_you(){
    return syscall(SYS_FUCK_YOU,11,22,33,44,55,66);
}
static void print_str(const char *s) {
    const char *p = s;
    while(*p) p++;
    sys_print(s, p - s);
}

static void print_int(int num) {
    if (num == 0) {
        sys_print("0", 1);
        return;
    }
    
    char buf[20];
    int i = 0;
    int is_negative = 0;
    
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    if (is_negative) {
        buf[i++] = '-';
    }
    
    // Reverse
    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = tmp;
    }
    
    sys_print(buf, i);
}

void main_program(int argc, char **argv) {
    for(int i = 0; i < argc; i++) {
        print_str("argv[");
        print_int(i);
        print_str("] = ");
        print_str(argv[i]);
        print_str("\n");
    }
}

void print_uname() {
    struct utsname u;
    uint64_t ligma = sys_uname(&u);
    if ( ligma == 0) {
        print_str("sysname: ");
        print_str(u.sysname);
        print_str("\n");

        print_str("nodename: ");
        print_str(u.nodename);
        print_str("\n");

        print_str("release: ");
        print_str(u.release);
        print_str("\n");

        print_str("version: ");
        print_str(u.version);
        print_str("\n");

        print_str("machine: ");
        print_str(u.machine);
        print_str("\n");
    } else {
        print_str("sys_uname failed\n");
    }
}
void _start(int argc, char **argv, char **envp) {
    const char msg[] = "Hello, DoorsOS! HI FROM C FILE!\n";
    char *msgfake =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam hendrerit "
      "nulla eget imperdiet varius. Cras at accumsan orci, non sodales eros. "
      "Aenean tincidunt tellus justo, eu vulputate dui eleifend sit amet. Sed "
      "eu nunc volutpat, scelerisque libero in, euismod enim. Orci varius "
      "natoque penatibus et magnis dis parturient montes, nascetur ridiculus "
      "mus. Maecenas efficitur accumsan enim, in tempus justo dignissim ac. "
      "Donec aliquam dignissim volutpat. Praesent mattis dui ac odio mattis "
      "luctus. Sed condimentum consectetur tempus. Sed vestibulum erat eget "
      "pellentesque pharetra. Proin et luctus metus.\n"
      "\n"
      "Donec malesuada ipsum tellus, eu consequat odio ullamcorper non. Proin "
      "cursus nec dolor vel porta. Aenean ac velit nisi. Proin nibh libero, "
      "tincidunt nec blandit nec, porttitor quis massa. Praesent pellentesque "
      "lectus eu orci malesuada, quis ultrices tellus malesuada. Curabitur eu "
      "tristique diam. Proin finibus nisi ligula, ut posuere diam elementum "
      "a.\n"
      "\n"
      "Praesent luctus venenatis dui eget pulvinar. Praesent justo urna, "
      "convallis eu fermentum vitae, lobortis id quam. Aliquam non dolor "
      "finibus, laoreet mauris consectetur, maximus neque. Aliquam sit amet "
      "commodo enim. Sed tempor pulvinar felis, sit amet faucibus neque "
      "fermentum sed. Nulla et euismod nunc, at bibendum odio. Nam neque "
      "justo, sagittis ut nulla hendrerit, semper varius nisi. Donec nec "
      "mattis neque. Cras ultrices ipsum sed lectus sollicitudin pellentesque. "
      "Duis maximus ligula magna. Sed aliquet dictum mi.\n"
      "\n"
      "Vestibulum sit amet dolor eu turpis venenatis vulputate. Vestibulum sed "
      "aliquet libero. Fusce vestibulum nisi turpis, ac molestie est sagittis "
      "non. Nunc eu enim odio. Maecenas id felis neque. Cras luctus metus vel "
      "orci tempor tempor. Sed nec erat lacus. Sed ultricies varius elit ac "
      "blandit. Vestibulum ut rutrum lorem. Fusce varius, dolor at malesuada "
      "pretium, nulla nisi tempus dolor, in vestibulum sem ante efficitur "
      "erat. Phasellus sed velit id justo egestas porta ut et massa.\n"
      "\n"
      "Aenean purus felis, semper a dapibus id, posuere ac est. Morbi in "
      "pulvinar ligula. Sed ullamcorper sapien nec nulla sollicitudin "
      "sollicitudin. In viverra enim quis turpis facilisis, non mollis metus "
      "mollis. Donec sed eleifend mi. Duis ultricies odio ex, ultrices mattis "
      "ipsum tempor et. Morbi rhoncus nulla sit amet arcu pulvinar bibendum. "
      "Integer sed tellus faucibus, feugiat magna ac, luctus arcu. Phasellus "
      "ultrices finibus nisi, in rutrum ante eleifend ac. Duis mi sapien, "
      "rhoncus ac enim id, molestie imperdiet sem. Ut id tortor in ligula "
      "viverra dignissim. Donec purus risus, blandit sed est nec, ullamcorper "
      "vestibulum ipsum. Etiam pharetra feugiat facilisis.\n";
    
    print_uname();
    sys_print(msg, sizeof(msg) - 1);
    print_str(msgfake);
    main_program(argc, argv);

    print_str("Environment variables:\n");
    if (!envp || !envp[0]) {
        print_str("  <none>\n");
    } else {
        for (int i = 0; envp[i]; i++) {
            print_str("envp[");
            print_int(i);
            print_str("] = ");
            print_str(envp[i]);
            print_str("\n");
        }
    }
    sys_fuck_you();
}

// gcc -nostdlib -nodefaultlibs -fno-stack-protector --save-temps -fPIC -fPIE -Wl,-e,_start -o test_add hello.c