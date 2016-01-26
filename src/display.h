#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

struct __text {
	uint8_t speed_delay;
	uint8_t direction_reserved;
	char *str;
};

struct __animation {
	uint8_t speed;
	uint8_t delay;
	char *data;
};

typedef struct __text text;
typedef struct __animation animation;

class Display {
	private:
		uint16_t update_delay;
		uint8_t need_update;
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
		void update(void);

		void show(text t);
		void show(char *str);
};

extern Display display;
