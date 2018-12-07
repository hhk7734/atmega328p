/********************************************************************
 *  filename:   I2C.h
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


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

#ifndef F_CPU
#warning "F_CPU is not defined, default 16MHz"
#define F_CPU 16000000UL
#endif

void    i2c_begin(uint32_t bit_rate);
void    i2c_trans_data(uint8_t data);
void    i2c_start_SLA_W(uint8_t addr);
void    i2c_start_SLA_R(uint8_t addr);
uint8_t i2c_receive_data(void);
uint8_t i2c_last_receive_data(void);
void    i2c_stop(void);
uint8_t i2c_get_erros_count(void);

// Example
void    i2c_write_REG(uint8_t addr, uint8_t regaddr , uint8_t data);
uint8_t i2c_read_REG (uint8_t addr, uint8_t regaddr);

#endif /* I2C_H_ */