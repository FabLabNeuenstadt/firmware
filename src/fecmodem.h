#include <avr/io.h>
#include <stdlib.h>

#ifndef FECMODEM_H_
#define FECMODEM_H_

#include "hamming.h"
#include "modem.h"

class FECModem : public Modem {
	private:
		uint8_t parity128(uint8_t byte);
		uint8_t parity2416(uint8_t byte1, uint8_t byte2);
		uint8_t correct128(uint8_t *byte, uint8_t parity);
		uint8_t hamming2416(uint8_t *byte1, uint8_t *byte2, uint8_t parity);
};

extern FECModem modem;

#endif /* FECMODEM_H_ */
