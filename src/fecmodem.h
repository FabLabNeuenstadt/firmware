#include <avr/io.h>
#include <stdlib.h>

#ifndef FECMODEM_H_
#define FECMODEM_H_

#include "hamming.h"
#include "modem.h"

class FECModem : public Modem {
	private:
		enum HammingState : uint8_t {
			FIRST_BYTE,
			SECOND_BYTE
		};
		HammingState hammingState;
		uint8_t buf_byte;

		uint8_t parity128(uint8_t byte);
		uint8_t parity2416(uint8_t byte1, uint8_t byte2);
		uint8_t correct128(uint8_t *byte, uint8_t parity);
		uint8_t hamming2416(uint8_t *byte1, uint8_t *byte2, uint8_t parity);
	public:
		FECModem() : Modem() { hammingState = FIRST_BYTE; };
		uint8_t buffer_available(void);
		uint8_t buffer_get(void);
};

extern FECModem modem;

#endif /* FECMODEM_H_ */
