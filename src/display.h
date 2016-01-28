#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

enum class AnimationType : uint8_t {
	TEXT = 1,
	FRAMES = 2
};

struct animation {
	AnimationType mode;
	uint8_t length;
	uint8_t speed;
	uint8_t delay;
	uint8_t direction;
	uint8_t *data;
};

typedef struct animation animation_t;

class Display {
	private:
		animation_t *active_anim;
		uint8_t need_update;
		uint8_t active_col;
		uint8_t disp_buf[8];
		uint8_t str_pos;
		int8_t char_pos;
	public:
		Display();

		void enable(void);
		void disable(void);

		void multiplex(void);
		void reset(void);
		void update(void);

		void show(animation_t *anim);
};

extern Display display;
