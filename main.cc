#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "font2.h"

#define SHUTDOWN_THRESHOLD 2048

volatile uint8_t disp[8];

class System {
	private:
		uint16_t want_shutdown;
	public:
		System() { want_shutdown = 0; };
		void loop(void);
		void shutdown(void);
};

class Display {
	public:
		Display() {};
		void turn_on(void);
		void turn_off(void);
};

System system;
Display display;

void Display::turn_off()
{
	TIMSK0 &= ~_BV(TOIE0);
	PORTB = 0;
	PORTD = 0;
}

void Display::turn_on()
{
	TIMSK0 |= _BV(TOIE0);
}

void System::loop()
{
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
		else {

			// turn off display to indicate we're about to shut down
			display.turn_off();

			// wait until both buttons are released
			while (!((PINC & _BV(PC3)) && (PINC & _BV(PC7)))) ;

			// and some more to debounce the buttons
			_delay_ms(10);

			// actual naptime

			// enable PCINT on PC3 (PCINT11) and PC7 (PCINT15) for wakeup
			PCMSK1 |= _BV(PCINT15) | _BV(PCINT11);
			PCICR |= _BV(PCIE1);

			// go to power-down mode
			SMCR = _BV(SM1) | _BV(SE);
			asm("sleep");

			// execution will resume here - disable PCINT again.
			// Don't disable PCICR, something else might need it.
			PCMSK1 &= ~(_BV(PCINT15) | _BV(PCINT11));

			// turn on display
			display.turn_on();

			want_shutdown = 0;
		}
	}
	else {
		want_shutdown = 0;
	}
}

int main (void)
{
	// disable ADC to save power
	PRR |= _BV(PRADC);

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

	disp[0] = font[8][1];
	disp[1] = font[8][2];
	disp[2] = font[8][3];
	disp[3] = font[8][4];
	disp[4] = font[8][5];
	disp[5] = font[8][6];
	disp[6] = font[8][7];
	disp[7] = font[8][8];

#if 0
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
#endif

	sei();

	while (1) {
		// nothing to do here, go to idle to save power
		SMCR = _BV(SE);
		asm("sleep");
		system.loop();
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

ISR(PCINT1_vect)
{
	// we use PCINT1 for wakeup, so we should catch it here (and do nothing)
}
