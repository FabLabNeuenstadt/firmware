#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"

Display display;

Display::Display()
{
	disp_buf[0] = 0xff;
	disp_buf[1] = 0xfb;
	disp_buf[2] = 0xdd;
	disp_buf[3] = 0xfd;
	disp_buf[4] = 0xdd;
	disp_buf[5] = 0xfb;
	disp_buf[6] = 0xff;
	disp_buf[7] = 0xff;
	char_pos = -1;
	scroll_delay = 400;
}

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

void Display::multiplex()
{
	static uint16_t scroll;
	uint8_t i, glyph_len;
	uint8_t *glyph_addr;

	/*
	 * To avoid flickering, do not put any code (or expensive index
	 * calculations) between the following three lines.
	 */
	PORTB = 0;
	PORTD = disp_buf[active_col];
	PORTB = _BV(active_col);

	if (++active_col == 8)
		active_col = 0;

	if (++scroll == scroll_delay) {
		scroll = 0;

		for (i = 0; i < 7; i++) {
			disp_buf[i] = disp_buf[i+1];
		}

		glyph_addr = (uint8_t *)pgm_read_ptr(&font[(uint8_t)display.string[str_pos]]);
		glyph_len = pgm_read_byte(&glyph_addr[0]);
		char_pos++;

		if (char_pos > glyph_len) {
			char_pos = 0;
			str_pos++;
		}

		if (display.string[str_pos] == 0) {
			str_pos = 0;
		}

		if (char_pos == 0) {
			disp_buf[7] = 0xff; // whitespace
		} else {
			disp_buf[7] = ~pgm_read_byte(&glyph_addr[char_pos]);
		}
	}
}

void Display::reset()
{
	for (int i = 0; i < 8; i++)
		disp_buf[i] = 0xff;
	str_pos = 0;
	char_pos = -1;
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
	display.multiplex();
}
