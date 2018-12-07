/***********************************************************************************
 * filename:  i2c0.cpp
 *
 * Created:   2018/01/25
 * edited:    2018/01/26
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "i2c0.h"

#include <avr/io.h>

//
// Static Variables initialization
// type i2c0_::variables = init;
//
uint8_t i2c0_::errors_count = 0;

//
// Constructor & Destructor
//
i2c0_::i2c0_()
{
    // variables initialization
}
i2c0_::~i2c0_() {}

//
// Public
//
void i2c0_::begin(const uint32_t &bit_rate)
{
    // prescaler = 1
    // I2C enable
    TWSR = 0;
    TWBR = ((F_CPU / bit_rate) - 16) / 2;
    TWCR = 1<<TWEN;
}

void i2c0_::write_REG(const uint8_t &addr, const uint8_t &regaddr , const uint8_t &data)
{
    start_SLA_W(addr);
    trans_data(regaddr);
    trans_data(data);
    stop();
}

uint8_t i2c0_::read_REG(const uint8_t &addr, const uint8_t &regaddr)
{
    start_SLA_W(addr);
    trans_data(regaddr);
    start_SLA_R(addr);
    uint8_t data = last_receive_data();
    return data;
}

void i2c0_::read_REG(const uint8_t &addr, const uint8_t &regaddr, uint8_t *buffer, const uint8_t &count)
{
    start_SLA_W(addr);
    trans_data(regaddr);
    start_SLA_R(addr);
    for (uint8_t i = 0; i < count-1; ++i)
    {
        *buffer++ = receive_data();
    }
    *buffer = last_receive_data();
}

uint8_t i2c0_::get_errors_count(void)
{
    return errors_count;
}

//
// Protected
//
void i2c0_::trans_data(const uint8_t &data)
{
    TWDR = data;
    trans_and_check((1<<TWINT) | (1<<TWEN));
}

void i2c0_::start_SLA_W(const uint8_t &addr)
{
    // START condition
    // Slave Address + write bit
    trans_and_check((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
    trans_data(addr<<1);
}

void i2c0_::start_SLA_R(const uint8_t &addr)
{
    //START condition
    //Slave Address + read bit
    trans_and_check((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
    trans_data((addr<<1) | 1);
}

uint8_t i2c0_::receive_data(void)
{
    //return ACK
    trans_and_check((1<<TWINT) | (1<<TWEA) | (1<<TWEN));
    return TWDR;
}

uint8_t i2c0_::last_receive_data(void)
{
    //retrun NACK
    trans_and_check((1<<TWINT) | (1<<TWEN));
    uint8_t twdr = TWDR;
    stop();
    return twdr;
}

void i2c0_::stop(void)
{
    TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

//
// Private
//
void __attribute__ ((noinline)) i2c0_::trans_and_check(const uint8_t &twcr)
{
    // 수신 후 수신 완료 확인
    // 일정시간 안에 확인 안되면 error++
    TWCR = twcr;
    uint8_t count = 0xFF;
    while(!(TWCR & (1 << TWINT))){
        count--;
        if(count == 0){
            TWCR = 0;
            errors_count++;
            break;
        }
    }
}

//
// i2c0_ [Object name] = i2c0_();
//
i2c0_ i2c0 = i2c0_();