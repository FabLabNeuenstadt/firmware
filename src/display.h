#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

class Display {
	public:
		Display() {};
		void enable(void);
		void disable(void);
		char string[128];
};

extern Display display;
