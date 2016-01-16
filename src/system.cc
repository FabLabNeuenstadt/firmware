#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "modem.h"
#include "system.h"

#define SHUTDOWN_THRESHOLD 2048

System rocket;

extern volatile uint8_t disp[8];

void System::loop()
{
	static uint8_t i = 0;
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
			display.disable();

			modem.disable();

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
			display.enable();

			// ... and modem
			modem.enable();

			want_shutdown = 0;
		}
	}
	else {
		want_shutdown = 0;
	}

	while (modem.buffer_available()) {
		disp[i++] = modem.buffer_get();
		if (i == 8)
			i = 0;
	}
}

ISR(PCINT1_vect)
{
	// we use PCINT1 for wakeup, so we should catch it here (and do nothing)
}
