/***********************************************************************************
 * filename:  tc2time.h
 *
 * Created:   2018/02/04
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _TC2TIME_H_
#define _TC2TIME_H_

#ifndef F_CPU
#warning "F_CPU is not defined"
#endif

#if F_CPU!=16000000UL
#warning "F_CPU is not 16MHz, don't use this code."
#endif

#if !defined(__AVR_ATmega128A__)
#error "maybe unsupported device"
#endif

#include <inttypes.h>

class tc2time_
{
public :
    tc2time_();
    ~tc2time_();

    uint32_t micros(void);
    uint32_t millis(void);
    void ovf_isr(void);
protected :

private :
    volatile uint32_t isr_count;
    volatile uint32_t isr_millis;
    volatile uint8_t remainder_micros_per_ovf;

}; // tc2time_

extern tc2time_ tc2;
#endif // _TC2TIME_H_