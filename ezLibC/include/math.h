#ifndef __EZLIBC_MATH_H
#define __EZLIBC_MATH_H 1

#define PI      3.14159265358979323846264338327950288
#define PI_2    1.57079632679489661923132169163975144
#define PI_4    0.78539816339744830961566084581987572
#define LN2     0.69314718055994530941723212145817656


int kalashinkov_isnan(double x);
int kalashinkov_isinf(double x);
int kalashinkov_isfinite(double x);
int kalashinkov_signbit(double x);

double kalashinkov_fabs(double x);

// core math
double kalashinkov_sqrt(double x);
double kalashinkov_exp(double x);
double kalashinkov_log(double x);

double kalashinkov_sin(double x);
double kalashinkov_cos(double x);
double kalashinkov_tan(double x);

double kalashinkov_atan(double x);
double kalashinkov_atan2(double y, double x);

// ================= CLEAN PUBLIC API =================
// rename without "kalashinkov"

#define sin     kalashinkov_sin
#define cos     kalashinkov_cos
#define tan     kalashinkov_tan

#define exp     kalashinkov_exp
#define log     kalashinkov_log
#define sqrt    kalashinkov_sqrt
#define fabs    kalashinkov_fabs

#define atan    kalashinkov_atan
#define atan2   kalashinkov_atan2

#define isnan   kalashinkov_isnan
#define isinf   kalashinkov_isinf
#define isfinite kalashinkov_isfinite
#define signbit kalashinkov_signbit

#ifdef __cplusplus
}
#endif
#endif