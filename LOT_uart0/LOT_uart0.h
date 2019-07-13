/**
 * @file LOT_uart0.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief ATmega328p UART0 통신
 */

#ifndef _LOT_UART0_H_
#define _LOT_UART0_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

/**
 * @brief UART 통신 초기화
 * @param baud_rate
 * @param data_bits 5, 6, 7, 8 or 9 bits
 * @param stop_bits 1 or 2 bits
 * @param parity 0-none, 1-odd, 2-even
 */
void uart0_init_all( const uint32_t baud_rate,
                     const uint8_t  data_bits,
                     const uint8_t  stop_bits,
                     const uint8_t  parity );

void uart0_init( const uint32_t baud_rate ) { uart0_init_all( baud_rate, 8, 1, 0 ); }

void uart0_putchar( const char c );

void uart0_putstr( const char *str );

char uart0_getchar( void );

void uart0_available( void );

void uart0_flush( void );

#ifdef __cplusplus
}
#endif
#endif    // _LOT_UART0_H_
