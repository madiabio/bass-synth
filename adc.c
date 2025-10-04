#include <TM4C129.h>
#include "ES_Lib.h"
#include "adc.h"
#define PD0 (1<<0)
#define PD1 (1<<1)
#define PD2 (1<<2)
#define SS1 (1<<1)
#define SS3 (1<<3)
#include <stdint.h>
/*
void init_adc()
{
	SYSCTL->RCGCGPIO |= (1<<3); // GPIO D
	while ( (SYSCTL->PRGPIO & (1<<3)) == 0 ) {} // wait
	
	GPIOD_AHB->AMSEL |= PD0 | PD1 | PD2; // Want these pins as ADCs for the 3 pots, so set to analogue
	GPIOD_AHB->DIR &= ~(PD0 | PD1 | PD2); // Enable as input
	GPIOD_AHB->AFSEL |= (PD0 | PD1 | PD2); // Enable alt function
	GPIOB_AHB->DEN &= ~(PD0 | PD1 | PD2); // Disable digital
	
	SYSCTL->RCGCADC |= (1<<0); // enable ADC 0
	ADC0->PC = 0x0; // only need 125ksps for human hand movement. keep it low to keep CPU burden low.
	ADC0->ACTSS &= ~SS1; // disable sequencer 1 (4 steps)
	
	ADC0->SSPRI |= (0x30); // set to lowest priority
	
	ADC0->EMUX &= ~(0xF<<4); // set mode to processor trigger FOR NOW, change later.
	
	
		
	ADC0->SSMUX0 = (0b1111 << 0) | (0b1110 << 4) | (0b1101 << 8); // AIN15, AIN14, AIN13	
}


void init_adc()
{
	SYSCTL->RCGCGPIO |= (1<<3); // GPIO D
	while ( (SYSCTL->PRGPIO & (1<<3)) == 0 ) {} // wait
	
	GPIOD_AHB->AMSEL |= PD7; // Want these pins as ADCs for the 3 pots, so set to analogue
	GPIOD_AHB->DIR &= ~PD7; // Enable as input
	GPIOD_AHB->AFSEL |= PD7; // Enable alt function
	GPIOB_AHB->DEN &= ~PD7; // Disable digital
	
	SYSCTL->RCGCADC |= (1<<0); // enable ADC 0
	ADC0->PC = 0x0; // only need 125ksps for human hand movement. keep it low to keep CPU burden low.
	ADC0->ACTSS &= ~SS1; // disable sequencer 1 (4 steps)
	
	ADC0->SSPRI |= (0x30); // set to lowest priority
	
	ADC0->EMUX &= ~(0xF<<4); // set mode to processor trigger FOR NOW, change later.
	
	ADC0->SSMUX0 = (0b0100 << 12); // AIN4
	
		
}
*/

void init_button_PD7()
{
	// 1. GPIO Clock Enable and Wait (Port D, Bit 3)
	SYSCTL->RCGCGPIO |= (1<<3); // Enable clock for GPIO D
	while ( (SYSCTL->PRGPIO & (1<<3)) == 0 ) {} // Wait for Port D to stabilize
	
	// --- COMMIT CONTROL: REQUIRED FOR PD7 ---
	// PD7 is locked. Must unlock GPIOLOCK and set GPIOCR bit 7 before configuration.
		
	// 1.1 Unlock GPIOLOCK register (Magic value 0x4C4F.434B)
	GPIOD_AHB->LOCK = 0x4C4F434B; 

	// 1.2 Enable commit control for PD7 (Bit 7) in GPIOCR
	*((volatile uint32_t *)(&GPIOD_AHB->CR)) |= (1<<7);  // Get the address of CR, cast it to a pointer to a volatile unsigned integer, and dereference it.

	GPIOD_AHB->DIR  &= ~PD7;   // input
	GPIOD_AHB->DEN  |= PD7;    // digital enable
	GPIOD_AHB->PUR  |= PD7;    // pull-up

	GPIOD_AHB->LOCK = 0;  // re-lock GPIO port
}

