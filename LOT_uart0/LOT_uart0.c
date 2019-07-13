/**
 * @file LOT_uart0.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief ATmega328p UART0 통신
 */

#include "LOT_uart0.h"

void uart0_init_all( const uint32_t baud_rate,
                     const uint8_t  data_bits,
                     const uint8_t  stop_bits,
                     const uint8_t  parity )
{
    /// @todo u2x 문제가 있을 수 있음

    /**
     * baudrate 설정
     * ( F_CPU / baud_rate / 8 ) - 1 을 계산한 뒤 소수점 첫째 자리에서 반올림
     * 부동소수 -> 정수로 갈때 소수점 버림
     * ( ( F_CPU / baud_rate / 4 ) - 1 ) / 2
     */

    /**
     * data bits 설정
     * @todo 9bit 지원
     */

    /// stop bits 설정

    /// parity 설정

    /// global interrupt 활성화
    sei();
}

void uart0_putchar( const char c ) {}

void uart0_putstr( const char *str ) {}

char uart0_getchar( void ) {}

void uart0_available( void ) {}

void uart0_flush( void ) {}

/**
 * @brief 데이터 수신 완료 interrupt
 */
ISR( USART_RX_vect ) {}

/**
 * @breif 데이터 송신 완료 interrupt
 */
ISR( USART_UDRE_vect ) {}