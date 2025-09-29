// Pre-calculates waveforms for function generator
#include "waveforms.h"
#include <math.h>
#ifndef PI
#define PI 3.14159265358979323846
#endif


uint16_t sine_table[TABLE_SIZE];


void init_sine_table(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        float angle = (2.0f * PI * i) / TABLE_SIZE;
        float val   = (sinf(angle) * 0.5f) + 0.5f;
        sine_table[i] = (uint16_t)(val * DAC_MAX);
    }
}

