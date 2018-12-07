/***********************************************************************************
 * filename:  ahrs.h
 *
 * Created:   2018/02/07
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _AHRS_H_
#define _AHRS_H_

#include <inttypes.h>

#include "mpu9250.h"

//#define

typedef struct{
    int32_t X,Y,Z;
}int32_struct;
typedef struct{
    uint16_t xL; int16_t x;
    uint16_t yL; int16_t y;
    uint16_t zL; int16_t z;
}int16_struct;
typedef union{
    int32_struct XYZ;
    int16_struct xyz;
}int32_16_union;

class ahrs_ : public mpu9250_
{
public :
    ahrs_();
    ~ahrs_();

    void begin(void);
    void begin(const int16_t *_acc_offset, const int16_t *_g_off, const int16_t *_m_off, const int16_t *_mag_sens);
    void update(const uint16_t &delta_t);
    void get_rpy(int16_t *_rpy);
protected :

private :
    int32_16_union acc_vector;
    int32_16_union mag_vector;
    int16_t delta_angle[3];
    int32_t acc_lpf[3];
    int16_t old_gyro[3];
    int16_t rpy[3];

    int16_t _atan2(int32_t y, int32_t x);
    float   InvSqrt(float x);
    int32_t __attribute__ ((noinline)) mul(int16_t a, int16_t b);
}; // ahrs_

extern ahrs_ ahrs;
#endif // _AHRS_H_