#include <stdlib.h>

#define SHUTDOWN_THRESHOLD 2048

/**
 * Contains the system idle loop. Checks for button presses, handles
 * standby/resume, reads data from the Modem and updates the Display.
 */
class System {
	private:
		uint16_t want_shutdown;
		uint8_t current_anim_no;
		void shutdown(void);
		void receive(void);
		void loadPattern(uint8_t anim_no);

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
		System() { want_shutdown = 0; rxExpect = START1; current_anim_no = 0; btnMask = BUTTON_NONE;};

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
};

extern System rocket;
