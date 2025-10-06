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
