#include "datandtime.h"

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
