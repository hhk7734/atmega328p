/**
 * @file LOT_spi0.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief SPI0 마스터 모드
 */

#ifndef _LOT_SPI0_H_
#define _LOT_SPI0_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if !defined( __AVR_ATmega328P__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

#define LOT_SPI0_MSB_FIRST 0
#define LOT_SPI0_LSB_FIRST 1
#define LOT_SPI0_CLK_IDLE_LOW 0
#define LOT_SPI0_CLK_IDLE_HIGH 1
#define LOT_SPI0_CLK_SAMPLING_1_EDGE 0
#define LOT_SPI0_CLK_SAMPLING_2_EDGE 1

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
/// @warning 16 MHz, 8 bit AVR과 통신하는 경우 최소 5 로 설정
#define LOT_SPI0_TRANSFER_INTERVAL_US 0
#define LOT_SPI0_DATA_ORDER LOT_SPI0_MSB_FIRST
#define LOT_SPI0_CLK_IDLE_STATE LOT_SPI0_CLK_IDLE_HIGH
#define LOT_SPI0_CLK_SAMPLING_EDGE LOT_SPI0_CLK_SAMPLING_2_EDGE
/// @}

typedef enum
{
    /// 시스템 클럭 / 2
    LOT_SPI0_SCK_DIV_2 = 0x00,
    /// 시스템 클럭 / 4
    LOT_SPI0_SCK_DIV_4,
    /// 시스템 클럭 / 8
    LOT_SPI0_SCK_DIV_8,
    /// 시스템 클럭 / 16
    LOT_SPI0_SCK_DIV_16,
    /// 시스템 클럭 / 32
    LOT_SPI0_SCK_DIV_32,
    /// 시스템 클럭 / 64
    LOT_SPI0_SCK_DIV_64,
    /// 시스템 클럭 / 128
    LOT_SPI0_SCK_DIV_128
} LOT_spi0_clk_divider_typedef;

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

#define LOT_SPI0_TIME_OUT 255

void               LOT_spi0_begin( const LOT_spi0_clk_divider_typedef clock_divide );
uint8_t            LOT_spi0_transmit_receive( volatile uint8_t *port, const uint8_t pin, const uint8_t data );
LOT_status_typedef LOT_spi0_transmit_receive_array( volatile uint8_t *port, const uint8_t pin, uint8_t *buf,
                                                    int8_t count );
LOT_status_typedef LOT_spi0_write_register( volatile uint8_t *port, const uint8_t pin, const uint8_t reg_address,
                                            const uint8_t data );
uint8_t            LOT_spi0_read_register( volatile uint8_t *port, const uint8_t pin, const uint8_t reg_address );
LOT_status_typedef LOT_spi0_read_successive_register( volatile uint8_t *port, const uint8_t pin,
                                                      const uint8_t reg_address, uint8_t *buf, int8_t count );

#ifdef __cplusplus
}
#endif
#endif // _LOT_SPI0_H_
