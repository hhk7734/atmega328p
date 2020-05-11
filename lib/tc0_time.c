/*
 * MIT License
 * Copyright (c) 2018 Hyeonki Hong <hhk7734@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tc0_time.h"

#include <avr/interrupt.h>

volatile static uint32_t isr_count     = 0;
volatile static uint32_t isr_millis    = 0;
volatile static uint8_t  isr_remainder = 0;

#if F_CPU == 8000000UL
#define TCNT_TO_US_SHIFT 3
#define MILLIS_TICK      2
#elif F_CPU == 16000000UL
#define TCNT_TO_US_SHIFT 2
#define MILLIS_TICK      1
#endif

#define COUNT_TO_US_SHIFT (8 + TCNT_TO_US_SHIFT)
#define REMAINDER_TICK    (MILLIS_TICK * 3)

void tc0_time_init() {
    TCCR0A = 0;

    /*
     *  8000000/64 => 1 count == 8 us
     * 16000000/64 => 1 count == 4 us
     */
    TCCR0B = _BV(CS01) | _BV(CS00);

    TIMSK0 = _BV(TOIE0);

    sei();
}

uint32_t tc0_millis() {
    uint32_t millis;
    uint8_t  sreg = SREG;

    cli();

#if F_CPU == 8000000UL
    millis = isr_millis + ((TCNT0 & 0x400) >> 10);
#elif F_CPU == 16000000UL
    millis = isr_millis;
#endif

    SREG = sreg;

    return millis;
}

uint32_t tc0_micros() {
    uint16_t tcnt;
    uint32_t count;
    uint8_t  sreg = SREG;

    cli();

    count = isr_count;
    tcnt  = TCNT0;

    if(bit_is_set(TIFR0, TOV0) && tcnt < 20) { count++; }

    SREG = sreg;

    return (count << COUNT_TO_US_SHIFT) + (tcnt << TCNT_TO_US_SHIFT);
}

void tc0_delay_ms(uint32_t msec) {
    uint32_t start = tc0_micros();

    while(msec > 0) {
        // if tc0_micros() < start, the condition below is true.
        if((tc0_micros() - start) >= 1000) {
            msec--;
            start += 1000;
        }
    }
}

ISR(TIMER0_OVF_vect) {
    /*
     *  8MHz => 2048 us
     * 16MHz => 1024 us
     */
    uint32_t millis    = isr_millis;
    uint8_t  remainder = isr_remainder;

    isr_count++;

    millis += MILLIS_TICK;
    remainder += REMAINDER_TICK;

    // 1 remainder == 8 us
    if(remainder >= 125) {
        millis++;
        remainder -= 125;
    }

    isr_remainder = remainder;
    isr_millis    = millis;
}