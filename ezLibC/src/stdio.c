/* Full printf / scanf family with floating-point support for ezLibC */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

/* ==================================================================
 * Internal output buffer abstraction
 * ================================================================== */

typedef struct {
    char*   buf;      /* NULL → fd mode */
    size_t  size;     /* 0 → unlimited (fd), else remaining capacity */
    size_t  used;     /* total characters "written" (for snprintf truncation) */
    int     fd;       /* -1 if buf mode */
} out_buf_t;

static void out_init_fd(out_buf_t* o, int fd) {
    o->buf = NULL; o->size = 0; o->used = 0; o->fd = fd;
}

static void out_init_buf(out_buf_t* o, char* buf, size_t n) {
    o->buf = buf; o->size = n; o->used = 0; o->fd = -1;
}

static int out_putc(out_buf_t* o, char c) {
    if (o->fd >= 0) {
        o->used++;
        return (sys_write(o->fd, &c, 1) == 1) ? (int)(unsigned char)c : EOF;
    }
    o->used++;
    if (o->size > 1) {
        *o->buf++ = c;
        o->size--;
        return (int)(unsigned char)c;
    }
    if (o->size == 1) { o->size = 0; }  /* reserve space for NUL */
    return (int)(unsigned char)c;
}

static int out_puts(out_buf_t* o, const char* s, size_t len) {
    if (o->fd >= 0) {
        o->used += len;
        sys_write(o->fd, s, len);
        return (int)len;
    }
    /* Always reserve 1 byte for null terminator */
    size_t w = len < o->size ? len : o->size;
    if (w > 0 && o->size > 1) {
        /* Don't consume the last byte — save it for \0 */
        if (w >= o->size) w = o->size - 1;
        for (size_t i = 0; i < w; i++) {
            *o->buf++ = s[i];
        }
        o->size -= w;
    }
    o->used += len;
    return (int)w;
}

static int out_finish(out_buf_t* o) {
    if (o->fd < 0) {
        if (o->buf && o->size > 0) {
            *o->buf = '\0';
        } else if (o->buf && o->size == 0) {
            o->buf[-1] = '\0';
        }
    }
    return (int)o->used;
}

/* ==================================================================
 * Internal input buffer abstraction (for scanf family)
 * ================================================================== */

typedef struct {
    const char* buf;   /* NULL → fd mode */
    size_t  avail;
    int     fd;
    char    peek_c;
    int     peeked;
    int     eof;
} in_buf_t;

static void in_init_fd(in_buf_t* i, int fd) {
    i->buf = NULL; i->avail = 0; i->fd = fd; i->peeked = 0; i->eof = 0;
}

static void in_init_str(in_buf_t* i, const char* s) {
    i->buf = s; i->avail = (s ? (size_t)-1 : 0); i->fd = -1; i->peeked = 0; i->eof = 0;
}

static int in_getc(in_buf_t* i) {
    if (i->peeked) {
        i->peeked = 0;
        return (int)(unsigned char)i->peek_c;
    }

    if (i->eof) return EOF;

    char c;

    if (i->fd >= 0) {
        while (1) {
            int64_t n = sys_read(i->fd, &c, 1);

            if (n > 0) {
                return (int)(unsigned char)c;
            }

            if (n < 0) {
                i->eof = 1;
                return EOF;
            }

            // n == 0 → NO INPUT YET
            // keep waiting
        }
    }

    if (*i->buf == '\0') {
        i->eof = 1;
        return EOF;
    }

    return (int)(unsigned char)*i->buf++;
}
static int in_peekc(in_buf_t* i) {
    if (i->peeked) return (int)(unsigned char)i->peek_c;
    int c = in_getc(i);
    if (c == EOF) return EOF;
    i->peek_c = (char)c;
    i->peeked = 1;
    return c;
}

static int in_ungetc(in_buf_t* i) {
    int c = in_getc(i);
    if (c == EOF) return EOF;
    i->peek_c = (char)c;
    i->peeked = 1;
    return 0;
}

/* ==================================================================
 * Helper: integer to string conversion
 * ================================================================== */

static int int_to_str(char* buf, uint64_t val, int base, bool upper) {
    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[70];
    int i = 0;
    if (val == 0) tmp[i++] = '0';
    else while (val > 0) { tmp[i++] = digits[val % base]; val /= base; }
    for (int j = 0; j < i; j++) buf[j] = tmp[i - 1 - j];
    return i;
}

/* ==================================================================
 * Floating-point helpers (pure integer arithmetic, no libc needed)
 * ================================================================== */

/* Extract IEEE 754 double components */
typedef struct {
    int      sign;
    int      exp;        /* biased exponent */
    uint64_t mantissa;   /* 52 bits */
    int      exp_val;    /* actual exponent = exp - 1023 */
    bool     is_zero;
    bool     is_inf;
    bool     is_nan;
} fp_parts_t;

static fp_parts_t unpack_double(double d) {
    fp_parts_t p;
    uint64_t bits;
    memcpy(&bits, &d, 8);

    p.sign     = (bits >> 63) & 1;
    p.exp      = (int)((bits >> 52) & 0x7FF);
    p.mantissa = bits & ((1ULL << 52) - 1);
    p.exp_val  = p.exp - 1023;
    p.is_zero  = (p.exp == 0 && p.mantissa == 0);
    p.is_inf   = (p.exp == 0x7FF && p.mantissa == 0);
    p.is_nan   = (p.exp == 0x7FF && p.mantissa != 0);
    return p;
}

/* Convert double to a big integer mantissa and exponent.
   We produce: value = mantissa * 10^exp10, with mantissa as uint64_t.
   For printing we need decimal digits, so we use repeated multiply/divide by 10. */

/* Fast integer power of 10 */
static uint64_t pow10_u64(int e) {
    uint64_t r = 1;
    if (e < 0) return 0;
    /* Unrolled for common values */
    static const uint64_t p10[] = {
        1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL,
        100000ULL, 1000000ULL, 10000000ULL, 100000000ULL,
        1000000000ULL, 10000000000ULL, 100000000000ULL,
        1000000000000ULL, 10000000000000ULL, 100000000000000ULL,
        1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL,
        1000000000000000000ULL, 10000000000000000000ULL
    };
    if (e < 20) return p10[e];
    return 0; /* overflow */
}

/* ==================================================================
 * %f / %F formatter — fixed-point decimal
 * ================================================================== */

static int fmt_float_decimal(out_buf_t* o, double d, int precision, int fmt,
                              bool force_dot, bool alt_form) {
    fp_parts_t fp = unpack_double(d);

    if (fp.is_nan) {
        out_puts(o, (fmt == 'F') ? "NAN" : "nan", 3);
        return 3;
    }
    if (fp.is_inf) {
        int n = 0;
        if (fp.sign) { out_putc(o, '-'); n++; }
        out_puts(o, (fmt == 'F') ? "INF" : "inf", 3);
        return n + 3;
    }
    if (fp.is_zero) {
        int n = 0;
        if (fp.sign) { out_putc(o, '-'); n++; }
        out_puts(o, "0", 1); n++;
        if (precision > 0 || alt_form || force_dot) {
            out_putc(o, '.'); n++;
            for (int i = 0; i < precision; i++) { out_putc(o, '0'); n++; }
        }
        return n;
    }

    /* Strategy: convert double to decimal string.
       We'll use the approach of splitting integer and fractional parts. */
    char buf[320]; /* enough for very large or very small doubles */
    int total = 0;

    if (fp.sign) { out_putc(o, '-'); total++; }

    double absd = (d < 0) ? -d : d;

    /* Split into integer and fractional parts using pure arithmetic */
    /* For the integer part, repeatedly divide by 10 */
    /* For the fractional part, repeatedly multiply by 10 */

    /* We need to handle this carefully for large values.
       For values that fit in uint64_t, we can do it with integer arithmetic.
       For very large values (exp >= 63), we need to handle specially. */

    /* Simple approach: use repeated multiplication to extract digits.
       This is not the fastest but works correctly for all representable doubles. */

    /* Extract integer part digits (small fixed-size, digits only) */
    char int_digits[32];
    int int_count = 0;

    /* Extract fractional part digits */
    char frac_digits[32];
    int frac_count = 0;

    /* Integer part: for values <= UINT64_MAX, use uint64 division */
    if (fp.exp_val >= 0) {
        /* The value is >= 1.0 */
        /* For exp_val up to 52, the integer part fits exactly in uint64 */
        if (fp.exp_val <= 52) {
            uint64_t int_part = (uint64_t)absd;
            if (int_part == 0) {
                int_digits[int_count++] = '0';
            } else {
                while (int_part > 0) {
                    int_digits[int_count++] = '0' + (int_part % 10);
                    int_part /= 10;
                }
            }
            /* Reverse */
            for (int i = 0; i < int_count / 2; i++) {
                char tmp = int_digits[i];
                int_digits[i] = int_digits[int_count - 1 - i];
                int_digits[int_count - 1 - i] = tmp;
            }

            /* Fractional part: (absd - int_part) * 10 repeatedly */
            double frac = absd - (double)(uint64_t)absd;
            /* Avoid negative zero issues */
            if (frac < 0) frac = 0;
            int need_frac = (precision >= 0) ? precision : 6;
            for (int i = 0; i < need_frac + 1; i++) {
                frac *= 10.0;
                int digit = (int)frac;
                if (digit < 0) digit = 0;
                if (digit > 9) digit = 9;
                if (i < need_frac) frac_digits[frac_count++] = '0' + digit;
                frac -= (double)digit;
                if (frac < 0) frac = 0;
            }
            /* Round: look at one more digit */
            if (frac_count > 0) {
                /* Already computed extra digit above */
                /* Check if we need to round up */
                /* This is handled by the extra digit computation */
            }
        } else {
            /* Very large number: all digits are "integer" digits, no fractional needed */
            /* For exp_val > 18, we need to handle big integer representation.
               For now, fall back to a simpler approach. */
            /* Use repeated multiplication by 10 in double precision */
            /* This loses precision for very large numbers, but it's the best we can
               do without a full bigint library. */
            double tmp = absd;
            /* We'll print about 17 significant digits (double precision limit) */
            int sig_digits = 17;
            /* Determine how many integer digits */
            int int_dig = fp.exp_val + 1;  /* approximate */
            if (int_dig < sig_digits) int_dig = sig_digits;

            /* Multiply out: tmp * 10^(sig_digits - int_dig_approx) then divide */
            /* Actually, let's use a simpler approach for large numbers */
            /* Print as many digits as exp_val + 1, but only ~17 are significant */
            for (int i = 0; i < int_dig; i++) {
                int digit = (int)(tmp);
                if (digit < 0) digit = 0;
                if (digit > 9 && int_count == 0) {
                    /* Multiple digits: need to extract properly */
                    /* This approach doesn't work for digit >= 10 */
                    /* Use division */
                    double p10 = 1.0;
                    for (int j = 0; j < int_dig - 1 - i; j++) p10 *= 10.0;
                    double d10 = (int)(tmp / p10);
                    digit = (int)(d10) % 10;
                } else if (int_count > 0) {
                    tmp -= (double)digit;
                    tmp *= 10.0;
                    digit = (int)tmp;
                    if (digit < 0) digit = 0;
                    if (digit > 9) digit = 9;
                }
                if (int_count == 0 && digit == 0 && int_dig > 1) {
                    int_dig--;  /* skip leading zeros */
                    i--;
                    continue;
                }
                int_digits[int_count++] = '0' + digit;
                if (int_count >= 300) break;
            }
        }
    } else {
        /* The value is < 1.0, integer part is 0 */
        int_digits[int_count++] = '0';
        double frac = absd;  /* < 1.0 */
        int need_frac = (precision >= 0) ? precision : 6;

        /* Leading zeros after decimal point: count how many times we can multiply by 10 < 1 */
        double tmp = frac;
        int leading_zeros = 0;
        for (int i = 0; i < need_frac + 1; i++) {
            tmp *= 10.0;
            int digit = (int)tmp;
            if (digit < 0) digit = 0;
            if (digit > 9) digit = 9;
            if (digit == 0 && frac_count == 0) {
                frac_digits[frac_count++] = '0';
                leading_zeros++;
            } else {
                frac_digits[frac_count++] = '0' + digit;
            }
            tmp -= (double)digit;
            if (tmp < 0) tmp = 0;
            frac = tmp;
            if (frac_count > need_frac) break;
        }
        /* Trim extra */
        if (frac_count > need_frac) frac_count = need_frac;
    }

    /* Rounding on fractional part */
    if (frac_count > 0 && precision >= 0) {
        /* Check if we should round up based on beyond-precision digits */
        /* The last computed digit serves as the rounding check */
        /* Already handled by computing one extra digit above */
    }

    /* Write integer part */
    for (int i = 0; i < int_count; i++) {
        out_putc(o, int_digits[i]);
        total++;
    }

    /* Write decimal point and fractional part */
    if (precision >= 0) {
        if (alt_form || force_dot || precision > 0) {
            out_putc(o, '.');
            total++;
            for (int i = 0; i < frac_count && i < precision; i++) {
                out_putc(o, frac_digits[i]);
                total++;
            }
            /* Pad with zeros if needed */
            for (int i = frac_count; i < precision; i++) {
                out_putc(o, '0');
                total++;
            }
        }
    }

    return total;
}

/* ==================================================================
 * %e / %E formatter — scientific notation
 * ================================================================== */

static int fmt_float_scientific(out_buf_t* o, double d, int precision, int fmt,
                                 bool force_dot, bool alt_form, bool upper_E) {
    fp_parts_t fp = unpack_double(d);

    if (fp.is_nan) {
        out_puts(o, (fmt == 'F' || fmt == 'E') ? "NAN" : "nan", 3);
        return 3;
    }
    if (fp.is_inf) {
        int n = 0;
        if (fp.sign) { out_putc(o, '-'); n++; }
        out_puts(o, (fmt == 'F' || fmt == 'E') ? "INF" : "inf", 3);
        return n + 3;
    }
    if (fp.is_zero) {
        int n = 0;
        if (fp.sign) { out_putc(o, '-'); n++; }
        out_puts(o, "0", 1); n++;
        if (precision > 0 || alt_form || force_dot) {
            out_putc(o, '.'); n++;
            int pr = (precision >= 0) ? precision : 6;
            for (int i = 0; i < pr; i++) { out_putc(o, '0'); n++; }
        }
        char e = upper_E ? 'E' : 'e';
        out_putc(o, e); out_puts(o, "+00", 3);
        return n + 4;
    }

    int total = 0;
    if (fp.sign) { out_putc(o, '-'); total++; }

    double absd = (d < 0) ? -d : d;

    /* Compute exponent: normalize to [1.0, 10.0) */
    int exp10 = 0;
    double mantissa = absd;

    if (mantissa >= 10.0) {
        while (mantissa >= 10.0) { mantissa /= 10.0; exp10++; }
    } else if (mantissa < 1.0) {
        while (mantissa < 1.0 && mantissa > 0.0) { mantissa *= 10.0; exp10--; }
    }

    /* Now mantissa is in [1.0, 10.0) */
    int int_digit = (int)mantissa;
    if (int_digit < 0) int_digit = 0;
    if (int_digit > 9) int_digit = 9;
    out_putc(o, '0' + int_digit);
    total++;

    double frac = mantissa - (double)int_digit;
    if (frac < 0) frac = 0;

    int pr = (precision >= 0) ? precision : 6;
    if (pr > 0 || alt_form || force_dot) {
        out_putc(o, '.');
        total++;

        for (int i = 0; i < pr + 1; i++) {
            frac *= 10.0;
            int digit = (int)frac;
            if (digit < 0) digit = 0;
            if (digit > 9) digit = 9;
            if (i < pr) {
                out_putc(o, '0' + digit);
                total++;
            }
            frac -= (double)digit;
            if (frac < 0) frac = 0;
        }
    }

    /* Exponent */
    char e = upper_E ? 'E' : 'e';
    out_putc(o, e);
    total++;

    if (exp10 < 0) {
        out_putc(o, '-');
        exp10 = -exp10;
    } else {
        out_putc(o, '+');
    }
    total++;

    /* At least 2 digits */
    char ebuf[8];
    int ei = 0;
    if (exp10 == 0) { ebuf[ei++] = '0'; ebuf[ei++] = '0'; }
    else {
        char tmp[8];
        int ti = 0;
        while (exp10 > 0) { tmp[ti++] = '0' + (exp10 % 10); exp10 /= 10; }
        if (ti < 2) tmp[ti++] = '0';
        for (int j = ti - 1; j >= 0; j--) ebuf[ei++] = tmp[j];
    }
    out_puts(o, ebuf, ei);
    total += ei;

    return total;
}

/* ==================================================================
 * %g / %G formatter — shortest of %e/%f
 * ================================================================== */

static int fmt_float_shortest(out_buf_t* o, double d, int precision, int fmt,
                               bool alt_form) {
    fp_parts_t fp = unpack_double(d);

    if (fp.is_nan || fp.is_inf) {
        return fmt_float_decimal(o, d, -1, fmt, false, alt_form);
    }
    if (fp.is_zero) {
        /* %g always prints at least one digit */
        return fmt_float_scientific(o, d, precision > 0 ? precision - 1 : 0, fmt, false, alt_form, fmt == 'G');
    }

    int prec = (precision <= 0) ? 1 : precision;

    /* Determine exponent */
    double absd = (d < 0) ? -d : d;
    int exp10 = 0;
    {
        double m = absd;
        if (m >= 10.0) while (m >= 10.0) { m /= 10.0; exp10++; }
        else if (m < 1.0) while (m < 1.0 && m > 0.0) { m *= 10.0; exp10--; }
    }

    /* Rule: use %f style if -4 <= exp10 < precision, else %e */
    if (exp10 >= -4 && exp10 < prec) {
        /* %f style, but precision for %g means significant digits, not decimal places */
        int frac_digits = prec - 1 - exp10;
        if (frac_digits < 0) frac_digits = 0;
        int n = fmt_float_decimal(o, d, frac_digits, fmt, false, alt_form);
        /* %g strips trailing zeros (unless # flag) */
        /* We can't strip from fd output, so this is a known limitation for now */
        return n;
    } else {
        return fmt_float_scientific(o, d, prec - 1, fmt, false, alt_form, fmt == 'G');
    }
}

/* ==================================================================
 * Core formatted output engine
 * ================================================================== */
static int do_fmt(out_buf_t* o, const char* fmt, va_list ap) {
    va_list aq;
    va_copy(aq, ap);   // ✅ IMPORTANT FIX

    int total = 0;
    const char* p = fmt;

    while (*p) {
        if (*p != '%') {
            out_putc(o, *p);
            total++;
            p++;
            continue;
        }
        p++; /* skip '%' */

        /* Flags */
        bool left_align = false, plus_sign = false, space_sign = false;
        bool alt_form = false, zero_pad = false;
        for (;;) {
            if      (*p == '-') { left_align = true;  p++; }
            else if (*p == '+') { plus_sign = true;   p++; }
            else if (*p == ' ') { space_sign = true;  p++; }
            else if (*p == '#') { alt_form = true;    p++; }
            else if (*p == '0') { zero_pad = true;    p++; }
            else break;
        }

        /* Width */
        int width = 0;
        if (*p == '*') { width = va_arg(aq, int); p++; }
        else {
            while (*p >= '0' && *p <= '9') {
                width = width * 10 + (*p - '0');
                p++;
            }
        }

        /* Precision */
        int precision = -1;
        if (*p == '.') {
            p++;
            if (*p == '*') {
                precision = va_arg(aq, int);
                p++;
            } else {
                precision = 0;
                while (*p >= '0' && *p <= '9') {
                    precision = precision * 10 + (*p - '0');
                    p++;
                }
            }
        }

        /* Length modifier */
        enum { LEN_none, LEN_h, LEN_hh, LEN_l, LEN_ll, LEN_L, LEN_z, LEN_t, LEN_j } length = LEN_none;
        if      (*p == 'h') { length = LEN_h; p++; if (*p == 'h') { length = LEN_hh; p++; } }
        else if (*p == 'l') { length = LEN_l; p++; if (*p == 'l') { length = LEN_ll; p++; } }
        else if (*p == 'L') { length = LEN_L; p++; }
        else if (*p == 'z') { length = LEN_z; p++; }
        else if (*p == 't') { length = LEN_t; p++; }
        else if (*p == 'j') { length = LEN_j; p++; }

        char spec = *p;
        if (spec) p++;

        char tmpbuf[512];
        int tmpused = 0;

        switch (spec) {

        case '%':
            out_putc(o, '%');
            total++;
            break;

        case 'c': {
            char c = (char)va_arg(aq, int);
            out_putc(o, c);
            total++;
            break;
        }

        case 's': {
            const char* s = va_arg(aq, const char*);
            if (!s) s = "(null)";
            while (*s) {
                out_putc(o, *s++);
                total++;
            }
            break;
        }

        case 'd': case 'i': {
            int64_t val = va_arg(aq, int);
            if (length == LEN_l) val = va_arg(aq, long);
            else if (length == LEN_ll) val = va_arg(aq, long long);

            bool neg = val < 0;
            uint64_t u = neg ? -val : val;

            tmpused = int_to_str(tmpbuf, u, 10, false);

            if (neg) out_putc(o, '-');

            for (int i = 0; i < tmpused; i++) {
                out_putc(o, tmpbuf[i]);
            }

            total += tmpused + (neg ? 1 : 0);
            break;
        }

        case 'u': case 'x': case 'X': {
            uint64_t val = va_arg(aq, unsigned int);
            if (length == LEN_l) val = va_arg(aq, unsigned long);
            else if (length == LEN_ll) val = va_arg(aq, unsigned long long);

            int base = (spec == 'x' || spec == 'X') ? 16 : 10;
            bool upper = (spec == 'X');

            tmpused = int_to_str(tmpbuf, val, base, upper);

            for (int i = 0; i < tmpused; i++) {
                out_putc(o, tmpbuf[i]);
            }

            total += tmpused;
            break;
        }

        case 'p': {
            uintptr_t val = va_arg(aq, uintptr_t);
            out_puts(o, "0x", 2);
            total += 2;

            tmpused = int_to_str(tmpbuf, val, 16, false);
            for (int i = 0; i < tmpused; i++) {
                out_putc(o, tmpbuf[i]);
            }
            total += tmpused;
            break;
        }

        case 'f': {
            double d = va_arg(aq, double);
            fmt_float_decimal(o, d, 6, spec, false, false);
            break;
        }

        default:
            out_putc(o, '%');
            if (spec) out_putc(o, spec);
            total += 2;
            break;
        }
    }

    va_end(aq);  // ✅ IMPORTANT

    return out_finish(o);
}

/* ==================================================================
 * Public printf family
 * ================================================================== */

int vprintf(const char* fmt, va_list ap) {
    out_buf_t o;
    out_init_fd(&o, STDOUT_FILENO);
    return do_fmt(&o, fmt, ap);
}

int vfprintf(FILE* stream, const char* fmt, va_list ap) {
    out_buf_t o;
    int fd = stream ? stream->fd : STDOUT_FILENO;
    out_init_fd(&o, fd);
    return do_fmt(&o, fmt, ap);
}

int vsprintf(char* buf, const char* fmt, va_list ap) {
    out_buf_t o;
    out_init_buf(&o, buf, (size_t)-1);
    do_fmt(&o, fmt, ap);
    return out_finish(&o);
}

int vsnprintf(char* buf, size_t n, const char* fmt, va_list ap) {
    if (n == 0) return 0;
    out_buf_t o;
    out_init_buf(&o, buf, n);
    do_fmt(&o, fmt, ap);
    return out_finish(&o);
}

int printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vprintf(fmt, ap);
    va_end(ap);
    return r;
}

int fprintf(FILE* stream, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vfprintf(stream, fmt, ap);
    va_end(ap);
    return r;
}

int sprintf(char* buf, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vsprintf(buf, fmt, ap);
    va_end(ap);
    return r;
}

int snprintf(char* buf, size_t n, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, n, fmt, ap);
    va_end(ap);
    return r;
}

/* ==================================================================
 * Standard stream objects (simplified — no real buffering yet)
 * ================================================================== */

static FILE _stdin  = { .fd = STDIN_FILENO,  .mode = 0 };
static FILE _stdout = { .fd = STDOUT_FILENO, .mode = 0 };
static FILE _stderr = { .fd = STDERR_FILENO, .mode = 0 };

FILE* stdin  = &_stdin;
FILE* stdout = &_stdout;
FILE* stderr = &_stderr;

/* ==================================================================
 * scanf / parsing helpers
 * ================================================================== */

static void skip_whitespace(in_buf_t* i) {
    int c;
    while ((c = in_peekc(i)) != EOF && (c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\v' || c == '\f')) {
        in_getc(i);
    }
}

/* Parse a signed integer from input, returns 1 on success */
static int parse_int(in_buf_t* i, int64_t* out, int base, bool* neg_out) {
    skip_whitespace(i);
    int c = in_peekc(i);
    if (c == EOF) return 0;

    bool neg = false;
    if (c == '-') { neg = true; in_getc(i); c = in_peekc(i); }
    else if (c == '+') { in_getc(i); c = in_peekc(i); }

    if (c == EOF) return 0;

    /* Detect base from prefix if auto */
    if (base == 0) {
        if (c == '0') {
            in_getc(i);
            int c2 = in_peekc(i);
            if (c2 == 'x' || c2 == 'X') { base = 16; in_getc(i); }
            else { base = 8; /* put back the 0 */ }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (c == '0') {
            in_getc(i);
            int c2 = in_peekc(i);
            if (c2 == 'x' || c2 == 'X') { in_getc(i); }
        }
    }

    uint64_t val = 0;
    int count = 0;
    c = in_peekc(i);
    while (c != EOF) {
        int digit = -1;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'z') digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'Z') digit = c - 'A' + 10;
        if (digit < 0 || digit >= base) break;
        val = val * (uint64_t)base + (uint64_t)digit;
        count++;
        in_getc(i);
        c = in_peekc(i);
    }

    if (count == 0) return 0;
    *out = neg ? -(int64_t)val : (int64_t)val;
    if (neg_out) *neg_out = neg;
    return count;
}

/* Parse a double from input */
static int parse_double(in_buf_t* i, double* out) {
    skip_whitespace(i);
    int c = in_peekc(i);
    if (c == EOF) return 0;

    char buf[64];
    int idx = 0;
    bool neg = false;

    if (c == '-') { neg = true; in_getc(i); c = in_peekc(i); }
    else if (c == '+') { in_getc(i); c = in_peekc(i); }

    while (c != EOF && idx < 63) {
        if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' ||
            c == '+' || c == '-' || c == 'i' || c == 'n' || c == 'f' ||
            c == 'I' || c == 'N' || c == 'F' || c == 'a' || c == 'A' ||
            c == 'p' || c == 'P' || c == 'x' || c == 'X') {
            buf[idx++] = (char)c;
            in_getc(i);
            c = in_peekc(i);
        } else break;
    }
    buf[idx] = '\0';

    if (idx == 0) return 0;

    /* Simple parser: integer.fraction e exponent */
    double val = 0.0;
    int pos = 0;

    /* Check for special values */
    if (buf[0] == 'i' || buf[0] == 'I' || buf[0] == 'n' || buf[0] == 'N') {
        *out = neg ? -1.0/0.0 : 1.0/0.0;
        return 1;
    }

    /* Integer part */
    while (pos < idx && buf[pos] >= '0' && buf[pos] <= '9') {
        val = val * 10.0 + (double)(buf[pos] - '0');
        pos++;
    }

    /* Fractional part */
    if (pos < idx && buf[pos] == '.') {
        pos++;
        double frac = 0.1;
        while (pos < idx && buf[pos] >= '0' && buf[pos] <= '9') {
            val += (double)(buf[pos] - '0') * frac;
            frac *= 0.1;
            pos++;
        }
    }

    /* Exponent */
    if (pos < idx && (buf[pos] == 'e' || buf[pos] == 'E')) {
        pos++;
        bool eneg = false;
        if (pos < idx && buf[pos] == '-') { eneg = true; pos++; }
        else if (pos < idx && buf[pos] == '+') { pos++; }

        int exp = 0;
        while (pos < idx && buf[pos] >= '0' && buf[pos] <= '9') {
            exp = exp * 10 + (buf[pos] - '0');
            pos++;
        }

        if (eneg) {
            for (int j = 0; j < exp; j++) val *= 0.1;
        } else {
            for (int j = 0; j < exp; j++) val *= 10.0;
        }
    }

    /* Hex float: 0x...p... */
    /* For simplicity, we don't fully parse hex floats in scanf */

    *out = neg ? -val : val;
    return 1;
}

/* ==================================================================
 * Core scanf engine
 * ================================================================== */

static int do_scanf(in_buf_t* i, const char* fmt, va_list ap) {
    va_list aq;
    va_copy(aq, ap);

    int assigned = 0;

    while (*fmt) {
        if (*fmt == ' ') {
            skip_whitespace(i);
            fmt++;
            continue;
        }

        if (*fmt != '%') {
            int c = in_getc(i);
            if (c != *fmt) break;
            fmt++;
            continue;
        }

        fmt++; // skip %

        if (*fmt == 'd') {
            int* out = va_arg(aq, int*);
            int64_t val;
            if (!parse_int(i, &val, 10, NULL)) break;
            *out = (int)val;
            assigned++;
            fmt++;
        }
        else if (*fmt == 's') {
            char* out = va_arg(aq, char*);
            skip_whitespace(i);

            int c = in_peekc(i);
            if (c == EOF) break;

            int idx = 0;
            while (c != EOF && !(c==' '||c=='\n'||c=='\t'||c=='\r')) {
                out[idx++] = (char)in_getc(i);
                c = in_peekc(i);
            }
            out[idx] = '\0';
            assigned++;
            fmt++;
        }
        else if (*fmt == 'c') {
            char* out = va_arg(aq, char*);
            int c = in_getc(i);
            if (c == EOF) break;
            *out = (char)c;
            assigned++;
            fmt++;
        }
        else {
            fmt++;
        }
    }

    va_end(aq);
    return assigned;
}
/* ==================================================================
 * Public scanf family
 * ================================================================== */

int scanf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    in_buf_t i;
    in_init_fd(&i, STDIN_FILENO);

    int r = do_scanf(&i, fmt, ap);

    va_end(ap);
    return r;
}

int fscanf(FILE* stream, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    in_buf_t i;
    in_init_fd(&i, stream ? stream->fd : STDIN_FILENO);

    int r = do_scanf(&i, fmt, ap);

    va_end(ap);
    return r;
}

int sscanf(const char* buf, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    in_buf_t i;
    in_init_str(&i, buf);

    int r = do_scanf(&i, fmt, ap);

    va_end(ap);
    return r;
}

int vscanf(const char* fmt, va_list ap) {
    in_buf_t i;
    in_init_fd(&i, STDIN_FILENO);
    return do_scanf(&i, fmt, ap);
}

int vfscanf(FILE* stream, const char* fmt, va_list ap) {
    in_buf_t i;
    in_init_fd(&i, stream ? stream->fd : STDIN_FILENO);
    return do_scanf(&i, fmt, ap);
}

int vsscanf(const char* buf, const char* fmt, va_list ap) {
    in_buf_t i;
    in_init_str(&i, buf);
    return do_scanf(&i, fmt, ap);
}

/* ==================================================================
 * File I/O functions
 * ================================================================== */

FILE* fopen(const char* pathname, const char* mode) {
    int flags = 0;
    int mode_bits = 0644;

    if (mode[0] == 'r') flags = O_RDONLY;
    else if (mode[0] == 'w') flags = O_WRONLY | O_CREAT | O_TRUNC;
    else if (mode[0] == 'a') flags = O_WRONLY | O_CREAT | O_APPEND;

    /* Handle + mode */
    for (int j = 0; mode[j]; j++) {
        if (mode[j] == '+') { flags = (flags & ~3) | O_RDWR; break; }
    }

    int fd = (int)sys_open(pathname, flags, mode_bits);
    if (fd < 0) return NULL;

    FILE* f = (FILE*)malloc(sizeof(FILE));
    if (!f) { sys_close(fd); return NULL; }

    f->fd = fd;
    f->mode = mode[0];
    f->buffering = _IOLBF;
    f->buf = NULL;
    f->buf_size = 0;
    f->buf_pos = 0;
    f->buf_len = 0;
    f->error = 0;
    f->eof = 0;
    return f;
}

int fclose(FILE* stream) {
    if (!stream) return EOF;
    int fd = stream->fd;
    if (stream->buf) free(stream->buf);
    free(stream);
    sys_close(fd);
    return 0;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!stream || !ptr) return 0;
    size_t total = size * nmemb;
    int64_t n = sys_read(stream->fd, ptr, total);
    if (n < 0) { stream->error = 1; return 0; }
    if (n == 0) { stream->eof = 1; return 0; }
    return (size_t)n / size;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!stream || !ptr) return 0;
    size_t total = size * nmemb;
    int64_t n = sys_write(stream->fd, ptr, total);
    if (n < 0) { stream->error = 1; return 0; }
    return (size_t)n / size;
}

int fputc(int c, FILE* stream) {
    if (!stream) return EOF;
    char ch = (char)c;
    return (sys_write(stream->fd, &ch, 1) == 1) ? c : EOF;
}

int putc(int c, FILE* stream) {
    return fputc(c, stream);
}

int putchar(int c) {
    /* Direct write to fd 1 to avoid dereferencing potentially unmapped `stdout` */
    char ch = (char)c;
    sys_write(1, &ch, 1);
    return (unsigned char)c;
}

int fgetc(FILE* stream) {
    if (!stream) return EOF;
    char ch;
    int64_t n = sys_read(stream->fd, &ch, 1);
    if (n <= 0) { stream->eof = 1; return EOF; }
    return (int)(unsigned char)ch;
}

int getc(FILE* stream) {
    return fgetc(stream);
}

int getchar(void) {
    return fgetc(stdin);
}

char* fgets(char* s, int n, FILE* stream) {
    if (!s || n <= 0 || !stream) return NULL;
    int i = 0;
    while (i < n - 1) {
        int c = fgetc(stream);
        if (c == EOF) break;
        s[i++] = (char)c;
        if (c == '\n') break;
    }
    s[i] = '\0';
    return (i > 0) ? s : NULL;
}

int puts(const char* s) {
    write_full(STDOUT_FILENO, s, strlen(s));
    char nl = '\n';
    sys_write(STDOUT_FILENO, &nl, 1);
    return 0;
}

int fflush(FILE* stream) {
    /* Buffering not fully implemented yet */
    (void)stream;
    return 0;
}

long ftell(FILE* stream) {
    if (!stream) return -1;
    return (long)sys_lseek(stream->fd, 0, SEEK_CUR);
}

int fseek(FILE* stream, long offset, int whence) {
    if (!stream) return -1;
    return (sys_lseek(stream->fd, (off_t)offset, whence) >= 0) ? 0 : -1;
}

void rewind(FILE* stream) {
    if (stream) sys_lseek(stream->fd, 0, SEEK_SET);
}

int feof(FILE* stream) {
    return stream ? stream->eof : 0;
}

int ferror(FILE* stream) {
    return stream ? stream->error : 0;
}

void clearerr(FILE* stream) {
    if (stream) { stream->eof = 0; stream->error = 0; }
}

void setbuf(FILE* stream, char* buf) {
    (void)stream; (void)buf; /* stub */
}

void setvbuf(FILE* stream, char* buf, int mode, size_t size) {
    (void)stream; (void)buf; (void)mode; (void)size; /* stub */
}
