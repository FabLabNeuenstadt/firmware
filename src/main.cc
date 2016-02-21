#include <avr/io.h>
#include <stdlib.h>

#include "system.h"

int main (void)
{
	rocket.initialize();

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
