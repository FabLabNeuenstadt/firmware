#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#include "display.h"
#include "fecmodem.h"
#include "storage.h"
#include "system.h"
#include "static_patterns.h"

#define SHUTDOWN_THRESHOLD 2048

System rocket;

animation_t active_anim;

uint8_t disp_buf[260]; // 4 byte header + 256 byte data
uint8_t *rx_buf = disp_buf + sizeof(disp_buf) - 33;

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

	//storage.reset();
	//storage.save((uint8_t *)"\x10\x0a\x11\x00nootnoot");
	//storage.save((uint8_t *)"\x10\x09\x20\x00" "fnordor");
	//storage.save((uint8_t *)"\x10\x05\x20\x00 \x01 ");
	//storage.save((uint8_t *)"\x20\x22\x08\x02"
	//		"\x00\x04\x22\x02\x22\x04\x00\x00"
	//		"\x00\x00\x00\x00\x00\x00\x00\x00"
	//		"\x00\x04\x22\x02\x22\x04\x00\x00"
	//		"\x00\x00\x00\x00");
	//storage.append((uint8_t *)"\x00\x00\x00\x00");

	sei();

	current_anim_no = 0;
	loadPattern(0);
}

void System::loadPattern_P(const uint8_t *pattern_ptr)
{
	uint8_t i;

	for (i = 0; i < 4; i++)
		disp_buf[i] = pgm_read_byte(pattern_ptr + i);

	for (i = 0; i < disp_buf[1]; i++)
		disp_buf[i+4] = pgm_read_byte(pattern_ptr + i + 4);

	loadPattern_buf(disp_buf);
}

void System::loadPattern_buf(uint8_t *pattern)
{
	active_anim.type = (AnimationType)(pattern[0] >> 4);
	active_anim.length = pattern[1];

	if (active_anim.type == AnimationType::TEXT) {
		active_anim.speed = (pattern[2] & 0xf0) + 15;
		active_anim.delay = (pattern[2] & 0x0f ) << 4;
		active_anim.direction = pattern[3] >> 4;
	} else if (active_anim.type == AnimationType::FRAMES) {
		active_anim.speed = ((pattern[2] & 0x0f) << 4) + 15;
		active_anim.delay = (pattern[3] & 0x0f) << 2;
	}

	active_anim.data = pattern + 4;
	display.show(&active_anim);
}

void System::loadPattern(uint8_t anim_no)
{
	if (storage.hasData()) {
		storage.load(anim_no, disp_buf);
		loadPattern_buf(disp_buf);
	} else {
		loadPattern_P(emptyPattern);
	}
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
		rx_buf[rx_pos++] = rx_byte;
		/*
		 * HEADER and META are not included in the length
		 * -> only count bytes for DATA.
		 */
		if (rxExpect > META2) {
			remaining_bytes--;
		}
	}

	switch(rxExpect) {
		case START1:
			if (rx_byte == BYTE_START)
				rxExpect = START2;
			else
				rxExpect = NEXT_BLOCK;
			break;
		case START2:
			if (rx_byte == BYTE_START) {
				rxExpect = PATTERN1;
				storage.reset();
				loadPattern_P(flashingPattern);
			} else {
				rxExpect = NEXT_BLOCK;
			}
			break;
		case NEXT_BLOCK:
			if (rx_byte == BYTE_START)
				rxExpect = START2;
			else if (rx_byte == BYTE_PATTERN)
				rxExpect = PATTERN2;
			else if (rx_byte == BYTE_END) {
				storage.sync();
				current_anim_no = 0;
				loadPattern(0);
				rxExpect = START1;
			}
			break;
		case PATTERN1:
			if (rx_byte == BYTE_PATTERN)
				rxExpect = PATTERN2;
			else
				rxExpect = NEXT_BLOCK;
			break;
		case PATTERN2:
			rx_pos = 0;
			if (rx_byte == BYTE_PATTERN)
				rxExpect = HEADER1;
			else
				rxExpect = NEXT_BLOCK;
			break;
		case HEADER1:
			rxExpect = HEADER2;
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
			if (remaining_bytes == 0) {
				rxExpect = NEXT_BLOCK;
				storage.save(rx_buf);
			} else if (rx_pos == 32) {
				rxExpect = DATA;
				rx_pos = 0;
				storage.save(rx_buf);
			}
			break;
		case DATA:
			if (remaining_bytes == 0) {
				rxExpect = NEXT_BLOCK;
				storage.append(rx_buf);
			} else if (rx_pos == 32) {
				rx_pos = 0;
				storage.append(rx_buf);
			}
			break;
	}
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

	// TODO refactor the blocks above and below into one

	if ((PINC & _BV(PC3)) == 0) {
		btnMask = (ButtonMask)(btnMask | BUTTON_RIGHT);
	}
	if ((PINC & _BV(PC7)) == 0) {
		btnMask = (ButtonMask)(btnMask | BUTTON_LEFT);
	}
	if ((PINC & (_BV(PC3) | _BV(PC7))) == (_BV(PC3) | _BV(PC7))) {
		if (btnMask == BUTTON_RIGHT) {
			current_anim_no = (current_anim_no + 1) % storage.numPatterns();
			loadPattern(current_anim_no);
		} else if (btnMask == BUTTON_LEFT) {
			if (current_anim_no == 0)
				current_anim_no = storage.numPatterns() - 1;
			else
				current_anim_no--;
			loadPattern(current_anim_no);
		}
		btnMask = BUTTON_NONE;
	}

	while (modem.buffer_available()) {
		receive();
	}

	display.update();
}

void System::shutdown()
{
	uint8_t i;

	modem.disable();

	// show power down image
	loadPattern_P(shutdownPattern);

	// wait until both buttons are released
	while (!((PINC & _BV(PC3)) && (PINC & _BV(PC7))))
		display.update();

	// and some more to debounce the buttons (and finish powerdown animation)
	for (i = 0; i < 100; i++) {
		display.update();
		_delay_ms(2);
	}

	// turn off display to indicate we're about to shut down
	display.disable();

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

	// debounce
	_delay_ms(50);

	// turn on display
	loadPattern(current_anim_no);
	display.enable();

	// ... and modem
	modem.enable();

	// also, reset state machine
	rxExpect = START1;
}

ISR(PCINT1_vect)
{
	// we use PCINT1 for wakeup, so we need an (in this case empty) ISR for it
}
