/***********************************************************************************
 * filename:  main.cpp
 *
 * Created:   2018/02/06
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
#include "mpu9250.h"

//#define

int main(int argc, char **argv)
{
    // setup
    uart1.begin(57600);
    mpu9250.begin();
    mpu9250.acc_calibration();
    mpu9250.gyro_calibration();
    mpu9250.mag_calibration();

    for(;;)
    {
        // loop
        mpu9250.update_all();
        int16_t xyz[3];
        mpu9250.get_mag(xyz);
        uart1.print(xyz[0]);
        uart1.print(',');
        uart1.print(xyz[1]);
        uart1.print(',');
        uart1.println(xyz[2]);
        _delay_ms(10);
    }

    return 0;
}