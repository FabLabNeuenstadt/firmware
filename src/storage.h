#include <stdlib.h>

#define I2C_EEPROM_ADDR 0x50

class Storage {
	private:
		uint8_t num_anims;
		uint8_t first_free_page;
		uint8_t i2c_start_write(void);
		uint8_t i2c_start_read(void);
		void i2c_stop(void);
		uint8_t i2c_send(uint8_t len, uint8_t *data);
		uint8_t i2c_receive(uint8_t len, uint8_t *data);
		uint8_t i2c_read(uint8_t addrhi, uint8_t addrlo, uint8_t len, uint8_t *data);
		uint8_t i2c_write(uint8_t addrhi, uint8_t addrlo, uint8_t len, uint8_t *data);

		enum I2CStatus : uint8_t {
			I2C_OK,
			I2C_START_ERR,
			I2C_ADDR_ERR,
			I2C_ERR
		};

	public:
		Storage() { num_anims = 0; first_free_page = 0;};

		/**
		 * Enable the storage hardware: Configures the internal I2C
		 * module and reads the number of stored patterns from the
		 * EEPROM.
		 */
		void enable();

		/**
		 * Prepares the storage for a complete overwrite by setting the
		 * number of stored animations to zero. The next save operation
		 * will get pattern id 0 and overwrite the first stored pattern.
		 *
		 * This function does not delete patterns from the EEPROM. However,
		 * it does reset the pattern counter, thus making all saved patterns
		 * unavailable to the load and numPatterns functions.
		 */
		void reset();

		void sync();

		/**
		 * Checks whether the EEPROM contains animathion data.
		 *
		 * @return true if the EEPROm contains valid-looking data
		 */
		bool hasData();

		/**
		 * Accessor for the number of saved patterns on the EEPROM.
		 * Only returns valid data if hasData() returned true.
		 *
		 * @return number of patterns
		 */
		uint8_t numPatterns() { return num_anims; };

		/**
		 * Load pattern from EEPROM.
		 *
		 * @param idx pattern index (starting with 0)
		 * @param data pointer to data structure for the pattern. Must be
		 *        at least 260 bytes
		 */
		void load(uint8_t idx, uint8_t *data);

		/**
		 * Save (possibly partial) pattern on the EEPROM. 32 bytes of
		 * dattern data will be read and stored, regardless of the
		 * pattern header.
		 *
		 * @param data pattern data. Must be at least 32 bytes
		 */
		void save(uint8_t *data);

		/**
		 * Continue saving a pattern on the EEPROM. Appends 32 bytes of
		 * pattern data after the most recently written block of data
		 * (i.e., to the pattern which is currently being saved).
		 *
		 * @param data pattern data. Must be at least 32 bytes
		 */
		void append(uint8_t *data);
};

extern Storage storage;
