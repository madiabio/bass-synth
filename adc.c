#include <TM4C129.h>
#include "ES_Lib.h"
#define PD0 (1<<0)
#define PD1 (1<<1)
#define PD2 (1<<2)
#define SS1 (1<<1)
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
