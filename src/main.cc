#include <stdlib.h>

#include "display.h"
#include "font.h"
#include "storage.h"
#include "modem.h"
#include "system.h"

animation_t ohai;

int main (void)
{
	ohai.type = AnimationType::TEXT;
	ohai.speed = (2 << 4) + 15;
	ohai.data = (uint8_t *)"\001 Ohai  ";
	ohai.length = 8;

	uint8_t anim_data[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0 };

	animation_t test;
	test.type = AnimationType::FRAMES;
	test.speed = 14;
	test.length = 2*8;
	test.data = anim_data;

	rocket.initialize();

	display.show(&ohai);

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
	}

	return 0;
}
