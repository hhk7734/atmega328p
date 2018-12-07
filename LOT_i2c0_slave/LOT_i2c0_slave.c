/**
 * @file LOT_i2c0_slave.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief I2C0 슬레이브 모드
 */

#include "LOT_i2c0_slave.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define SLAVE_ACK_AFTER_MASTER_SLA_R 0xA8
#define MASTER_ACK_AFTER_SLAVE_DATA 0xB8
#define MASTER_NACK_AFTER_SLAVE_DATA 0xC0
#define MASTER_ACK_AFTER_SLAVE_LAST_DATA 0xC8
#define SLAVE_ACK_AFTER_MASTER_SLA_W 0x60
#define SLAVE_ACK_AFTER_MASTER_DATA 0x80
#define SLAVE_NACK_AFTER_MASTER_DATA 0x88
#define AFTER_MASTER_STOP 0xA0
#define I2C_BUS_ERROR 0x00

#define SLAVE_ACK_RESPONSE TWCR |= _BV( TWINT ) | _BV( TWEA ) | _BV( TWEN ) | _BV( TWIE )
#define SLAVE_NACK_RESPONSE TWCR |= _BV( TWINT ) | _BV( TWEN ) | _BV( TWIE )

volatile static uint8_t slave_register[LOT_I2C0_SLAVE_REGISTER_LENGTH] = { 0 };
volatile static uint8_t slave_register_address;
volatile static uint8_t slave_receive_count;

/**
 * @brief I2C0 인터럽트
 */
ISR( TWI_vect )
{
    switch ( TWSR )
    {
        case SLAVE_ACK_AFTER_MASTER_SLA_R:
            TWDR = slave_register[slave_register_address];
            SLAVE_ACK_RESPONSE;
            break;
        case MASTER_ACK_AFTER_SLAVE_DATA:
            if ( slave_register_address < LOT_I2C0_SLAVE_REGISTER_LENGTH - 1 )
            {
                TWDR = slave_register[++slave_register_address];
                SLAVE_ACK_RESPONSE;
            }
            else
                SLAVE_NACK_RESPONSE;
            break;
        case SLAVE_ACK_AFTER_MASTER_SLA_W:
            slave_receive_count = 0;
            SLAVE_ACK_RESPONSE;
            break;
        case SLAVE_ACK_AFTER_MASTER_DATA:
            if ( slave_receive_count == 0 )
            {
                slave_register_address = TWDR;
                ++slave_receive_count;
                SLAVE_ACK_RESPONSE;
            }
            else
            {
                if ( slave_register_address < LOT_I2C0_SLAVE_REGISTER_LENGTH )
                {
                    slave_register[slave_register_address] = TWDR;
                }
                SLAVE_ACK_RESPONSE;
            }
            break;
        case SLAVE_NACK_AFTER_MASTER_DATA:
            if ( slave_register_address < LOT_I2C0_SLAVE_REGISTER_LENGTH )
            {
                slave_register[slave_register_address] = TWDR;
            }
            break;
        case I2C_BUS_ERROR:
            TWCR |= _BV( TWSTO ) | _BV( TWINT ) | _BV( TWEN ) | _BV( TWIE );
            break;
        default:
            SLAVE_NACK_RESPONSE;
            break;
    }
}

/**
 * @brief I2C0 슬레이브 주소 설정, 슬레이브 모드 설정
 * @param uint8_t slave_address 슬레이브 장치 주소
 */
void LOT_i2c0_slave_begin( const uint8_t slave_address )
{
    TWAR = slave_address << 1;
    SLAVE_ACK_RESPONSE;
    sei();
}

/**
 * @brief 레지스터 데이터 읽기
 * @param uint8_t reg_address 레지스터 주소
 * @return uint8_t 레지스터에서 읽은 데이터
 */
uint8_t LOT_i2c0_slave_read_register( const uint8_t reg_address )
{
    if ( reg_address < LOT_I2C0_SLAVE_REGISTER_LENGTH )
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
void LOT_i2c0_slave_write_register( const uint8_t reg_address, const uint8_t data )
{
    if ( reg_address < LOT_I2C0_SLAVE_REGISTER_LENGTH )
    {
        slave_register[reg_address] = data;
    }
}