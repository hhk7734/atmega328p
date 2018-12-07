/********************************************************************
 *  filename:   MPU9150.h
 *  created:    2017/09/18  22:16
 *  edited :    2017/09/20  18:18
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  MPU9150
 ********************************************************************/

#ifndef MPU9150_H_
#define MPU9150_H_

#define ACC_1G 256

#include <avr/io.h>

void mpu9150_setup(void);
void mpu9150_get_acc(int16_t *acc_xyz);
void mpu9150_get_gyro(int16_t *gyro_xyz);
void mpu9150_get_mag(int16_t *mag_xyz);
void mpu9150_get_acc_gyro(int16_t *acc_xyz,int16_t *gyro_xyz);
void mpu9150_get_all(int16_t *acc_xyz,int16_t *gyro_xyz,int16_t *mag_xyz);
void mpu9150_acc_calibration(int16_t *acc_xyz_offset);
void mpu9150_gyro_calibration(int16_t *gyro_xyz_offset);
void mpu9150_mag_calibration(int16_t *mag_xyz_offset, int16_t *mag_xyz_sensitivity);

#endif /* MPU9150_H_ */
