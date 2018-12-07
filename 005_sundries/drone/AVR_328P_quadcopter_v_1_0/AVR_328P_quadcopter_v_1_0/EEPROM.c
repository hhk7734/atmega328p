/********************************************************************
 *  filename:   EEPROM.c
 *  created:    2017/09/19  0:41
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  EEPROM
 ********************************************************************/

#include "EEPROM.h"
#include "config.h"
#include "MPU9150.h"
#include <avr/io.h>

#if defined(__AVR_ATmega328P__)
#else
    #error "unsupported device"
#endif

static void eeprom_write(uint16_t _addr, uint8_t _data)
{
    while(EECR & (1<<EEPE));
    EEARH = _addr>>8;
    EEARL = _addr;
    EEDR = _data;
    EECR |= (1<<EEMPE);
    EECR |= (1<<EEPE);
}

uint8_t eeprom_read(uint16_t _addr)
{
    while(EECR & (1<<EEPE));
    EEARH = _addr>>8;
    EEARL = _addr;
    EECR |= (1<<EERE);
    return EEDR;
}

void eeprom_update(uint16_t _addr, uint8_t _data)
{
    if(_data != eeprom_read(_addr)) eeprom_write(_addr,_data);
}

void eeprom_write_i16(uint16_t _addr,int16_t _data)
{
    union{
        int16_t A;
        uint8_t a[2];
    }VAL;
    VAL.A = _data;
    //  eeprom_write(_addr,VAL.a[0]);
    //  eeprom_write(_addr + 1,VAL.a[1]);
    eeprom_update(_addr,VAL.a[0]);
    eeprom_update(_addr + 1,VAL.a[1]);
}

int16_t eeprom_read_i16(uint16_t _addr)
{
    union{
        int16_t A;
        uint8_t a[2];
    }VAL;
    VAL.a[0] = eeprom_read(_addr);
    VAL.a[1] = eeprom_read(_addr + 1);
    return VAL.A;
}