#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "fecmodem.h"
#include "storage.h"
#include "system.h"

#define SHUTDOWN_THRESHOLD 2048

System rocket;

extern animation_t ohai;

uint8_t disp_buf[128];
uint8_t *rx_buf = disp_buf + 64;

void System::initialize()
{
	// disable ADC to save power
	PRR |= _BV(PRADC);

	// dito
	wdt_disable();

	// Enable pull-ups on PC3 and PC7 (button pins)
	PORTC |= _BV(PC3) | _BV(PC7);

	display.enable();
	modem.enable();
	storage.enable();

	sei();
}

// ! This function has not been tested yet
void System::receive(void)
{
	static uint8_t rx_pos = 0;
	static uint16_t remaining_bytes = 0;
	uint8_t rx_byte = modem.buffer_get();

	/*
	 * START* and PATTERN* are sync signals, everything else needs to be
	 * stored on the EEPROM.
	 * (Note that the C++ standard guarantees "rxExpect > PATTERN2" to match
	 * for HEADER*, META* and DATA since they are located after PATTERN2
	 * in the RxExpect enum declaration)
	 */
	if (rxExpect > PATTERN2) {
		rx_buf[rx_pos] = modem.buffer_get();
	}

	switch(rxExpect) {
		case START1:
			if (rx_byte == 0x99)
				rxExpect = START2;
			break;
		case START2:
			if (rx_byte == 0x99)
				rxExpect = PATTERN1;
			break;
		case PATTERN1:
			if (rx_byte == 0xa9)
				rxExpect = PATTERN2;
			break;
		case PATTERN2:
			if (rx_byte == 0xa9)
				rxExpect = HEADER1;
			break;
		case HEADER1:
			rxExpect = HEADER2;
			rx_pos = 0;
			remaining_bytes = (rx_byte & 0x0f) << 8;
			break;
		case HEADER2:
			rxExpect = META1;
			remaining_bytes += rx_byte;
			break;
		case META1:
			rxExpect = META2;
			break;
		case META2:
			rxExpect = DATA_FIRSTBLOCK;
			break;
		case DATA_FIRSTBLOCK:
			remaining_bytes--;
			if (remaining_bytes == 0) {
				rxExpect = PATTERN1; // TODO or new START1
				storage.save(rx_buf);
			} else if (rx_pos == 64) {
				rxExpect = DATA;
				rx_pos = 0;
				storage.save(rx_buf);
			}
			break;
		case DATA:
			remaining_bytes--;
			if (remaining_bytes == 0) {
				rxExpect = PATTERN1; // TODO or new START1
				storage.append(rx_buf);
			} else if (rx_pos == 64) {
				rx_pos = 0;
				storage.append(rx_buf);
			}
			break;
	}

	/*
	if (i == 127) {
		i = 0;
	} else if (modem_byte == 0) {
		if (i > 1) { // workaround for trailing double null bytes
			ohai.data = disp_buf;
			ohai.length = i-1;
			display.show(&ohai);
		}
		i = 0;
	}
	*/
}

void System::loop()
{
	// both buttons are pressed
	if ((PINC & (_BV(PC3) | _BV(PC7))) == 0) {
		// naptime!
		// But not before both buttons have been pressed for
		// SHUTDOWN_THRESHOLD * 0.256 ms. And then, not before both have
		// been released, because otherwise we'd go te sleep when
		// they're pressed and wake up when they're released, which
		// isn't really the point here.

		if (want_shutdown < SHUTDOWN_THRESHOLD) {
			want_shutdown++;
		}
		else {
			shutdown();

			want_shutdown = 0;
		}
	}
	else {
		want_shutdown = 0;
	}

	while (modem.buffer_available()) {
		receive();
	}

	display.update();
}

void System::shutdown()
{
	// turn off display to indicate we're about to shut down
	display.disable();

	modem.disable();

	// wait until both buttons are released
	while (!((PINC & _BV(PC3)) && (PINC & _BV(PC7)))) ;

	// and some more to debounce the buttons
	_delay_ms(10);

	// actual naptime

	// enable PCINT on PC3 (PCINT11) and PC7 (PCINT15) for wakeup
	PCMSK1 |= _BV(PCINT15) | _BV(PCINT11);
	PCICR |= _BV(PCIE1);

	// go to power-down mode
	SMCR = _BV(SM1) | _BV(SE);
	asm("sleep");

	// execution will resume here - disable PCINT again.
	// Don't disable PCICR, something else might need it.
	PCMSK1 &= ~(_BV(PCINT15) | _BV(PCINT11));

	// turn on display
	display.enable();

	// ... and modem
	modem.enable();
}

ISR(PCINT1_vect)
{
	// we use PCINT1 for wakeup, so we should catch it here (and do nothing)
}
