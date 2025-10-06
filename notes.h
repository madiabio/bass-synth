#ifndef NOTES_H
#define NOTES_H

#include <stdint.h>

#define CHROMATIC_LEN 25   // number of notes defined (might change)
#define SAMPLE_RATE 48000.0f
#define BASE_FREQ 65.406f   // C2
// #define BASE_FREQ 261.63f   // C4
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

// helper to build a scale in code
// might need to extend notes to deal with bounds idk. come back to this (like B3 might cause OOB for chromatic[])
void build_scale(uint8_t root_index,
                 const int *offsets,
                 int len,
                 const NoteEntry *chrom,
                 NoteEntry *out);

#endif
