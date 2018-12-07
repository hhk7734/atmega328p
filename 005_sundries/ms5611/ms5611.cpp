/***********************************************************************************
 * filename:  ms5611.cpp
 *
 * Created:   2018/02/03
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "ms5611.h"

#include <avr/io.h>
#include <util/delay.h>

//
// Constructor & Destructor
//
ms5611_::ms5611_()
{
    // variables initialization
    sequence = 0;
    old_time = 0;
    dT = 0;
    temperature = 0;
    OFF = 0;
    SENS = 0;
    pressure = 0;
}
ms5611_::~ms5611_() {}

//
// Public
//
void ms5611_::begin(void)
{
    i2c0_::begin();

    write_REG(MS561101BA_RESET);
    _delay_ms(50);

    for (uint8_t i = 0; i<6 ; ++i)
    {
        // read C1, C2, C3, C4, C5, C6
        // can't read read_REG(,,temp,12);
        start_SLA_W(MS561101BA_I2C_ADDRESS_CSB_LOW);
        trans_data(MS561101BA_PROM_C1 + 2*i);
        start_SLA_R(MS561101BA_I2C_ADDRESS_CSB_LOW);
        if(i != 5)C[i] = receive_data()<<8 | receive_data();
        else C[i] = receive_data()<<8 | last_receive_data();
    }
}

void ms5611_::update(const uint32_t &micros)
{
    switch(sequence)
    {
        case 0:
        // pressure
        OFF =  ((uint32_t)C[2-1] << 16) + dT * C[4-1] / 128;
        SENS = ((uint32_t)C[1-1] << 15) + dT * C[3-1] / 256;
        if(temperature<2000){
            int32_t temp = 5 * (temperature-2000) * (temperature-2000) >> 1;
            OFF -= temp;
            SENS -= temp >> 1;
            if(temperature<-1500){
                temp = (temperature +1500)*(temperature +1500);
                OFF -= 7*temp;
                SENS -= (11*temp)>>1;
            }
        }
        pressure = (SENS * D[1-1] / ((uint32_t)1<<21) - OFF) / ((uint32_t)1<<15);

        // D2(uncompensated temperature) conversion start
        write_REG(MS561101BA_CONVERT_D2_OSR_4096);
        old_time = micros;
        ++sequence;
        break;

        case 1: // read ADC D2(uncompensated temperature)
        if(micros - old_time > 9500)
        {
            D[2-1] = read_ADC();
            ++sequence;
        }
        break;

        case 2:
        // temperature
        dT = D[2-1] - ((uint32_t)C[5-1]<<8);
        temperature = 2000 + dT * C[6-1] / ((uint32_t)1<<23);
        if(temperature<2000) temperature -= dT*dT / ((uint32_t)1<<31);

        // D1(uncompensated pressure) conversion start
        write_REG(MS561101BA_CONVERT_D1_OSR_4096);
        old_time = micros;
        ++sequence;
        break;

        case 3: // read ADC D1(uncompensated pressure)
        if(micros - old_time > 9500)
        {
            D[1-1] = read_ADC();
            sequence = 0;
        }
        break;
    }
}

int32_t ms5611_::get_pressure(void)
{
    return pressure;
}

int32_t ms5611_::get_temperature(void)
{
    return temperature;
}

//
// Private
//
void ms5611_::write_REG(const uint8_t &data)
{
    start_SLA_W(MS561101BA_I2C_ADDRESS_CSB_LOW);
    trans_data(data);
    stop();
}

uint32_t ms5611_::read_ADC(void)
{
    union {uint32_t val; uint8_t raw[4];} data;
    start_SLA_W(MS561101BA_I2C_ADDRESS_CSB_LOW);
    trans_data(MS561101BA_ADC_READ);
    start_SLA_R(MS561101BA_I2C_ADDRESS_CSB_LOW);
    data.raw[2] = receive_data();
    data.raw[1] = receive_data();
    data.raw[0] = last_receive_data();
    data.raw[3] = 0;

    return data.val;
}

//
// ms5611_ [Object name] = ms5611_();
//
ms5611_ ms5611 = ms5611_();