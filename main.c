#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRB |= _BV(DD5);
    while(1) {
        PORTB |= _BV(PB5);
        _delay_ms(100);
        PORTB &= ~_BV(PB5);
        _delay_ms(100);
    }
}