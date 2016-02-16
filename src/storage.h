#include <stdlib.h>

#define I2C_EEPROM_ADDR 0x50

class Storage {
	private:
		uint8_t num_anims;
		uint8_t first_free_page;
		void i2c_start_write(void);
		void i2c_start_read(void);
		void i2c_stop(void);
		int8_t i2c_send(uint8_t len, uint8_t *data);
		int8_t i2c_receive(uint8_t len, uint8_t *data);
		int8_t i2c_read(uint16_t addr, uint8_t len, uint8_t *data);
		int8_t i2c_write(uint16_t addr, uint8_t len, uint8_t *data);
		// TODO "file system" housekeeping (index of first free page)
	public:
		Storage() { num_anims = 0xff; first_free_page = 0;};
		void enable();
		void reset();
		void load(uint16_t idx, uint8_t *data);
		void save(uint16_t idx, uint8_t *data);
		void append(uint8_t *data);
		// TODO load / save methods for animations
};

extern Storage storage;
