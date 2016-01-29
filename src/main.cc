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
	animation_t ohai;
	ohai.mode = AnimationType::TEXT;
	ohai.speed = (4 << 4) + 15;
	ohai.data = (uint8_t *)"\001 Ohai  ";
	ohai.length = 8;

	uint8_t anim_data[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0 };

	animation_t test;
	test.mode = AnimationType::FRAMES;
	test.speed = 14;
	test.length = 2*8;
	test.data = anim_data;

	// disable ADC to save power
	PRR |= _BV(PRADC);

	// dito
	wdt_disable();

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	display.show(&ohai);

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
