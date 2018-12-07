/**
 * @file LOT_uart0.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief UART 통신
 */

#ifndef _LOT_UART0_H_
#define _LOT_UART0_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
/// 연산 속도를 위해 2^n 값만 사용
#define LOT_UART0_RX_BUF_SIZE 64
/// 연산 속도를 위해 2^n 값만 사용
#define LOT_UART0_TX_BUF_SIZE 64
/// @}

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega128__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__, __AVR_ATmega128__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

void    LOT_uart0_begin( const uint32_t baud_rate );
uint8_t LOT_uart0_read( void );
void    LOT_uart0_write( const uint8_t data );
void    LOT_uart0_print_str( const char *str );
uint8_t LOT_uart0_available( void );
void    LOT_uart0_flush( void );

#ifdef __cplusplus
}
#endif
#endif /* _LOT_UART0_H_ */