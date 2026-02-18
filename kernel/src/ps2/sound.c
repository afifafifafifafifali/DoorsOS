
/*#include "sound.h"
#include "../ps2/io.h"
#include "../gfx/serial_io.h"

// PC speaker / PIT ports
#define PIT_FREQ    1193180
#define PIT_CMD     0x43
#define PIT_CHANNEL2 0x42
#define SPEAKER_CTRL 0x61

// Simple delay loop
static void delay_ms(uint16_t ms) {
    for (volatile uint64_t i = 0; i < ms * 5000; i++);
}

// Initialize sound system (dummy for now)
void sound_init(void) {
    serial_io_printf("Sound system initialized (polling mode)\n");
}

// Blocking beep via PIT + PC speaker
void sound_play(uint16_t frequency, uint16_t duration_ms) {
    if (frequency == 0) return;

    // Calculate divisor
    uint16_t divisor = PIT_FREQ / frequency;

    // Send command to PIT channel 2
    outb(PIT_CMD, 0xB6);                 // channel 2, mode 3, binary
    outb(PIT_CHANNEL2, divisor & 0xFF);  // low byte
    outb(PIT_CHANNEL2, (divisor >> 8));  // high byte

    // Enable speaker
    uint8_t tmp = inb(SPEAKER_CTRL);
    if (!(tmp & 3)) {
        outb(SPEAKER_CTRL, tmp | 3);
    }

    // Wait while tone plays
    delay_ms(duration_ms);

    // Disable speaker
    tmp = inb(SPEAKER_CTRL);
    outb(SPEAKER_CTRL, tmp & ~3);
}*/

#include <stdint.h>
#include "io.h"
#include "../interrupts/timer.h" // for timer_sleep_ms

// PC speaker ports
#define SPEAKER_PORT 0x61
#define PIT_FREQ_PORT 0x42
#define PIT_CMD_PORT 0x43

static inline void pit_set_freq(uint32_t freq) {
    if (freq == 0) return;

    uint16_t divisor = 1193180 / freq;
    outb(PIT_CMD_PORT, 0xB6); // Channel 2, lo/hi byte, mode 3 square wave
    outb(PIT_FREQ_PORT, divisor & 0xFF);
    outb(PIT_FREQ_PORT, (divisor >> 8) & 0xFF);
}

static inline void speaker_on(void) {
    uint8_t tmp = inb(SPEAKER_PORT);
    if (!(tmp & 3))
        outb(SPEAKER_PORT, tmp | 3); // Enable speaker
}

 void speaker_off(void) {
    uint8_t tmp = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, tmp & ~3); // Disable speaker

    // Reset PIT channel 2 to stop any remaining square wave
    outb(0x43, 0xB6);  // channel 2, lobyte/hibyte, mode 3
    outb(0x42, 0);     // LSB = 0
    outb(0x42, 0);     // MSB = 0
}

void sound_play(uint32_t freq, uint32_t ms) {
    if (freq == 0) return;

    pit_set_freq(freq);
    speaker_on();

    // Use your OS timer for accurate delay
    timer_sleep_ms(ms);

    speaker_off();
}

void sound_init(void) {
    // Nothing needed for PC speaker
}