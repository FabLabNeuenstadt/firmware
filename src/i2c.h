#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define I2C_EEPROM_ADDR 0x50

class I2C {
	public:
		I2C() {};
		void enable();
		void xmit(int num_tx, int num_rx, uint8_t *txbuf, uint8_t *rxbuf);
};

extern I2C i2c;
