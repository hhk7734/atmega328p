/**
 * @file LOT_tc2_time.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief TC2, 전원 인가 후 시간
 */

#include "LOT_tc2_time.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile static uint32_t tc2_isr_count  = 0;
volatile static uint32_t tc2_isr_millis = 0;
volatile static uint8_t  tc2_remainder  = 0;

/**
 * @brief 8 bit TC2 오버 플로우 인터럽트, 1024 us에 한 번씩 실행
 */
ISR( TIMER2_OVF_vect )
{
    ++tc2_isr_count;
    ++tc2_isr_millis;
    /// tc2_remainder * 8 us
    tc2_remainder += 3;
    if ( tc2_remainder > 124 )
    {
        ++tc2_isr_millis;
        tc2_remainder -= 125;
    }
}

/**
 * @brief TC2 노말 모드, 오버플로우 인터럽트 설정
 */
void LOT_tc2_time_setup( void )
{
    TCCR2A = 0;
#if F_CPU == 8000000UL
    /// prescaler == 32, TCNT * 4 us
    TCCR2B = _BV( CS21 ) | _BV( CS20 );
#elif F_CPU == 16000000UL
    /// prescaler == 64, TCNT * 4 us
    TCCR2B = _BV( CS22 );
#endif
    TIMSK2 = _BV( TOIE2 );
    sei();
}

/**
 * @brief 전원 인가 후 흐른 시간 us
 * @return uint32_t 흐른 시간 us
 */
uint32_t LOT_micros( void )
{
    uint16_t tcnt;
    uint32_t count;
    cli();
    count = tc2_isr_count;
    tcnt  = TCNT2;
    if ( ( TIFR2 & _BV( TOV2 ) ) && tcnt < 32 )
    {
        ++count;
    }
    sei();
    return ( count << 10 ) + ( tcnt << 2 );
}

/**
 * @brief 전원 인가 후 흐른 시간 ms
 * @return uint32_t 흐른 시간 ms
 */
uint32_t LOT_millis( void )
{
    return tc2_isr_millis;
}