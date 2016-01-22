#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

class Display {
	private:
		uint16_t scroll_delay;
		uint8_t active_col;
		uint8_t disp_buf[8];
		uint8_t str_pos;
		int8_t char_pos;
		char string[128];
	public:
		Display();
		void enable(void);
		void disable(void);
		void multiplex(void);
		void reset(void);
		void setString(char *str);
};

extern Display display;
