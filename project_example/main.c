#include <avr/io.h>
#include <util/delay.h>

int main( void )
{
    /// arduino Uno LED blink Example
    DDRB |= _BV(DDB5);

    for( ;; )
    {
        PORTB |= _BV(PB5);
        _delay_ms(200);
        PORTB &= ~_BV(PB5);
        _delay_ms(500);
    }
}
