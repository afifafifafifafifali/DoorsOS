#include <stdint.h>
#include <stdbool.h>
#include "ps2/io.h"

#define TIMEZONE 6

typedef struct DateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DateTime;

// Helper: read a CMOS register
static uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg | 0x80); // Select register with NMI disable bit
    return inb(0x71);
}

// Convert BCD to binary
static uint8_t bcd2bin(uint8_t val) {
    return (val & 0x0F) + ((val >> 4) * 10);
}

DateTime read_rtc_datetime() {
    DateTime dt;

    asm volatile("cli"); // disable interrupts

    dt.second = bcd2bin(cmos_read(0x00));
    dt.minute = bcd2bin(cmos_read(0x02));
    dt.hour   = bcd2bin(cmos_read(0x04));
    dt.day    = bcd2bin(cmos_read(0x07));
    dt.month  = bcd2bin(cmos_read(0x08));
    dt.year   = 2000 + bcd2bin(cmos_read(0x09)); // Adjust to your century

    asm volatile("sti"); // enable interrupts

    // Optional: apply timezone offset
    dt.hour += TIMEZONE;
    if (dt.hour >= 24) {
        dt.hour -= 24;
        dt.day += 1; // naive, doesn't handle month overflow
    }

    return dt;
}
