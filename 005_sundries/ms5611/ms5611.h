/***********************************************************************************
 * filename:  ms5611.h
 *
 * Created:   2018/02/03
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _MS5611_H_
#define _MS5611_H_

#include <inttypes.h>

#include "i2c0.h"

#define MS561101BA_I2C_ADDRESS_CSB_LOW 0x77

#define MS561101BA_RESET               0x1E
#define MS561101BA_CONVERT_D1_OSR_256  0x40
#define MS561101BA_CONVERT_D1_OSR_512  0x42
#define MS561101BA_CONVERT_D1_OSR_1024 0x44
#define MS561101BA_CONVERT_D1_OSR_2048 0x46
#define MS561101BA_CONVERT_D1_OSR_4096 0x48
#define MS561101BA_CONVERT_D2_OSR_256  0x50
#define MS561101BA_CONVERT_D2_OSR_512  0x52
#define MS561101BA_CONVERT_D2_OSR_1024 0x54
#define MS561101BA_CONVERT_D2_OSR_2048 0x56
#define MS561101BA_CONVERT_D2_OSR_4096 0x58
#define MS561101BA_ADC_READ            0x00
#define MS561101BA_PROM_C1             0xA2
#define MS561101BA_PROM_C2             0xA4
#define MS561101BA_PROM_C3             0xA6
#define MS561101BA_PROM_C4             0xA8
#define MS561101BA_PROM_C5             0xAA
#define MS561101BA_PROM_C6             0xAC

class ms5611_ : public i2c0_
{
public :
    ms5611_();
    ~ms5611_();

    void begin(void);
    void update(const uint32_t &micros);
    int32_t get_pressure(void);
    int32_t get_temperature(void);
protected :

private :
    uint16_t C[6];
    uint8_t  sequence;
    uint32_t D[2];
    uint32_t old_time;
    float    dT;
    int32_t  temperature;
    float    OFF;
    float    SENS;
    int32_t  pressure;

    void     write_REG(const uint8_t &data);
    uint32_t read_ADC(void);
}; // ms5611_

extern ms5611_ ms5611;
#endif // _MS5611_H_