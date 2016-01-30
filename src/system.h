#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#define SHUTDOWN_THRESHOLD 2048

/**
 * Contains the system idle loop. Checks for button presses, handles
 * standby/resume, reads data from the Modem and updates the Display.
 */
class System {
	private:
		uint16_t want_shutdown;
		void shutdown(void);
	public:
		System() { want_shutdown = 0; };
		void loop(void);
};

extern System rocket;
