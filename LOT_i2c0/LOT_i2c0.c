/**
 * @file LOT_i2c0.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief I2C0 마스터 모드
 */

#include "LOT_i2c0.h"
#include <avr/io.h>
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
#    include <util/delay.h>
#endif

#define LOT_i2c0_transmit( x ) \
    TWDR = ( x );              \
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) )

/**
 * @brief
 * I2C 통신을 통해 데이터를 전송\n
 * 자주 호출되는 함수 -> noinline
 * @param uint8_t twcr I2C0 컨트롤 레지스터
 * @return LOT_status_typedef
 */
__attribute__( ( noinline ) ) static LOT_status_typedef LOT_i2c0_transmit_and_check( uint8_t twcr )
{
    TWCR = twcr;
#if LOT_I2C0_TIME_OUT <= 0xFF
    uint8_t count = LOT_I2C0_TIME_OUT;
#else
    uint16_t count = LOT_I2C0_TIME_OUT;
#endif
    while ( !( TWCR & _BV( TWINT ) ) )
    {
        --count;
        if ( count == 0 )
        {
            TWCR = 0;
            return LOT_ERROR;
        }
    }
    return LOT_OK;
}

/**
 * @brief I2C0 통신 속도 설정
 * @param LOT_i2c0_speed_typedef speed
 */
void LOT_i2c0_begin( const LOT_i2c0_speed_typedef speed )
{
    TWSR = 0;
    if ( speed == LOT_I2C0_STANDARD_MODE )
    {
        TWBR = ( ( uint8_t )( F_CPU / 100000UL ) - 16 ) / 2;
    }
    else if ( speed == LOT_I2C0_FAST_MODE )
    {
        TWBR = ( ( uint8_t )( F_CPU / 400000UL ) - 16 ) / 2;
    }
}

/**
 * @brief 슬레이브 장치로 데이터 송신
 * @param uint8_t address 슬레이브 장치 주소
 * @param uint8_t data 송신할 데이터
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_i2c0_transmit_data( const uint8_t address, const uint8_t data )
{
    /// START
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
    /// 슬레이브 주소 << 1 | 쓰기 비트 0
    TWDR = address << 1;
    if ( LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) ) == LOT_OK )
    {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// DATA
        LOT_i2c0_transmit( data );
        /// STOP
        TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );
        return LOT_OK;
    }
    return LOT_ERROR;
}

/**
 * @brief 슬레이브 장치로부터 데이터 수신
 * @param uint8_t address 슬레이브 장치 주소
 * @return uint8_t 수신된 데이터
 */
uint8_t LOT_i2c0_receive_data( const uint8_t address )
{
    /// START
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
    /// 슬레이브 주소 << 1 | 읽기 비트 1
    TWDR = ( address << 1 ) | 1;
    if ( LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) ) == LOT_OK )
    {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// DATA (NACK)
        LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) );
        uint8_t twdr = TWDR;
        /// STOP
        TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );
        return twdr;
    }
    return -1;
}

/**
 * @brief 슬레이브 장치의 레지스터에 데이터 쓰기
 * @param uint8_t address 슬레이브 장치 주소
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @param uint8_t data 레지스터에 쓸 데이터
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_i2c0_write_register( const uint8_t address, const uint8_t reg_address, const uint8_t data )
{
    /// START
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
    /// 슬레이브 주소 << 1 | 쓰기 비트 0
    TWDR = address << 1;
    if ( LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) ) == LOT_OK )
    {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// 레지스터 주소
        LOT_i2c0_transmit( reg_address );
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// DATA
        LOT_i2c0_transmit( data );
        /// STOP
        TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );
        return LOT_OK;
    }
    return LOT_ERROR;
}

/**
 * @brief 슬레이브 장치의 연속적인 레지스터에 데이터 쓰기
 * @param uint8_t address 슬레이브 장치 주소
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @param uint8_t *buf 레지스터에 쓸 데이터
 * @param int8_t count 레지스터에 쓸 데이터 수
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_i2c0_write_successive_register( const uint8_t address, const uint8_t reg_address,
                                                       const uint8_t *buf, int8_t count )
{
    /// START
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
    /// 슬레이브 주소 << 1 | 쓰기 비트 0
    TWDR = address << 1;
    if ( LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) ) == LOT_OK )
    {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// 레지스터 주소
        LOT_i2c0_transmit( reg_address );
        while ( count-- > 0 )
        {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
            _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
            /// DATA
            LOT_i2c0_transmit( *buf++ );
        }
        /// STOP
        TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );
        return LOT_OK;
    }
    return LOT_ERROR;
}

/**
 * @brief 슬레이브 장치의 레지스터에서 데이터 읽기
 * @param uint8_t address 슬레이브 장치 주소
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @return uint8_t 레지스터에서 읽은 데이터
 */
uint8_t LOT_i2c0_read_register( const uint8_t address, const uint8_t reg_address )
{
    /// START
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
    /// 슬레이브 주소 << 1 | 쓰기 비트 0
    TWDR = address << 1;
    if ( LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) ) == LOT_OK )
    {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// 레지스터 주소
        LOT_i2c0_transmit( reg_address );
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// REPEATED START
        LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
        /// 슬레이브 주소 << 1 | 읽기 비트 1
        LOT_i2c0_transmit( ( address << 1 ) | 1 );
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// DATA (NACK)
        LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) );
        uint8_t twdr = TWDR;
        /// STOP
        TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );
        return twdr;
    }
    return -1;
}

/**
 * @brief 슬레이브 장치의 연속적인 레지스터에서 데이터 읽기
 * @param uint8_t address 슬레이브 장치 주소
 * @param uint8_t reg_address 슬레이브 장치의 레지스터 주소
 * @param uint8_t *buf 레지스터에서 읽은 데이터
 * @param int8_t count 레지스터에서 읽을 데이터 수
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_i2c0_read_successive_register( const uint8_t address, const uint8_t reg_address, uint8_t *buf,
                                                      int8_t count )
{
    /// START
    LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
    /// 슬레이브 주소 << 1 | 쓰기 비트 0
    TWDR = address << 1;
    if ( LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) ) == LOT_OK )
    {
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// 레지스터 주소
        LOT_i2c0_transmit( reg_address );
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        /// REPEATED START
        LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWSTA ) | _BV( TWEN ) );
        /// 슬레이브 주소 << 1 | 읽기 비트 1
        LOT_i2c0_transmit( ( address << 1 ) | 1 );
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
        _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        while ( count-- > 1 )
        {
            /// DATA (ACK) ...
            LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEA ) | _BV( TWEN ) );
            *buf++ = TWDR;
#if LOT_I2C0_TRANSFER_INTERVAL_US != 0
            _delay_us( LOT_I2C0_TRANSFER_INTERVAL_US );
#endif
        }
        /// DATA (NACK)
        LOT_i2c0_transmit_and_check( _BV( TWINT ) | _BV( TWEN ) );
        *buf = TWDR;
        /// STOP
        TWCR = _BV( TWINT ) | _BV( TWSTO ) | _BV( TWEN );
        return LOT_OK;
    }
    return LOT_ERROR;
}