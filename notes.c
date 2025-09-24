#include "notes.h"

const NoteEntry chromatic[] = {
    {"C2", 14046108}, {"C#2/Db2", 14854813}, {"D2", 15766007},
    {"D#2/Eb2", 16784281}, {"E2", 17696745}, {"F2", 18724013},
    {"F#2/Gb2", 19870467}, {"G2", 21045340}, {"G#2/Ab2", 22354170},
    {"A2", 23622320}, {"A#2/Bb2", 25093074}, {"B2", 26526492},
    {"C3", 28091831}, {"C#3/Db3", 29709626}, {"D3", 31568010},
    {"D#3/Eb3", 33568562}, {"E3", 35385951}, {"F3", 37479097},
    {"F#3/Gb3", 39740934}, {"G3", 42090680}, {"G#3/Ab3", 44708340},
    {"A3", 47244640}, {"A#3/Bb3", 50186148}, {"B3", 53052984},
    {"C4", 56229845}
};

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
