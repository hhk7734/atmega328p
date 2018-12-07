/********************************************************************
 *  filename:   RPY.h
 *  created:    2017/10/02  0:48
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  calculate roll pitch yaw
 ********************************************************************/

#ifndef RPY_H_
#define RPY_H_

#include <avr/io.h>

void rpy_setup(void);
void rpy_get_rpy(int16_t *rpy);

#endif /* RPY_H_ */