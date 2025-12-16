#include "math.h"
#include <stdint.h>
float sqrt(float number) {
    

     union {
        float f;
        uint32_t i;
    } conv;

    conv.f = number;

    const float threehalfs = 1.5f;
    float x2 = number * 0.5f;
    float y = number;

    conv.f = y;
    conv.i = 0x5f3759df - (conv.i >> 1);
    y = conv.f;

    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y)); // 2nd iteration
    y = y * (threehalfs - (x2 * y * y)); // 3rd iteration

    return number * y; 

}

static inline float normalize_angle(float x) {
    while (x > M_PI)  x -= 2.0f * M_PI;
    while (x < -M_PI) x += 2.0f * M_PI;
    return x;
}
float sin(float x) {
    x = normalize_angle(x);
    float term = x;
    float sum = term;
    for (int i = 1; i <= 5; i++) {
        term *= -x * x / ((2*i)*(2*i+1));
        sum += term;
    }
    return sum;
}

float cos(float x) {
    x = normalize_angle(x);
    float term = 1.0f;
    float sum = term;
    for (int i = 1; i <= 5; i++) {
        term *= -x * x / ((2*i-1)*(2*i));
        sum += term;
    }
    return sum;
}

float tan(float x) {
    float c = cos(x);
    if (c == 0.0f) return 0.0f; // avoid div by zero
    return sin(x)/c;
}

float atan(float x) {
    if (x > 1.0f) return M_PI/2 - atan(1.0f/x);
    if (x < -1.0f) return -M_PI/2 - atan(1.0f/x);
    float term = x;
    float sum = term;
    float x2 = x*x;
    for (int i = 1; i <= 5; i++) {
        term *= -x2 * (2*i-1)/(2*i+1);
        sum += term;
    }
    return sum;
}
float asin(float x) {
    if (x > 1.0f) x = 1.0f;
    if (x < -1.0f) x = -1.0f;
    return atan(x / sqrtf(1.0f - x*x));
}

float acos(float x) {
    return M_PI/2 - asin(x);
}


float pow(float base, float exponent) {
    if (base == 0.0f) return 0.0f;
    return expf(exponent * logf(base));
}

static float expf(float x) {
    float sum = 1.0f;
    float term = 1.0f;
    for (int i = 1; i <= 10; i++) {
        term *= x / i;
        sum += term;
    }
    return sum;
}


static float logf(float x) {
    if (x <= 0.0f) return 0.0f; 
    float y = (x - 1) / (x + 1);
    float y2 = y * y;
    float sum = 0.0f;
    for (int i = 0; i < 10; i++) {
        float y2i = 1.0f;
        for (int j = 0; j < i; j++) y2i *= y2;
        sum += (1.0f / (2*i + 1)) * (y * y2i);
    }
    return 2.0f * sum;
}