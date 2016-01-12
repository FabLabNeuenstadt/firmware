#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

int main (void)
{
	unsigned int i, j, h;
	wdt_disable();

	DDRB = 0xff;
	DDRD = 0xff;

	PORTB = 0;
	PORTD = 0;

	while (1) {
		for (i = 1; i < 256; i *= 2) {
			PORTB = i;
			for (j = 1; j < 256; j *= 2) {
				PORTD = ~j;
				for (h = 1; h < 1; h++) { // use "h < 4096" for visible pixels (e.g. finding soldering errors)
					asm("nop");
				}
			}
			PORTB = 0;
		}
	}

	return 0;
}
