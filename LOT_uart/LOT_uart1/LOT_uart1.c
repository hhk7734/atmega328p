/**
 * @file LOT_uart1.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief UART 통신
 */

#include "LOT_uart1.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#define LOT_UCSRA UCSR1A
#define LOT_UCSRB UCSR1B
#define LOT_UCSRC UCSR1C
#define LOT_UBRRH UBRR1H
#define LOT_UBRRL UBRR1L
#ifdef UBRR1
#    define LOT_UBRR1 UBRR1
#endif
#define LOT_UDR UDR1

/// USART Register A
#define LOT_TXC TXC1

/// USART Register B
#define LOT_RXCIE RXCIE1
#define LOT_UDRIE UDRIE1
#define LOT_RXEN RXEN1
#define LOT_TXEN TXEN1

/// USART Register C
#define LOT_UCSZ1 UCSZ11
#define LOT_UCSZ0 UCSZ10

#define LOT_USART_RX_vect USART1_RX_vect
#define LOT_USART_UDRE_vect USART1_UDRE_vect

volatile static uint8_t tx_buf_head                   = 0;
volatile static uint8_t tx_buf_tail                   = 0;
volatile static uint8_t tx_buf[LOT_UART1_TX_BUF_SIZE] = { 0 };
volatile static uint8_t rx_buf_head                   = 0;
volatile static uint8_t rx_buf_tail                   = 0;
volatile static uint8_t rx_buf[LOT_UART1_RX_BUF_SIZE] = { 0 };

ISR( LOT_USART_RX_vect )
{
    rx_buf[rx_buf_head] = LOT_UDR;
    rx_buf_head         = ( rx_buf_head + 1 ) % LOT_UART1_RX_BUF_SIZE;
    if ( rx_buf_head == rx_buf_tail )
    {
        rx_buf_tail = ( rx_buf_tail + 1 ) % LOT_UART1_RX_BUF_SIZE;
    }
}

ISR( LOT_USART_UDRE_vect )
{
    LOT_UDR     = tx_buf[tx_buf_tail];
    tx_buf_tail = ( tx_buf_tail + 1 ) % LOT_UART1_TX_BUF_SIZE;
    LOT_UCSRA |= _BV( LOT_TXC );
    if ( tx_buf_head == tx_buf_tail )
    {
        LOT_UCSRB &= ~_BV( LOT_UDRIE );
    }
}

/**
 * @brief 통신 속도, 데이터 비트, 정지 비트, 패리티 비트를 설정
 * @param uint32_t baud_rate 통신 속도
 * @todo 데이터 비트, 정지 비트, 패리티 비트 설정
 */
void LOT_uart1_begin( const uint32_t baud_rate )
{
#ifndef LOT_UBRR1
    uint16_t temp = ( F_CPU / baud_rate / 16 ) - 1;
#endif
    LOT_UCSRA = 0;
    LOT_UCSRB = _BV( LOT_RXCIE ) | _BV( LOT_RXEN ) | _BV( LOT_TXEN );
    LOT_UCSRC = _BV( LOT_UCSZ1 ) | _BV( LOT_UCSZ0 );
#ifdef LOT_UBRR1
    LOT_UBRR1 = ( uint16_t )( ( F_CPU / baud_rate / 16 ) - 1 );
#else
    LOT_UBRRH = ( uint8_t )( temp >> 8 );
    LOT_UBRRL = ( uint8_t )( temp & 0xFF );
#endif
    sei();
}

/**
 * @brief 수신 버퍼에 있는 값 중 가장 먼저 들어온 값을 읽음(FIFO)
 * @return uint8_t 버퍼에서 읽은 값
 */
uint8_t LOT_uart1_read( void )
{
    if ( rx_buf_head == rx_buf_tail )
        return 0;
    else
    {
        uint8_t buf = rx_buf[rx_buf_tail];
        rx_buf_tail = ( rx_buf_tail + 1 ) % LOT_UART1_RX_BUF_SIZE;
        return buf;
    }
}

/**
 * @brief 송신 중인 데이터가 있으면 송신 버퍼에 추가, 없으면 바로 송신
 * @param uint8_t 송신할 데이터
 */
void LOT_uart1_write( const uint8_t data )
{
    if ( ( tx_buf_head == tx_buf_tail ) && ( LOT_UCSRA & _BV( UDRE0 ) ) )
    {
        LOT_UDR = data;
        LOT_UCSRA |= _BV( LOT_TXC );
        return;
    }
    tx_buf[tx_buf_head] = data;
    tx_buf_head         = ( tx_buf_head + 1 ) % LOT_UART1_TX_BUF_SIZE;
    while ( tx_buf_head == tx_buf_tail )
    {
    }
    LOT_UCSRB |= _BV( LOT_UDRIE );
}

/**
 * @brief 문자열 송신
 * @param char *str 송신할 문자열
 */
void LOT_uart1_print_str( const char *str )
{
    while ( *str )
    {
        LOT_uart1_write( *str++ );
    }
}

/**
 * @brief 수신 버퍼에 읽지 않은 데이터 확인
 * @return uint8_t 읽지 않은 데이터 개수
 */
uint8_t LOT_uart1_available( void )
{
    return ( LOT_UART1_RX_BUF_SIZE + rx_buf_head - rx_buf_tail ) % LOT_UART1_RX_BUF_SIZE;
}

/**
 * @brief 수신 버퍼 비움
 */
void LOT_uart1_flush( void )
{
    rx_buf_head = 0;
    rx_buf_tail = 0;
    rx_buf[0]   = 0;
}
