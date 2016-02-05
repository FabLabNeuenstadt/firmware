#include <stdlib.h>

#include "display.h"
#include "font.h"
#include "storage.h"
#include "fecmodem.h"
#include "system.h"

animation_t ohai;

int main (void)
{
	ohai.type = AnimationType::TEXT;
	ohai.speed = (2 << 4) + 15;
	ohai.data = (uint8_t *)" Ohai  \001";
	ohai.length = 8;
	ohai.delay = (0 << 4) + 0; // -> 4bit 0 remains 0

	uint8_t anim_data[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

	animation_t test;
	test.type = AnimationType::FRAMES;
	test.speed = 14;
	test.length = 2*8;
	test.data = anim_data;
	test.delay = (1 << 4);

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
