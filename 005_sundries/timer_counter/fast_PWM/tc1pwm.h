/***********************************************************************************
 * filename:  tc1pwm.h
 *
 * Created:   2018/03/01
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _TC1PWM_H_
#define _TC1PWM_H_

#include <inttypes.h>

#if defined(__AVR_ATmega128A__)
#else
#error "unsupported device"
#endif

#if F_CPU!=16000000UL
#warning "F_CPU is not 16MHz, don't use this code."
#endif

class tc1pwm_
{
public :
    tc1pwm_();
    ~tc1pwm_();

    void setup(void);
    void motor1(uint16_t _us);
    void motor2(uint16_t _us);
    void motor3(uint16_t _us);
protected :

private :

}; // tc1pwm_

extern tc1pwm_ tc1pwm;
#endif // _TC1PWM_H_