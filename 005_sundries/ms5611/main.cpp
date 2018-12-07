/***********************************************************************************
 * filename:  main.cpp
 *
 * Created:   2018/02/03
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
// SDA --- PD1/SDA/ INT1
// SCL --- PD0/SCL/ INT0

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "uart1.h"
#include "ms5611.h"
#include "tc2time.h"

int main(int argc, char **argv)
{
    uart1.begin(57600);
    ms5611.begin();

    uart1.println("start");

    for(;;)
    {
        _delay_ms(20);
        ms5611.update(tc2.micros());
        uart1.print(ms5611.get_temperature());
        uart1.print(',');
        uart1.println(ms5611.get_pressure());
    }

    return 0;
}