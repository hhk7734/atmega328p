/**
 * @file LOT_ds3231.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief DS3231 RTC
 */

#ifndef _LOT_DS3231_H_
#define _LOT_DS3231_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define LOT_DS3231_ADDRESS 0x68

#define LOT_DS3231_SECOND 0x00
#define LOT_DS3231_MINUTE 0x01
#define LOT_DS3231_HOUR 0x02
#define LOT_DS3231_DAY 0x03
#define LOT_DS3231_DATE 0x04
#define LOT_DS3231_MONTH 0x05
#define LOT_DS3231_YEAR 0x06
#define LOT_DS3231_ALARM_1_SECOND 0x07
#define LOT_DS3231_ALARM_1_MINUTE 0x08
#define LOT_DS3231_ALARM_1_HOUR 0x09
#define LOT_DS3231_ALARM_1_DAY_DATE 0x0A
#define LOT_DS3231_ALARM_2_MINUTE 0x0B
#define LOT_DS3231_ALARM_2_HOUR 0x0C
#define LOT_DS3231_ALARM_2_DAY_DATE 0x0D
#define LOT_DS3231_CONTROL_1 0x0E
#define LOT_DS3231_CONTROL_2 0x0F
#define LOT_DS3231_AGING_OFFSET 0x10
#define LOT_DS3231_MSB_TEMP 0x11
#define LOT_DS3231_LSB_TEMP 0x12

#ifndef _LOT_STATUS_TYPEDEF_
#    define _LOT_STATUS_TYPEDEF_
typedef enum
{
    LOT_OK = 0x00,
    LOT_ERROR,
    LOT_BUSY,
    LOT_TIMEOUT
} LOT_status_typedef;
#endif

LOT_status_typedef LOT_ds3231_set_time( const uint8_t _xx_year, const uint8_t _month, const uint8_t _date,
                                        const uint8_t _day, const uint8_t _24_hour, const uint8_t _minute,
                                        const uint8_t _second );
LOT_status_typedef LOT_ds3231_get_time( uint8_t *_xx_year, uint8_t *_month, uint8_t *_date, uint8_t *_day,
                                        uint8_t *_24_hour, uint8_t *_minute, uint8_t *_second );
LOT_status_typedef LOT_ds3231_write_time_register( const uint8_t _time_register, const uint8_t _time );
uint8_t            LOT_ds3231_read_time_register( const uint8_t _time_register );

/// @todo 알람, 시스템 컨트롤, 온도

#ifdef __cplusplus
}
#endif
#endif // _LOT_DS3231_H_
