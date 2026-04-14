/* Minimal time.h for ezLibC */

#ifndef __EZLIBC_TIME_H
#define __EZLIBC_TIME_H 1

#include <_ezLibC_deps.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
typedef long time_t;

struct tm {
    int tm_sec;     /* Seconds [0, 60] */
    int tm_min;     /* Minutes [0, 59] */
    int tm_hour;    /* Hour [0, 23] */
    int tm_mday;    /* Day of month [1, 31] */
    int tm_mon;     /* Month of year [0, 11] */
    int tm_year;    /* Years since 1900 */
    int tm_wday;    /* Day of week [0, 6] (Sunday = 0) */
    int tm_yday;    /* Day of year [0, 365] */
    int tm_isdst;   /* Daylight savings flag */
};

/* Constants */
#define CLOCKS_PER_SEC  1000000L

/* Time acquisition */
time_t      time(time_t* t);
int         gettimeofday(struct timeval* tv, struct timezone* tz);
clock_t     clock(void);
double      difftime(time_t t1, time_t t0);

/* Conversion */
struct tm*  gmtime(const time_t* t);
struct tm*  gmtime_r(const time_t* restrict t, struct tm* restrict result);
struct tm*  localtime(const time_t* t);
struct tm*  localtime_r(const time_t* restrict t, struct tm* restrict result);
time_t      mktime(struct tm* tm);

/* Formatting */
char*       asctime(const struct tm* tm);
char*       asctime_r(const struct tm* restrict tm, char* restrict buf);
char*       ctime(const time_t* t);
char*       ctime_r(const time_t* restrict t, char* restrict buf);
size_t      strftime(char* s, size_t max, const char* fmt, const struct tm* tm);

#ifdef __cplusplus
}
#endif

#endif
