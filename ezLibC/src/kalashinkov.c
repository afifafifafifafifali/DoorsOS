#include <math.h>

typedef unsigned long long u64;
typedef long long i64;

// ================= CONSTANTS =================



// ================= BIT ACCESS =================

static u64 k_bits(double x){ union{double d;u64 u;}v; v.d=x; return v.u; }
static double k_frombits(u64 x){ union{double d;u64 u;}v; v.u=x; return v.d; }

// ================= IEEE =================

#define K_EXP(x) ((int)((k_bits(x)>>52)&0x7FF))
#define K_MANT(x) (k_bits(x)&((1ULL<<52)-1))

int kalashinkov_isnan(double x){ return (K_EXP(x)==0x7FF)&&(K_MANT(x)); }
int kalashinkov_isinf(double x){ return (K_EXP(x)==0x7FF)&&!(K_MANT(x)); }
int kalashinkov_isfinite(double x){ return K_EXP(x)!=0x7FF; }
int kalashinkov_signbit(double x){ return (k_bits(x)>>63)&1; }

// ================= BASIC =================

double kalashinkov_fabs(double x){
    return k_frombits(k_bits(x)&0x7FFFFFFFFFFFFFFFULL);
}

// ================= SQRT =================

double kalashinkov_sqrt(double x){
    if(kalashinkov_isnan(x)) return x;
    if(x<0) return 0.0/0.0;
    if(kalashinkov_isinf(x)) return x;

    double g=x;
    for(int i=0;i<8;i++) g=0.5*(g+x/g);
    return g;
}

// ================= EXP =================

double kalashinkov_exp(double x){
    if(kalashinkov_isnan(x)) return x;
    if(x>709) return 1.0/0.0;
    if(x<-745) return 0.0;

    i64 k=(i64)(x/LN2);
    double r=x-k*LN2;

    double y=1+r+r*r*(0.5+r*(1.0/6+r*(1.0/24+r*(1.0/120))));

    while(k>0){ y*=2;k--; }
    while(k<0){ y*=0.5;k++; }

    return y;
}

// ================= LOG =================

double kalashinkov_log(double x){
    if(kalashinkov_isnan(x)) return x;
    if(x<0) return 0.0/0.0;
    if(x==0) return -1.0/0.0;
    if(kalashinkov_isinf(x)) return x;

    i64 k=0;
    while(x>=2){ x*=0.5;k++; }
    while(x<1){ x*=2;k--; }

    double y=(x-1)/(x+1);
    double y2=y*y;

    double s=2*(y+y*y2*(1.0/3+y2*(1.0/5+y2*(1.0/7))));
    return s+k*LN2;
}

// ================= RANGE REDUCTION =================

static double k_floor(double x){
    i64 i=(i64)x;
    if(x<0 && x!=i) i--;
    return (double)i;
}

static void reduce(double x,double* r,int* q){
    double n=k_floor(x/PI_2+(x>=0?0.5:-0.5));
    *q=((int)n)&3;
    *r=x-n*PI_2;
}

// ================= SIN/COS POLY =================

static double sin_poly(double x){
    double x2=x*x;
    return x*(1 + x2*(-0.16666666666666666 +
           x2*(0.008333333333333333 +
           x2*(-0.0001984126984126984 +
           x2*(2.755731922398589e-6)))));
}

static double cos_poly(double x){
    double x2=x*x;
    return 1 + x2*(-0.5 +
           x2*(0.041666666666666664 +
           x2*(-0.001388888888888889 +
           x2*(2.48015873015873e-5))));
}

// ================= SIN/COS =================

double kalashinkov_sin(double x){
    if(kalashinkov_isnan(x)||kalashinkov_isinf(x)) return 0.0/0.0;

    double r; int q;
    reduce(x,&r,&q);

    double s=sin_poly(r);
    double c=cos_poly(r);

    switch(q){
        case 0: return s;
        case 1: return c;
        case 2: return -s;
        case 3: return -c;
    }
    return s;
}

double kalashinkov_cos(double x){
    return kalashinkov_sin(x+PI_2);
}

double kalashinkov_tan(double x){
    double c=kalashinkov_cos(x);
    if(kalashinkov_fabs(c)<1e-15) return 1.0/0.0;
    return kalashinkov_sin(x)/c;
}

// ================= ATAN =================

double kalashinkov_atan(double x){
    int s=0; if(x<0){x=-x;s=1;}

    double r;
    if(x>2.41421356237)
        r=PI_2-kalashinkov_atan(1/x);
    else if(x>0.41421356237)
        r=PI_4+kalashinkov_atan((x-1)/(x+1));
    else{
        double x2=x*x;
        r=x*(1 + x2*(-0.3333333333333 +
           x2*(0.2 +
           x2*(-0.142857142857 +
           x2*(0.111111111111)))));
    }

    return s?-r:r;
}

double kalashinkov_atan2(double y,double x){
    if(x>0) return kalashinkov_atan(y/x);
    if(x<0 && y>=0) return kalashinkov_atan(y/x)+PI;
    if(x<0 && y<0) return kalashinkov_atan(y/x)-PI;
    if(x==0 && y>0) return PI_2;
    if(x==0 && y<0) return -PI_2;
    return 0;
}
