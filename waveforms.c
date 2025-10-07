// Pre-calculates waveforms for function generator
#include "waveforms.h"
#include <math.h>
#ifndef PI
#define PI 3.14159265358979323846
#endif


uint16_t sine_table[TABLE_SIZE];
uint16_t tri_table[TABLE_SIZE];
uint16_t saw_table[TABLE_SIZE];
uint16_t sqr_table[TABLE_SIZE];


void init_wave_tables(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float t = (float)i / TABLE_SIZE;
        // sine
        float angle = (2.0f * PI * i) / TABLE_SIZE;
        float val   = (sinf(angle) * 0.5f) + 0.5f;
        sine_table[i] = (uint16_t)(val * DAC_MAX);
        // sawtooth
        saw_table[i] = (uint16_t)(t * DAC_MAX);
        // triangle
        tri_table[i] = (uint16_t)((t < 0.5f ? t * 2.0f : (1.0f - t) * 2.0f) * DAC_MAX);
        // square
				sqr_table[i] = (i < TABLE_SIZE / 2)
						? (DAC_MID + DAC_MID / 2)
						: (DAC_MID - DAC_MID / 2);
    }
}

NoteEntry chromatic[CHROMATIC_LEN];
const char *noteNames[] = {
    "C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2",
    "C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3"
};

void init_chromatic_table(void) {
    for (int i = 0; i < CHROMATIC_LEN; i++) {
        float freq = BASE_FREQ * powf(2.0f, i / 12.0f);
        chromatic[i].name = noteNames[i];
        chromatic[i].step = (uint32_t)((freq * TWO32) / SAMPLE_RATE);
    }
}

// intervals in semitones from root
const int major_offsets[7]     = {0, 2, 4, 5, 7, 9, 11};
const int minor_offsets[7]     = {0, 2,	 3, 5, 7, 8, 10};
const int pentatonic_offsets[5]= {0, 2, 4, 7, 9};
const int blues_offsets[6]     = {0, 3, 5, 6, 7, 10};

void build_scale(uint8_t root_index,
                 const int *offsets,
                 int len,
                 const NoteEntry *chrom,
                 NoteEntry *out) {
    for (int i = 0; i < len; i++) {
        out[i] = chrom[root_index + offsets[i]];
    }
}
