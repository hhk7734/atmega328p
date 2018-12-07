/**
 * @file LOT_uart1.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief UART 통신
 */

#ifndef _LOT_UART1_H_
#define _LOT_UART1_H_
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
#define LOT_UART1_RX_BUF_SIZE 64
/// 연산 속도를 위해 2^n 값만 사용
#define LOT_UART1_TX_BUF_SIZE 64
/// @}

#if !defined( __AVR_ATmega128__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega128__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

void    LOT_uart1_begin( const uint32_t baud_rate );
uint8_t LOT_uart1_read( void );
void    LOT_uart1_write( const uint8_t data );
void    LOT_uart1_print_str( const char *str );
uint8_t LOT_uart1_available( void );
void    LOT_uart1_flush( void );

#ifdef __cplusplus
}
#endif
#endif /* _LOT_UART1_H_ */