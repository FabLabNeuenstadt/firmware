#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#define SHUTDOWN_THRESHOLD 2048

class System {
	private:
		uint16_t want_shutdown;
	public:
		System() { want_shutdown = 0; };
		void loop(void);
		void shutdown(void);
};

extern System rocket;
