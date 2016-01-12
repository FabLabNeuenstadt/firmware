#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#define SHUTDOWN_THRESHOLD 2048

volatile uint8_t disp[32];

int main (void)
{
	uint16_t want_shutdown = 0;

	// disable ADC to save power
	power_adc_disable();

	// dito
	wdt_disable();

	// Ports B and D drive the dot matrix display -> set all as output
	DDRB = 0xff;
	DDRD = 0xff;
	PORTB = 0;
	PORTD = 0;

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	// Enable 8bit counter with prescaler=8 (-> timer frequency = 1MHz)
	TCCR0A = _BV(CS01);
	// raise timer interrupt on counter overflow (-> interrupt frequency = ~4kHz)
	TIMSK0 = _BV(TOIE0);

	// smile!
	disp[0] = 0x08;
	disp[1] = 0x04;
	disp[2] = 0x62;
	disp[3] = 0x02;
	disp[4] = 0x02;
	disp[5] = 0x62;
	disp[6] = 0x04;
	disp[7] = 0x08;
	disp[8] = 0x00;
	disp[9] = 0x00;
	disp[10] = 0x00;
	disp[11] = 0x00;
	disp[12] = 0x00;
	disp[13] = 0x00;
	disp[14] = 0x00;
	disp[15] = 0x00;
	disp[16] = 0x28;
	disp[17] = 0x44;
	disp[18] = 0x22;
	disp[19] = 0x02;
	disp[20] = 0x02;
	disp[21] = 0x22;
	disp[22] = 0x44;
	disp[23] = 0x28;
	disp[24] = 0x00;
	disp[25] = 0x00;
	disp[26] = 0x00;
	disp[27] = 0x00;
	disp[28] = 0x00;
	disp[29] = 0x00;
	disp[30] = 0x00;
	disp[31] = 0x00;

	sei();

	while (1) {
		// nothing to do here, go to idle to save power
		SMCR = _BV(SE);
		asm("sleep");

		// both buttons are pressed
		if ((PINC & (_BV(PC3) | _BV(PC7))) == 0) {
			// naptime!
			// But not before both buttons have been pressed for
			// SHUTDOWN_THRESHOLD * 0.256 ms. And then, not before both have
			// been released, because otherwise we'd go te sleep when
			// they're pressed and wake up when they're released, which
			// isn't really the point here.

			if (want_shutdown < SHUTDOWN_THRESHOLD) {
				want_shutdown++;
			}
			// TODO turn off display when want_shutdown >= SHUTDOWN_THRESHOLD
			// to indicate we're about to shut down
		} // both buttons released
		else if ((PINC & _BV(PC3)) && (PINC & _BV(PC7))) {
			if (want_shutdown >= SHUTDOWN_THRESHOLD) {
				// actual naptime

				// turn off display
				PORTB = 0;
				PORTD = 0;

				// enable PCINT on PC3 (PCINT11) and PC7 (PCINT15) for wakeup
				PCMSK1 |= _BV(PCINT15) | _BV(PCINT11);
				PCICR |= _BV(PCIE1);

				// go to power-down mode
				SMCR = _BV(SM1) | _BV(SE);
				asm("sleep");

				// execution will resume here - disable PCINT again.
				// Don't disable PCICR, something else might need it.
				PCMSK1 &= ~(_BV(PCINT15) | _BV(PCINT11));
				PCICR &= ~_BV(PCIE1);
			}
			want_shutdown = 0;
		}
	}

	return 0;
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

	uint8_t buffer_col;

	if (++scroll == 256) {
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

ISR(PCINT1_vect)
{
	// we use PCINT1 for wakeup, so we should catch it here (and do nothing)
}
