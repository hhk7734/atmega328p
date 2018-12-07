/********************************************************************
 *  filename:   TC.c
 *  created:    2017/09/23  11:30
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  PWM 모터 제어, micros()
 ********************************************************************/

#include "TC.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_ATmega328P__)
#else
#error "unsupported device"
#endif

static volatile uint32_t isr_millis_;

ISR(TIMER1_CAPT_vect)
{
    isr_millis_++;
}

void tc_setup(void)
{
    // CTC top ICR1
    // prescaler 8
    TCCR1A = 0;
    TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
    // the high byte must be written before the low byte!!!!!!
    // 4096 - 1 == 0x0FFF
    ICR1H = 0x0F;
    ICR1L = 0xFF;
    // input capture interrupt enable
    TIMSK1 = (1<<ICIE1);
    // global interrupt enable
    sei();

    isr_millis_ = 0;

    // OC0A(PD6), OC0B(PD5) non-inverting
    // WGM - Phase Correct PWM
    TCCR0A = (1<<COM0A1)|(1<<COM0B1)|(1<<WGM00);
    // prescaler clk/64 - 4us
    TCCR0B = (1<<CS01)|(1<<CS00);
    // OUTPUT OC0A, OC0B
    DDRD |= (1<<PORTD6)|(1<<PORTD5);

    // OC2A(PB3), OC2B(PD3) non-inverting
    // WGM - Phase Correct PWM
    TCCR2A = (1<<COM2A1)|(1<<COM2B1)|(1<<WGM20);
    // prescaler clk/64 - 4us
    TCCR2B = (1<<CS22);
    // OUTPUT OC2A, OC2B
    DDRB |= (1<<PORTB3);
    DDRD |= (1<<PORTD3);
}

uint32_t tc_micros(void)
{
    uint32_t _millis_;
    uint16_t _tcnt;

    cli();
    _millis_ = isr_millis_;

    // the low byte must be read before the high byte!!!!!!
    _tcnt = TCNT1L;
    _tcnt = ((uint16_t)TCNT1H<<8) | _tcnt;

    // cli()로 인해 인터럽트가 발생할 수 없는 동안
    // TOV1 == 1 이 됐다면 시간이 흘렀다는 것이므로
    // _millis_++;
    if ((TIFR1 & (1<<ICF1)) && (_tcnt < 200)) _millis_++;

    sei();

    // _tcnt의 1은 0.5us이므로 _tcnt/2 => us
    // _tcnt 가 2047us까지 측정하므로 11bit 사용
    // _millis_가 21bit => 21+11 = 32bit
    return (_millis_ << 11) + (_tcnt >> 1);
}

void tc_motor_0(int16_t _us)
{
    OCR2B = _us>>3;
}

 void tc_motor_1(int16_t _us)
{
    OCR0A = _us>>3;
}

void tc_motor_2(int16_t _us)
{
    OCR0B = _us>>3;
}

void tc_motor_3(int16_t _us)
{
    OCR2A = _us>>3;
}

 void tc_motor_All(int16_t _us)
{
    OCR2B = _us>>3;
    OCR0A = _us>>3;
    OCR0B = _us>>3;
    OCR2A = _us>>3;
}