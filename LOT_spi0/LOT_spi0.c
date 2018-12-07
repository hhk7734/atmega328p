/**
 * @file LOT_spi0.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief SPI0 마스터 모드
 */

#include "LOT_spi0.h"

#include <avr/io.h>
#if LOT_SPI0_TRANSFER_INTERVAL_US != 0
#    include <util/delay.h>
#endif

/// 데이터 송신 매크로
#define LOT_spi0_transmit( x )        \
    time_out = LOT_SPI0_TIME_OUT;     \
    SPDR     = x;                     \
    while ( !( SPSR & _BV( SPIF ) ) ) \
    {                                 \
        --time_out;                   \
        if ( time_out == 0 )          \
        {                             \
            *port |= _BV( pin );      \
            return LOT_ERROR;         \
        }                             \
    }

/**
 * @brief SPI0 핀 모드, 통신 방법, 통신 속도 설정
 * @param LOT_spi0_clk_divider_typedef clock_divider
 */
void LOT_spi0_begin( const LOT_spi0_clk_divider_typedef clock_divider )
{
    /// MOSI, SCK -- output, MISO -- input
    DDRB |= _BV( DDB5 ) | _BV( DDB3 );
    DDRB &= ~_BV( DDB4 );

    /// 마스터 모드, 데이터 순서, 클럭 기본 상태, 샘플링 위치 설정
    SPCR = _BV( SPE ) | ( LOT_SPI0_DATA_ORDER << DORD ) | _BV( MSTR ) | ( LOT_SPI0_CLK_IDLE_STATE << CPOL )
           | ( LOT_SPI0_CLK_SAMPLING_EDGE << CPHA );

    /// 통신 속도 설정
    SPSR &= ~_BV( SPI2X );
    switch ( clock_divider )
    {
        case LOT_SPI0_SCK_DIV_2:
            SPSR |= _BV( SPI2X );
            break;
        case LOT_SPI0_SCK_DIV_4:
            break;
        case LOT_SPI0_SCK_DIV_8:
            SPSR |= _BV( SPI2X );
            SPCR |= _BV( SPR0 );
            break;
        case LOT_SPI0_SCK_DIV_16:
            SPCR |= _BV( SPR0 );
            break;
        case LOT_SPI0_SCK_DIV_32:
            SPSR |= _BV( SPI2X );
            SPCR |= _BV( SPR1 );
            break;
        case LOT_SPI0_SCK_DIV_64:
            SPCR |= _BV( SPR1 );
            break;
        case LOT_SPI0_SCK_DIV_128:
            SPCR |= _BV( SPR1 ) | _BV( SPR0 );
            break;
    }
}

/**
 * @brief 슬레이브 장치와 데이터 교환
 * @param uint8_t *port 슬레이브 장치와 연결된 CS 포트
 * @param uint8_t pin 슬레이브 장치와 연결된 CS 핀
 * @param uint8_t data 송신할 데이터
 * @return uint8_t 수신된 데이터
 */
uint8_t LOT_spi0_transmit_receive( volatile uint8_t *port, const uint8_t pin, const uint8_t data )
{
    /// 슬레이브 선택
    *port &= ~_BV( pin );
#if LOT_SPI0_TIME_OUT <= 255
    uint8_t time_out;
#else
    uint16_t time_out;
#endif
    /// 데이터 송신
    LOT_spi0_transmit( data );
    /// 슬레이브 해제
    *port |= _BV( pin );
    /// 데이터 수신
    return SPDR;
}

/**
 * @brief 슬레이브 장치와 배열 데이터 교환
 * @param uint8_t *port 슬레이브 장치와 연결된 CS 포트
 * @param uint8_t pin 슬레이브 장치와 연결된 CS 핀
 * @param uint8_t *buf 송신할 데이터, 송신 후 수신된 데이터가 저장 된다.
 * @param int8_t count 배열 크기
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_spi0_transmit_receive_array( volatile uint8_t *port, const uint8_t pin, uint8_t *buf,
                                                    int8_t count )
{
    *port &= ~_BV( pin );
#if LOT_SPI0_TIME_OUT <= 255
    uint8_t time_out;
#else
    uint16_t time_out;
#endif

    while ( count > 1 )
    {
        --count;
        LOT_spi0_transmit( *buf );
        *buf++ = SPDR;
#if LOT_SPI0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_SPI0_TRANSFER_INTERVAL_US );
#endif
    }
    LOT_spi0_transmit( *buf );
    *buf = SPDR;

    *port |= _BV( pin );
    return LOT_OK;
}

/**
 * @brief 슬레이브 장치의 레지스터에 데이터 쓰기
 * @param uint8_t *port 슬레이브 장치와 연결된 CS 포트
 * @param uint8_t pin 슬레이브 장치와 연결된 CS 핀
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @param uint8_t data 레지스터에 쓸 데이터
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_spi0_write_register( volatile uint8_t *port, const uint8_t pin, const uint8_t reg_address,
                                            const uint8_t data )
{
    *port &= ~_BV( pin );
#if LOT_SPI0_TIME_OUT <= 255
    uint8_t time_out;
#else
    uint16_t time_out;
#endif

    LOT_spi0_transmit( reg_address );
#if LOT_SPI0_TRANSFER_INTERVAL_US != 0
    _delay_us( LOT_SPI0_TRANSFER_INTERVAL_US );
#endif
    LOT_spi0_transmit( data );

    *port |= _BV( pin );
    return LOT_OK;
}

/**
 * @brief 슬레이브 장치의 레지스터에서 데이터 읽기
 * @param uint8_t *port 슬레이브 장치와 연결된 CS 포트
 * @param uint8_t pin 슬레이브 장치와 연결된 CS 핀
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @return uint8_t 레지스터에서 읽은 데이터
 */
uint8_t LOT_spi0_read_register( volatile uint8_t *port, const uint8_t pin, const uint8_t reg_address )
{
    *port &= ~_BV( pin );
#if LOT_SPI0_TIME_OUT <= 255
    uint8_t time_out;
#else
    uint16_t time_out;
#endif

    LOT_spi0_transmit( reg_address | 0x80 );
#if LOT_SPI0_TRANSFER_INTERVAL_US != 0
    _delay_us( LOT_SPI0_TRANSFER_INTERVAL_US );
#endif
    LOT_spi0_transmit( 0 );

    *port |= _BV( pin );
    return SPDR;
}

/**
 * @brief 슬레이브 장치의 연속적인 레지스터에서 데이터 읽기
 * @param uint8_t *port 슬레이브 장치와 연결된 CS 포트
 * @param uint8_t pin 슬레이브 장치와 연결된 CS 핀
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @param uint8_t *buf 레지스터에서 읽은 데이터
 * @param int8_t count 레지스터에서 읽을 데이터 수
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_spi0_read_successive_register( volatile uint8_t *port, const uint8_t pin,
                                                      const uint8_t reg_address, uint8_t *buf, int8_t count )
{
    *port &= ~_BV( pin );
#if LOT_SPI0_TIME_OUT <= 255
    uint8_t time_out;
#else
    uint16_t time_out;
#endif

    LOT_spi0_transmit( reg_address | 0x80 );
    while ( count > 0 )
    {
        --count;
#if LOT_SPI0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_SPI0_TRANSFER_INTERVAL_US );
#endif
        LOT_spi0_transmit( 0 );
        *buf++ = SPDR;
    }

    *port |= _BV( pin );
    return LOT_OK;
}