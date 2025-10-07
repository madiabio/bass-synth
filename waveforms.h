#ifndef WAVEFORMS_H
#define WAVEFORMS_H

#include <stdint.h>
#include "config.h"


#define CHROMATIC_LEN 25   // number of notes defined (might change)
#define SAMPLE_RATE 48000.0f
// #define BASE_FREQ 32.703f   // C1  (one octave below C2)

// #define BASE_FREQ 65.406f   // C2
// #define BASE_FREQ 261.63f   // C4
#define BASE_FREQ 523.251 // C5
#define TWO32 4294967296.0f // 2^32

typedef struct {
    const char *name;
    uint32_t step;
} NoteEntry;

// full chromatic table (C2–C4, 25 notes)
extern NoteEntry chromatic[CHROMATIC_LEN];

// scale definitions as semitone offsets
extern const int major_offsets[7];
extern const int minor_offsets[7];
extern const int pentatonic_offsets[5];
extern const int blues_offsets[6];

// does the math to initialise the chromatic note table
void init_chromatic_table(); 

extern uint16_t sine_table[TABLE_SIZE];
extern uint16_t tri_table[TABLE_SIZE];
extern uint16_t saw_table[TABLE_SIZE];
extern uint16_t sqr_table[TABLE_SIZE];

void init_wave_tables(void); // initialises the wave precomputed tables


#endif
