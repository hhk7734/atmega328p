/**
 * @file LOT_adc.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief ADC 아날로그 디지털 컨버터
 */

#include "LOT_adc.h"

#include <avr/io.h>

/**
 * @brief ADC 기능을 on/off 하여 전력소모를 조절, 사용하지 않으면 기본적으로 on
 * @param LOT_power_on_off_typedef on_off `LOT_ADC_POWER_OFF` 또는 `LOT_ADC_POWER_ON`
 */
void LOT_adc_power_on_off( const LOT_power_on_off_typedef on_off )
{
    if ( on_off == LOT_ADC_POWER_OFF )
    {
        PRR |= _BV( PRADC );
    }
    else
    {
        PRR &= ~_BV( PRADC );
    }
}

/**
 * @brief ADC 값 읽기
 * @param LOT_adc_pin_typedef adc_pin 사용할 ADC 핀 선택, `LOT_ADC_x` 또는 `x` 입력(x 는 ADC 번호)
 * @return 변환된 디지털 값, 0~1023 을 반환
 */
uint16_t LOT_adc_read( const LOT_adc_pin_typedef adc_pin )
{
    /// Voltage Reference : AVCC
    ADMUX = _BV( REFS0 ) | adc_pin;
    /// ADC에 사용되는 clock이 100 kHz 이상으로 설정되어야 최대 분해능을 사용할 수 있음
#if F_CPU >= 12800000UL
    ADCSRA = _BV( ADEN ) | _BV( ADSC ) | _BV( ADPS2 ) | _BV( ADPS1 ) | _BV( ADPS0 );
#elif F_CPU >= 6400000UL
    ADCSRA = _BV( ADEN ) | _BV( ADSC ) | _BV( ADPS2 ) | _BV( ADPS1 );
#else
    ADCSRA = _BV( ADEN ) | _BV( ADSC ) | _BV( ADPS2 ) | _BV( ADPS0 );
#endif
    while ( ADCSRA & _BV( ADSC ) )
        ; // wait
    uint16_t temp = ADCL;
    temp = (ADCH << 8) | temp;
    return temp;
}

/// @todo auto triggering
/// @todo AREF