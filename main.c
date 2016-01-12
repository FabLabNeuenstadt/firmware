#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

volatile uint8_t disp[8];

int main (void)
{
	wdt_disable();

	DDRB = 0xff;
	DDRD = 0xff;

	PORTB = 0;
	PORTD = 0;

	TCCR0A = _BV(CS00); // no prescaler (/8 without software prescaler is okay too)
	TIMSK0 = _BV(TOIE0); // interrupt on overflow

	// smile!
	disp[0] = 0x08;
	disp[1] = 0x04;
	disp[2] = 0x62;
	disp[3] = 0x02;
	disp[4] = 0x02;
	disp[5] = 0x62;
	disp[6] = 0x04;
	disp[7] = 0x08;

	sei();

	while (1) {
		sleep_enable();
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t active_col = 0;

	PORTB = 0;
	PORTD = ~disp[active_col];
	PORTB = _BV(active_col);

	if (++active_col == 8)
		active_col = 0;
}
