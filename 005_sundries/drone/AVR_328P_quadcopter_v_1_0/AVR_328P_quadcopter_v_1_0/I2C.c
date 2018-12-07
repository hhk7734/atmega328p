/********************************************************************
 *  filename:   I2C.c
 *  created:    2017/09/18  21:57
 *  edited :    2017/10/05  1:28
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  i2c comunication
 ********************************************************************/

#include "I2C.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_ATmega328P__)
#else
#error "unsupported device"
#endif

static uint8_t i2c_errors_count = 0;

static void __attribute__ ((noinline)) i2c_trans_and_check(uint8_t twcr)
{
    // 수신 후 수신 완료 확인
    // 일정시간 안에 확인 안되면 error++
    TWCR = twcr;
    uint8_t count = 0xFF;
    while(!(TWCR & (1 << TWINT))){
        count--;
        if(count == 0){
            TWCR = 0;
            i2c_errors_count++;
            break;
        }
    }
}

void i2c_begin(uint32_t bit_rate)
{
    // prescaler = 1
    // I2C enable
    TWSR = 0;
    TWBR = ((F_CPU / bit_rate) - 16) / 2;
    TWCR = 1<<TWEN;
}

void i2c_trans_data(uint8_t data)
{
    TWDR = data;
    i2c_trans_and_check((1<<TWINT) | (1<<TWEN));
}

void i2c_start_SLA_W(uint8_t addr)
{
    // START condition
    // Slave Address + write bit
    i2c_trans_and_check((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
    i2c_trans_data(addr<<1);
}

void i2c_start_SLA_R(uint8_t addr)
{
    //START condition
    //Slave Address + read bit
    i2c_trans_and_check((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
    i2c_trans_data((addr<<1) | 1);
}

uint8_t i2c_receive_data(void)
{
    //return ACK
    i2c_trans_and_check((1<<TWINT) | (1<<TWEA) | (1<<TWEN));
    return TWDR;
}

uint8_t i2c_last_receive_data(void)
{
    //retrun NACK
    i2c_trans_and_check((1<<TWINT) | (1<<TWEN));
    uint8_t twdr = TWDR;
    i2c_stop();
    return twdr;
}

void i2c_stop(void)
{
    TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

uint8_t i2c_get_erros_count(void)
{
    return i2c_errors_count;
}

void i2c_write_REG(uint8_t addr, uint8_t regaddr , uint8_t data)
{
    i2c_start_SLA_W(addr);
    i2c_trans_data(regaddr);
    i2c_trans_data(data);
    i2c_stop();
}

uint8_t i2c_read_REG(uint8_t addr, uint8_t regaddr)
{
    i2c_start_SLA_W(addr);
    i2c_trans_data(regaddr);
    i2c_start_SLA_R(addr);
    //uint8_t data = i2c_receive_data();
    //i2c_stop();
    uint8_t data = i2c_last_receive_data();
    return data;
}