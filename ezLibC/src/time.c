/* time.c for ezLibC */

#include <time.h>
#include <unistd.h>
#include <_ezLibC_deps.h>
#include <string.h>

/* ==================================================================
 * time()
 * ================================================================== */
time_t time(time_t* t) {
    struct timeval tv;
    struct timezone tz;
    if (sys_gettimeofday(&tv, &tz) != 0) return (time_t)-1;
    if (t) *t = (time_t)tv.tv_sec;
    return (time_t)tv.tv_sec;
}

/* ==================================================================
 * difftime()
 * ================================================================== */
double difftime(time_t t1, time_t t0) {
    return (double)(t1 - t0);
}

/* ==================================================================
 * clock() — stub (needs kernel support for CLOCK_PROCESS_CPUTIME_ID)
 * ================================================================== */
clock_t clock(void) {
    return (clock_t)-1;
}

/* ==================================================================
 * Helpers: days in month, leap year
 * ================================================================== */

static int is_leap(int year) {
    /* year is years since 1900 */
    int y = year + 1900;
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int month_days(int month, int year) {
    if (month == 1 && is_leap(year)) return 29;
    return days_in_month[month];
}

/* ==================================================================
 * gmtime_r — UTC time from epoch
 * ================================================================== */

struct tm* gmtime_r(const time_t* restrict t, struct tm* restrict result) {
    time_t secs = *t;
    int64_t days = secs / 86400;
    int rem = (int)(secs % 86400);
    if (rem < 0) { rem += 86400; days--; }

    /* 1970-01-01 was a Thursday (wday = 4) */
    result->tm_wday = (int)((days + 4) % 7);
    if (result->tm_wday < 0) result->tm_wday += 7;

    result->tm_hour = rem / 3600;
    rem %= 3600;
    result->tm_min  = rem / 60;
    result->tm_sec  = rem % 60;

    /* Compute year/month/day from days since epoch */
    int year = 70;  /* 1970 */
    int yday = 0;

    while (1) {
        int ylen = is_leap(year) ? 366 : 365;
        if (days < ylen) break;
        days -= ylen;
        year++;
    }

    result->tm_year = year;
    result->tm_yday = (int)days;

    int month = 0;
    while (1) {
        int mlen = month_days(month, year);
        if (days < mlen) break;
        days -= mlen;
        month++;
    }

    result->tm_mon  = month;
    result->tm_mday = (int)days + 1;
    result->tm_isdst = 0;

    return result;
}

struct tm* gmtime(const time_t* t) {
    static struct tm result;
    return gmtime_r(t, &result);
}

/* ==================================================================
 * localtime — same as gmtime (no timezone support yet)
 * ================================================================== */

struct tm* localtime(const time_t* t) {
    return gmtime(t);
}

struct tm* localtime_r(const time_t* restrict t, struct tm* restrict result) {
    return gmtime_r(t, result);
}

/* ==================================================================
 * mktime — broken-down time to time_t (UTC assumed)
 * ================================================================== */

time_t mktime(struct tm* tm) {
    int year = tm->tm_year;
    int month = tm->tm_mon;
    int mday = tm->tm_mday;
    int hour = tm->tm_hour;
    int min  = tm->tm_min;
    int sec  = tm->tm_sec;

    /* Normalize seconds/minutes/hours */
    min  += sec / 60; sec %= 60;
    if (sec < 0) { sec += 60; min--; }
    hour += min / 60; min %= 60;
    if (min < 0) { min += 60; hour--; }
    mday += hour / 24; hour %= 24;
    if (hour < 0) { hour += 24; mday--; }

    /* Normalize month/day */
    while (month < 0) { month += 12; year--; }
    while (month > 11)  { month -= 12; year++; }

    while (mday < 1) {
        month--;
        if (month < 0) { month += 12; year--; }
        mday += month_days(month, year);
    }

    while (mday > month_days(month, year)) {
        mday -= month_days(month, year);
        month++;
        if (month > 11) { month -= 12; year++; }
    }

    tm->tm_year = year;
    tm->tm_mon  = month;
    tm->tm_mday = mday;
    tm->tm_hour = hour;
    tm->tm_min  = min;
    tm->tm_sec  = sec;

    /* Days since epoch */
    int64_t days = 0;
    for (int y = 70; y < year; y++)
        days += is_leap(y) ? 366 : 365;
    for (int m = 0; m < month; m++)
        days += month_days(m, year);
    days += mday - 1;

    /* Wday and yday */
    tm->tm_wday = (int)((days + 4) % 7);
    tm->tm_yday = (int)(days % 365);
    tm->tm_isdst = 0;

    return days * 86400 + hour * 3600 + min * 60 + sec;
}

/* ==================================================================
 * asctime_r / asctime — "Wed Jun 30 21:49:08 1993\n"
 * ================================================================== */

char* asctime_r(const struct tm* restrict tm, char* restrict buf) {
    static const char* day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* mon_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char* p = buf;
    /* Day name */
    *p++ = day_names[tm->tm_wday][0]; *p++ = day_names[tm->tm_wday][1];
    *p++ = day_names[tm->tm_wday][2]; *p++ = ' ';
    /* Month name */
    *p++ = mon_names[tm->tm_mon][0]; *p++ = mon_names[tm->tm_mon][1];
    *p++ = mon_names[tm->tm_mon][2]; *p++ = ' ';
    /* Day */
    if (tm->tm_mday < 10) { *p++ = ' '; *p++ = '0' + tm->tm_mday; }
    else { *p++ = '0' + tm->tm_mday / 10; *p++ = '0' + tm->tm_mday % 10; }
    *p++ = ' ';
    /* HH:MM:SS */
    int h = tm->tm_hour;
    *p++ = '0' + h / 10; *p++ = '0' + h % 10; *p++ = ':';
    int m = tm->tm_min;
    *p++ = '0' + m / 10; *p++ = '0' + m % 10; *p++ = ':';
    int s = tm->tm_sec;
    *p++ = '0' + s / 10; *p++ = '0' + s % 10; *p++ = ' ';
    /* Year */
    int yr = tm->tm_year + 1900;
    *p++ = '0' + (yr / 1000) % 10;
    *p++ = '0' + (yr / 100) % 10;
    *p++ = '0' + (yr / 10) % 10;
    *p++ = '0' + yr % 10;
    *p++ = '\n';
    *p = '\0';
    return buf;
}

char* asctime(const struct tm* tm) {
    static char buf[26];
    return asctime_r(tm, buf);
}

char* ctime(const time_t* t) {
    struct tm tm;
    gmtime_r(t, &tm);
    return asctime(&tm);
}

char* ctime_r(const time_t* restrict t, char* restrict buf) {
    struct tm tm;
    gmtime_r(t, &tm);
    return asctime_r(&tm, buf);
}

/* ==================================================================
 * strftime — format time into string
 * ================================================================== */

size_t strftime(char* s, size_t max, const char* fmt, const struct tm* tm) {
    static const char* day_names[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                                      "Thursday", "Friday", "Saturday"};
    static const char* day_names_short[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* mon_names[] = {"January", "February", "March", "April", "May", "June",
                                      "July", "August", "September", "October", "November", "December"};
    static const char* mon_names_short[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    char buf[64];
    int len = 0;
    size_t total = 0;

    while (*fmt && total < max - 1) {
        if (*fmt != '%') {
            s[total++] = *fmt++;
            continue;
        }
        fmt++;
        char spec = *fmt;
        if (spec) fmt++;

        int wrote = 0;
        switch (spec) {
            case '%':  buf[0] = '%'; wrote = 1; break;
            case 'Y':  wrote = sprintf(buf, "%04d", tm->tm_year + 1900); break;
            case 'y':  wrote = sprintf(buf, "%02d", tm->tm_year % 100); break;
            case 'm':  wrote = sprintf(buf, "%02d", tm->tm_mon + 1); break;
            case 'd':  wrote = sprintf(buf, "%02d", tm->tm_mday); break;
            case 'H':  wrote = sprintf(buf, "%02d", tm->tm_hour); break;
            case 'I':  wrote = sprintf(buf, "%02d", (tm->tm_hour % 12 == 0) ? 12 : tm->tm_hour % 12); break;
            case 'M':  wrote = sprintf(buf, "%02d", tm->tm_min); break;
            case 'S':  wrote = sprintf(buf, "%02d", tm->tm_sec); break;
            case 'p':  wrote = sprintf(buf, "%s", tm->tm_hour < 12 ? "AM" : "PM"); break;
            case 'A':  strcpy(buf, day_names[tm->tm_wday]); wrote = (int)strlen(buf); break;
            case 'a':  strcpy(buf, day_names_short[tm->tm_wday]); wrote = (int)strlen(buf); break;
            case 'B':  strcpy(buf, mon_names[tm->tm_mon]); wrote = (int)strlen(buf); break;
            case 'b': case 'h': strcpy(buf, mon_names_short[tm->tm_mon]); wrote = (int)strlen(buf); break;
            case 'j':  wrote = sprintf(buf, "%03d", tm->tm_yday + 1); break;
            case 'U': case 'W': /* week number — not implemented */ wrote = sprintf(buf, "00"); break;
            case 'c':  wrote = sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
                         tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                         tm->tm_hour, tm->tm_min, tm->tm_sec); break;
            case 'x':  wrote = sprintf(buf, "%02d/%02d/%04d", tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900); break;
            case 'X':  wrote = sprintf(buf, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec); break;
            case 'Z':  buf[0] = '\0'; wrote = 0; break;  /* no timezone */
            case 'r':  wrote = sprintf(buf, "%02d:%02d:%02d %s",
                         (tm->tm_hour % 12 == 0) ? 12 : tm->tm_hour % 12,
                         tm->tm_min, tm->tm_sec, tm->tm_hour < 12 ? "AM" : "PM"); break;
            case 'n':  buf[0] = '\n'; wrote = 1; break;
            case 't':  buf[0] = '\t'; wrote = 1; break;
            default:   buf[0] = '%'; buf[1] = spec; wrote = 2; break;
        }

        if (total + (size_t)wrote >= max) break;
        for (int i = 0; i < wrote; i++) s[total++] = buf[i];
    }

    s[total] = '\0';
    return total;
}
