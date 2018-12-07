/***********************************************************************************
 * filename:  main.cpp
 *
 * Created:   2018/03/01
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

// ATmega128A
// OC1A/PB5
// OC1B/PB6
// CO1C/PB7/OC2

#define F_CPU 16000000UL

#include <avr/io.h>

#include "tc1pwm.h"

int main(int argc, char **argv)
{
    // setup
    tc1pwm.setup();

    tc1pwm.motor1(1000);
    tc1pwm.motor2(1500);
    tc1pwm.motor3(2000);
    for(;;)
    {
        // loop

    }

    return 0;
}