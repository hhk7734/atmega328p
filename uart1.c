#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRB |= _BV(DDB5);

	while(1)
	{
		PORTB |= _BV(PB5);
		delay_ms(100);
		PORTB &= ~_BV(PB5);
		delay_ms(100);
	}

	return 0;

}
