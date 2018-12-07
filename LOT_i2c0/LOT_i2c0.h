/**
 * @file LOT_i2c0.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief I2C0 마스터 모드
 */

#ifndef _LOT_I2C0_H_
#define _LOT_I2C0_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

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

typedef enum
{
    /// 100kbps
    LOT_I2C0_STANDARD_MODE = 0x00,
    /// 400kbps
    LOT_I2C0_FAST_MODE
} LOT_i2c0_speed_typedef;

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
/// @warning 통신 간격 확인
/// @todo 16 MHz, 8 bit AVR과 통신하는 경우 간격 확인
#define LOT_I2C0_TRANSFER_INTERVAL_US 0
/// @}

#define LOT_I2C0_TIME_OUT 255

#if !defined( __AVR_ATmega328P__ ) && !defined( __AVR_ATmega128__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__, __AVR_ATmega128__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

void               LOT_i2c0_begin( const LOT_i2c0_speed_typedef speed );
LOT_status_typedef LOT_i2c0_transmit_data( const uint8_t address, const uint8_t data );
uint8_t            LOT_i2c0_receive_data( const uint8_t address );
LOT_status_typedef LOT_i2c0_write_register( const uint8_t address, const uint8_t reg_address, const uint8_t data );
LOT_status_typedef LOT_i2c0_write_successive_register( const uint8_t address, const uint8_t reg_address,
                                                       const uint8_t *buf, int8_t count );
uint8_t            LOT_i2c0_read_register( const uint8_t address, const uint8_t reg_address );
LOT_status_typedef LOT_i2c0_read_successive_register( const uint8_t address, const uint8_t reg_address, uint8_t *buf,
                                                      int8_t count );

#ifdef __cplusplus
}
#endif
#endif // _LOT_I2C0_H_
