#include <avr/io.h>
#include <util/delay.h>

#include "LOT_uart0.h"

int main(void)
{
    DDRB |= _BV(DDB5);
    LOT_uart0_begin(9600);
    LOT_uart0_print_str("start");
    for (;;)
    {
        PORTB ^= _BV(PB5);
        _delay_ms(100);
    }
}