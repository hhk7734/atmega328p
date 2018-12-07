/***********************************************************************************
 * filename:  mpu9250.cpp
 *
 * Created:   2018/02/06
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "mpu9250.h"

#include <avr/io.h>
#include <util/delay.h>

//#include ""

//
// Static Variables initialization
// type mpu9250_::variables = init;
//


//
// Constructor & Destructor
//
mpu9250_::mpu9250_()
{
    // variables initialization
}
mpu9250_::~mpu9250_() {}


//
// Public
//
void mpu9250_::begin(void)
{
    i2c0_::begin();

    write_REG(MPU9250_PWR_MGMT_1    ,0b10000000);
    _delay_ms(100);

    write_REG(MPU9250_PWR_MGMT_1    ,0b00000001); // Auto selects the clock source
    write_REG(MPU9250_CONFIG        ,0b00000000); // gyro DLPF 0
    write_REG(MPU9250_GYRO_CONFIG   ,0b00011001); // FS_SEL +-2000 dps, Filter choice gyro delay 0.11ms
    write_REG(MPU9250_ACCEL_CONFIG  ,0b00010000); // FS_SEL +-8 g
    write_REG(MPU9250_ACCEL_CONFIG2 ,0b00001000); // Fchoice 1, DLPF 0, delay 1.88ms

    write_REG(MPU9250_I2C_MST_CTRL  ,0b00001101);            // i2c master clock 400kHz
    write_REG(MPU9250_I2C_SLV0_ADDR ,AK8963_ADDRESS|0x80); // read AK8963
    write_REG(MPU9250_I2C_SLV0_REG  ,AK8963_YOUT_L);         // mag y-axis == acc, gyro x-axis
    // i2c_slv0 enable, swap byte, group of 2 bytes, read 2 bytes
    write_REG(MPU9250_I2C_SLV0_CTRL ,0b11010010);
    write_REG(MPU9250_I2C_SLV1_ADDR ,AK8963_ADDRESS|0x80);
    write_REG(MPU9250_I2C_SLV1_REG  ,AK8963_XOUT_L);         // mag x-axis == acc, gyro y-axis
    write_REG(MPU9250_I2C_SLV1_CTRL ,0b11010010);
    write_REG(MPU9250_I2C_SLV2_ADDR ,AK8963_ADDRESS|0x80);
    write_REG(MPU9250_I2C_SLV2_REG  ,AK8963_ZOUT_L);         // mag z-axis == acc, gyro (-)z-axis
    // read 3 bytes ==> When ST2 register is read, AK8963 judges that data reading is finished.
    write_REG(MPU9250_I2C_SLV2_CTRL ,0b11010011);
    write_REG(MPU9250_I2C_SLV4_ADDR ,AK8963_ADDRESS);  // write AK8963
    write_REG(MPU9250_I2C_SLV4_REG  ,AK8963_CNTL1);
    write_REG(MPU9250_I2C_SLV4_DO   ,0b00010110);      // mag output 16bit, 100Hz rate
    write_REG(MPU9250_I2C_SLV4_CTRL ,0b10000000);      // i2c_slv4 enable, this bit auto clears
    write_REG(MPU9250_USER_CTRL     ,0b00100000);      // i2c master enable
}


void mpu9250_::read_raw_acc(int16_t *xyz)
{
    read_xyz(MPU9250_ACCEL_XOUT_H,xyz);
}

void mpu9250_::read_raw_gyro(int16_t *xyz)
{
    read_xyz(MPU9250_GYRO_XOUT_H,xyz);
}

void mpu9250_::read_raw_mag(int16_t *xyz)
{
    read_xyz(MPU9250_EXT_SENS_DATA_00,xyz);
    xyz[2] = -xyz[2]; // mag z-axis == acc, gyro (-)z-axis
}

void mpu9250_::acc_calibration(void)
{
    int16_t raw_acc[3];
    int32_t raw_acc_sum[3] = {0};

    for (uint8_t i = 0; i<128 ; ++i)
    {
        read_raw_acc(raw_acc);
        for (uint8_t j = 0; j<3 ; ++j)
        {
            raw_acc_sum[j] += raw_acc[j];
        }
        _delay_ms(3);
    }

    for (uint8_t i = 0; i<3 ; ++i) acc_offset[i] = raw_acc_sum[i]>>7;

    acc_offset[2] -= 4096;
}

void mpu9250_::gyro_calibration(void)
{
    int16_t raw_gyro[3];
    int32_t raw_gyro_sum[3] = {0};

    for (uint8_t i = 0; i<128 ; ++i)
    {
        read_raw_gyro(raw_gyro);
        for (uint8_t j = 0; j<3 ; ++j)
        {
            raw_gyro_sum[j] += raw_gyro[j];
        }
        _delay_ms(3);
    }

    for (uint8_t i = 0; i<3 ; ++i) gyro_offset[i] = raw_gyro_sum[i]>>7;
}

void mpu9250_::mag_calibration(void)
{
    //http://hbrotherswiki.blogspot.kr/2017/09/avr-mpu9150-calibration.html

    float parameter[6] = {0};
    int64_t b[6] = {0};
    int64_t A[6][6] = {0};


    for (uint8_t k = 0; k<70 ; ++k)
    {
        int16_t raw_mag[3];
        float f_raw_mag[3];
        read_raw_mag(raw_mag);
        for (uint8_t i = 0; i< 3; ++i) f_raw_mag[i] = raw_mag[i];
        float component[6];

        component[0] = f_raw_mag[0]*f_raw_mag[0];
        component[1] = f_raw_mag[0];
        component[2] = f_raw_mag[1]*f_raw_mag[1];
        component[3] = f_raw_mag[1];
        component[4] = f_raw_mag[2]*f_raw_mag[2];
        component[5] = f_raw_mag[2];

        for (uint8_t i = 0; i<6 ; i++)
        {
            for (uint8_t j = 0; j<6 ; j++)
            {
                A[i][j] +=component[i]*component[j];
            }
            b[i] += component[i];
        }
        _delay_ms(150);
    }

    for (uint8_t i = 0; i<6 ; ++i) b[i] *=10000;


    for (uint8_t k = 0; k<5 ; k++)
    {
        // pivoting
        uint8_t max = k;
        for (uint8_t i = k+1; i<6 ; i++)
        {
            if(abs(A[i][k])>abs(A[max][k])) max = i;
        }
        if (max != k)
        {
            for (uint8_t i = k; i<6 ; i++)
            {
                int64_t aTemp = A[k][i];
                A[k][i] = A[max][i];
                A[max][i] = aTemp;
            }

            int64_t bTemp = b[k];
            b[k]=b[max];
            b[max]=bTemp;
        }

        // Gauss elimination
        for (uint8_t i = k+1 ; i<6 ; i++)
        {
            for (uint8_t j = k+1 ; j<6 ; j++)
            {
                A[i][j] = A[i][j] - A[k][j]*((float)A[i][k]/A[k][k]);
            }
            b[i] = b[i] - b[k]*((float)A[i][k]/A[k][k]);
            A[i][k] = 0;
        }
    }

    // backward substitution
    for (uint8_t i = 6; i; i--)
    {
        uint8_t k = i-1;
        parameter[k] = b[k];
        for (uint8_t j = k+1; j<6 ; j++)
        {
            parameter[k] -= A[k][j]*parameter[j];
        }
        parameter[k] /= A[k][k];
    }

    mag_sensitivity[0] = 16;
    mag_sensitivity[1] = 16*InvSqrt(parameter[0]/parameter[2]);
    mag_sensitivity[2] = 16*InvSqrt(parameter[0]/parameter[4]);
    mag_offset[0] = -parameter[1]/(2*parameter[0]);
    mag_offset[1] = -parameter[3]/(2*parameter[2]);
    mag_offset[2] = -parameter[5]/(2*parameter[4]);
}

void mpu9250_::update_all(void)
{
    start_SLA_W(MPU9250_ADDRESS);
    trans_data(MPU9250_ACCEL_XOUT_H);
    start_SLA_R(MPU9250_ADDRESS);
    acc[0]  = (receive_data() << 8) | receive_data();
    acc[1]  = (receive_data() << 8) | receive_data();
    acc[2]  = (receive_data() << 8) | receive_data();
    receive_data();receive_data();                     // temperature data
    gyro[0] = (receive_data() << 8) | receive_data();
    gyro[1] = (receive_data() << 8) | receive_data();
    gyro[2] = (receive_data() << 8) | receive_data();
    mag[0]  = (receive_data() << 8) | receive_data();
    mag[1]  = (receive_data() << 8) | receive_data();
    mag[2]  = (receive_data() << 8) | last_receive_data();
    mag[2]  = -mag[2];

    for (uint8_t i = 0; i<3 ; ++i)
    {
        acc[i]  -= acc_offset[i];
        gyro[i] -= gyro_offset[i];
        mag[i]  -= mag_offset[i];
        mag[i]  *= mag_sensitivity[i];
    }
}

void mpu9250_::update_acc_gyro(void)
{
    start_SLA_W(MPU9250_ADDRESS);
    trans_data(MPU9250_ACCEL_XOUT_H);
    start_SLA_R(MPU9250_ADDRESS);
    acc[0]  = (receive_data() << 8) | receive_data();
    acc[1]  = (receive_data() << 8) | receive_data();
    acc[2]  = (receive_data() << 8) | receive_data();
    receive_data();receive_data();                     // temperature data
    gyro[0] = (receive_data() << 8) | receive_data();
    gyro[1] = (receive_data() << 8) | receive_data();
    gyro[2] = (receive_data() << 8) | last_receive_data();

    for (uint8_t i = 0; i<3 ; ++i)
    {
        acc[i]  -= acc_offset[i];
        gyro[i] -= gyro_offset[i];
    }
}

void mpu9250_::get_acc(int16_t *xyz)
{
    for (uint8_t i = 0; i<3 ; ++i) xyz[i] = acc[i];
}

void mpu9250_::get_gyro(int16_t *xyz)
{
    for (uint8_t i = 0; i<3 ; ++i) xyz[i] = gyro[i];
}

void mpu9250_::get_mag(int16_t *xyz)
{
    for (uint8_t i = 0; i<3 ; ++i) xyz[i] = mag[i];
}

void mpu9250_::get_offset(int16_t *_acc, int16_t *_gyro, int16_t *_mag, int16_t *_mag_sens)
{
    for (uint8_t i = 0; i<3 ; ++i)
    {
        _acc[i]      = acc_offset[i];
        _gyro[i]     = gyro_offset[i];
        _mag[i]      = mag_offset[i];
        _mag_sens[i] = mag_sensitivity[i];
    }
}

void mpu9250_::set_offset(const int16_t *_acc, const int16_t *_gyro, const int16_t *_mag, const int16_t *_mag_sens)
{
    for (uint8_t i = 0; i<3 ; ++i)
    {
        acc_offset[i]      = _acc[i];
        gyro_offset[i]     = _gyro[i];
        mag_offset[i]      = _mag[i];
        mag_sensitivity[i] = _mag_sens[i];
    }
}

//
// Protected
//


//
// Private
//
void mpu9250_::write_REG(const uint8_t &regaddr , const uint8_t &data)
{
    i2c0_::write_REG(MPU9250_ADDRESS, regaddr, data);
}

void mpu9250_::read_REG(const uint8_t &regaddr, uint8_t *buffer, const uint8_t &count)
{
    i2c0_::read_REG(MPU9250_ADDRESS, regaddr, buffer, count);
}

void mpu9250_::read_xyz(const uint8_t &regaddr, int16_t *xyz)
{
    start_SLA_W(MPU9250_ADDRESS);
    trans_data(regaddr);
    start_SLA_R(MPU9250_ADDRESS);
    xyz[0] = (receive_data() << 8) | receive_data();
    xyz[1] = (receive_data() << 8) | receive_data();
    xyz[2] = (receive_data() << 8) | last_receive_data();
}

float mpu9250_::InvSqrt(float x)
{
/*
 * ÕôÛ½?ÕÉ ?? ???? ?òÚ×ú
 * https://ko.wikipedia.org/wiki/?Ê¥_?_ÕôÛ½?
 */
    union{
        int32_t i;
        float   f;
    } conv;
    conv.f = x;
    conv.i = 0x5f1ffff9 - (conv.i >> 1);
    return conv.f * (1.68191409f - 0.703952253f * x * conv.f * conv.f);
}

//
// mpu9250_ [Object name] = mpu9250_();
//
mpu9250_ mpu9250 = mpu9250_();