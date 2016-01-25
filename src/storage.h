#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define I2C_EEPROM_ADDR 0x50

class Storage {
	private:
		void i2c_start_write(void);
		void i2c_start_read(void);
		void i2c_stop(void);
		int8_t i2c_send(uint8_t len, uint8_t *data);
		int8_t i2c_receive(uint8_t len, uint8_t *data);
		int8_t i2c_read(uint16_t addr, uint8_t len, uint8_t *data);
		int8_t i2c_write(uint16_t addr, uint8_t len, uint8_t *data);
		// TODO "file system" housekeeping (index of first free page)
	public:
		Storage() {};
		void enable();
		// TODO load / save methods for animations
};

extern Storage storage;
