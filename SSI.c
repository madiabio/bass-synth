#include <TM4C129.h>
#include "ES_Lib.h"
#include <stdint.h>
#include "SSI.h"
#include "config.h" // for priorities
#include "function_gen.h" // for next_sample();
#define TXIM (1<<3) // QSSI Transmit FIFO Interrupt Mask
#define TXMIS (1<<3) // QSSI Transmit FIFO Masked Interrupt Status
#define SSE (1<<1) // QSSI Synchronous Serial Port Enable
#define TNF (1<<1) // QSSI Transmit FIFO Not Full
#define SPH (1<<7) // QSSI Serial Clock phase
#define SPO (1<<6) // QSSI Serial Clock Polarity
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
			
	SSI1->CR1 &= ~(SSE); // disable SSI1
	
	SSI1->CPSR = 2; 
	
	// Configure CR0: SCR, mode, data size
	SSI1->CR0 = (6 << 8)   // SCR=6, ~1.14 MHz
						| (0 << 7)   // SPO=0
						| (1 << 6)   // SPH=1 (mode 1)
						| (0xF);     // 16-bit data
	
	SSI1->CR1 |= SSE; // Enable SSI1
	
}

void initSPI(void) {
		GPIOG_AHB->DATA |= PG1; // enable PG1 to show this has been called
    // Enable Port D
    SYSCTL->RCGCGPIO |= (1<<3);
    while ((SYSCTL->PRGPIO & (1<<3)) == 0) {}

    // PD0=D/C (manual); PD1=SSI2XDAT0 (Tx); PD2= typically SSI2Fss; PD3=SSI2Clk; 
    GPIOD_AHB->DIR  |= (PD0 | PD1 | PD2 | PD3);    // drive GPIO outputs before enabling alt
    GPIOD_AHB->DEN  |= (PD0 | PD1 | PD2 | PD3);
    GPIOD_AHB->AFSEL |= (PD1 | PD3);    // SSI2Tx, SSI2Clk
    GPIOD_AHB->AFSEL &= ~(PD0 | PD2);   // PD0=D/C, PD2=CS as GPIO
    GPIOD_AHB->PCTL &= ~((0xF<<(1*4)) | (0xF<<(3*4)));
    GPIOD_AHB->PCTL |=  (0xF<<(1*4)) | (0xF<<(3*4)); // SSI2 on PD1,PD3

    GPIOD_AHB->PUR |= PD3; // pull-up on CLK if required

    // Init SSI2
    SYSCTL->RCGCSSI |= (1<<2);
    while ((SYSCTL->PRSSI & (1<<2)) == 0) {}

    SSI2->CR1 &= ~SSE;                 // disable SSI2, master mode
    SSI2->CPSR = 60;                // prescale: 120/60 = 2 MHz
    SSI2->CR0  = (29<<8) | SPH | SPO | 0x7; // set SCR to 29; sample on rising edge; idle clock high; 8-bit data len
		SSI2->CR1 |= (0x3 << 6); // mode 3 -> advanced SSI mode with 8-bit packet size

    GPIOD_AHB->DATA &= ~PD2; // Hold CS low permanently
		GPIOD_AHB->DATA |= PD0; // Put D/C High so LCD is in 'data' mode at reset.
		
		SSI2->CR1 |= SSE;  // Enable SSI2
}

// for Dislpay
void spi_Transmit(uint8_t data) {
	if ((GPIOK->DATA & PK4) == 0)
	{
		GPIOK->DATA = PK4;  // toggle each call	
	}
	
	while ((SSI2->SR & TNF) == 0) {} // wait until TX FIFO not full
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
	
	// Enable NVIC for SSI3 for interrupts	
	NVIC->IPR[55] = PRIORITY_SSI3;
	NVIC->ISER[1] |= (1 << (55-32)); // enable IRQ 55
	
	SYSCTL->RCGCSSI |= (1<<3); // enable SSI3 clock
	while ( (SYSCTL->PRSSI & (1<<3)) == 0) {} // wait
	
	SSI3->CR1 &= ~(1<<1); // disable SSI3
	
	// fSSI = f_sysclk / ( CPSDVSR * (1+SCR) ) = 1.536 MHz
	// fSSI = 120 MHz / 10 = 1.6 MHz = 1.6 MHz (˜ 4% high)                                                                                 
	SSI3->CPSR = 2; // fSSICLk = f_sysclk / (CPSDVSR * (1+SCR)); CPDVSR  = 48kHz*16; even prescalar
	SSI3->CR0 = (38 << 8) // SCR (Serial Clock Rate) = 38
                        // Together with CPSDVSR (from SSI3->CPSR), this divides the system clock:
                        // fSSI = f_sysclk / (CPSDVSR * (1 + SCR))
                        // This sets the bit clock frequency for I2S/SPI.

            | (0x2 << 6) // SPO/SPH bits = 0b10
                        // Bit 7 = SPH = 1 ? data shifted/captured on second edge (adds 1-bit delay)
                        // Bit 6 = SPO = 0 ? clock idles low
                        // This combination matches I2S requirement (WS changes one bit before MSB).

            | (0xF);     // DSS (Data Size Select) = 0xF ? 16-bit data frames
                        // SSI will transmit/receive 16 bits per frame.
 

	SSI3->IM |= TXIM; // Unmask transmit FIFO interrupt
	
	// Pre-fill the FIFO array
	for (int i=0; i<8; i++) {
			uint16_t out = next_sample();
			SSI3->DR = out;
			SSI3->DR = out;
	}

	SSI3->CR1 |= (1<<1); // enable SSI3 again
}

void SSI3_Handler(void) {
	if (SSI3->MIS & TXMIS) // if TX FIFO half empty interrupt
	{
		// Fill it up until its full
		while (SSI3->SR & TNF)
		{
			uint16_t out = next_sample();
			SSI3->DR = out; // L
			SSI3->DR = out; // R
		}
	}
}


