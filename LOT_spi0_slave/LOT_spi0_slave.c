/**
 * filename : LOT_spi0_slave.c
 *
 * Hyeon-ki, Hong
 * hhk7734@gmail.com
 *
 * purpose : SPI slave mode
 */

#include "LOT_spi0_slave.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile static uint8_t slave_register[LOT_SPI0_SLAVE_REGISTER_LENGTH] = { 0 };
volatile static uint8_t slave_register_address;
volatile static uint8_t slave_read_state    = 0;
volatile static uint8_t slave_address_state = 1;

ISR( SPI_STC_vect )
{
    //
    // ! don't read or write data without interval !
    //
    uint8_t temp = SPDR;

    // if master transmit 0 again, master can read next register
    if ( temp && slave_address_state )
    {
        slave_register_address = temp & 0x7F;
        slave_read_state       = temp & 0x80;

        // master read slave register
        if ( slave_read_state )
        {
            SPDR = slave_register[slave_register_address];
        }

        // master write slave register
        // do nothing

        slave_address_state = 0;
    }
    else
    {
        // master read slave register
        // if master transmit 0 again, master can read next register
        if ( slave_read_state )
        {
            if ( slave_register_address < LOT_SPI0_SLAVE_REGISTER_LENGTH - 1 )
            {
                SPDR = slave_register[++slave_register_address];
            }
        }
        else
        {
            // master write slave register
            slave_register[slave_register_address] = temp;
        }

        slave_address_state = 1;
    }
}

/**
 * @brief SPI0 핀 모드, 통신 방법, 슬레이브 모드 설정
 */
void LOT_spi0_slave_begin( void )
{
    /// SCK, MOSI, SS -- input, MISO -- output
    DDRB &= ~( _BV( DDB5 ) | _BV( DDB3 ) | _BV( DDB2 ) );
    DDRB |= _BV( DDB4 );

    /// 인터럽트, 데이터 순서, 클럭 기본 상태, 샘플링 위치, 슬레이브 모드 설정
    SPCR = _BV( SPIE ) | _BV( SPE ) | ( LOT_SPI0_SLAVE_DATA_ORDER << DORD ) | ( LOT_SPI0_SLAVE_CLK_IDLE_STATE << CPOL )
           | ( LOT_SPI0_SLAVE_CLK_SAMPLING_EDGE << CPHA );
    SPCR &= ~_BV( MSTR );
    sei();
}

/**
 * @brief 레지스터 데이터 읽기
 * @param uint8_t reg_address 레지스터 주소
 * @return uint8_t 레지스터에서 읽은 데이터
 */
uint8_t LOT_spi0_slave_read_register( const uint8_t reg_address )
{
    if ( reg_address < LOT_SPI0_SLAVE_REGISTER_LENGTH )
    {
        return slave_register[reg_address];
    }
    else
    {
        return -1;
    }
}

/**
 * @brief 레지스터에 데이터 쓰기
 * @param uint8_t reg_address 레지스터 주소
 * @param uint8_t data 레지스터에 쓸 데이터
 */
void LOT_spi0_slave_write_register( const uint8_t reg_address, const uint8_t data )
{
    if ( reg_address < LOT_SPI0_SLAVE_REGISTER_LENGTH )
    {
        slave_register[reg_address] = data;
    }
}