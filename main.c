#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

volatile uint8_t disp[16];

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
	disp[8] = 0x28;
	disp[9] = 0x44;
	disp[10] = 0x22;
	disp[11] = 0x02;
	disp[12] = 0x02;
	disp[13] = 0x22;
	disp[14] = 0x44;
	disp[15] = 0x28;

	sei();

	while (1) {
		sleep_enable();
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t active_col = 0;
	static uint16_t scroll = 0;
	static uint8_t disp_offset = 0;

	uint8_t buffer_col;

	if (++scroll == 1024) {
		scroll = 0;
		if (++disp_offset == sizeof(disp)) {
			disp_offset = 0;
		}
	}

	buffer_col = (disp_offset + active_col) % sizeof(disp);

	/*
	 * To avoid flickering, do not put any code (or expensive index
	 * calculations) between the following three lines.
	 */
	PORTB = 0;
	PORTD = ~disp[buffer_col];
	PORTB = _BV(active_col);

	if (++active_col == 8)
		active_col = 0;
}
