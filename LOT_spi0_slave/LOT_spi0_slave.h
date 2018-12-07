/**
 * filename : LOT_spi0_slave.h
 *
 * Hyeon-ki, Hong
 * hhk7734@gmail.com
 *
 * purpose : SPI slave mode
 */

//
// ! don't read or write data without interval !
//

#ifndef _LOT_SPI0_SLAVE_H_
#define _LOT_SPI0_SLAVE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if defined( __AVR_ATmega328P__ )
#else
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

#define LOT_SPI0_SLAVE_MSB_FIRST 0
#define LOT_SPI0_SLAVE_LSB_FIRST 1
#define LOT_SPI0_SLAVE_CLK_IDLE_LOW 0
#define LOT_SPI0_SLAVE_CLK_IDLE_HIGH 1
#define LOT_SPI0_SLAVE_CLK_SAMPLING_1_EDGE 0
#define LOT_SPI0_SLAVE_CLK_SAMPLING_2_EDGE 1

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
#define LOT_SPI0_SLAVE_DATA_ORDER LOT_SPI0_SLAVE_MSB_FIRST
#define LOT_SPI0_SLAVE_CLK_IDLE_STATE LOT_SPI0_SLAVE_CLK_IDLE_HIGH
#define LOT_SPI0_SLAVE_CLK_SAMPLING_EDGE LOT_SPI0_SLAVE_CLK_SAMPLING_2_EDGE
#define LOT_SPI0_SLAVE_REGISTER_LENGTH 128
/// @}

void    LOT_spi0_slave_begin( void );
uint8_t LOT_spi0_slave_read_register( const uint8_t reg_address );
void    LOT_spi0_slave_write_register( const uint8_t reg_address, const uint8_t data );

#ifdef __cplusplus
}
#endif
#endif /* _LOT_SPI0_SLAVE_H_ */