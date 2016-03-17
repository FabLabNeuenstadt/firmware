/*
 * Copyright (C) 2016 by Daniel Friesel
 *
 * License: You may use, redistribute and/or modify this file under the terms
 * of either:
 * * The GNU LGPL v3 (see COPYING and COPYING.LESSER), or
 * * The 3-clause BSD License (see COPYING.BSD)
 *
 */

#include <stdlib.h>

#define SHUTDOWN_THRESHOLD 2048

/**
 * Contains the system idle loop. Checks for button presses, handles
 * standby/resume, reads data from the Modem and updates the Display.
 */
class System {
	private:
		uint16_t want_shutdown;
		uint8_t btn_debounce;
		uint8_t current_anim_no;
		void shutdown(void);
		void receive(void);
		void loadPattern(uint8_t pattern_no);
		void loadPattern_buf(uint8_t *pattern);
		void loadPattern_P(const uint8_t *pattern_ptr);

		enum TransmissionControl : uint8_t {
			BYTE_END = 0x84,
			BYTE_START = 0x99,
			BYTE_PATTERN = 0xa9,
		};

		enum ButtonMask : uint8_t {
			BUTTON_NONE = 0,
			BUTTON_LEFT = 1,
			BUTTON_RIGHT = 2,
			BUTTON_BOTH = 3
		};

		enum RxExpect : uint8_t {
			START1,
			START2,
			NEXT_BLOCK,
			PATTERN1,
			PATTERN2,
			HEADER1,
			HEADER2,
			META1,
			META2,
			DATA_FIRSTBLOCK,
			DATA,
		};

		RxExpect rxExpect;
		ButtonMask btnMask;

	public:
		System() { want_shutdown = 0; rxExpect = START1; current_anim_no = 0; btnMask = BUTTON_NONE; btn_debounce = 0;};

		/**
		 * Initial MCU setup. Turns off unused peripherals to save power
		 * and configures the button pins. Also configures all other pins
		 * and peripherals using the enable function of their respective
		 * classes. Turns on interrupts once that's done.
		 */
		void initialize(void);

		/**
		 * System idle loop. Checks for button presses, handles
		 * standby/resume, reads data from the Modem and updates the Display.
		 *
		 * It is recommended to run this function before going back to sleep
		 * whenever the system is woken up by an interrupt.
		 */
		void loop(void);

		void handleTimeout(void);
};

extern System rocket;
