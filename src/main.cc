#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"
#include "storage.h"
#include "modem.h"
#include "system.h"

int main (void)
{
	text ohai;
	ohai.str = (char *)"Ohai! ";

	// disable ADC to save power
	PRR |= _BV(PRADC);

	// dito
	wdt_disable();

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	display.show(ohai);

	display.enable();
	modem.enable();
	storage.enable();

	sei();

	while (1) {
		// nothing to do here, go to idle to save power
		SMCR = _BV(SE);
		asm("sleep");
		/*
		 * The display timer causes a wakeup after 256µs. Run the system
		 * loop after the timer's ISR is done.
		 * The Modem also causes wakeups, which is pretty convenient since
		 * it means we can immediately process the received data.
		 */
		rocket.loop();
		display.update();
	}

	return 0;
}
