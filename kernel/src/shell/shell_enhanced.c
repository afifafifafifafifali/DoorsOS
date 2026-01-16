#include "../gfx/printf.h"
#include "../gfx/term.h"
#include "../ps2/kbio.h"
#include "../ps2/io.h"
#include "../libs/string.h"
#include "../storage/storage.h"
#include "../fs/fat32.h"
#include "../fs/ahci_driver.h"
#include "../interrupts/timer.h"
#include "../mem/new/pmm.h"
#include "../user/user_enhanced.h"
#include "../info/meminfo.h"
#include "sghsc_logo.h"
#include "../snake.h"
#include "gui/colorama.h"
#include "gui/windows.h"
#include "../interrupts/test_scheduler.h"

static char current_dir[256] = "/";

static void cmd_help(void) {
    printf("Commands:\n");
    printf("  help, clear, storage, ahcitest, fat32test\n");
    printf("  ls [path]    - List directory\n");
    printf("  cd <path>    - Change directory\n");
    printf("  cat <file>   - Display file\n");
    printf("  mkdir <dir>  - Create directory\n");
    printf("  mkfile <f>   - Create file with content\n");
    printf("  nano <file>  - Edit file\n");
    printf("  rm <file>    - Delete file\n");
    printf("  pwd          - Print working directory\n");
    printf("  snake        - Play snake game\n");
    printf("  testpipe     - Test multitasking & pipes\n");
    printf("  mem, tasks, run <task>, time, reboot\n");
}

static void cmd_clear(void) {
    kprint("\e[2J\e[H");
}

#define COLOR_BLACK 0x000000

void draw_sghsc_logo_exact(int x, int y) {
    for (int j = 0; j < 64; j++) {
        for (int i = 0; i < 64; i++) {
            uint32_t color = sghsc_logo[j][i];
            putPixel(x + i, y + j, color);
        }
    }
}

void clear_screen_original(void) {
    struct limine_framebuffer* fb = framebuffer_request.response->framebuffers[0];
    uint64_t pixels = fb->width * fb->height;
    uint32_t* framebuffer = (uint32_t*) fb->address;

    for (uint64_t i = 0; i < pixels; i++) {
        framebuffer[i] = COLOR_BLACK;
    }
    // Move cursor to top-left
    kprint("\e[2J\e[H");

}

int trigger_div0() {
    int result;
    int x = 1, y = 0;
    __asm__ volatile("idivl %1" : "=a"(result) : "r"(y), "a"(x));
    return result;
}


static void cmd_storage(void) {
    printf("Storage: %s\n", storage_get_type() == STORAGE_AHCI ? "AHCI" : "ATA");
}

static void cmd_ahcitest(void) {
    ahci_test();
}

static void cmd_fat32test(void) {
    fat32_test_complete();
}

static void print_size(const char* label, uint64_t bytes) {
    if (bytes >= 1024ULL * 1024 * 1024) {
        printf("%s: %llu GB\n", label, bytes / (1024ULL * 1024 * 1024));
    } else if (bytes >= 1024 * 1024) {
        printf("%s: %llu MB\n", label, bytes / (1024 * 1024));
    } else if (bytes >= 1024) {
        printf("%s: %llu KB\n", label, bytes / 1024);
    } else {
        printf("%s: %llu B\n", label, bytes);
    }
}

static void cmd_mem(void) {
    struct meminfo info = get_memory_info();
    print_size("Total Memory", info.total_memory);
    print_size("Free Memory", info.free_memory);
    print_size("Used Memory", info.used_memory);
}

static void cmd_time(void) {
    printf("Uptime: %llu seconds\n", timer_get_ticks() / 1000);
}

static void cmd_tasks(void) {
    user_list_tasks();
}

static void cmd_run(const char* name) {
    if (!user_run_task(name)) {
        printf("Unknown task: %s\n", name);
    }
}

static void cmd_reboot(void) {
    printf("Rebooting...\n");
    timer_sleep_ms(1000);
    outb(0x64, 0xFE);
}

static void cmd_pwd(void) {
    printf("%s\n", current_dir);
}

static void cmd_cd(const char* path) {
    if (!path || strlen(path) == 0) {
        strcpy(current_dir, "/");
        fat32_set_cwd("/");
        return;
    }
    
    char new_dir[256];
    
    if (strcmp(path, "..") == 0) {
        strcpy(new_dir, current_dir);
        char* last = strrchr(new_dir, '/');
        if (last && last != new_dir) {
            *last = 0;
        } else {
            strcpy(new_dir, "/");
        }
    } else if (path[0] == '/') {
        strcpy(new_dir, path);
    } else {
        strcpy(new_dir, current_dir);
        if (new_dir[strlen(new_dir)-1] != '/') {
            strcat(new_dir, "/");
        }
        strcat(new_dir, path);
    }
    
    // Root is always valid
    if (strcmp(new_dir, "/") == 0) {
        strcpy(current_dir, new_dir);
        fat32_set_cwd("/");
        return;
    }
    
    // Check if directory exists
    if (!fat32_dir_exists(new_dir)) {
        printf("cd: %s: No such directory\n", path);
        return;
    }
    
    strcpy(current_dir, new_dir);
    fat32_set_cwd(new_dir);
}

static void cmd_ls(const char* path) {
    const char* target = (path && strlen(path) > 0) ? path : current_dir;
    fat32_list_dir(target);
}

static void cmd_cat(const char* filename) {
    if (!filename) {
        printf("Usage: cat <file>\n");
        return;
    }
    
    uint8_t* buf = k_malloc(4096);
    if (!buf) {
        printf("Out of memory\n");
        return;
    }
    
    uint32_t size = 0;
    if (fat32_read_file(filename, buf, &size)) {
        for (uint32_t i = 0; i < size && i < 4096; i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
    } else {
        printf("File not found: %s\n", filename);
    }
    
    k_free(buf);
}

static void cmd_mkdir(const char* dirname) {
    if (!dirname) {
        printf("Usage: mkdir <dir>\n");
        return;
    }
    
    if (fat32_create_directory(dirname)) {
        printf("Created: %s\n", dirname);
    } else {
        printf("Failed to create: %s\n", dirname);
    }
}

static void cmd_rm(const char* filename) {
    if (!filename) {
        printf("Usage: rm <file>\n");
        return;
    }

    if (fat32_delete_file(filename)) {
        printf("Deleted: %s\n", filename);
    } else {
        printf("Failed to delete: %s\n", filename);
    }
}

static void cmd_rmdir(const char* dirname) {
    if (!dirname) {
        printf("Usage: rmdir <directory>\n");
        return;
    }

    if (fat32_delete_directory(dirname)) {
        printf("Removed directory: %s\n", dirname);
    } else {
        printf("Failed to remove directory: %s (may not be empty or not exist)\n", dirname);
    }
}

static void cmd_nano(const char* filename) {
    if (!filename) {
        printf("Usage: nano <file>\n");
        return;
    }
    
    char content[4096];
    int pos = 0;
    content[0] = 0;
    
    // Try to load existing file
    uint32_t size = 0;
    if (fat32_read_file(filename, content, &size) && size > 0 && size < 4000) {
        pos = size;
        content[pos] = 0;
        printf("--- Existing content ---\n%s\n--- End ---\n", content);
    }
    
    printf("Editing: %s\n", filename);
    printf("Commands: :wq = save & quit, :q = quit without saving\n");
    printf("Enter text:\n");

    while (1) {
        printf("> ");
        char line[256];
        char* input = ps2_kbio_read(line, 255);
        if (!input) continue;
        
        if (strcmp(input, ":wq") == 0) {
            k_free(input);
            if (fat32_write_file(filename, (uint8_t*)content, pos)) {
                printf("Saved: %s (%d bytes)\n", filename, pos);
            } else {
                printf("Failed to save\n");
            }
            return;
        }
        if (strcmp(input, ":q") == 0) {
            k_free(input);
            printf("Quit without saving\n");
            return;
        }
        
        int len = strlen(input);
        if (pos + len + 1 < 4096) {
            strcpy(&content[pos], input);
            pos += len;
            content[pos++] = '\n';
            content[pos] = 0;
        }
        
        k_free(input);
    }
}

static void cmd_mkfile(const char* filename) {
    if (!filename) {
        printf("Usage: mkfile <filename>\n");
        return;
    }
    
    printf("Enter contents for %s (type ':end' alone to finish):\n", filename);
    
    char* content = k_malloc(16384);
    if (!content) {
        printf("Memory allocation failed\n");
        return;
    }
    memset(content, 0, 16384);
    size_t content_len = 0;
    
    while (1) {
        printf("> ");
        char line[1024];
        char* entered = ps2_kbio_read(line, 1023);
        if (!entered) {
            printf("Input error\n");
            break;
        }
        printf("\n");
        
        if (strcmp(entered, ":end") == 0) {
            k_free(entered);
            break;
        }
        
        size_t line_len = strlen(entered);
        if (content_len + line_len + 1 >= 16384) {
            printf("File content too long, stopping\n");
            k_free(entered);
            break;
        }
        
        memcpy(content + content_len, entered, line_len);
        content_len += line_len;
        content[content_len++] = '\n';
        content[content_len] = '\0';
        k_free(entered);
    }
    
    printf("Creating file '%s'...\n", filename);
    if (fat32_write_file(filename, (const uint8_t*)content, content_len)) {
        printf("File '%s' created (%zu bytes)\n", filename, content_len);
    } else {
        printf("Failed to create '%s'\n", filename);
    }
    k_free(content);
}

void print_doors_logo() {
    printf("________                             ________    _________\n");
    printf("\\______ \\   ____   ___________  _____\\_____  \\  /   _____/\n");
    printf(" |    |  \\ /  _ \\ /  _ \\_  __ \\/  ___//   |   \\ \\_____  \\ \n");
    printf(" |    `   (  <_> |  <_> )  | \\/\\___ \\/    |    \\/        \\\n");
    printf("/_______  /\\____/ \\____/|__|  /____  >_______  /_______  /\n");
    printf("        \\/                         \\/        \\/        \\/ \n");
}

void shell_run(void) {
    printf("\nDoorsOS Shell v2.0\nCopyright(c),Afif Ali Saadman, 2025 or whatever year it is\n");
    printf("Type 'help' for commands\n\n");
    
    while (1) {
        printf("%s $ ", current_dir);
        
        char input[256];
        char* result = ps2_kbio_read(input, 255);
        if (!result) continue;
        
        printf("\n");
        
        char* cmd = result;
        char* arg = strchr(result, ' ');
        if (arg) {
            *arg = 0;
            arg++;
            while (*arg == ' ') arg++;
        }
        
        if (strlen(cmd) == 0) {
            // Empty
        } else if (strcmp(cmd, "help") == 0) {
            cmd_help();
        } else if (strcmp(cmd, "clear") == 0) {
            cmd_clear();
        } else if (strcmp(cmd, "storage") == 0) {
            cmd_storage();
        } else if (strcmp(cmd, "ahcitest") == 0) {
            cmd_ahcitest();
        } else if (strcmp(cmd, "fat32test") == 0) {
            cmd_fat32test();
        } else if (strcmp(cmd, "mem") == 0) {
            cmd_mem();
        } else if (strcmp(cmd, "time") == 0) {
            cmd_time();
        } else if (strcmp(cmd, "tasks") == 0) {
            cmd_tasks();
        } else if (strcmp(cmd, "run") == 0) {
            cmd_run(arg);
        } else if (strcmp(cmd, "reboot") == 0) {
            cmd_reboot();
        } else if (strcmp(cmd, "pwd") == 0) {
            cmd_pwd();
        } else if (strcmp(cmd, "cd") == 0) {
            cmd_cd(arg);
        } else if (strcmp(cmd, "ls") == 0) {
            cmd_ls(arg);
        } else if (strcmp(cmd,"crash") == 0){
            trigger_div0();
        }
        else if (strcmp(cmd, "cat") == 0) {
            cmd_cat(arg);
        } else if (strcmp(cmd, "mkdir") == 0) {
            cmd_mkdir(arg);
        } else if (strcmp(cmd, "mkfile") == 0) {
            cmd_mkfile(arg);
        } else if (strcmp(cmd, "rm") == 0) {
            cmd_rm(arg);
        } else if (strcmp(cmd, "rmdir") == 0) {
            cmd_rmdir(arg);
        } else if (strcmp(cmd, "nano") == 0) {
            cmd_nano(arg);
        } else if (strcmp(cmd, "snake") == 0) {
            snake_run();
        }else if(strcmp(cmd,"doorsfetch") == 0){
            print_doors_logo();
        } else if (strcmp(cmd, "testpipe") == 0) {
            test_multitasking_pipes();
        } else {
            printf("Unknown: %s\n", cmd);
        }
        
        k_free(result);
    }
}
