#include "config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "I2C.h"
#include "EEPROM.h"
#include "MPU9150.h"
#include "TC.h"
#include "RPY.h"

#define LED_ON  PORTB |=   1<<PORTB5
#define LED_OFF PORTB &= ~(1<<PORTB5)

uint32_t startTime;

void setup(void);
void loop(void);

int main(void)
{
	setup();
    while (1) 
    {
		startTime = tc_micros();
		loop();
		//uart0_write_i16(tc_micros() - startTime);
		//uart0_write('\n');
		while (tc_micros() - startTime < LOOP_TIME);
    }
}

void setup(void)
{
	DDRB |= 1<<DDB5;
	
	uart0_begin(250000);
	
	i2c_begin(400000);
	
	mpu9150_setup();
	
	for (uint8_t i = 0; i<50 ; i++)
	{
		int16_t a[3];
		mpu9150_get_all(a,a,a);
		_delay_ms(2);
	}
	
	//mpu9150_acc_calibration();
	//eeprom_write_i16(EEPROM_ACC_OFFSET_X,acc_xyz_offset[0]);
	//eeprom_write_i16(EEPROM_ACC_OFFSET_Y,acc_xyz_offset[1]);
	//eeprom_write_i16(EEPROM_ACC_OFFSET_Z,acc_xyz_offset[2]);
	//mpu9150_gyro_calibration();
	//eeprom_write_i16(EEPROM_GYRO_OFFSET_X,gyro_xyz_offset[0]);
	//eeprom_write_i16(EEPROM_GYRO_OFFSET_Y,gyro_xyz_offset[1]);
	//eeprom_write_i16(EEPROM_GYRO_OFFSET_Z,gyro_xyz_offset[2]);
	//mpu9150_mag_calibration();
	//eeprom_write_i16(EEPROM_MAG_OFFSET_X,mag_xyz_offset[0]);
	//eeprom_write_i16(EEPROM_MAG_OFFSET_Y,mag_xyz_offset[1]);
	//eeprom_write_i16(EEPROM_MAG_OFFSET_Z,mag_xyz_offset[2]);
	//eeprom_write_i16(EEPROM_MAG_SENSITIVITY_X,mag_xyz_sensitivity[0]);
	//eeprom_write_i16(EEPROM_MAG_SENSITIVITY_Y,mag_xyz_sensitivity[1]);
	//eeprom_write_i16(EEPROM_MAG_SENSITIVITY_Z,mag_xyz_sensitivity[2]);
		
	acc_xyz_offset[0] = eeprom_read_i16(EEPROM_ACC_OFFSET_X);
	acc_xyz_offset[1] = eeprom_read_i16(EEPROM_ACC_OFFSET_Y);
	acc_xyz_offset[2] = eeprom_read_i16(EEPROM_ACC_OFFSET_Z);
	gyro_xyz_offset[0] = eeprom_read_i16(EEPROM_GYRO_OFFSET_X);
	gyro_xyz_offset[1] = eeprom_read_i16(EEPROM_GYRO_OFFSET_Y);
	gyro_xyz_offset[2] = eeprom_read_i16(EEPROM_GYRO_OFFSET_Z);
	mag_xyz_offset[0] = eeprom_read_i16(EEPROM_MAG_OFFSET_X);
	mag_xyz_offset[1] = eeprom_read_i16(EEPROM_MAG_OFFSET_Y);
	mag_xyz_offset[2] = eeprom_read_i16(EEPROM_MAG_OFFSET_Z);
	mag_xyz_sensitivity[0] = eeprom_read_i16(EEPROM_MAG_SENSITIVITY_X);
	mag_xyz_sensitivity[1] = eeprom_read_i16(EEPROM_MAG_SENSITIVITY_Y);
	mag_xyz_sensitivity[2] = eeprom_read_i16(EEPROM_MAG_SENSITIVITY_Z);
	
	tc_setup();
	
	rpy_setup();	
}

void loop(void)
{
	int16_t rpy[3];
	rpy_get_rpy(rpy);
}