/* Name: modem.h
 * Author: Jari Tulilahti
 * Copyright: 2014 Rakettitiede Oy
 * License: LGPLv3, see COPYING, and COPYING.LESSER -files for more info
 */

#ifndef MODEM_H_
#define MODEM_H_

#include <avr/interrupt.h>
#include <stdlib.h>

/* Modem ring buffer size must be power of 2 */
#define MODEM_BUFFER_SIZE	4

/* Modem defines */
#define MODEM_SYNC_LEN		42
#define MODEM_TIMER		TCNT1L
#define MODEM_PCINT		PCINT24
#define MODEM_PCMSK		PCMSK3
#define MODEM_PCIE		PCIE3
#define MODEM_PIN		PA0
#define MODEM_DDR		DDRA

class Modem {
	public:
		Modem() {};
		uint8_t buffer_available(void);
		uint8_t buffer_get(void);
		void enable(void);
		void disable(void);
		void receive(void);
};

extern Modem modem;

#endif /* MODEM_H_ */
