#include <TM4C129.h>
#include "ES_Lib.h"
#include <stdint.h>
#include "SSI.h"
#include "config.h" // for priorities
#include "function_gen.h" // for next_sample();

void init_SSI1() // for the 12 bit DAC
{
	
	SYSCTL->RCGCGPIO |= (1<<1) | (1<<4); // enable GPIOB, GPIOE
	while ( (SYSCTL->PRGPIO & ((1<<1) | (1<<4))) == 0 ) {}
	GPIOB_AHB->DIR |= PB4 | PB5;  // enable Tx, Fss, Clk be output
	GPIOE_AHB->DIR |= PE4;
	
	GPIOE_AHB->DIR &= ~(PE4); // enable Rx to be input
	GPIOB_AHB->AFSEL |= (PB4 | PB5); // set Tx, Fss, Rx and Clk to be alt function
	GPIOE_AHB->AFSEL |= (PE4);
		
	GPIOB_AHB->PCTL |= (0xF << (4*4)) | (0xF << (5*4)); // Select alt function with PCTL
	GPIOE_AHB->PCTL |= (0xF << (4*4)) | (0xF << (5*4));
		
	GPIOB_AHB->DEN |= PB4 | PB5; // Enable all to be digital
	GPIOE_AHB->DEN |= PE4;
	
	GPIOB_AHB->PUR|= PB5; // Set clk pin to have PUR
		
	SYSCTL->RCGCSSI |= (1<<1); // enable SSI1
	while ( (SYSCTL->PRSSI & (1<<1)) == 0) {} // wait til ready
			
	SSI1->CR1 &= ~(SSE); // disable SSI1
	
	SSI1->CPSR = SSI1_CPSR; 
	
	// Configure CR0: SCR, mode, data size
	SSI1->CR0 = SSI1_SCR   // SCR=6, ~1.14 MHz
						| (0 << 7)   // SPO=0
						| (1 << 6)   // SPH=1 (mode 1)
						| (0xF);     // 16-bit data
	
	SSI1->CR1 |= SSE; // Enable SSI1
}

void initSPI(void) {
	// Pins: PD0=D/C (GPIO), PD1=SSI2XDAT0, PD2=SSI2Fss, PD3=SSI2Clk
	SYSCTL->RCGCGPIO |= (1<<3); while(!(SYSCTL->PRGPIO & (1<<3)));

	GPIOD_AHB->DIR  |= PD0;                   // D/C only
	GPIOD_AHB->DEN  |= PD0|PD1|PD2|PD3;
	GPIOD_AHB->AFSEL |= (PD1|PD2|PD3);        // HW MOSI, FSS, CLK
	GPIOD_AHB->AFSEL &= ~PD0;
	GPIOD_AHB->PCTL &= ~((0xF<<(0*4))|(0xF<<(1*4))|(0xF<<(2*4))|(0xF<<(3*4)));
	GPIOD_AHB->PCTL |=  (0x0<<(0*4))|(0xF<<(1*4))|(0xF<<(2*4))|(0xF<<(3*4));
	GPIOD_AHB->AMSEL &= ~(PD0|PD1|PD2|PD3);

	SYSCTL->RCGCSSI |= (1<<2); while(!(SYSCTL->PRSSI & (1<<2)));

	SSI2->CR1 = 0;                // legacy, master
	SSI2->ICR = 0x3;              // clear ROR/RT
	SSI2->CPSR = SSI2_CPSR;              // prescale
	SSI2->CR0  = SSI2_SCR | (1<<7) | (1<<6) | 0x7; // SCR=5, SPO=1, SPH=1, 8-bit
	SSI2->CR1 |= (1<<1);          // SSE enable

	// idle levels
	GPIOD_AHB->DATA |= PD0;       // D/C=1 (data) until library toggles it
}


// for Dislpay
void spi_Transmit(uint8_t data) {
    while ((SSI2->SR & TNF) == 0) {}
    SSI2->DR = data;
    while (SSI2->SR & (1<<4)) {}
}




void init_SSI3() // for I2S
{
	SYSCTL->RCGCGPIO |= (1 << 14);
	while ( (SYSCTL->PRGPIO & (1<<14)) == 0) {}
	
	GPIOQ->DIR |= PQ2 | PQ1 | PQ0 ; // enable tx, fss, clk as output
	
	GPIOQ->PUR |= PQ0; // put PUR on clk
	
	GPIOQ->AFSEL |= PQ0 | PQ1 |  PQ2; // enable all as alt functions
	GPIOQ->PCTL |= (0xE << 0*4) | (0xE << 1*4) | (0xE << 2*4); // Select SSI in PCTL
		
	GPIOQ->DEN |= PQ0 | PQ1 | PQ2; // enable all as digital
		
	SYSCTL->RCGCSSI |= (1<<3); // enable SSI3 clock
	while ( (SYSCTL->PRSSI & (1<<3)) == 0) {} // wait
	
	SSI3->CR1 &= ~(SSE); // disable SSI3
	
	SSI3->CPSR = SSI3_CPSR ; 
	SSI3->CR0 = SSI3_SCR   // SCR
						| (0 << 7)   // SPO
						| (1 << 6)   // SPH
						| (0xF);     // 16-bit data
 
	SSI3->CR1 |= (1<<1); // enable SSI3 again
}

