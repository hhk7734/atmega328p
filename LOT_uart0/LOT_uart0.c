/**
 * @file LOT_uart0.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief
 */

#include "LOT_uart0.h"

void uart0_init_all( const uint32_t baud_rate,
                     const uint8_t  data_bits,
                     const uint8_t  stop_bits,
                     const uint8_t  parity )
{
}

void uart0_putchar( const char c ) {}

void uart0_putstr( const char *str ) {}

char uart0_getchar( void ) {}

void uart0_available( void ) {}

void uart0_flush( void ) {}

ISR( USART_RX_vect ) {}

ISR( USART_UDRE_vect ) {}