#include <TM4C129.h>
#include "ES_Lib.h"
#include <stdint.h>
#include "SSI.h"
#include "config.h" // for priorities
#include "function_gen.h" // for next_sample();
#define SSI1_CPSR 2
#define SSI1_SCR (6 << 8)
#define SSI3_SCR (38 << 8)
#define SSI3_CPSR 2
void init_SSI1() // for the 12 bit DAC
{
	
	SYSCTL->RCGCGPIO |= (1<<1) | (1<<4); // enable GPIOB, GPIOE
	while ( (SYSCTL->PRGPIO & ((1<<1) | (1<<4))) == 0 ) {}
	GPIOB_AHB->DIR |= PB4 | PB5;  // enable Tx, Fss, Clk be output
	GPIOE_AHB->DIR |= PE5;
	
	GPIOE_AHB->DIR &= ~(PE4); // enable Rx to be input
	GPIOB_AHB->AFSEL |= (PB4 | PB5); // set Tx, Fss, Rx and Clk to be alt function
	GPIOE_AHB->AFSEL |= (PE4 | PE5);
		
	GPIOB_AHB->PCTL |= (0xF << (4*4)) | (0xF << (5*4)); // Select alt function with PCTL
	GPIOE_AHB->PCTL |= (0xF << (4*4)) | (0xF << (5*4));
		
	GPIOB_AHB->DEN |= PB4 | PB5; // Enable all to be digital
	GPIOE_AHB->DEN |= PE4 | PE5;
	
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
	SSI2->CPSR = 10;              // prescale
	SSI2->CR0  = (5<<8) | (1<<7) | (1<<6) | 0x7; // SCR=5, SPO=1, SPH=1, 8-bit
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
	
	GPIOQ->DIR |= PQ3 | PQ1 | PQ0 ; // enable tx, fss, clk as output
	
	GPIOQ->PUR |= PQ0; // put PUR on clk
	
	GPIOQ->AFSEL |= PQ0 | PQ1 |  PQ3; // enable all as alt functions
	GPIOQ->PCTL |= (0xE << 0*4) | (0xE << 1*4) | (0xE << 3*4); // Select SSI in PCTL
		
	GPIOQ->DEN |= PQ0 | PQ1 | PQ3; // enable all as digital
		
	SYSCTL->RCGCSSI |= (1<<3); // enable SSI3 clock
	while ( (SYSCTL->PRSSI & (1<<3)) == 0) {} // wait
	
	SSI3->CR1 &= ~(SSE); // disable SSI3
	
	SSI3->CPSR = SSI3_CPSR ; 
	SSI3->CR0 = SSI3_SCR // SCR (Serial Clock Rate)
                        // Together with CPSDVSR (from SSI3->CPSR), this divides the system clock:
                        // fSSI = f_sysclk / (CPSDVSR * (1 + SCR))
                        // This sets the bit clock frequency for I2S/SPI.

            | (0 << 7) // SPO/SPH bits = 0b10
                        // Bit 7 = SPH = 1 ? data shifted/captured on second edge (adds 1-bit delay)
                        // Bit 6 = SPO = 0 ? clock idles low
                        // This combination matches I2S requirement (WS changes one bit before MSB).
						| (1 << 6)

            | (0xF);    // DSS (Data Size Select) = 0xF ? 16-bit data frames
                        // SSI will transmit/receive 16 bits per frame.
 
	SSI3->CR1 |= (1<<1); // enable SSI3 again
}

void SSI3_Handler(void) {
    if (SSI3->MIS & TXMIS) {
        while (SSI3->SR & TNF) {
            uint16_t sample = next_sample();
            SSI3->DR = sample << 4; // L
            SSI3->DR = sample << 4; // R
        }
    }
}

