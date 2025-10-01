// Configs i2c for Tiva board
#include <stdint.h>
#define I2C_MCS_RUN   (1 << 0)
#define I2C_MCS_START (1 << 1)
#define I2C_MCS_STOP  (1 << 2)
#define I2C_MCS_BUSY  (1 << 0)
#define I2C_MCS_ERROR (1 << 1)

void init_i2c_0(void); // initialises Tiva's i2c0 subsystem
int i2c0_writeByte(uint8_t addr7, uint8_t data); // Writes a Byte to I2C with slave addr addr7
int i2c0_writeBuffer(uint8_t addr7, uint8_t* buf, uint8_t len); // General function to write a buffer of length len to I2C
void mcp4725_write(uint16_t value); // write 12 bits to the MCP4275 DAC in fast write mode
void mcp4725_test_connection(); // test the MCP4725 is connected