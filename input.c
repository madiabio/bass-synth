// Handles inputs
#include "notes.h"
#include "function_gen.h"
#include "input.h"

void handle_note_input(uint8_t note_index, bool reset_phase) {
    if (note_index < CHROMATIC_LEN) {
        phase_step = chromatic[note_index].step; // get 32 bit phase increment for that note
        if (reset_phase) {
            phase_acc = 0;
        }
    }
}
