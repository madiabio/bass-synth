// Configs i2c for Tiva board
#include <stdint.h>
void init_i2c_0(void); // initialises Tiva's i2c0 subsystem
void i2c0_writeByte(uint8_t addr7, uint8_t data); // Writes a Byte to I2C with slave addr addr7
void i2c0_writeBuffer(uint8_t addr7, uint8_t* buf, uint8_t len); // General function to write a buffer of length len to I2C
void mcp4275_write(uint16_t value); // write 12 bits to the MCP4275 DAC in fast write mode
