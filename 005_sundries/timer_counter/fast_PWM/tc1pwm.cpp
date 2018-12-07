/***********************************************************************************
 * filename:  tc1pwm.cpp
 *
 * Created:   2018/03/01
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "tc1pwm.h"

#include <avr/io.h>

//
// Constructor & Destructor
//
tc1pwm_::tc1pwm_()
{
    // variables initialization
}
tc1pwm_::~tc1pwm_() {}

//
// Public
//
void tc1pwm_::setup(void)
{
    // OC1A, OC1B, OC1C non inverting mode
    //
    // Waveform Generation Mode 14
    // Fast PWM, Top == ICR1
    //
    // prescaler == clk/8, 16MHz/8 == 1 clock == 0.5us
    TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1)|(1<<WGM11);
    TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);

    // TOP == ICR1 == 4096 - 1 == 0x0FFF
    // the high byte must be written before the low byte!!!!!!
    ICR1H = 0x0F;
    ICR1L = 0xFF;

    // OC1A/PB5
    // OC1B/PB6
    // CO1C/PB7/OC2
    DDRB |= (1<<DDRB7)|(1<<DDRB6)|(1<<DDRB5);
}

void tc1pwm_::motor1(uint16_t _us)
{
    // the high byte must be written before the low byte!!!!!!
    union{uint16_t val; uint8_t raw[2];} data;
    data.val = _us<<1;
    OCR1AH = data.raw[1];
    OCR1AL = data.raw[0];
}

void tc1pwm_::motor2(uint16_t _us)
{
    // the high byte must be written before the low byte!!!!!!
    union{uint16_t val; uint8_t raw[2];} data;
    data.val = _us<<1;
    OCR1BH = data.raw[1];
    OCR1BL = data.raw[0];
}

void tc1pwm_::motor3(uint16_t _us)
{
    // the high byte must be written before the low byte!!!!!!
    union{uint16_t val; uint8_t raw[2];} data;
    data.val = _us<<1;
    OCR1CH = data.raw[1];
    OCR1CL = data.raw[0];
}

//
// tc1pwm_ [Object name] = tc1pwm_();
//
tc1pwm_ tc1pwm = tc1pwm_();