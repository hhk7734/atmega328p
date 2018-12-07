/**
 * @file LOT_i2c0_slave.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief I2C0 슬레이브 모드
 */

#ifndef _LOT_I2C0_SLAVE_H_
#define _LOT_I2C0_SLAVE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if !defined( __AVR_ATmega328P__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
#define LOT_I2C0_SLAVE_REGISTER_LENGTH 128
/// @}

#ifndef _LOT_STATUS_TYPEDEF_
#    define _LOT_STATUS_TYPEDEF_
typedef enum
{
    LOT_OK = 0x00,
    LOT_ERROR,
    LOT_BUSY,
    LOT_TIMEOUT
} LOT_status_typedef;
#endif

void    LOT_i2c0_slave_begin( const uint8_t slave_address );
uint8_t LOT_i2c0_slave_read_register( const uint8_t reg_address );
void    LOT_i2c0_slave_write_register( const uint8_t reg_address, const uint8_t data );

#ifdef __cplusplus
}
#endif
#endif // _LOT_I2C0_SLAVE_H_
