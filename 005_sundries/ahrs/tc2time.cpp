/***********************************************************************************
 * filename:  tc2time.cpp
 *
 * Created:   2018/02/04
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "tc2time.h"

#include <avr/io.h>
#include <avr/interrupt.h>
//
// Constructor & Destructor
//
tc2time_::tc2time_()
{
    // variables initialization
    isr_count = 0;
    isr_millis = 0;
    remainder_micros_per_ovf = 0;

    // Normal Mode, prescaler == 64
    TCCR2 = (1<<CS21)|(1<<CS20);

    // Overflow interrupt per 256 * 4 us => 1024us
    TIMSK |= (1<<TOIE2);

    sei();
}
tc2time_::~tc2time_() {}

//
// Public
//
uint32_t tc2time_::micros(void)
{
    uint16_t tcnt;
    uint32_t count;

    cli();

    count = isr_count;
    tcnt = TCNT2;

    // after cli();, if TOV2 was set, count ++;
    if((TIFR & (1<<TOV2)) && tcnt < 32) ++count;

    sei();

    return (count<<10) + (tcnt<<2);
}

uint32_t tc2time_::millis(void)
{
    return isr_millis;
}

void tc2time_::ovf_isr(void)
{

    ++isr_count;
    ++isr_millis;
    remainder_micros_per_ovf += 3; // 8*3 == 24us
    if (remainder_micros_per_ovf > 124) // 8*125 == 1000us
    {
        ++isr_millis;
        remainder_micros_per_ovf -= 125;
    }
}

//
// tc2time_ [Object name] = tc2time_();
//
tc2time_ tc2 = tc2time_();

ISR(TIMER2_OVF_vect)
{
    tc2.ovf_isr();
}