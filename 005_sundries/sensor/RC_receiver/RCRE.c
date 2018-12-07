/********************************************************************
 *  filename:   RCRE.c
 *  created:    2017/10/05  1:28
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  RC receiver
 ********************************************************************/

#include "RCRE.h"
#include "TC.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_ATmega328P__)
#else
#error "unsupported device"
#endif

#define FAILSAFE_THRESHOLD 300

static volatile uint8_t rcre_last_value[4] = {0};
static volatile uint32_t rcre_start_time[4] = {0};
volatile uint32_t rcre_pulse_width[4] = {0};

static uint16_t rcre_errors_count = 0;
static uint8_t rcre_pre_check = 0;

ISR(PCINT1_vect)
{
    if(rcre_last_value[0] == 0 && (PINC & (1<<PINC0)))
    {
        rcre_last_value[0] = 1;
        rcre_start_time[0] = tc_micros();
        sei();
    }
    else if(rcre_last_value[0] == 1 && !(PINC & (1<<PINC0)))
    {
        rcre_last_value[0] = 0;
        rcre_pulse_width[0] = tc_micros() - rcre_start_time[0];
    }

    if(rcre_last_value[1] == 0 && (PINC & (1<<PINC1)))
    {
        rcre_last_value[1] = 1;
        rcre_start_time[1] = tc_micros();
        sei();
    }
    else if(rcre_last_value[1] == 1 && !(PINC & (1<<PINC1)))
    {
        rcre_last_value[1] = 0;
        rcre_pulse_width[1] = tc_micros() - rcre_start_time[1];
    }

    if(rcre_last_value[2] == 0 && (PINC & (1<<PINC2)))
    {
        rcre_last_value[2] = 1;
        rcre_start_time[2] = tc_micros();
        sei();
    }
    else if(rcre_last_value[2] == 1 && !(PINC & (1<<PINC2)))
    {
        rcre_last_value[2] = 0;
        rcre_pulse_width[2] = tc_micros() - rcre_start_time[2];
    }

    if(rcre_last_value[3] == 0 && (PINC & (1<<PINC3)))
    {
        rcre_last_value[3] = 1;
        rcre_start_time[3] = tc_micros();
        sei();
    }
    else if(rcre_last_value[3] == 1 && !(PINC & (1<<PINC3)))
    {
        rcre_last_value[3] = 0;
        rcre_pulse_width[3] = tc_micros() - rcre_start_time[3];
    }
}

void rcre_setup(void)
{
    DDRC &= ~((1<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0));
    PCICR |= 1<<PCIE1;
    PCMSK1 = (1<<PCINT11)|(1<<PCINT10)|(1<<PCINT9)|(1<<PCINT8);
}

uint8_t rcre_connection_state(void)
{
    uint8_t rc_check = rcre_last_value[0] + (rcre_last_value[1]<<1) + (rcre_last_value[2]<<2) + (rcre_last_value[3]<<3);
    if (rcre_pre_check == rc_check)
    {
        rcre_errors_count++;
    }
    else
    {
        rcre_errors_count = 0;
    }
    rcre_pre_check = rc_check;

    if (rcre_errors_count < FAILSAFE_THRESHOLD)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}