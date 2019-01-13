#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= _BV(DDB5);
    for (;;)
    {
        PORTB ^= _BV(PB5);
        _delay_ms(100);
    }
}