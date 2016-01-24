#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "i2c.h"

I2C i2c;

void I2C::enable()
{
	/*
	 * Set I2C clock frequency to 100kHz.
	 * freq = F_CPU / (16 + (2 * TWBR * TWPS) )
	 * let TWPS = "00" = 1
	 * -> TWBR = (F_CPU / 100000) - 16 / 2
	 */
	TWSR = 0; // the lower two bits control TWPS
	TWBR = ((F_CPU / 100000UL) - 16) / 2;
}

// TODO Everything[tm] (error handling and generic code)
// Also TODO: Use interrupts instead of polling
/*
 * This method encapsules both, transmit and read into one function.
 * Although it could be split into read/write, all reads require a transmit.
 * Hence this approach reduces code duplication.
 * 
 * num_tx, num_rx = number of byte to transmit/receive
 * txbuf, rxbuf = pointer to the uint8_t array for tx/rx
 */
void I2C::xmit(int num_tx, int num_rx, uint8_t *txbuf, uint8_t *rxbuf)
{
	int i;

	if (num_tx) {
		TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
		while (!(TWCR & _BV(TWINT)));
		TWDR = (I2C_EEPROM_ADDR << 1) | 0;
		TWCR = _BV(TWINT) | _BV(TWEN);
		while (!(TWCR & _BV(TWINT)));
		for (i = 0; i < num_tx; i++) {
			TWDR = txbuf[i];
			TWCR = _BV(TWINT) | _BV(TWEN);
			while (!(TWCR & _BV(TWINT)));
		}
	}
	if (num_rx) {
		TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
		while (!(TWCR & _BV(TWINT)));
		TWDR = (I2C_EEPROM_ADDR << 1) | 1;
		TWCR = _BV(TWINT) | _BV(TWEN);
		while (!(TWCR & _BV(TWINT)));
		for (i = 0; i < num_rx; i++) {
			if (i == num_rx-1) {
				TWCR = _BV(TWINT) | _BV(TWEN);
			} else {
				TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
			}
			while (!(TWCR & _BV(TWINT)));
			rxbuf[i] = TWDR;
		}
	}
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}
