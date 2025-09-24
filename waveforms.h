#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <stdint.h>

#define TABLE_SIZE 256
#define DAC_MAX    4095

extern uint16_t sine_table[TABLE_SIZE];

void init_sine_table(void); // initialises the sine wave precomputed table

// saw & triangle waves are procedurally calced in the ISR.

#endif
