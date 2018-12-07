/********************************************************************
 *  filename:   EEPROM.h
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

#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

uint8_t eeprom_read(uint16_t _addr);
void    eeprom_update(uint16_t _addr, uint8_t _data);
void    eeprom_write_i16(uint16_t _addr,int16_t _data);
int16_t eeprom_read_i16(uint16_t _addr);

#endif /* EEPROM_H_ */