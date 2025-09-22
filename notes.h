#ifndef NOTES_H
#define NOTES_H

#include <stdint.h>

typedef struct {
    const char *name;
    uint32_t step;
} NoteEntry;

// full chromatic table (C2–C4, 25 notes)
extern const NoteEntry chromatic[];

// scale definitions as semitone offsets
extern const int major_offsets[7];
extern const int minor_offsets[7];
extern const int pentatonic_offsets[5];
extern const int blues_offsets[6];

// helper to build a scale in code
void build_scale(uint8_t root_index,
                 const int *offsets,
                 int len,
                 const NoteEntry *chrom,
                 NoteEntry *out);

#endif
