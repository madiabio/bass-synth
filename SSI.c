#include <TM4C129.h>
#include "ES_Lib.h"
#include <stdint.h>
#define PB4 (1<<4) // SSI1Fss
#define PB5 (1<<5) // SSI1Clk
#define PE4	(1<<4) // SSI1XDAT0 (unused) Rx
#define PE5 (1<<5) // SSI1XDAT1 Tx

#define PQ0 (1<<0) // SSI3Clk
#define PQ1 (1<<1) // SSI3Fss
#define PQ2 (1<<2) // SSI3XDAT0 (unused) Rx
#define PQ3 (1<<3) // SSI3XDAT1 Tx

#define PD0 (1<<0) // SSI2XDAT1 Tx
#define PD1 (1<<1) // SSI2XDAT0 (unused) Rx
#define PD2 (1<<2) // SS2Fss
#define PD3 (1<<3) // SSI2Clk


void init_SSI1() // for the 12 bit DAC
{
	SYSCTL->RCGCGPIO |= (1<<1) | (1<<4); // enable GPIOA
	while ( (SYSCTL->PRGPIO & ((1<<1) | (1<<4))) == 0 ) {}
	GPIOB_AHB->DIR |= PB4 | PB5;  // enable Tx, Fss, Clk be output
	GPIOE_AHB->DIR |= PE5;
	
	GPIOE_AHB->DIR &= ~(PE4); // enable Rx to be input
	GPIOB_AHB->AFSEL |= (PB4 | PB5); // set Tx, Fss, Rx and Clk to be alt function
	GPIOE_AHB->AFSEL |= (PE4 | PE5);
		
	GPIOB_AHB->PCTL |= (0x15 << (4*4)) | (0x15 << (5*4)); // Select alt function with PCTL
	GPIOE_AHB->PCTL |= (0x15 << (4*4)) | (0x15 << (5*4));
		
	GPIOB_AHB->DEN |= PB4 | PB5; // Enable all to be digital
	GPIOE_AHB->DEN |= PE4 | PE5;
	
	GPIOB_AHB->PUR|= PB5; // Set clk pin to have PUR
		
	SYSCTL->RCGCSSI |= (1<<1); // enable SSI1
	while ( (SYSCTL->PRSSI & (1<<1)) == 0) {} // wait til ready
			
	SSI1->CR1 &= ~(1<<1); // disable SSI1
	
	SSI1->CPSR = 2; 
	
	// Configure CR0: SCR, mode, data size
	SSI1->CR0 = (6 << 8)   // SCR=6, ~1.14 MHz
						| (0 << 7)   // SPO=0
						| (1 << 6)   // SPH=1 (mode 1)
						| (0xF);     // 16-bit data
	
	SSI1->CR1 |= (1<<1); // Enable SSI1
	
}



void initSPI(void) {
    // Enable Port D
    SYSCTL->RCGCGPIO |= (1<<3);
    while ((SYSCTL->PRGPIO & (1<<3)) == 0) {}

    // PD1=SSI2Tx, PD3=SSI2Clk, PD2=CS (manual), PD0=D/C (manual)
    GPIOD_AHB->DIR  |= (PD0 | PD1 | PD2 | PD3);
    GPIOD_AHB->DEN  |= (PD0 | PD1 | PD2 | PD3);
    GPIOD_AHB->AFSEL |= (PD1 | PD3);    // SSI2Tx, SSI2Clk
    GPIOD_AHB->AFSEL &= ~(PD0 | PD2);   // PD0=D/C, PD2=CS as GPIO
    GPIOD_AHB->PCTL &= ~((0xF<<(1*4)) | (0xF<<(3*4)));
    GPIOD_AHB->PCTL |=  (0xF<<(1*4)) | (0xF<<(3*4)); // SSI2 on PD1,PD3

    GPIOD_AHB->PUR |= PD3; // pull-up on CLK if required

    // Enable SSI2
    SYSCTL->RCGCSSI |= (1<<2);
    while ((SYSCTL->PRSSI & (1<<2)) == 0) {}

    SSI2->CR1 = 0;                 // disable SSI2, master mode
    SSI2->CPSR = 8;                // prescale: 16/8 = 2 MHz
    SSI2->CR0  = (0<<8) | (1<<7) | (1<<6) | 0x7; // mode 3, 8-bit
    SSI2->CR1 |= (1<<1);           // enable SSI2

    // Hold CS low permanently
    GPIOD_AHB->DATA &= ~PD2;
}

void spi_Transmit(uint8_t data) {
    while ((SSI2->SR & (1<<1)) == 0) {} // wait TX FIFO not full
    SSI2->DR = data;
    while (SSI2->SR & (1<<4)) {}        // wait not busy
}

void init_SSI3() // for I2S
{
	SYSCTL->RCGCGPIO |= (1 << 14);
	while ( (SYSCTL->PRGPIO & (1<<14)) == 0) {}
	
	GPIOQ->DIR |= PQ3 | PQ1 | PQ0; // enable tx, fss, clk as output
	GPIOQ->DIR &= ~(PQ2); // enable rx as input
	GPIOQ->AFSEL |= PQ0 | PQ1 | PQ2 | PQ3; // enable all as alt functions
	GPIOQ->PCTL |= (0xE << 0*4) | (0xE << 1*4) | (0xE << 2*4) | (0xE << 3*4); // Select SSI in PCTL
	
	GPIOQ->DEN |= PQ0 | PQ1 | PQ2 | PQ3; // enable all as digital
	GPIOQ->PUR |= PQ0; // put PUR on clk
	
	SYSCTL->RCGCSSI |= (1<<3); // enable SSI3 clock
	while ( (SYSCTL->PRSSI & (1<<3)) == 0) {} // wait
	
	SSI3->CR1 &= ~(1<<1); // disable SSI3
	SSI3->CPSR = 2; // fSSICLk = f_sysclk / (CPSDVSR * (1+SCR))
	SSI3->CR0 = (4<<8) | (0x0 << 6) | (0xF); // SCR = 4, SPI/TI frame format , DSS = 16 bit data.
}