/********************************************************************
 *  filename:   RCRE.h
 *  created:    2017/10/05  1:28
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  RC receiver
 ********************************************************************/

#ifndef RCRE_H_
#define RCRE_H_

#include <avr/io.h>

extern volatile uint32_t rcre_pulse_width[4];

void rcre_setup(void);
uint8_t rcre_connection_state(void);

#endif /* RCRE_H_ */