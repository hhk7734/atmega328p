/********************************************************************
 *  filename:   TC.h
 *  created:    2017/09/23  11:30
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  PWM 모터 제어, micros()
 ********************************************************************/

#ifndef TC_H_
#define TC_H_

#include <avr/io.h>

#ifndef F_CPU
#warning "F_CPU is not defined, default 16MHz"
#define F_CPU 16000000UL
#endif

void        tc_setup(void);
uint32_t    tc_micros(void);
uint32_t    tc_millis(void);

void        tc_motor_0(int16_t _us);
void        tc_motor_1(int16_t _us);
void        tc_motor_2(int16_t _us);
void        tc_motor_3(int16_t _us);
void        tc_motor_All(int16_t _us);

#endif /* TC_H_ */