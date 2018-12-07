/********************************************************************
 *  filename:   RPY.c
 *  created:    2017/10/02  0:47
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  calculate roll pitch yaw
 ********************************************************************/

#include "RPY.h"
#include "config.h"
#include "TC.h"
#include "MPU9150.h"

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

uint32_t old_micros;
float dt;

int32_16_union acc_vector;
int32_16_union mag_vector;

int32_t acc_temp_xyz[3];
int16_t acc_lpf_xyz[3];
int16_t old_gyro[3];

/*
 * deg/s/LSB  rad/deg    2^16     s/us
 * (1/16.4) * (PI/180) * 65536 * (1/1000000)
 */
#define GYRO2RAD_SCALE     0.0000697451

// signed16 * signed16
// 22 cycles
// http://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2) \
: \
"r26" \
)

int32_t __attribute__ ((noinline)) _mul(int16_t a, int16_t b)
{
    int32_t r;
    MultiS16X16to32(r, a, b);
    return r;
}

float InvSqrt(float x)
{
    /*
     * 제곱근의 역수 근삿값을 구해줌
     * https://ko.wikipedia.org/wiki/고속_역_제곱근
     */
    union{
        int32_t i;
        float   f;
    } conv;
    conv.f = x;
    conv.i = 0x5f1ffff9 - (conv.i >> 1);
    return conv.f * (1.68191409f - 0.703952253f * x * conv.f * conv.f);
}

int16_t _atan2(int32_t y, int32_t x)
{
    /*
     * 근삿값을 구해줌
     * -1800(-180도) ~ 1800(180도)
     * C. Hastings approximation for atan2()
     * Optimization tricks: http://www.hackersdelight.org/
     */
    float z = y;
    int16_t a;
    uint8_t c;
    c = abs(y) < abs(x);
    if ( c ) {z = z / x;} else {z = x / z;}
    a = 2046.43 * (z / (3.5714 +  z * z));
    if ( c ){
        if (x<0) {
            if (y<0) a -= 1800;
            else a += 1800;
        }
    } else {
        a = 900 - a;
        if (y<0) a -= 1800;
    }
    return a;
}

void rpy_setup(void)
{
    int16_t mag[3];
    mpu9150_get_mag(mag);
	
	old_gyro[0] = 0;
	old_gyro[1] = 0;
	old_gyro[2] = 0;
	
	acc_temp_xyz[0] = 0;
	acc_temp_xyz[1] = 0;
	acc_temp_xyz[2] = ACC_1G<<ACC_LPF_FACTOR;
	acc_lpf_xyz[0] = 0;
	acc_lpf_xyz[0] = 0;
	acc_lpf_xyz[0] = ACC_1G;
	
    acc_vector.xyz.x = 0;
    acc_vector.xyz.y = 0;
    acc_vector.xyz.z = ACC_1G;
	
    mag_vector.xyz.x = mag[0];
    mag_vector.xyz.y = mag[1];
    mag_vector.xyz.z = mag[2];
	
}

void rpy_get_rpy(int16_t *rpy)
{
    uint32_t new_micros = tc_micros();
    dt = (new_micros - old_micros) * GYRO2RAD_SCALE;
    old_micros = new_micros;
    int16_t acc[3],gyro[3],mag[3];

    mpu9150_get_all(acc,gyro,mag);
    
	int16_t delta_rad[3];
    delta_rad[0] = ((gyro[0] + old_gyro[0])>>1) * dt;
    delta_rad[1] = ((gyro[1] + old_gyro[1])>>1) * dt;
    delta_rad[2] = ((gyro[2] + old_gyro[2])>>1) * dt;
	
    // low pass filter
    acc_temp_xyz[0] -= acc_lpf_xyz[0];
    acc_temp_xyz[1] -= acc_lpf_xyz[1];
    acc_temp_xyz[2] -= acc_lpf_xyz[2];
    acc_temp_xyz[0] += acc[0];
    acc_temp_xyz[1] += acc[1];
    acc_temp_xyz[2] += acc[2];
    acc_lpf_xyz[0] = acc_temp_xyz[0]>>ACC_LPF_FACTOR;
    acc_lpf_xyz[1] = acc_temp_xyz[1]>>ACC_LPF_FACTOR;
    acc_lpf_xyz[2] = acc_temp_xyz[2]>>ACC_LPF_FACTOR;

    // rotation matrix
    acc_vector.XYZ.X += _mul(acc_vector.xyz.y , delta_rad[2]) - _mul(acc_vector.xyz.z , delta_rad[1]);
    acc_vector.XYZ.Y += _mul(acc_vector.xyz.z , delta_rad[0]) - _mul(acc_vector.xyz.x , delta_rad[2]);
    acc_vector.XYZ.Z += _mul(acc_vector.xyz.x , delta_rad[1]) - _mul(acc_vector.xyz.y , delta_rad[0]);
    mag_vector.XYZ.X += _mul(mag_vector.xyz.y , delta_rad[2]) - _mul(mag_vector.xyz.z , delta_rad[1]);
    mag_vector.XYZ.Y += _mul(mag_vector.xyz.z , delta_rad[0]) - _mul(mag_vector.xyz.x , delta_rad[2]);
    mag_vector.XYZ.Z += _mul(mag_vector.xyz.x , delta_rad[1]) - _mul(mag_vector.xyz.y , delta_rad[0]);

    // complementary filter
    int32_t acc_sq_sum = _mul(acc_lpf_xyz[0],acc_lpf_xyz[0]) + _mul(acc_lpf_xyz[1],acc_lpf_xyz[1]) + _mul(acc_lpf_xyz[2],acc_lpf_xyz[2]);
    if((int32_t)(0.85*ACC_1G*ACC_1G) < acc_sq_sum && acc_sq_sum < (int32_t)(1.15*ACC_1G*ACC_1G))
    {
        float scale = 256 * InvSqrt(acc_sq_sum);
        acc_vector.XYZ.X += (int32_t)((scale * acc_lpf_xyz[0]) - acc_vector.xyz.x) << (16-ACCV_COMPLE_FACTOR);
        acc_vector.XYZ.Y += (int32_t)((scale * acc_lpf_xyz[1]) - acc_vector.xyz.y) << (16-ACCV_COMPLE_FACTOR);
        acc_vector.XYZ.Z += (int32_t)((scale * acc_lpf_xyz[2]) - acc_vector.xyz.z) << (16-ACCV_COMPLE_FACTOR);
    }
    mag_vector.XYZ.X += (int32_t)(mag[0]-mag_vector.xyz.x) << (16-MAGV_COMPLE_FACTOR);
    mag_vector.XYZ.Y += (int32_t)(mag[1]-mag_vector.xyz.y) << (16-MAGV_COMPLE_FACTOR);
    mag_vector.XYZ.Z += (int32_t)(mag[2]-mag_vector.xyz.z) << (16-MAGV_COMPLE_FACTOR);

    // get rpy
    int32_t sqY_sum_sqZ = _mul(acc_vector.xyz.y,acc_vector.xyz.y) + _mul(acc_vector.xyz.z,acc_vector.xyz.z);
    rpy[0] = _atan2(acc_vector.xyz.y, acc_vector.xyz.z);
    rpy[1] = _atan2(-acc_vector.xyz.x, InvSqrt(sqY_sum_sqZ)*sqY_sum_sqZ);

    int16_t cross_x = (_mul(acc_vector.xyz.y , mag_vector.xyz.z) - _mul(acc_vector.xyz.z , mag_vector.xyz.y))>>7;
    int16_t cross_y = (_mul(acc_vector.xyz.z , mag_vector.xyz.x) - _mul(acc_vector.xyz.x , mag_vector.xyz.z))>>7;
    int16_t cross_z = (_mul(acc_vector.xyz.x , mag_vector.xyz.y) - _mul(acc_vector.xyz.y , mag_vector.xyz.x))>>7;
    int32_t sqY_sqZ = _mul(cross_y , cross_y) + _mul(cross_z , cross_z);
    if(cross_y>0){
        rpy[2] = _atan2(cross_x , sqY_sqZ*InvSqrt(sqY_sqZ));
        }else{
        rpy[2] = _atan2(cross_x , -sqY_sqZ*InvSqrt(sqY_sqZ));
    }

    mpu9150_get_gyro(old_gyro);
}



	/*
	 * 각도변화를 이용한 가속도 벡터 추정
	 * 회전 변환
	 * | AX'|   | 1  0      0      |   | cos(y) 0 -sin(y) |   |  cos(z) sin(z) 0 |   | AX |
	 * | AY'| = | 0  cos(x) sin(x) | * | 0      1  0      | * | -sin(z) cos(z) 0 | * | AY |
	 * | AZ'|   | 0 -sin(x) cos(x) |   | sin(y) 0  cos(y) |   |  0      0      1 |   | AZ |
	 *
	 * 테일러 급수 전개(taylor series expansion)
	 * cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! ....
	 * sin(x) = x - x^3/3! + x^5/5! - x^7/7! ....
	 *
	 * 최대 각속도 1000 deg/s 라고 가정
	 * 1000 deg/s * 0.003 s = 3 deg
	 * 3 deg * pi/180 rad/deg = 0.05236 rad
	 * 0.05236 rad가 최대 각도
	 * cos(x) = 1  sin(x) = x 라고 가정해도 오차가 0.1% 이하
	 *
	 * 회전 변환 결과
	 * | AX'|   | 1    z     -y |   | AX |
	 * | AY'| = | xy-z xyz+1  x | * | AY |
	 * | AZ'|   | y+xz yz-x   1 |   | AZ |
	 *
	 * x, y, z 가 위의 계산 결과와 같이 작으므로
	 * xy-z = -z  xyz+1 = 1  y+xz = y  yz-x = -x 라고 가정
	 * AX' = AX    + AY*z  + AZ*-y
	 * AY' = AX*-z + AY    + AZ*x
	 * AZ' = AX*y  + AY*-x + AZ
	 *
	 * 벡터 값 16bits 각도<<8 16bit
	 * union
	 * int32
	 * int16               uint16
	 * XXXX XXXX XXXX XXXX YYYY YYYY YYYY YYYY
	 * 벡터 값              소수 부분
	 */
	
	
	/*
	 * z축 가속도만 있을 때 안정상태라고 가정
	 * | AX |   | 1  0      0      |   | cos(P) 0 -sin(P) |   | 0  |
	 * | AY | = | 0  cos(R) sin(R) | * | 0      1  0      | * | 0  |
	 * | AZ |   | 0 -sin(R) cos(R) |   | sin(P) 0  cos(p) |   | 1g |
	 * AX = -sin(P)
	 * AY = sin(R)*cos(P)
	 * AZ = cos(R)*cos(P)
	 * sqrt(AY^2 + AZ^2) = cos(P)
	 *
	 * AY/AZ = tan(R)                  --> R = atan(AY/AZ)
	 * AX/sqrt(AY^2 + AZ^2) = -tan(P)  --> P = atan(-AX/sqrt(AY^2 + AZ^2))
	 *
	 * Roll -1800(-180) ~ 1800(180)
	 * Pitch -900 ~ 900
	 */
		
		
	/*
	 * 북반구 기준
	 * AV X MV (외적(cross product)) = 서쪽(절대 좌표 +Y값)
	 * | X'|   | 1  0      0      |   |  cos(Y) sin(Y) 0 |   | 0 |
	 * | Y'| = | 0  cos(R) sin(R) | * | -sin(Y) cos(Y) 0 | * | Y |
	 * | Z'|   | 0 -sin(R) cos(R) |   |  0      0      1 |   | 0 |
	 *       X
	 *       |
	 *       |
	 * Y-----Z
	 *
	 * X' = sin(Y)
	 * sqrt(Y'^2 + Z'^2) = cos(Yaw)
	 *
	 * X'/sqrt(Y'^2 + Z'^2) = tan(Yaw) --> Yaw = atan(X'/sqrt(Y'^2 + Z'^2))
	 * atan2에서 -1800~1800 범위가 나오려면 sqrt는 항상 양수기 때문에 Y값에 따라 나눠줘야함
	 */