#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>

enum class AnimationType : uint8_t {
	TEXT = 1,
	FRAMES = 2
};

/**
 * Generic struct for anything which can be displayed, e.g. texts or
 * sequences of frames.
 */
struct animation {
	/**
	 * Specific kind of animation described in this struct. Controls the
	 * behaviour of Display::multiplex() and Display::update().
	 */
	AnimationType mode;

	/**
	 * Length of data in bytes
	 */
	uint8_t length;

	/**
	 * * If mode == TEXT: Text scroll speed in columns per TODO
	 * * If mode == FRAMES: Frames per TODO
	 */
	uint8_t speed;

	/**
	 * Delay after the last text symbol / animation frame. Not yet supported.
	 */
	uint8_t delay;

	/**
	 * Scroll mode / direction. Not yet supported.
	 */
	uint8_t direction;

	/**
	 * * If mode == TEXT: character array pointing to the
	 *   animation text in standard ASCII format (+ special font chars)
	 * * If mode == FRAMES: Frame array. Each element encodes
	 *   a display column (starting with the leftmost one), each group of
	 *   eight elements is a frame.
	 */
	uint8_t *data;
};

typedef struct animation animation_t;

/**
 * Controls the display. Handles multiplexing, scrolling and supports loading
 * arbitrary animations.
 */
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

		/**
		 * Enable the display driver.
		 * Configures ports B and D as output and enables the display
		 * timer and interrupt.
		 */
		void enable(void);

		/**
		 * Disable the display driver.
		 * Turns off both the display itself and the display timer.
		 */
		void disable(void);

		/**
		 * Draws a single display column. Called every 256 microseconds
		 * by the timer interrupt (TIMER0_OVF_vect), resulting in
		 * a display refresh rate of ~500Hz (one refresh per 2048µs)
		 */
		void multiplex(void);

		/**
		 * Reset display and animation state. Fills the screen with "black"
		 * (that is, no active pixels) and sets the animation offset to zero.
		 */
		void reset(void);

		/**
		 * Update display content.
		 * Checks active_anim->speed and active_anim->type and scrolls
		 * the text / advances a frame when appropriate. Does nothing
		 * otherwise.
		 */
		void update(void);

		/**
		 * Set the active animation to be shown on the display. Automatically
		 * calls reset().
		 *
		 * @param anim active animation. Note that the data is not copied,
		 *        so anim has to be kept in memory until a new one is loaded
		 */
		void show(animation_t *anim);
};

extern Display display;
