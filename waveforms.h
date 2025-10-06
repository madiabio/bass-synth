#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <stdint.h>
#include "config.h"

extern uint16_t sine_table[TABLE_SIZE];
extern uint16_t tri_table[TABLE_SIZE];
extern uint16_t saw_table[TABLE_SIZE];
extern uint16_t sqr_table[TABLE_SIZE];

void init_wave_tables(void); // initialises the wave precomputed tables


#endif
