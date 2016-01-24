#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"
#include "i2c.h"
#include "modem.h"
#include "system.h"

int main (void)
{
	char testbuf[] = "..Olol I2C extra lang bla foo wololo moep   ";
	char testbuf2[32];
	testbuf[0] = 1;
	testbuf[1] = 0;
	// disable ADC to save power
	PRR |= _BV(PRADC);

	// dito
	wdt_disable();

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	display.setString("Ohai! ");

	display.enable();
	modem.enable();
	i2c.enable();

	sei();
	//display.setString("tx");

	i2c.xmit(64, 0, (uint8_t *)testbuf, (uint8_t *)testbuf);

	//display.setString("rx");

	i2c.xmit(2, 64, (uint8_t *)testbuf, (uint8_t *)testbuf2);
	display.setString(testbuf2);

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
