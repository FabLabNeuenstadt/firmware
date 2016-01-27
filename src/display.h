#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

struct __text {
	uint8_t speed;
	uint8_t delay;
	uint8_t direction;
	uint8_t *str;
};

struct __animation {
	uint8_t speed;
	uint8_t delay;
	uint8_t *data;
};

typedef struct __text text_t;
typedef struct __animation animation_t;

class Display {
	private:
		text_t active_text;
		animation_t active_anim;
		uint8_t need_update;
		uint8_t active_col;
		uint8_t disp_buf[8];
		uint8_t str_pos;
		int8_t char_pos;
		uint8_t data_buf[128];
		enum DisplayMode : uint8_t {
			TEXT = 1,
			ANIMATION = 2
		};
		DisplayMode mode;
	public:
		Display();
		void enable(void);
		void disable(void);

		void multiplex(void);
		void reset(void);
		void update(void);

		void show(text_t text);
		void show(animation_t anim);
		void show(uint8_t *str);
};

extern Display display;
