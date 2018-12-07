/***********************************************************************************
 * filename:  main.cpp
 *
 * Created:   2018/01/29
 * edited:    2018/02/01
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/
// ATmega128A
// VIN --- VCC
// GND --- GND
// RX  --- PE1/TXD0/PDO ==> ISP와 동시에 사용 불가
// TX  --- PE0/RXD0/PDI ==> ISP와 동시에 사용 불가
// OR
// RX  --- PD3/TXD1/INT3
// TX  --- PD2/RXD1/INT2

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "uart1.h"
#include "mtk3339.h"

//#define

int main(int argc, char **argv)
{
    // setup
    uart1.begin(57600);
    mtk3339.begin(57600);

    uart1.println("start");

    for(;;)
    {
        if(mtk3339.update())
        {
            uart1.println(mtk3339.get_latitude());  // -----
            uart1.println(mtk3339.get_longitude()); //   |
        }

        while(uart1.available())
        {
            mtk3339.write(uart1.read());
        }

        _delay_ms(2);
    }

    return 0;
}