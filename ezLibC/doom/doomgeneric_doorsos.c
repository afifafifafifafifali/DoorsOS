#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// ============================================================
// Syscall numbers
// ============================================================
#define SYS_READ          0
#define SYS_WRITE         1
#define SYS_OPEN          2
#define SYS_CLOSE         3
#define SYS_IOCTL         16
#define SYS_TICKS      686769


#define FBIOGET_INFO      0x4601
#define FD_KBIO_EVENTS    3

// ============================================================
// Framebuffer info
// ============================================================
struct fb_info {
    uint64_t addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;

    uint16_t bpp;
    uint8_t memory_model;

    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;

    uint8_t unused[7];
} __attribute__((packed));


// ============================================================
// Globals
// ============================================================
static uint32_t* FrameBuffer = 0;
static uint32_t s_ScreenWidth = 0;
static uint32_t s_ScreenHeight = 0;
static uint32_t s_Pitch = 0;

static unsigned int s_PositionX = 0;
static unsigned int s_PositionY = 0;

// ============================================================
// Keyboard queue
// ============================================================
#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

// ============================================================
// Key conversion
// ============================================================
static unsigned char convertToDoomKey(unsigned char scancode)
{
    printf("[KEY] scancode=%x\n", scancode);

    switch (scancode)
    {
        case 0x1C: return KEY_ENTER;
        case 0x01: return KEY_ESCAPE;
        case 0x4B: return KEY_LEFTARROW;
        case 0x4D: return KEY_RIGHTARROW;
        case 0x48: return KEY_UPARROW;
        case 0x50: return KEY_DOWNARROW;
        case 0x1D: return KEY_FIRE;
        case 0x39: return KEY_USE;
        case 0x2A:
        case 0x36: return KEY_RSHIFT;
        default: return 0;
    }
}

static void addKeyToQueue(int pressed, unsigned char keyCode)
{
    printf("[KEYQ] pressed=%d code=%x\n", pressed, keyCode);

    unsigned char key = convertToDoomKey(keyCode);

    if (key == 0 && keyCode >= 32 && keyCode <= 126)
        key = keyCode;

    unsigned short data = (pressed << 8) | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = data;
    printf("[KEYQ] write idx=%u val=%x\n", s_KeyQueueWriteIndex, data);

    s_KeyQueueWriteIndex = (s_KeyQueueWriteIndex + 1) % KEYQUEUE_SIZE;
}

// ============================================================
// Input
// ============================================================
static void handleKeyInput()
{
    unsigned char c;
    int n;

    while ((n = sys_read(FD_KBIO_EVENTS, &c, 1)) > 0)
    {
        printf("[INPUT] raw=%x n=%d\n", c, n);

        unsigned char release = c & 0x80;
        c &= 0x7F;

        if (!release)
            addKeyToQueue(1, c);
        else
            addKeyToQueue(0, c);
    }
}

// ============================================================
// INIT
// ============================================================
void DG_Init()
{
    printf("[DG] Init start\n");

    int fb = sys_open("/dev/fb0", O_RDWR, 0);
    printf("[DG] fb fd=%d\n", fb);

    if (fb < 0)
    {
        printf("[DG] fb open FAILED\n");
        return;
    }

    struct fb_info info;
    int r = sys_ioctl(fb, FBIOGET_INFO, (uint64_t)&info);
    printf("[DG] ioctl ret=%d\n", r);

    if (r != 0)
    {
        printf("[DG] ioctl FAILED\n");
        return;
    }

    printf("[DG] FB addr=%p w=%llu h=%llu pitch=%llu bpp=%u\n",
        (void*)info.addr,
        info.width,
        info.height,
        info.pitch,
        info.bpp);

    FrameBuffer = (uint32_t*)info.addr;
    s_ScreenWidth = info.width;
    s_ScreenHeight = info.height;
    s_Pitch = info.pitch / 4;

    printf("[DG] Pitch=%u\n", s_Pitch);

    int argPosX = M_CheckParmWithArgs("-posx", 1);
    if (argPosX > 0)
        s_PositionX = atoi(myargv[argPosX + 1]);

    int argPosY = M_CheckParmWithArgs("-posy", 1);
    if (argPosY > 0)
        s_PositionY = atoi(myargv[argPosY + 1]);

    printf("[DG] posX=%u posY=%u\n", s_PositionX, s_PositionY);
}

// ============================================================
// DRAW
// ============================================================
void DG_DrawFrame()
{
    printf("[DRAW] frame start\n");

    if (FrameBuffer)
    {
        for (int y = 0; y < DOOMGENERIC_RESY; y++)
        {
            uint32_t* dst =
                FrameBuffer +
                (y + s_PositionY) * s_Pitch +
                s_PositionX;

            uint32_t* src =
                (uint32_t*)(DG_ScreenBuffer + y * DOOMGENERIC_RESX);

            for (int x = 0; x < DOOMGENERIC_RESX; x++)
                dst[x] = src[x];
        }
    }
    else
    {
        printf("[DRAW] FrameBuffer NULL\n");
    }

    handleKeyInput();
}

// ============================================================
// TIMING
// ============================================================
uint32_t DG_GetTicksMs()
{
    uint32_t t = (uint32_t)(sys_getticks() * 10);
    printf("[TIME] ms=%u\n", t);
    return t;
}

void DG_SleepMs(uint32_t ms)
{
    printf("[SLEEP] %u ms\n", ms);

    uint32_t start = DG_GetTicksMs();

    while ((DG_GetTicksMs() - start) < ms)
        __asm__ volatile("pause");
}


#include <stddef.h>

/* =========================
   atoi / atof
   ========================= */

int atoi(const char *s) {
    int sign = 1, res = 0;

    while (*s == ' ' || *s == '\t') s++;

    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;

    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }

    return sign * res;
}

double atof(const char *s) {
    double res = 0.0, div = 10.0;
    int sign = 1;

    while (*s == ' ' || *s == '\t') s++;

    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;

    while (*s >= '0' && *s <= '9') {
        res = res * 10.0 + (*s - '0');
        s++;
    }

    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9') {
            res += (*s - '0') / div;
            div *= 10.0;
            s++;
        }
    }

    return sign * res;
}

/* =========================
   abs
   ========================= */

int abs(int x) {
    return (x < 0) ? -x : x;
}

/* =========================
   system (stub)
   ========================= */

int system(const char *cmd) {
    (void)cmd;
    return -1; // no shell in your OS
}

/* =========================
   remove / rename (stubs)
   ========================= */

int remove(const char *path) {
    (void)path;
    return 0;
}

int rename(const char *oldp, const char *newp) {
    (void)oldp;
    (void)newp;
    return 0;
}

// ============================================================
// INPUT API
// ============================================================
int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
        return 0;

    unsigned short data = s_KeyQueue[s_KeyQueueReadIndex];

    printf("[GETKEY] idx=%u val=%x\n", s_KeyQueueReadIndex, data);

    s_KeyQueueReadIndex =
        (s_KeyQueueReadIndex + 1) % KEYQUEUE_SIZE;

    *pressed = data >> 8;
    *doomKey = data & 0xFF;

    return 1;
}

// ============================================================
void DG_SetWindowTitle(const char * title)
{
    printf("[TITLE] %s\n", title);
}

// ============================================================
// MAIN
// ============================================================
int main(int argc, char **argv)
{
    printf("[MAIN] start argc=%d\n", argc);

    doomgeneric_Create(argc, argv);

    printf("[MAIN] entering loop\n");

    while (1)
    {
        printf("[MAIN] tick\n");
        doomgeneric_Tick();
    }

    return 0;
}