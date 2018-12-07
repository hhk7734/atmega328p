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

#include "opt_math.h"

#include <avr/io.h>

int32_t __attribute__ ((noinline)) opt_muls16x16(int16_t a, int16_t b)
{
/**
 * s16 x s16 -> s32
 * https://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
 */
    int32_t r;
    __asm__ __volatile__ (
        "clr r26 \n\t"          // MSB D C B A LSB
        "mul %A1, %A2 \n\t"     // r1:r0    <- Rd x Rr
        "movw %A0, r0 \n\t"     // Rd+1:Rd  <- Rr+1:Rr
        "muls %B1, %B2 \n\t"    // r1:r0    <- Rd x Rr (signed)
        "movw %C0, r0 \n\t"
        "mulsu %B2, %A1 \n\t"   // r1:r0    <- Rd(signed) x Rr(unsigned)
        "sbc %D0, r26 \n\t"     // Rd       <- Rd - Rr - Carry
        "add %B0, r0 \n\t"
        "adc %C0, r1 \n\t"
        "adc %D0, r26 \n\t"
        "mulsu %B1, %A2 \n\t"
        "sbc %D0, r26 \n\t"
        "add %B0, r0 \n\t"
        "adc %C0, r1 \n\t"
        "adc %D0, r26 \n\t"
        "clr r1 \n\t"
        :"=&r" (r)              // %0
        :"a" (a),"a" (b)        // %1, %2
        :"r26"
    );
	return r;
}

uint64_t __attribute__ ((noinline)) opt_mul32x32(uint32_t a, uint32_t b)
{
    uint64_t r;
    __asm__ __volatile__ (
        "clr r26 \n\t"
        "mul %A1,%A2 \n\t"
        "movw %A0,r0 \n\t"
        "clr %C0 \n\t"
        "clr %D0 \n\t"
        "clr r22 \n\t"
        "clr r23 \n\t"
        "clr r24 \n\t"
        "clr r25 \n\t"
        "mul %B1,%A2 \n\t"
        "add %B0,r0 \n\t"
        "adc %C0,r1 \n\t"
        "mul %C1,%A2 \n\t"
        "add %C0,r0 \n\t"
        "adc %D0,r1 \n\t"
        "mul %D1,%A2 \n\t"
        "add %D0,r0 \n\t"
        "adc r22,r1 \n\t"

        "mul %A1,%B2 \n\t"
        "add %B0,r0 \n\t"
        "adc %C0,r1 \n\t"
        "adc %D0,r26 \n\t"
        "adc r22,r26 \n\t"
        "adc r23,r26 \n\t"
        "mul %B1,%B2 \n\t"
        "add %C0,r0 \n\t"
        "adc %D0,r1 \n\t"
        "adc r22,r26 \n\t"
        "adc r23,r26 \n\t"
        "mul %C1,%B2 \n\t"
        "add %D0,r0 \n\t"
        "adc r22,r1 \n\t"
        "adc r23,r26 \n\t"
        "mul %D1,%B2 \n\t"
        "add r22,r0 \n\t"
        "adc r23,r1 \n\t"

        "mul %A1,%C2 \n\t"
        "add %C0,r0 \n\t"
        "adc %D0,r1 \n\t"
        "adc r22,r26 \n\t"
        "adc r23,r26 \n\t"
        "adc r24,r26 \n\t"
        "mul %B1,%C2 \n\t"
        "add %D0,r0 \n\t"
        "adc r22,r1 \n\t"
        "adc r23,r26 \n\t"
        "adc r24,r26 \n\t"
        "mul %C1,%C2 \n\t"
        "add r22,r0 \n\t"
        "adc r23,r1 \n\t"
        "adc r24,r26 \n\t"
        "mul %D1,%C2 \n\t"
        "add r23,r0 \n\t"
        "adc r24,r1 \n\t"

        "mul %A1,%D2 \n\t" // TODO check last 8 bit
        "add %D0,r0 \n\t"
        "adc r22,r1 \n\t"
        "adc r23,r26 \n\t"
        "adc r24,r26 \n\t"
        "adc r25,r26 \n\t"
        "mul %B1,%D2 \n\t"
        "add r22,r0 \n\t"
        "adc r23,r1 \n\t"
        "adc r24,r26 \n\t"
        "adc r25,r26 \n\t"
        "mul %C1,%D2 \n\t"
        "add r23,r0 \n\t"
        "adc r24,r1 \n\t"
        "adc r25,r26 \n\t"
        "mul %D1,%D2 \n\t"
        "add r24,r0 \n\t"
        "adc r25,r1 \n\t"
        :"=&r" (r)
        :"l" (a),"l" (b)
        :"r26"
    );
    return r;
}

int64_t __attribute__ ((noinline)) opt_muls32x32(int32_t a, int32_t b)
{
    int64_t r;
    __asm__ __volatile__ (
        "clr r26 \n\t"      // TODO ALL
        :"=&r" (r)
        :"l" (a),"l" (b)
        :"r26"
    );
    return r;
}

int16_t opt_atan2(int32_t y, int32_t x)
{
/**
 * -1800(-180도) ~ 1800(180도)
 * C. Hastings approximation for atan2()
 * Optimization tricks: http://www.hackersdelight.org/
 */
    float z = y;
    int16_t a;
    uint8_t c;
    c = abs(y) < abs(x);
    if ( c ) {z = z / x;} else {z = x / z;}
    a = 2046.43 * (z / (3.5714 +  z * z));
    if ( c ){
        if (x<0) {
            if (y<0) a -= 1800;
            else a += 1800;
        }
    } else {
        a = 900 - a;
        if (y<0) a -= 1800;
    }
    return a;
}

float opt_InvSqrt(float x)
{
/**
 * approximation for inverse sqrt
 * https://ko.wikipedia.org/wiki/고속_역_제곱근
 */
	union{
		int32_t i;
		float   f;
	} conv;
	conv.f = x;
	conv.i = 0x5f1ffff9 - (conv.i >> 1);
	return conv.f * (1.68191409f - 0.703952253f * x * conv.f * conv.f);
}
