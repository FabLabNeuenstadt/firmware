#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"
#include "modem.h"
#include "system.h"

int main (void)
{
	// disable ADC to save power
	PRR |= _BV(PRADC);

	// dito
	wdt_disable();

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	display.string[0] = 'O';
	display.string[1] = 'h';
	display.string[2] = 'a';
	display.string[3] = 'i';
	display.string[4] = '!';
	display.string[5] = ' ';
	display.string[6] = 0;

	display.enable();
	modem.enable();

	sei();

	while (1) {
		// nothing to do here, go to idle to save power
		SMCR = _BV(SE);
		asm("sleep");
		// The display timer causes a wakeup after 256µs. Run the system
		// loop after the timer's ISR is done.
		rocket.loop();
	}

	return 0;
}
