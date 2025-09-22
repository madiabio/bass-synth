// Generates the sawtooth waveform necessary
#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ES_Lib.h"

extern int frequency;

void timer0a_init(uint32_t fs_hz) {
    SYSCTL->RCGCTIMER |= (1<<0);        // enable timer 0
		while((SYSCTL->PRTIMER & (1 << 0)) == 0); // Wait until ready

    TIMER0->CTL &= ~(1<<0);      						// disable during setup
    TIMER0->CFG = 0;                     		// 32-bit timer
    uint32_t reload = (80000000/fs_hz) - 1;
    
		TIMER0->TAMR = 0x2; 										// periodic down counter mode
    TIMER0-TAILR = reload;              		// load value
    TIMER0_IMR_R = TIMER_IMR_TATOIM;      	// enable timeout interrupt
    NVIC_EN0_R |= (1U<<19);               	// enable IRQ 19
    TIMER0_CTL_R |= TIMER_CTL_TAEN;       	// enable timer
}


void Timer0A_Handler(void) {
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // clear flag
    phase_acc += phase_step;              // advance phase
    uint16_t sample = phase_acc >> 20;    // top 12 bits = 0..4095 sawtooth
    dac_write12(sample);                  // send to DAC
}
