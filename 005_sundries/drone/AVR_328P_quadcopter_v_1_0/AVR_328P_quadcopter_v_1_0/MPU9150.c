/********************************************************************
 *  filename:   MPU9150.c
 *  created:    2017/09/18  22:16
 *  edited :    2017/09/20  18:18
 *  edited :    2017/10/02  0:12
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

#include "MPU9150.h"
#include "I2C.h"
#include <avr/io.h>
#include <util/delay.h>

#ifdef abs
#undef abs
#endif
#define abs(x) ((x)>0?(x):-(x))

#define MPU9150_AD0_ADDRESS         0x68
#define MPU9150_SMPLRT_DIV          0x19
#define MPU9150_CONFIG              0x1A
#define MPU9150_GYRO_CONFIG         0x1B
#define MPU9150_ACCEL_CONFIG        0x1C
#define MPU9150_I2C_MST_CTRL        0x24
#define MPU9150_I2C_SLV0_ADDR       0x25
#define MPU9150_I2C_SLV0_REG        0x26
#define MPU9150_I2C_SLV0_CTRL       0x27
#define MPU9150_I2C_SLV1_ADDR       0x28
#define MPU9150_I2C_SLV1_REG        0x29
#define MPU9150_I2C_SLV1_CTRL       0x2A
#define MPU9150_I2C_SLV2_ADDR       0x2B
#define MPU9150_I2C_SLV2_REG        0x2C
#define MPU9150_I2C_SLV2_CTRL       0x2D
#define MPU9150_I2C_SLV3_ADDR       0x2E
#define MPU9150_I2C_SLV3_REG        0x2F
#define MPU9150_I2C_SLV3_CTRL       0x30
#define MPU9150_I2C_SLV4_CTRL       0x34
#define MPU9150_INT_PIN_CFG         0x37
#define MPU9150_ACCEL_XOUT_H        0x3B
#define MPU9150_GYRO_XOUT_H         0x43
#define MPU9150_EXT_SENS_DATA_00    0x49
#define MPU9150_EXT_SENS_DATA_01    0x4A
#define MPU9150_I2C_SLV1_DO         0x64
#define MPU9150_I2C_SLV3_DO         0x66
#define MPU9150_I2C_MST_DELAY_CTRL  0x67
#define MPU9150_USER_CTRL           0x6A
#define MPU9150_PWR_MGMT_1          0x6B

#define AK8975_ADDRESS     0x0C
#define AK8975_ST1         0x02
#define AK8975_HXL         0x03
#define AK8975_HXH         0x04
#define AK8975_HYL         0x05
#define AK8975_HYH         0x06
#define AK8975_HZL         0x07
#define AK8975_HZH         0x08
#define AK8975_ST2         0x09
#define AK8975_CNTL        0x0A
#define AK8975_ASAX        0x10

int16_t acc_xyz_offset[3];
int16_t gyro_xyz_offset[3];
int16_t mag_xyz_offset[3];
int16_t mag_xyz_sensitivity[3];

void mpu9150_setup(void)
{
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_PWR_MGMT_1,0x80);
    _delay_ms(100);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_PWR_MGMT_1,0x03);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_SMPLRT_DIV,0x03);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_CONFIG,0x00);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_GYRO_CONFIG,0x18);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_ACCEL_CONFIG,0x10);

    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_MST_CTRL,0x0D);

    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV0_ADDR,0x80 + AK8975_ADDRESS);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV0_REG,AK8975_HYL);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV0_CTRL,0xD2);

    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV1_ADDR,0x80 + AK8975_ADDRESS);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV1_REG,AK8975_HXL);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV1_CTRL,0xD2);

    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV2_ADDR,0x80 + AK8975_ADDRESS);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV2_REG,AK8975_HZL);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV2_CTRL,0xD2);

    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV3_ADDR,AK8975_ADDRESS);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV3_REG,AK8975_CNTL);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV3_CTRL,0x81);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV4_CTRL,0x13);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_SLV3_DO,0x01);

    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_I2C_MST_DELAY_CTRL,0x0F);
    i2c_write_REG(MPU9150_AD0_ADDRESS,MPU9150_USER_CTRL,0x20);
}


void mpu9150_get_raw_acc(int16_t *acc_xyz)
{
    i2c_start_SLA_W(MPU9150_AD0_ADDRESS);
    i2c_trans_data(MPU9150_ACCEL_XOUT_H);
    i2c_start_SLA_R(MPU9150_AD0_ADDRESS);
    acc_xyz[0] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    acc_xyz[1] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    acc_xyz[2] = (((int16_t)i2c_receive_data()<<8) | i2c_last_receive_data())>>4;
}


void mpu9150_get_raw_gyro(int16_t *gyro_xyz)
{
    i2c_start_SLA_W(MPU9150_AD0_ADDRESS);
    i2c_trans_data(MPU9150_GYRO_XOUT_H);
    i2c_start_SLA_R(MPU9150_AD0_ADDRESS);
    gyro_xyz[0] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    gyro_xyz[1] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    gyro_xyz[2] = ((int16_t)i2c_receive_data()<<8) | i2c_last_receive_data();
}


void mpu9150_get_raw_mag(int16_t *mag_xyz)
{
    i2c_start_SLA_W(MPU9150_AD0_ADDRESS);
    i2c_trans_data(MPU9150_EXT_SENS_DATA_00);
    i2c_start_SLA_R(MPU9150_AD0_ADDRESS);
    mag_xyz[0] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    mag_xyz[1] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    mag_xyz[2] = -(((int16_t)i2c_receive_data()<<8) | i2c_last_receive_data());
}


void mpu9150_get_raw_acc_gyro(int16_t *acc_xyz,int16_t *gyro_xyz)
{
    i2c_start_SLA_W(MPU9150_AD0_ADDRESS);
    i2c_trans_data(MPU9150_ACCEL_XOUT_H);
    i2c_start_SLA_R(MPU9150_AD0_ADDRESS);
    acc_xyz[0] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    acc_xyz[1] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    acc_xyz[2] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    i2c_receive_data();i2c_receive_data();
    gyro_xyz[0] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    gyro_xyz[1] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    gyro_xyz[2] = ((int16_t)i2c_receive_data()<<8) | i2c_last_receive_data();
}

void mpu9150_get_raw_all(int16_t *acc_xyz,int16_t *gyro_xyz,int16_t *mag_xyz)
{
    i2c_start_SLA_W(MPU9150_AD0_ADDRESS);
    i2c_trans_data(MPU9150_ACCEL_XOUT_H);
    i2c_start_SLA_R(MPU9150_AD0_ADDRESS);
    acc_xyz[0] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    acc_xyz[1] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    acc_xyz[2] = (((int16_t)i2c_receive_data()<<8) | i2c_receive_data())>>4;
    i2c_receive_data();i2c_receive_data();
    gyro_xyz[0] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    gyro_xyz[1] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    gyro_xyz[2] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    mag_xyz[0] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    mag_xyz[1] = ((int16_t)i2c_receive_data()<<8) | i2c_receive_data();
    mag_xyz[2] = -(((int16_t)i2c_receive_data()<<8) | i2c_last_receive_data());
}

void mpu9150_acc_calibration(void)
{
    int16_t acc_xyz[3];
    int32_t acc_xyz_sum[3] = {0};
    for(uint8_t i = 0; i<128; i++)
    {
        mpu9150_get_raw_acc(acc_xyz);
        for(uint8_t j = 0; j<3; j++)
        {
            acc_xyz_sum[j] += acc_xyz[j];
        }
        _delay_ms(2);
    }
    for(uint8_t j = 0; j<3; j++)
    {
        acc_xyz_offset[j] = acc_xyz_sum[j]>>7;
    }
    acc_xyz_offset[2] -= ACC_1G;
}

void mpu9150_gyro_calibration(void)
{
    int16_t gyro_xyz[3];
    int32_t gyro_xyz_sum[3] = {0};
    for(uint8_t i = 0; i<128; i++)
    {
        mpu9150_get_raw_gyro(gyro_xyz);
        for(uint8_t j = 0; j<3; j++)
        {
            gyro_xyz_sum[j] += gyro_xyz[j];
        }
        _delay_ms(3);
    }
    for(uint8_t j = 0; j<3; j++)
    {
        gyro_xyz_offset[j] = gyro_xyz_sum[j]>>7;
    }
}

void mpu9150_mag_calibration(void)
{
    int16_t mag_xyz[3]; // ¾à 8~9bit °ª
    int32_t mag_XxYyZz[6];
    int64_t ATb_vector[6] = {0};
    int64_t ATA_matrix[6][6] = {0};
    float parameter[6] = {0.};
    for(uint16_t i = 0; i<200; i++)
    {
        mpu9150_get_raw_mag(mag_xyz);
        mag_XxYyZz[0] = (int32_t)mag_xyz[0]*mag_xyz[0];
        mag_XxYyZz[1] = mag_xyz[0];
        mag_XxYyZz[2] = (int32_t)mag_xyz[1]*mag_xyz[1];
        mag_XxYyZz[3] = mag_xyz[1];
        mag_XxYyZz[4] = (int32_t)mag_xyz[2]*mag_xyz[2];
        mag_XxYyZz[5] = mag_xyz[2];

        for (uint8_t j = 0; j<6 ; j++)
        {
            for (uint8_t k = 0; k<6 ; k++)
            {
                ATA_matrix[j][k] += mag_XxYyZz[j]*mag_XxYyZz[k];
            }
            ATb_vector[j] +=mag_XxYyZz[j];
        }
        _delay_ms(50);
    }

    for (uint8_t i = 0; i<6; i++)
    {
        ATb_vector[i] *= 10000; //R^2
    }

    for (uint8_t k = 0; k<5 ; k++)
    {
        // pivoting
        uint8_t max = k;
        for (uint8_t i = k+1; i<6 ; i++)
        {
            if(abs(ATA_matrix[i][k])>abs(ATA_matrix[max][k])) max = i;
        }
        if (max != k)
        {
            for (uint8_t i = k; i<6 ; i++)
            {
                int64_t aTemp = ATA_matrix[k][i];
                ATA_matrix[k][i] = ATA_matrix[max][i];
                ATA_matrix[max][i] = aTemp;
            }

            int64_t bTemp = ATb_vector[k];
            ATb_vector[k]=ATb_vector[max];
            ATb_vector[max]=bTemp;
        }

        // Gauss elimination
        for (uint8_t i = k+1 ; i<6 ; i++)
        {
            for (uint8_t j = k+1 ; j<6 ; j++)
            {
                ATA_matrix[i][j] = ATA_matrix[i][j] - ATA_matrix[k][j]*((float)ATA_matrix[i][k]/ATA_matrix[k][k]);
            }
            ATb_vector[i] = ATb_vector[i] - ATb_vector[k]*((float)ATA_matrix[i][k]/ATA_matrix[k][k]);
            ATA_matrix[i][k] = 0;
        }
    }


    // backward substitution
    for (uint8_t i = 6; i; i--)
    {
        uint8_t k = i-1;
        parameter[k] = ATb_vector[k];
        for (uint8_t j = k+1; j<6 ; j++)
        {
            parameter[k] -= ATA_matrix[k][j]*parameter[j];
        }
        parameter[k] /= ATA_matrix[k][k];
    }

    mag_xyz_sensitivity[0] = 32;
    mag_xyz_sensitivity[1] = 32/sqrt(parameter[0]/parameter[2]);
    mag_xyz_sensitivity[2] = 32/sqrt(parameter[0]/parameter[4]);
    mag_xyz_offset[0] = -parameter[1]/(2*parameter[0]);
    mag_xyz_offset[1] = -parameter[3]/(2*parameter[2]);
    mag_xyz_offset[2] = -parameter[5]/(2*parameter[4]);
}

void mpu9150_get_acc(int16_t *acc_xyz)
{
    mpu9150_get_raw_acc(acc_xyz);
    acc_xyz[0] -= acc_xyz_offset[0];
    acc_xyz[1] -= acc_xyz_offset[1];
    acc_xyz[2] -= acc_xyz_offset[2];
}

void mpu9150_get_gyro(int16_t *gyro_xyz)
{
    mpu9150_get_raw_gyro(gyro_xyz);
    gyro_xyz[0] -= gyro_xyz_offset[0];
    gyro_xyz[1] -= gyro_xyz_offset[1];
    gyro_xyz[2] -= gyro_xyz_offset[2];
}

void mpu9150_get_mag(int16_t *mag_xyz)
{
    mpu9150_get_raw_mag(mag_xyz);
    mag_xyz[0] = (mag_xyz_sensitivity[0] * (mag_xyz[0] - mag_xyz_offset[0]))>>5;
    mag_xyz[1] = (mag_xyz_sensitivity[1] * (mag_xyz[1] - mag_xyz_offset[1]))>>5;
    mag_xyz[2] = (mag_xyz_sensitivity[2] * (mag_xyz[2] - mag_xyz_offset[2]))>>5;
}

void mpu9150_get_acc_gyro(int16_t *acc_xyz,int16_t *gyro_xyz)
{
    mpu9150_get_raw_acc_gyro(acc_xyz,gyro_xyz);
    acc_xyz[0] -= acc_xyz_offset[0];
    acc_xyz[1] -= acc_xyz_offset[1];
    acc_xyz[2] -= acc_xyz_offset[2];
    gyro_xyz[0] -= gyro_xyz_offset[0];
    gyro_xyz[1] -= gyro_xyz_offset[1];
    gyro_xyz[2] -= gyro_xyz_offset[2];
}

void mpu9150_get_all(int16_t *acc_xyz,int16_t *gyro_xyz,int16_t *mag_xyz)
{
    mpu9150_get_raw_all(acc_xyz,gyro_xyz,mag_xyz);
    acc_xyz[0] -= acc_xyz_offset[0];
    acc_xyz[1] -= acc_xyz_offset[1];
    acc_xyz[2] -= acc_xyz_offset[2];
    gyro_xyz[0] -= gyro_xyz_offset[0];
    gyro_xyz[1] -= gyro_xyz_offset[1];
    gyro_xyz[2] -= gyro_xyz_offset[2];
    mag_xyz[0] = (mag_xyz_sensitivity[0] * (mag_xyz[0] - mag_xyz_offset[0]))>>5;
    mag_xyz[1] = (mag_xyz_sensitivity[1] * (mag_xyz[1] - mag_xyz_offset[1]))>>5;
    mag_xyz[2] = (mag_xyz_sensitivity[2] * (mag_xyz[2] - mag_xyz_offset[2]))>>5;
}