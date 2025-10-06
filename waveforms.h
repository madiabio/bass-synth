#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <stdint.h>
#include "config.h"

extern uint16_t sine_table[TABLE_SIZE];

void init_sine_table(void); // initialises the sine wave precomputed table

// saw & triangle waves are procedurally calced in the ISR.


#endif
