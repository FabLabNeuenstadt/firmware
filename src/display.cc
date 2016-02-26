#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"

Display display;

Display::Display()
{
	char_pos = -1;
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
	static uint8_t scroll;

	/*
	 * To avoid flickering, do not put any code (or expensive index
	 * calculations) between the following three lines.
	 */
	PORTB = 0;
	PORTD = disp_buf[active_col];
	PORTB = _BV(active_col);

	if (++active_col == 8) {
		active_col = 0;
		if (++scroll == current_anim->speed) {
			scroll = 0;
			need_update = 1;
		}
	}
}

void Display::update() {
	uint8_t i, glyph_len;
	uint8_t *glyph_addr;
	if (need_update) {
		need_update = 0;

		if (status == RUNNING) {
			if (current_anim->type == AnimationType::TEXT) {
				for (i = 0; i < 7; i++) {
					disp_buf[i] = disp_buf[i+1];
				}

				glyph_addr = (uint8_t *)pgm_read_ptr(&font[current_anim->data[str_pos]]);
				glyph_len = pgm_read_byte(&glyph_addr[0]);
				char_pos++;

				if (char_pos > glyph_len) {
					char_pos = 0;
					str_pos++;
				}

				if (char_pos == 0) {
					disp_buf[7] = 0xff; // whitespace
				} else {
					disp_buf[7] = ~pgm_read_byte(&glyph_addr[char_pos]);
				}
			} else if (current_anim->type == AnimationType::FRAMES) {
				for (i = 0; i < 8; i++) {
					disp_buf[i] = ~current_anim->data[str_pos+i];
				}
				str_pos += 8;
			}
			if (str_pos >= current_anim->length) {
				str_pos = 0;
				if (current_anim->delay > 0) {
					status = PAUSED;
				}
			}
		} else if (status == PAUSED) {
			str_pos++;
			if (str_pos >= current_anim->delay) {
				str_pos = 0;
				status = RUNNING;
			}
		}
	}
}

void Display::reset()
{
	for (uint8_t i = 0; i < 8; i++)
		disp_buf[i] = 0xff;
	str_pos = 0;
	char_pos = -1;
	status = RUNNING;
}

void Display::show(animation_t *anim)
{
	current_anim = anim;
	reset();
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
