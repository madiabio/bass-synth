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
	GPIOB_AHB->PCTL &= ~( (0xF << (2*4)) | (0xF << (3*4)) ); // Clear PCTL bits for PB2 and PB3
	GPIOB_AHB->PCTL |= (0x2 << (2*4)) | (0x2 << (3*4)); // Set to values from PCTL table for I2C0
	
	GPIOB_AHB->ODR |= PB3; // Enable open drain for SDA (PB3)
	
	// GPIOB_AHB->ODR |= PB2; // Enable opend reain for SCL
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
		
		I2C0->MCS &= ~(1<<5); // disable QCMD
		return 0;
}

void mcp4725_write(uint16_t code) {
    uint8_t upper = (code >> 8) & 0x0F;    // D11..D8
    uint8_t byte2 = (upper & 0x0F);        // PD=00
    uint8_t byte3 = code & 0xFF;           // D7..D0

    I2C0->MSA = (MCP4725_ADDR<<1) | 0;   // write

    I2C0->MDR = byte2;
    I2C0->MCS = I2C_MCS_START | I2C_MCS_RUN;
    while (I2C0->MCS & I2C_MCS_BUSY) {}

    I2C0->MDR = byte3;
    I2C0->MCS = I2C_MCS_STOP | I2C_MCS_RUN;
    while (I2C0->MCS & I2C_MCS_BUSY) {}
}


void mcp4725_test_connection()
{
	I2C0->MSA = (0x78 << 1) | 1; // R/W = 1 for read probe
	I2C0->MCS = (1<<0) | (1<<1) | (1<<2); // START + RUN
	while (I2C0->MCS & (1<<0)); // wait for BUSY to clear
	if (I2C0->MCS & (1<<2)) {
			ES_printf("NACK\n");
	} else {
			ES_printf("Ackd\n");
	}
	// I2C0->MCS = (1<<2); // STOP
}

void dac_square_test(){
    const uint8_t addr = MCP4725_ADDR;                 // or 0x61 if A0=VDD
    for(;;){
        mcp4725_write(0);           // 0 V
        msDelay(10);                        // 10 ms
        mcp4725_write(4095);        // ~3.3 V
        msDelay(10);                        // 10 ms  => ~50 Hz square
    }
}
