/********************************************************************
 *  filename:   config.h
 *  created:    2017/08/26  1:58
 *	edited :    2017/08/31  22:09
 *	edited :    2017/09/05  0:36
 *    
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *  
 *  purpose:
 *  config    
 ********************************************************************/


#ifndef CONFIG_H_
#define CONFIG_H_
#define F_CPU 16000000UL
#include "UART.h"

/***********************************************************************************
 * ATmega328P
 *          TXO     RAW
 *          RXI     GND - BAT(-)          A4 - SDA
 *          RST     RST                   A5 - SCL
 *          GND     VCC - BAT(+) 5V       A6 - BAT(+)/3
 *          D2      A3  - Yaw             A7
 * MOTOR0 - D3      A2  - Pitch           BAT(+) - 2R(저항) - A6 - R - GND(-)
 *          D4      A1  - Roll
 * MOTOR2 - D5      A0  - Throttle
 * MOTOR1 - D6      D13
 *          D7      D12 - BUZZER          D3 forward D6    0   1
 *          D8      D11 - MOTOR3                X            X
 *      X - D9      D10 - X               D11        D5    3   2
 ***********************************************************************************/

/************************************************************************
 * TC - micros, motor
 * TWI - read MPU9150
 * MPU9150 - calibration, low pass filter
 * RPY - Roll, Pitch, Yaw
 * RCRE - RC receiver, throttle, roll, pitch, yaw
 * PID
 ************************************************************************/

#define ACC_1G		   256

// sampling
#define ACC_LPF_FACTOR   5

#define ACCV_COMPLE_FACTOR   9  // 16 - 9 == 7 
#define MAGV_COMPLE_FACTOR   8  // 16 - 8 == 8

#define LOOP_TIME      3000

#define BASE_THROTTLE  1050
#define MIN_THROTTLE   1000
#define MAX_THROTTLE   2000

#define STATE_CHANGE_COUNT 150

#define ROLL_P_GAIN 0.45f
#define ROLL_I_GAIN 0.005f
#define ROLL_D_GAIN 13.5f
#define PITCH_P_GAIN 0.45f
#define PITCH_I_GAIN 0.005f
#define PITCH_D_GAIN 13.5f
#define YAW_P_GAIN 0.4f
#define YAW_I_GAIN 0
#define YAW_D_GAIN 5.0f

/*********************************************************************
 * 매크로
 ********************************************************************/
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#ifdef abs
#undef abs
#endif
#define abs(x) ((x)>0?(x):-(x))

/*********************************************************************
 * EEPROM MAP
 ********************************************************************/
#define EEPROM_ACC_OFFSET_X			0x0010
#define EEPROM_ACC_OFFSET_Y			0x0012
#define EEPROM_ACC_OFFSET_Z			0x0014
#define EEPROM_GYRO_OFFSET_X		0x0016
#define EEPROM_GYRO_OFFSET_Y		0x0018
#define EEPROM_GYRO_OFFSET_Z		0x001A
#define EEPROM_MAG_OFFSET_X			0x001C
#define EEPROM_MAG_OFFSET_Y			0x001E
#define EEPROM_MAG_OFFSET_Z			0x0020
#define EEPROM_MAG_SENSITIVITY_X	0x0022
#define EEPROM_MAG_SENSITIVITY_Y	0x0024
#define EEPROM_MAG_SENSITIVITY_Z	0x0026

#endif /* CONFIG_H_ */