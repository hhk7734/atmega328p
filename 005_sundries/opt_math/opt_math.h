/**
 * filename : opt_math.h
 *
 * created  : 2017/09/20
 *
 * Hyeon-ki, Hong
 * hhk7734@gmail.com
 *
 * purpose : math optimization
 */

#ifndef _OPT_MATH_H_
#define _OPT_MATH_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>

#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

int32_t  __attribute__ ((noinline)) opt_muls16x16(int16_t a, int16_t b);
uint64_t __attribute__ ((noinline)) opt_mul32x32(uint32_t a, uint32_t b);
int64_t  __attribute__ ((noinline)) opt_muls32x32(int32_t a, int32_t b);
int16_t  opt_atan2(int32_t y, int32_t x);
float    opt_InvSqrt(float x);

#ifdef __cplusplus
}
#endif

#endif // _OPT_MATH_H_