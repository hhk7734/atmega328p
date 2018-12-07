/**
 * @file LOT_ds3231.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief DS3231 RTC
 */

#include "LOT_ds3231.h"

#include "LOT_i2c0.h"

/**
 * @brief 십진법 -> 이진화 십진법
 * @param uint8_t dec 십진법
 * @return uint8_t 이진화 십진법
 */
static uint8_t LOT_dec_to_bcd( uint8_t dec )
{
    return ( ( dec / 10 ) << 4 ) | ( dec % 10 );
}

/**
 * @brief 이진화 십진법 -> 십진법
 * @param uint8_t bcd 이진화 십진법
 * @return 십진법
 */
static uint8_t LOT_bcd_to_dec( uint8_t bcd )
{
    return ( ( ( bcd >> 4 ) & 0x0F ) * 10 ) + ( bcd & 0x0F );
}

/**
 * @brief 날짜, 요일, 시간 설정
 * @param uint8_t _xx_year xx 년
 * @param uint8_t _month 월
 * @param uint8_t _date 일
 * @param uint8_t _day 요일 1-7, 사용자 정의 - 일월화수.. or 월화수목..
 * @param uint8_t _24_hour 시 (24시간 )
 * @param uint8_t _minute 분
 * @param uint8_t _second 초
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_ds3231_set_time( const uint8_t _xx_year, const uint8_t _month, const uint8_t _date,
                                        const uint8_t _day, const uint8_t _24_hour, const uint8_t _minute,
                                        const uint8_t _second )
{
    uint8_t buf[7];
    buf[0] = LOT_dec_to_bcd( _second );
    buf[1] = LOT_dec_to_bcd( _minute );
    buf[2] = LOT_dec_to_bcd( _24_hour );
    buf[3] = LOT_dec_to_bcd( _day );
    buf[4] = LOT_dec_to_bcd( _date );
    buf[5] = LOT_dec_to_bcd( _month );
    buf[6] = LOT_dec_to_bcd( _xx_year );
    return LOT_i2c0_write_successive_register( LOT_DS3231_ADDRESS, LOT_DS3231_SECOND, buf, 7 );
}

/**
 * @brief 날짜, 요일, 시간 확인
 * @param uint8_t *_xx_year xx 년
 * @param uint8_t *_month 월
 * @param uint8_t *_date 일
 * @param uint8_t *_day 요일 1-7, 사용자 정의 - 일월화수.. or 월화수목..
 * @param uint8_t *_24_hour 시 (24시간 )
 * @param uint8_t *_minute 분
 * @param uint8_t *_second 초
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_ds3231_get_time( uint8_t *_xx_year, uint8_t *_month, uint8_t *_date, uint8_t *_day,
                                        uint8_t *_24_hour, uint8_t *_minute, uint8_t *_second )
{
    uint8_t buf[7];
    if ( LOT_i2c0_read_successive_register( LOT_DS3231_ADDRESS, LOT_DS3231_SECOND, buf, 7 ) != LOT_OK )
    {
        return LOT_ERROR;
    }
    *_second  = LOT_bcd_to_dec( buf[0] );
    *_minute  = LOT_bcd_to_dec( buf[1] );
    *_24_hour = LOT_bcd_to_dec( buf[2] );
    *_day     = LOT_bcd_to_dec( buf[3] );
    *_date    = LOT_bcd_to_dec( buf[4] );
    *_month   = LOT_bcd_to_dec( buf[5] );
    *_xx_year = LOT_bcd_to_dec( buf[6] );
    return LOT_OK;
}

/**
 * @brief 시간 관련 레지스터 설정
 * @param uint8_t _time_register 시간 관련 레지스터
 * @param uint8_t _time 시간
 * @return LOT_status_typedef
 */
LOT_status_typedef LOT_ds3231_write_time_register( const uint8_t _time_register, const uint8_t _time )
{
    if ( _time_register > LOT_DS3231_YEAR )
    {
        return LOT_ERROR;
    }
    return LOT_i2c0_write_register( LOT_DS3231_ADDRESS, _time_register, LOT_dec_to_bcd( _time ) );
}

/**
 * @brief 시간 관련 레지스터 읽기
 * @param uint8_t _time_register 시간 관련 레지스터
 * @return 시간
 */
uint8_t LOT_ds3231_read_time_register( const uint8_t _time_register )
{
    if ( _time_register > LOT_DS3231_YEAR )
    {
        return -1;
    }
    return LOT_bcd_to_dec( LOT_i2c0_read_register( LOT_DS3231_ADDRESS, _time_register ) );
}