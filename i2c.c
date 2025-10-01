#include <TM4C129.h>
#include "ES_Lib.h"
#include "i2c.h"
#define PB2 (1<<2) // I2C0SCL
#define PB3 (1<<3) // I2C0SDA
#define MCP4725_ADDR 0x60  // A0 tied to GND

void init_i2c_0(void)
{
	SYSCTL->RCGCGPIO |= (1<<1); // enable GPIOB RCGC
	while ( (SYSCTL->PRGPIO & (1<<1)) == 0 ) {} // wait til stable
		
	// PB2 = I2C0SCL, PB3 = I2C0SDA
	GPIOB_AHB->DEN |= PB2 | PB3;
	GPIOB_AHB->AFSEL |= PB2 | PB3;
	GPIOB_AHB->PCTL &= ~( (0xF << (2*4)) | (0xF << 3*4) ); // Clear PCTL bits for PB2 and PB3
	GPIOB_AHB->PCTL |= (0x2 << (2*4)) | (0x2 << (3*4)); // Set to values from PCTL table for I2C0
	
	GPIOB_AHB->ODR |= PB3; // Enable open drain for SDA (PB3)
	GPIOB_AHB->PUR |= PB2 | PB3; // Tempt est
	
		
	SYSCTL->RCGCI2C |= (1<<0); // Enable clock for I2C0
	while ( (SYSCTL->PRI2C & (1<<0)) == 0 ) {} // wait til stable
	

	I2C0->MCR = (1<<4); // Enable I2C as master
	
	//I2C0->MTPR = 1; // MTPR = fclck/(2*((SCL_LP + SCL_HP) * f_scl)) - 1  , we want 400kHz fast mode.
	I2C0->MTPR = 7;
}

int i2c0_writeByte(uint8_t addr7, uint8_t data)
{
	
		while (I2C0->MCS & I2C_MCS_BUSY) {} // wait while I2C busy
		
		I2C0->MSA = (addr7 << 1); // shift 7bit addr to rigth by 1 and set to write mode

		I2C0->MDR = data; // send data to I2C bus
		
		I2C0->MCS = I2C_MCS_RUN | I2C_MCS_START  | I2C_MCS_STOP; // since doing writing a single byte, can do this all in one step. 

		while (I2C0->MCS & (1<<0)) {} // wait til its not busy
		
		if (I2C0->MCS & I2C_MCS_ERROR) return -1; // error handling
			
		return 0;
}

int i2c_writeBuffer(uint8_t addr7, const uint8_t *buf, uint8_t len)
{
	if (len <= 0) return -1; // invalid length
	
	while (I2C0->MCS & I2C_MCS_BUSY) {} // wait til not busy

	I2C0->MSA = (addr7 <<1); // shift 7bit addr to right by 1 and set to write mode (0 in LSB)
		
	I2C0->MDR = buf[0]; // send initial byte of value to I2C bus
	ES_printf("Sending: %02X %02X\n", buf[0], buf[1]);

	I2C0->MCS = I2C_MCS_RUN | I2C_MCS_START; // MCS_RUN, MCS_START
	if (I2C0->MCS & I2C_MCS_ERROR) {
		if (I2C0->MCS & (1<<2)) ES_printf("ADDR NACK\n"); // Address not ack’d
		if (I2C0->MCS & (1<<3)) ES_printf("DATA NACK\n"); // Data not ack’d
		if (I2C0->MCS & (1<<4)) ES_printf("Arbitration lost\n");
		return -1;
	}
	
	while (I2C0->MCS & I2C_MCS_BUSY) {} // wait til not busy

	if (I2C0->MCS & I2C_MCS_ERROR) return -1; // Error handling
	
	for (uint8_t i=1; i<len; i++)
	{
		I2C0->MDR = buf[i]; // send byte to I2C bus
		I2C0->MCS = (i == len-1) ? (I2C_MCS_RUN | I2C_MCS_STOP) : I2C_MCS_RUN; // If its not the last byte, clock out hte byte but keep bus active for more data. Else, clock out the byte and then generate STOP to release the bus.
		while (I2C0->MCS & I2C_MCS_BUSY); // wait til stable
		if (I2C0->MCS & I2C_MCS_ERROR)
		{
			ES_printf("I2C error\n");
			return -1;
		}			
	}
	
	return 0; // worked correctly
}

void mcp4725_write(uint16_t value)
{
    uint8_t buf[2];
    buf[0] = 0x40 | (value >> 8);    // control + D11..D8, extract the top 4 bits of 'value' (0x40 puts the DAC in fast write mode, which just writes to teh DAC and doesn't save anything to EEPROM)
    buf[1] = (uint8_t)(value & 0xFF); // D7..D0, extract the lowest 8 bits of 'value' 
    i2c_writeBuffer(MCP4725_ADDR, buf, 2);
}


void mcp4725_test_connection()
{
	I2C0->MSA = (0x60 << 1) | 1; // R/W = 1 for read probe
	I2C0->MCS = (1<<0) | (1<<1); // START + RUN
	while (I2C0->MCS & (1<<0)); // wait for BUSY to clear
	if (I2C0->MCS & (1<<1)) {
			ES_printf("NACK\n");
	} else {
			ES_printf("Ackd\n");
	}
	I2C0->MCS = (1<<2); // STOP

}