#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"

Display display;

extern volatile uint8_t disp[8];

void Display::disable()
{
	TIMSK0 &= ~_BV(TOIE0);
	PORTB = 0;
	PORTD = 0;
}

void Display::enable()
{
	// Ports B and D drive the dot matrix display -> set all as output
	DDRB = 0xff;
	DDRD = 0xff;

	// Enable 8bit counter with prescaler=8 (-> timer frequency = 1MHz)
	TCCR0A = _BV(CS01);
	// raise timer interrupt on counter overflow (-> interrupt frequency = ~4kHz)
	TIMSK0 = _BV(TOIE0);
}

/*
 * Draws a single display column. This function should be called at least once
 * per millisecond.
 *
 * Current configuration:
 * Called every 256 microseconds. The whole display is refreshed every 2048us,
 * giving a refresh rate of ~500Hz
 */
ISR(TIMER0_OVF_vect)
{
	static uint8_t active_col = 0;
	static uint16_t scroll = 0;
	static uint8_t disp_offset = 0;

	static uint8_t disp_buf[8];

	uint8_t i;

	if (++scroll == 512) {
		scroll = 0;
		if (++disp_offset == sizeof(disp)) {
			disp_offset = 0;
		}

		for (i = 0; i < 8; i++) {
			disp_buf[i] = ~disp[(disp_offset + i) % sizeof(disp)];
		}
	}

	/*
	 * To avoid flickering, do not put any code (or expensive index
	 * calculations) between the following three lines.
	 */
	PORTB = 0;
	PORTD = disp_buf[active_col];
	PORTB = _BV(active_col);

	if (++active_col == 8)
		active_col = 0;
}
