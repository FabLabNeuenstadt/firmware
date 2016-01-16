#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "font.h"
#include "modem.h"
#include "system.h"

volatile uint8_t disp[8];

int main (void)
{
	// disable ADC to save power
	PRR |= _BV(PRADC);

	// dito
	wdt_disable();

	// Ports B and D drive the dot matrix display -> set all as output
	DDRB = 0xff;
	DDRD = 0xff;
	PORTB = 0;
	PORTD = 0;

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	// Enable 8bit counter with prescaler=8 (-> timer frequency = 1MHz)
	TCCR0A = _BV(CS01);
	// raise timer interrupt on counter overflow (-> interrupt frequency = ~4kHz)
	TIMSK0 = _BV(TOIE0);

	disp[0] = 0x01;
	disp[1] = 0x02;
	disp[2] = 0x04;
	disp[3] = 0x08;
	disp[4] = 0x10;
	disp[5] = 0x20;
	disp[6] = 0x40;
	disp[7] = 0x80;

#if 0
	// smile!
	disp[0] = 0x08;
	disp[1] = 0x04;
	disp[2] = 0x62;
	disp[3] = 0x02;
	disp[4] = 0x02;
	disp[5] = 0x62;
	disp[6] = 0x04;
	disp[7] = 0x08;
	disp[8] = 0x00;
	disp[9] = 0x00;
	disp[10] = 0x00;
	disp[11] = 0x00;
	disp[12] = 0x00;
	disp[13] = 0x00;
	disp[14] = 0x00;
	disp[15] = 0x00;
	disp[16] = 0x28;
	disp[17] = 0x44;
	disp[18] = 0x22;
	disp[19] = 0x02;
	disp[20] = 0x02;
	disp[21] = 0x22;
	disp[22] = 0x44;
	disp[23] = 0x28;
	disp[24] = 0x00;
	disp[25] = 0x00;
	disp[26] = 0x00;
	disp[27] = 0x00;
	disp[28] = 0x00;
	disp[29] = 0x00;
	disp[30] = 0x00;
	disp[31] = 0x00;
#endif

	modem.enable();

	sei();

	while (1) {
		// nothing to do here, go to idle to save power
		SMCR = _BV(SE);
		asm("sleep");
		rocket.loop();
	}

	return 0;
}
