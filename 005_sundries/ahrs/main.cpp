/***********************************************************************************
 * filename:  main.cpp
 *
 * Created:   2018/02/07
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

#include "tc2time.h"
#include "uart1.h"
#include "ahrs.h"

//#define


int main(int argc, char **argv)
{
    // setup
    uart1.begin(57600);

    // todo
    // use EEPROM
    // use get offset
    // use set offset
    int16_t acc[3]  = {29, 35, 36};
    int16_t gyro[3] = {26, 20, -12};
    int16_t mag[3]  = {-220, -10, 485};
    int16_t magse[3] = {16, 16, 17};
    ahrs.begin(acc,gyro,mag,magse);
    uint32_t delta_t;
    uint32_t new_t,old_t = tc2.micros();

    //uart1.println("acc, gyro");
    //while(!uart1.available());
    //uart1.flush();
    //ahrs.acc_calibration();
    //ahrs.gyro_calibration();
    //ahrs.get_offset(acc,gyro,mag,magse);
    //uart1.println(acc[0]);
    //uart1.println(acc[1]);
    //uart1.println(acc[2]);
    //uart1.println(gyro[0]);
    //uart1.println(gyro[1]);
    //uart1.println(gyro[2]);
    //while(!uart1.available());
    //uart1.flush();

    //uart1.println("mag, magse");
    //while(!uart1.available());
    //uart1.flush();
    //ahrs.mag_calibration();
    //ahrs.get_offset(acc,gyro,mag,magse);
    //uart1.println(mag[0]);
    //uart1.println(mag[1]);
    //uart1.println(mag[2]);
    //uart1.println(magse[0]);
    //uart1.println(magse[1]);
    //uart1.println(magse[2]);
    //while(!uart1.available());
    //uart1.flush();

    for(;;)
    {
        // loop
        new_t = tc2.micros();
        delta_t = new_t - old_t;
        old_t = new_t;

        ahrs.update(delta_t);

        int16_t rpy[3];
        ahrs.get_rpy(rpy);
        uart1.print(rpy[0]);uart1.print(',');
        uart1.print(rpy[1]);uart1.print(',');
        uart1.println(rpy[2]);

        _delay_ms(2);
    }

    return 0;
}