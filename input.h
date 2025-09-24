#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

// handle a new note index (from keypad, UART, etc.)
// reset phase will make the waveform restart cleanly @ the beginning of its cycle every time you press a new note
void handle_note_input(uint8_t note_index, bool reset_phase);

#endif
