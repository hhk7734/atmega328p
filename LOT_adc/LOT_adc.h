/**
 * @file LOT_adc.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief ADC 아날로그 디지털 컨버터
 */

#ifndef _LOT_ADC_H_
#define _LOT_ADC_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if defined( __AVR_ATmega328P__ )
#else
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

typedef enum
{
    LOT_ADC_POWER_OFF = 0x00,
    LOT_ADC_POWER_ON
} LOT_power_on_off_typedef;

typedef enum
{
    LOT_ADC_0 = 0x00,
    LOT_ADC_1,
    LOT_ADC_2,
    LOT_ADC_3,
    LOT_ADC_4,
    LOT_ADC_5,
    LOT_ADC_6,
    LOT_ADC_7
} LOT_adc_pin_typedef;

void     LOT_adc_power_on_off( const LOT_power_on_off_typedef on_off );
uint16_t LOT_adc_read( const LOT_adc_pin_typedef adc_pin );

#ifdef __cplusplus
}
#endif
#endif /* _LOT_ADC_H_ */
