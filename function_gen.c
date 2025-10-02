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
#include "SSI.h" // for 
#include "LCD_Display.h"
#define DRAW_DECIMATE 64

volatile uint16_t audio_buf[AUDIO_BUF_SIZE];
volatile int buf_index = 0;

volatile waveform_t waveform_mode = WAVE_SQUARE;  // default to saw
volatile uint32_t phase_acc = 0; // holds current phase of the waveform
volatile uint32_t phase_step = 56229845; // controls how much phase_acc advances each sample (determines output frequency) (starts at middle C)
volatile uint16_t current_sample = 0; // current global sample
static int16_t x_pos = 0;
static int16_t prev_x = 0;
static int16_t prev_y = ILI9341_TFTHEIGHT / 2; // start mid-screen


#define WAVEFORM_CAPTURE_DOWNSAMPLE 64

static volatile uint16_t waveform_buffers[2][WAVE_BUF_LEN];
static volatile uint16_t waveform_write_index = 0;
static volatile uint8_t waveform_write_buffer = 0;
static volatile uint8_t waveform_ready_buffer = 0xFF;
static volatile bool waveform_ready_flag = false;

static inline void capture_waveform_sample(uint16_t sample)
{
    static uint16_t downsample_counter = 0;

    if (++downsample_counter < WAVEFORM_CAPTURE_DOWNSAMPLE) {
        return;
    }

    downsample_counter = 0;

    waveform_buffers[waveform_write_buffer][waveform_write_index++] = sample;

    if (waveform_write_index >= WAVE_BUF_LEN) {
        waveform_ready_buffer = waveform_write_buffer;
        waveform_write_buffer ^= 1;
        waveform_write_index = 0;
        waveform_ready_flag = true;
    }
}

bool function_gen_waveform_ready(void)
{
    return waveform_ready_flag;
}

size_t function_gen_copy_waveform(uint16_t *dest, size_t max_samples)
{
    if ((dest == NULL) || (max_samples < WAVE_BUF_LEN)) {
        return 0;
    }

    size_t copied = 0;

    __disable_irq();

    if (waveform_ready_flag && waveform_ready_buffer <= 1) {
        for (size_t i = 0; i < WAVE_BUF_LEN; ++i) {
            dest[i] = waveform_buffers[waveform_ready_buffer][i];
        }

        waveform_ready_flag = false;
        waveform_ready_buffer = 0xFF;
        copied = WAVE_BUF_LEN;
    }

    __enable_irq();

    return copied;
}


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

void init_timer0a() {
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
		// TIMER0->CTL = 1;       	// enable timer
}

uint16_t next_sample(void) {
    phase_acc += phase_step;
    uint16_t sample = 0;

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
            sample = (saw < 2048) ? (saw * 2) : ((4095 - saw) * 2);
            break;
        }
        case WAVE_SQUARE:
            sample = (phase_acc & 0x80000000) ? 4095 : 0;
            break;
    }
		
		current_sample = sample; // update global copy
    capture_waveform_sample(sample);

    return sample;   // align 12-bit to 16-bit (TEMP)
}


// Timer0A Handler function as defined in header file
// Calculates the next value of the waveform
void TIMER0A_Handler(void) {
	TIMER0->ICR = 1; // clear flag
	// draw();
}