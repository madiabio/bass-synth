#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ES_Lib.h"
#include "function_gen.h"
#include "waveforms.h" // for waves and notes
#include "i2c.h" // for mcp4275_write()
#include "SSI.h" // for 
#include "LCD_Display.h"
#include "input.h"
#include "adc.h"

// for waveform state
volatile waveform_t waveform_mode = WAVE_SINE; 
volatile uint32_t phase_acc = 0; // holds current phase of the waveform
volatile uint32_t phase_step = 0; // controls how much phase_acc advances each sample (determines output frequency) (starts at middle C)
volatile uint16_t prev_sample = DAC_MID; // current global sample

// for waveform select button
#define STABLE_COUNT 5  // number of consecutive samples required (˜5ms if timer = 1ms)
uint8_t prev_button_edge  = 1;   // track previous button state
volatile uint8_t waveform_changed = 0;

volatile int current_channel = 0; // 0 = Left, 1 = Right

// for drawing
#define MIDLINE  (ILI9341_TFTHEIGHT / 2)   // 160 for a 320-pixel-tall display
#define SCALE  (ILI9341_TFTHEIGHT / 65536.0f)
static int16_t x_pos = 0;
static int16_t prev_x = 0;
static int16_t prev_y = ILI9341_TFTHEIGHT / 2; // start mid-screen

// for drawing buffer
volatile size_t scope_write_index = 0;
uint16_t display_buffer[SCOPE_BUFFER_SIZE];
volatile int scope_ready = 0;

// for envelope
#define ENV_SCALE     16       // envelope resolution bits (0–65535)
#define SR_DIV_1024   47       // ~48000 / 1024 ˜ 46.9, close enough
#define ENV_RELEASE_STEP 800   // release decrement per sample

typedef enum {
    ENV_IDLE,
    ENV_ATTACK,
    ENV_DECAY,
    ENV_SUSTAIN,
    ENV_RELEASE
} env_state_t;

typedef struct {
    env_state_t state;
    uint32_t value;         // 0–65535
    uint32_t attack_inc;
    uint32_t decay_dec;
    uint32_t sustain_level;
} envelope_t;

static envelope_t env;



// for waveform select LED MSB
void init_PG1()
{
	SYSCTL->RCGCGPIO |= (1<<6);
	while ( (SYSCTL->PRGPIO & (1<<6)) == 0 ) {} // wait for stable
	
	GPIOG_AHB->DIR |= PG1 ; // enable 
	GPIOG_AHB->DEN |= PG1 ; // enable DEN
	GPIOG_AHB->DATA &= ~PG1 ; // clear data reg
}

// for waveform select LED LSB
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

void handle_waveform_state(void) {
    static uint8_t stable_state = 1;   // 1 = released
    static uint8_t last_sample = 1;
    static uint8_t count = 0;

    uint8_t now = (GPIOD_AHB->DATA & PD7) ? 1 : 0;

    // wait until input is stable for N samples
    if (now == last_sample) {
        if (count < STABLE_COUNT) count++;
    } else {
        count = 0;
    }
    last_sample = now;

    // update only when stable and changed
    if (count >= STABLE_COUNT && stable_state != now) {
        stable_state = now;
        count = 0;

        // trigger only on falling edge (press)
        if (stable_state == 0) {
            waveform_mode = (waveform_t)((waveform_mode + 1) & 0x3);
            update_LEDs();
						waveform_changed = 1;

        }
    }
}

void handle_note_input(uint8_t note_index, bool reset_phase) {
    if (note_index < CHROMATIC_LEN) {
        phase_step = chromatic[note_index].step;
        if (reset_phase) {
            phase_acc = 0;
        }
    }
}


void envelope_updateParams(void)
{
    // approximate sample count: (ms * 48) using integer math (~48000/1000)
    uint32_t a_samples = (uint32_t)Attack_ms * 48U;
    uint32_t d_samples = (uint32_t)Decay_ms * 48U;

    if (a_samples == 0) a_samples = 1;
    if (d_samples == 0) d_samples = 1;

    env.attack_inc = 65535U / a_samples;
    env.decay_dec  = (65535U - (Sustain_lv * 65U)) / d_samples;
    env.sustain_level = Sustain_lv * 65U;
}

static inline uint16_t envelope_next(void)
{
    if (note_on && env.state == ENV_IDLE)
        env.state = ENV_ATTACK;
    else if (!note_on && env.state != ENV_IDLE && env.state != ENV_RELEASE)
        env.state = ENV_RELEASE;

    switch (env.state) {
        case ENV_IDLE:
            break;

        case ENV_ATTACK:
            env.value += env.attack_inc;
            if (env.value >= 65535U) {
                env.value = 65535U;
                env.state = ENV_DECAY;
            }
            break;

        case ENV_DECAY:
            if (env.value > env.sustain_level) {
                env.value -= env.decay_dec;
                if (env.value <= env.sustain_level) {
                    env.value = env.sustain_level;
                    env.state = ENV_SUSTAIN;
                }
            }
            break;

        case ENV_SUSTAIN:
            env.value = env.sustain_level;
            break;

        case ENV_RELEASE:
            if (env.value > ENV_RELEASE_STEP)
                env.value -= ENV_RELEASE_STEP;
            else {
                env.value = 0;
                env.state = ENV_IDLE;
            }
            break;
    }
    return (uint16_t)env.value;
}



uint16_t next_sample(void) {
    uint16_t sample;
		uint16_t idx = PHASE_TO_INDEX(phase_acc, TABLE_SIZE);

		if (current_channel == 0) // LEFT CHANNEL
		{
			
			switch (waveform_mode) {
					case WAVE_SINE:   sample = sine_table[idx]; break;
					case WAVE_SAW:    sample = saw_table[idx];  break;
					case WAVE_TRI:    sample = tri_table[idx];  break;
					case WAVE_SQUARE: sample = sqr_table[idx];  break;
			}
			// === scale and recenter before sending to DAC ===

			uint16_t env_amp = envelope_next();
			int32_t s = ((int32_t)(sample - 32768) * env_amp) >> 16;
			sample = (uint16_t)(s + 32768);
			prev_sample = sample; // update global copy	
		}
		else // RIGHT CHANNEL
		{
			// advance the master phase accumulator (once per stereo frame)
			if (note_on) phase_acc += phase_step; // only advance when note is active
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
		sample = next_sample(); // next_sample updates current_channel
    
		current_channel ^= 1; 	// switch channel
		buffer[i] = sample; 	// push sample into buffer
		
		/*
		if (current_channel == 0) // if left channel, push sample to display buffer, update scope idx.
		{
			display_buffer[scope_write_index++] = sample; 
			if (scope_write_index >= SCOPE_BUFFER_SIZE)
			{
				scope_write_index = 0;
			}
			if (scope_write_index == 0)
					scope_ready = 1;
			}
		*/
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
