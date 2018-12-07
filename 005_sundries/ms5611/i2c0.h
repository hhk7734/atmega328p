/***********************************************************************************
 * filename:  i2c0.h
 *
 * Created:   2018/01/25
 * edited:    2018/01/26
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _I2C0_H_
#define _I2C0_H_

#include <inttypes.h>

#if defined(__AVR_ATmega128A__) || defined(__AVR_ATmega328P__)
#else
#error "maybe unsupported device"
#endif

#ifndef F_CPU
#warning "F_CPU is not defined, default 16MHz"
#define F_CPU 16000000UL
#endif

class i2c0_
{
public :
    i2c0_();
    ~i2c0_();
    void    begin(const uint32_t &bit_rate);
    inline void begin(void) {begin(400000);}
    void    write_REG(const uint8_t &addr, const uint8_t &regaddr , const uint8_t &data);
    uint8_t read_REG (const uint8_t &addr, const uint8_t &regaddr);
    void    read_REG (const uint8_t &addr, const uint8_t &regaddr, uint8_t *buffer, const uint8_t &count);
    uint8_t get_errors_count(void);

protected :
    void    trans_data(const uint8_t &data);
    void    start_SLA_W(const uint8_t &addr);
    void    start_SLA_R(const uint8_t &addr);
    uint8_t receive_data(void);
    uint8_t last_receive_data(void);
    void    stop(void);

private :
    static uint8_t errors_count;
    void __attribute__ ((noinline)) trans_and_check(const uint8_t &twcr);

}; // i2c0_

extern i2c0_ i2c0;
#endif // _I2C0_H_