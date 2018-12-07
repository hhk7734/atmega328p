/***********************************************************************************
 * filename:  ahrs.cpp
 *
 * Created:   2018/02/07
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "ahrs.h"

#include <avr/io.h>
#include <util/delay.h>
//#include ""

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

/*
 * deg/s/LSB  rad/deg    2^16     s/us
 * (1/16.4) * (PI/180) * 65536 * (1/1000000)
 */
#define GYRO2RAD_SCALE         0.0000697451
#define ACCV_COMPLE_FACTOR     10
#define MAGV_COMPLE_LPF_FACTOR 10
// 자침 편차 보정 http://www.magnetic-declination.com
#define MAG_DECLINATION        -82

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

//
// Static Variables initialization
// type ahrs_::variables = init;
//


//
// Constructor & Destructor
//
ahrs_::ahrs_()
{
    // variables initialization
    acc_vector.XYZ.X = 0;
    acc_vector.XYZ.Y = 0;
    acc_vector.XYZ.Z = 0;
    mag_vector.XYZ.X = 0;
    mag_vector.XYZ.Y = 0;
    mag_vector.XYZ.Z = 0;
}
ahrs_::~ahrs_() {}


//
// Public
//
void ahrs_::begin(void)
{
    mpu9250_::begin();
    
    // acc_calibration();
    // gyro_calibration();
    // _delay_ms(500);
    // mag_calibration();

    update_all();
    int16_t xyz[3];
    get_mag(xyz);
    acc_vector.xyz.x = 0;
    acc_vector.xyz.y = 0;
    acc_vector.xyz.z = 4096;
    mag_vector.xyz.x = xyz[0];
    mag_vector.xyz.y = xyz[1];
    mag_vector.xyz.z = xyz[2];
}

void ahrs_::begin(const int16_t *_acc_offset, const int16_t *_g_off, const int16_t *_m_off, const int16_t *_mag_sens)
{
    mpu9250_::begin();
    set_offset(_acc_offset, _g_off, _m_off, _mag_sens);

    int32_t acc_sum[3] = {0};
    int32_t mag_sum[3] = {0};
    for(uint8_t i = 64; i ;i--){
        update_all();
        int16_t xyz[3];
        get_acc(xyz);
        for(uint8_t j = 0; j < 3 ;j++) acc_sum[j] += xyz[j];
        get_mag(xyz);
        for(uint8_t j = 0; j < 3 ;j++) mag_sum[j] += xyz[j];
        _delay_ms(4);
    }
    acc_vector.xyz.x = acc_sum[0] >> 6;
    acc_vector.xyz.y = acc_sum[1] >> 6;
    acc_vector.xyz.z = acc_sum[2] >> 6;
    mag_vector.xyz.x = mag_sum[0] >> 6;
    mag_vector.xyz.y = mag_sum[1] >> 6;
    mag_vector.xyz.z = mag_sum[2] >> 6;
}

void ahrs_::update(const uint16_t &delta_t)
{
    //
    // Roll -1800(-180) ~ 1800(180)
    // Pitch -900 ~ 900
    // Yaw -1800 ~ 1800
    //       X
    //       |
    //       |
    // Y-----Z
    // 
	// 각속도를 이용한 회전 각도 변화 계산
	//                     각도 값 xxxx xxxx xxxx xxxx << 16
	// 시프트  xxxx xxxx xxxx xxxx 0000 0000 0000 0000
	//        정수부               소수부
	// 정수형으로는 소수점 계산이 안되므로 시프트 연산을 통해 소수점 부분을 만들 수 있음
	// 각속도가 16bits이므로 deltaAng는 16bits보다 낮음 (곱해지는 값이 약 0.105)
	// 
    update_all();
    int16_t _acc[3],_gyro[3],_mag[3];
    get_acc(_acc);
    get_gyro(_gyro);
    get_mag(_mag);
    for(uint8_t i=0; i<3; i++){
        _gyro[i] = constrain(_gyro[i],old_gyro[i]-1700,old_gyro[i]+1700);
        old_gyro[i] = _gyro[i];
        acc_lpf[i] = (31 * acc_lpf[i] + _acc[i])>>5;
    }
    for(uint8_t i = 0; i < 3; i++) delta_angle[i] = mul(_gyro[i],delta_t) * GYRO2RAD_SCALE;
    // 
	// 각도변화를 이용한 가속도 벡터 추정
	// 회전 변환
	// | AX'|   | 1  0      0      |   | cos(y) 0 -sin(y) |   |  cos(z) sin(z) 0 |   | AX |
	// | AY'| = | 0  cos(x) sin(x) | * | 0      1  0      | * | -sin(z) cos(z) 0 | * | AY |
	// | AZ'|   | 0 -sin(x) cos(x) |   | sin(y) 0  cos(y) |   |  0      0      1 |   | AZ |
	// 
	// 테일러 급수 전개(taylor series expansion)
	// cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! ....
	// sin(x) = x - x^3/3! + x^5/5! - x^7/7! ....
	// 
	// 최대 각속도 1000 deg/s 라고 가정하면
	// 1000 deg/s * 0.003 s = 3 deg
	// 3 deg * pi/180 rad/deg = 0.05236 rad
	// 0.05236 rad가 최대 각도
	// cos(x) = 1  sin(x) = x 라고 가정해도 오차가 0.1% 이하
	// 
	// 회전 변환 결과
	// | AX'|   | 1    z     -y |   | AX |
	// | AY'| = | xy-z xyz+1  x | * | AY |
	// | AZ'|   | y+xz yz-x   1 |   | AZ |
	// 
	// x, y, z 가 위의 계산 결과와 같이 작으므로
	// xy-z = -z  xyz+1 = 1  y+xz = y  yz-x = -x 라고 가정
	// AX' = AX    + AY*z  + AZ*-y
	// AY' = AX*-z + AY    + AZ*x
	// AZ' = AX*y  + AY*-x + AZ
	// 
	// 벡터 값 16bits 각도<<8 16bit
	// union
	// int32
	// int16               uint16
	// XXXX XXXX XXXX XXXX YYYY YYYY YYYY YYYY
	// 벡터 값              소수 부분
	// 
    acc_vector.XYZ.X += mul(acc_vector.xyz.y , delta_angle[2]) - mul(acc_vector.xyz.z , delta_angle[1]);
    acc_vector.XYZ.Y += mul(acc_vector.xyz.z , delta_angle[0]) - mul(acc_vector.xyz.x , delta_angle[2]);
    acc_vector.XYZ.Z += mul(acc_vector.xyz.x , delta_angle[1]) - mul(acc_vector.xyz.y , delta_angle[0]);

    mag_vector.XYZ.X += mul(mag_vector.xyz.y , delta_angle[2]) - mul(mag_vector.xyz.z , delta_angle[1]);
    mag_vector.XYZ.Y += mul(mag_vector.xyz.z , delta_angle[0]) - mul(mag_vector.xyz.x , delta_angle[2]);
    mag_vector.XYZ.Z += mul(mag_vector.xyz.x , delta_angle[1]) - mul(mag_vector.xyz.y , delta_angle[0]);
    // 
	// complementary filter
	// 상보 필터
	// 1g일 때 가속도 크기 4096
	// 0.85g ~ 1.15g 일때만 동작
    //
    // todo
    // use Kalman Filter
    //

    int32_t rawAccSqSum = mul(acc_lpf[0],acc_lpf[0]) + mul(acc_lpf[1],acc_lpf[1]) + mul(acc_lpf[2],acc_lpf[2]);
    if(12121538 < rawAccSqSum && rawAccSqSum < 22187868){
        acc_vector.XYZ.X += (int32_t)(acc_lpf[0]-acc_vector.xyz.x) << (16-ACCV_COMPLE_FACTOR);
        acc_vector.XYZ.Y += (int32_t)(acc_lpf[1]-acc_vector.xyz.y) << (16-ACCV_COMPLE_FACTOR);
        acc_vector.XYZ.Z += (int32_t)(acc_lpf[2]-acc_vector.xyz.z) << (16-ACCV_COMPLE_FACTOR);
    }

    mag_vector.XYZ.X += (int32_t)(_mag[0]-mag_vector.xyz.x) << (16-MAGV_COMPLE_LPF_FACTOR);
    mag_vector.XYZ.Y += (int32_t)(_mag[1]-mag_vector.xyz.y) << (16-MAGV_COMPLE_LPF_FACTOR);
    mag_vector.XYZ.Z += (int32_t)(_mag[2]-mag_vector.xyz.z) << (16-MAGV_COMPLE_LPF_FACTOR);
    
    // 
	// z축 가속도만 있을 때 안정상태라고 가정
	// | AX |   | 1  0      0      |   | cos(P) 0 -sin(P) |   | 0  |
	// | AY | = | 0  cos(R) sin(R) | * | 0      1  0      | * | 0  |
	// | AZ |   | 0 -sin(R) cos(R) |   | sin(P) 0  cos(p) |   | 1g |
	// AX = -sin(P)
	// AY = sin(R)*cos(P)
	// AZ = cos(R)*cos(P)
	// sqrt(AY^2 + AZ^2) = cos(P)
	// 
	// AY/AZ = tan(R)                  --> R = atan(AY/AZ)
	// AX/sqrt(AY^2 + AZ^2) = -tan(P)  --> P = atan(-AX/sqrt(AY^2 + AZ^2))
	// 
	// Roll -1800(-180) ~ 1800(180)
	// Pitch -900 ~ 900
	// 
    int32_t sqY_sum_sqZ = mul(acc_vector.xyz.y,acc_vector.xyz.y) + mul(acc_vector.xyz.z,acc_vector.xyz.z);
    rpy[0] = _atan2(acc_vector.xyz.y, acc_vector.xyz.z);
    rpy[1] = _atan2(-acc_vector.xyz.x, InvSqrt(sqY_sum_sqZ)*sqY_sum_sqZ);

    // 
	// 북반구 기준
	// AV X MV (외적(cross product)) = 서쪽(절대 좌표 +Y값)
	// | X'|   | 1  0      0      |   |  cos(Y) sin(Y) 0 |   | 0 |
	// | Y'| = | 0  cos(R) sin(R) | * | -sin(Y) cos(Y) 0 | * | Y |
	// | Z'|   | 0 -sin(R) cos(R) |   |  0      0      1 |   | 0 |
	//       X
	//       |
	//       |
	// Y-----Z
	// 
	// X' = sin(Y)
	// sqrt(Y'^2 + Z'^2) = cos(Yaw)
	// 
	// X'/sqrt(Y'^2 + Z'^2) = tan(Yaw) --> Yaw = atan(X'/sqrt(Y'^2 + Z'^2))
	// atan2에서 -1800~1800 범위가 나오려면 sqrt는 항상 양수기 때문에 Y값에 따라 나눠줘야함
	// 
     int16_t YY = (mul(acc_vector.xyz.z , mag_vector.xyz.x) - mul(acc_vector.xyz.x , mag_vector.xyz.z))>>10;
     int16_t ZZ = (mul(acc_vector.xyz.x , mag_vector.xyz.y) - mul(acc_vector.xyz.y , mag_vector.xyz.x))>>10;
     int32_t sqYY_sqZZ = mul(YY , YY) + mul(ZZ , ZZ);
     if(YY>0){
         rpy[2] = _atan2(((mul(acc_vector.xyz.y , mag_vector.xyz.z) - mul(acc_vector.xyz.z , mag_vector.xyz.y))>>10)
         ,sqYY_sqZZ*InvSqrt(sqYY_sqZZ));
         }else{
         rpy[2] = _atan2(((mul(acc_vector.xyz.y , mag_vector.xyz.z) - mul(acc_vector.xyz.z , mag_vector.xyz.y))>>10)
         ,-sqYY_sqZZ*InvSqrt(sqYY_sqZZ));
     }
}

void ahrs_::get_rpy(int16_t *_rpy)
{
    for (uint8_t i = 0; i < 3 ; i++) _rpy[i] = rpy[i];
}

//
// Protected
//


//
// Private
//
int16_t ahrs_::_atan2(int32_t y, int32_t x)
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

float ahrs_::InvSqrt(float x)
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

int32_t __attribute__ ((noinline)) ahrs_::mul(int16_t a, int16_t b)
{
/*
 * 16bits * 16bits 를 32비트로 리턴
 * 실행이 안되면 함수사용하지 말고 int32_t로 캐스팅후 곱셈
 */
	int32_t r;
	MultiS16X16to32(r, a, b);
	return r;
}

//
// ahrs_ [Object name] = ahrs_();
//
ahrs_ ahrs = ahrs_();