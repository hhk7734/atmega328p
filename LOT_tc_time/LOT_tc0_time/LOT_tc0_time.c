/**
 * @file LOT_tc0_time.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief TC0, 전원 인가 후 시간
 */

#include "LOT_tc0_time.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile static uint32_t tc0_isr_count  = 0;
volatile static uint32_t tc0_isr_millis = 0;
volatile static uint8_t  tc0_remainder  = 0;

/**
 * @brief 8 bit TC0 오버 플로우 인터럽트, 1024 us에 한 번씩 실행
 */
ISR( TIMER0_OVF_vect )
{
    ++tc0_isr_count;
    ++tc0_isr_millis;
    /// tc2_remainder * 8 us
    tc0_remainder += 3;
    if ( tc0_remainder > 124 )
    {
        ++tc0_isr_millis;
        tc0_remainder -= 125;
    }
}

/**
 * @brief TC0 노말 모드, 오버플로우 인터럽트 설정
 */
void LOT_tc0_time_setup( void )
{
#if F_CPU == 8000000UL
    /// prescaler == 32, TCNT * 4 us
    TCCR0 = ( _BV( CS01 ) | _BV( CS00 ) );
#elif F_CPU == 16000000UL
    /// prescaler == 64, TCNT * 4 us
    TCCR0 = _BV( CS02 );
#endif
    TIMSK = _BV( TOIE0 );
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
    count = tc0_isr_count;
    tcnt  = TCNT0;
    if ( ( TIFR & _BV( TOV0 ) ) && tcnt < 32 )
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
    return tc0_isr_millis;
}