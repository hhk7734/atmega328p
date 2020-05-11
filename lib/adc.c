#include "adc.h"

uint16_t adc_read(const uint8_t adc_pin) {
    // Voltage Reference : AVCC
    ADMUX = _BV(REFS0) | adc_pin;

    // Sets clock over 100 kHz for max resolution
#if F_CPU >= 12800000UL
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
#elif F_CPU >= 6400000UL
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1);
#else
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS0);
#endif

    loop_until_bit_is_set(ADCSRA, ADSC);
    uint16_t temp = ADCL;
    temp          = (ADCH << 8) | temp;
    return temp;
}