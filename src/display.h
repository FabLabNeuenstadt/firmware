#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

class Display {
	public:
		Display() {};
		void turn_on(void);
		void turn_off(void);
};

extern Display display;
