// Generates the sawtooth waveform necessary
#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ES_Lib.h"
#include "function_gen.h"
#include "waveforms.h" // for sine wave
#include "notes.h" // for notes dict
#include "i2c.h" // for mcp4275_write()
#include "SSI.h" // for 
#include "LCD_Display.h"
#include "input.h"

volatile waveform_t waveform_mode = WAVE_SAW;  // default to saw
volatile uint32_t phase_acc = 0; // holds current phase of the waveform
volatile uint32_t phase_step = 56229845; // controls how much phase_acc advances each sample (determines output frequency) (starts at middle C)
volatile uint16_t prev_sample = DAC_MID; // current global sample
static int16_t x_pos = 0;
static int16_t prev_x = 0;
static int16_t prev_y = ILI9341_TFTHEIGHT / 2; // start mid-screen

volatile int current_channel = 0; // 0 = Left, 1 = Right
volatile size_t scope_write_index = 0;
uint16_t display_buffer[SCOPE_BUFFER_SIZE];

// for testing
void init_PG1()
{
	SYSCTL->RCGCGPIO |= (1<<6);
	while ( (SYSCTL->PRGPIO & (1<<6)) == 0 ) {} // wait for stable
	
	GPIOG_AHB->DIR |= PG1 ; // enable 
	GPIOG_AHB->DEN |= PG1 ; // enable DEN
	GPIOG_AHB->DATA &= ~PG1 ; // clear data reg
}

// for testing
void init_PK4()
{
	SYSCTL->RCGCGPIO |= (1<<9);
	while ( (SYSCTL->PRGPIO & (1<<9)) == 0 ) {}
	
	GPIOK->DEN |= PK4;
	GPIOK->DIR |= PK4;
	GPIOK->DATA &= ~PK4;
}

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

void update_LEDs() {
    uint32_t bits = 0;

    switch(waveform_mode) {
        case WAVE_SINE:    bits = 0b00; break;
        case WAVE_SAW:     bits = 0b01; break;
        case WAVE_TRI:     bits = 0b10; break;
        case WAVE_SQUARE:  bits = 0b11; break;
    }

    // Clear both outputs first
    GPIOG_AHB->DATA &= ~PG1;
    GPIOK->DATA     &= ~PK4;

    // Then set according to bits
    if(bits & 0b10) GPIOG_AHB->DATA |= PG1; // MSB
    if(bits & 0b01) GPIOK->DATA     |= PK4; // LSB
}


uint16_t next_sample(void) {
    uint16_t sample;
		if (current_channel == 0) // LEFT CHANNEL
		{
			
			switch (waveform_mode) {
					case WAVE_SINE: {
							uint16_t idx = PHASE_TO_INDEX(phase_acc, TABLE_SIZE);
							sample = sine_table[idx];
							break;
					}
					case WAVE_SAW:
							sample = phase_acc >> 20;
							break;
					case WAVE_TRI: {
							uint16_t saw = phase_acc >> 20;
							sample = (saw < 2048) ? (saw * 2) : ((DAC_MAX - saw) * 2);
							break;
					}
					case WAVE_SQUARE:
							sample = (phase_acc & 0x80000000) ? DAC_MAX : 0;
							break;
			}
			sample = note_on ? sample : DAC_MID;
			prev_sample = sample; // update global copy	
		}
		else // RIGHT CHANNEL
		{
			phase_acc += phase_step; // advance the master phase accumulator (once per stereo frame)
			sample = prev_sample; 	 // for now just want to have mono.
		}
		
		return sample;
}



void fillBuffer(uint16_t *buffer, size_t frameCount)
{
	// Iterate through the entire buffer size
	for (size_t i = 0; i < frameCount; i++)
	{
    uint16_t sample;
		if (note_on) sample = next_sample(); // next_sample updates current_channel
    else sample = DAC_MID;
		
		current_channel ^= 1; 	// switch channel
		buffer[i] = sample; 	// push sample into buffer
		
		if (current_channel == 0) // if left channel, push sample to display buffer, update scope idx.
		{
			display_buffer[scope_write_index++] = sample; 
			scope_write_index++;
			if (scope_write_index >= SCOPE_BUFFER_SIZE)
			{
				scope_write_index = 0;
			}
		}
		
	
	}
}

// wrappers, unnecessary, remove later.
void fillPingBuffer(uint16_t *buffer, size_t frameCount)
{
	fillBuffer(buffer, frameCount);
}

void fillPongBuffer(uint16_t *buffer, size_t frameCount)
{
	fillBuffer(buffer, frameCount);
}
