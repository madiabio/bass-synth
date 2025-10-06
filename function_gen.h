#ifndef FUNCTION_GEN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "config.h"

#define FUNCTION_GEN_H
#define WAVE_BUF_LEN 240
#define PG1 (1<<1)
#define PK4 (1<<4)

#define PD7 (1<<7)



// Handles waveform mode
typedef enum {
    WAVE_SINE,
    WAVE_SAW,
    WAVE_TRI,
    WAVE_SQUARE
} waveform_t;

void handle_waveform_state(); // updates the state based off button presses


// Used for generating next sample
extern volatile waveform_t waveform_mode;
extern volatile uint32_t phase_acc;
extern volatile uint32_t phase_step;
extern volatile uint16_t current_sample;
extern volatile int current_channel; // 0 = Left, 1 = Right
uint16_t next_sample(void);

// Used for DMA to send sample to SSI3 for I2S DAC output
void fillPingBuffer(uint16_t *buffer, size_t frameCount);
void fillPongBuffer(uint16_t *buffer, size_t frameCount);

// Used for displaying mono waveform
extern uint16_t display_buffer[SCOPE_BUFFER_SIZE];
extern volatile size_t scope_write_index;
extern volatile int scope_ready;

void drawWaveform();

// Convert 32-bit phase accumulator to a table index
// tbl_size must be a power of two (e.g. 256, 1024, 4096)
// EXAMPLE: 
// the sine table is 256 points ( see waveform.h )
// only need 256 = 2^8, therefore only need 8 bits of the phase accumulator to walk thru the table.
// phase_acci s a 32 bit int, therefore, extract the top 8 bits by shifting right. 32 - 8 = 24.
#define PHASE_TO_INDEX(acc, tbl_size) ((uint32_t)((acc * (uint64_t)tbl_size) >> 32))


void init_PG1();  // waveform select LED0
void init_PK4(); // waveform select LED1
void init_button_PD7(); // waveform select button
void update_LEDs();
#endif