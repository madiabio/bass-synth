// Generates the sawtooth waveform necessary
#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ES_Lib.h"
#include "function_gen.h"
#include "config.h" 
#include "waveforms.h" // for sine wave
#include "notes.h" // for notes dict
#include "i2c.h" // for mcp4275_write()

volatile waveform_t waveform_mode = WAVE_SINE;  // default to saw
volatile uint32_t phase_acc = 0; // holds current phase of the waveform
volatile uint32_t phase_step = 0; // controls how much phase_acc advances each sample (determines output frequency)

// for testing
void init_PG1()
{
	SYSCTL->RCGCGPIO |= (1<<6);
	while ( (SYSCTL->PRGPIO & (1<<6)) == 0 ) {} // wait for stable
	
	GPIOG_AHB->DIR |= (1<<1); // enable 
	GPIOG_AHB->DEN |= (1<<1); // enable DEN
	GPIOG_AHB->DATA &= ~(1<<1); // clear data reg
}

void timer0a_init() {
    SYSCTL->RCGCTIMER |= (1<<0); // enable timer 0 clock
		while((SYSCTL->PRTIMER & (1 << 0)) == 0) {}// Wait until ready

    TIMER0->CTL &= ~(1<<0); // disable during setup
    TIMER0->CFG = 0; // 32-bit timer
    
		TIMER0->TAMR = 0x2; // periodic down counter mode
    TIMER0->TAILR = TIMER0A_RELOAD; // load value (defined in config.h)
    TIMER0->IMR |= (1<<0); // enable timeout interrupt
    
		// enable IRQ 19 to enable interrupts on NVIC for TIMER0 (see NVIC Register Descriptions in datasheet and TIMER0 Handler in regref/course notes)
		NVIC->IPR[19] = PRIORITY_TIMER0A; // set priority as defined in config header
		NVIC->ISER[0] |= (1<<19); // this is the same thing as NVIC_ENn in the data sheet
		TIMER0->CTL = 1;       	// enable timer
}


// Timer0A Handler function as defined in header file
// Calculates the next value of the waveform
void TIMER0A_Handler(void) {
    TIMER0->ICR = (1<<0); // clear flag (set 1 to TATOCINT)
		GPIOG_AHB->DATA ^= (1<<1);  // toggle PG1  
		phase_acc += phase_step; // advance phase
		uint16_t sample = 0;     // placeholder
		
    switch (waveform_mode) {
			case WAVE_SINE: { 
				uint16_t idx = PHASE_TO_INDEX(phase_acc, TABLE_SIZE);  // e.g. 256 >> 24
				sample = sine_table[idx]; // and then index the sine wave table with that, thats the sample.
				break;
			}
			case WAVE_SAW:
				sample = phase_acc >> 20; // this gives you between 0 and 4095, ideal for the 12 bit DAC.
				break;
			case WAVE_TRI: {
				uint16_t saw = phase_acc >> 20;
				sample = (saw < 2048) ? (saw * 2) : ((4095 - saw) * 2);
				break;
			}
			case WAVE_SQUARE:
				// 0x80000000 checks the MSB of the phase accumulator. 
				// if MSB changes to 1, then we're halfway thru the phase, so change state.
				sample = (phase_acc & 0x80000000) ? 4095 : 0;
				break;
    }
		
		mcp4275_write(sample); // send sample to mcp4275
}
