// Pre-calculates waveforms for function generator
#include "waveforms.h"
#ifndef PI
#define PI 3.14159265358979323846
#endif

#include <math.h>
uint16_t sine_table[TABLE_SIZE];
uint16_t tri_table[TABLE_SIZE];
uint16_t saw_table[TABLE_SIZE];
uint16_t sqr_table[TABLE_SIZE];


void init_wave_tables(void)
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        float angle = (2.0f * PI * i) / TABLE_SIZE;
        float val   = (sinf(angle) * 0.5f) + 0.5f;
        sine_table[i] = (uint16_t)(val * 65535.0f);

        // sawtooth
        saw_table[i] = (uint16_t)(((uint32_t)i * 65535U) / TABLE_SIZE);

        // triangle
        if (i < TABLE_SIZE / 2)
            tri_table[i] = (uint16_t)(((uint32_t)i * 131070U) / TABLE_SIZE);
        else
            tri_table[i] = (uint16_t)(((uint32_t)(TABLE_SIZE - i) * 131070U) / TABLE_SIZE);

        // square
        sqr_table[i] = (i < TABLE_SIZE / 2) ? 49152 : 16384;
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
        chromatic[i].step = (uint32_t)((freq * TWO32) / SAMPLE_FREQ);
    }
}
