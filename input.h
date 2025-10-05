#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

#define PKs (1<<1) | (1<<0) | (1<<2)	 // cols 
#define PEs (1<<0) | (1<<3) | (1<<2) | (1<<1) // ROWS

extern volatile uint8_t note_on;      // gate flag
extern volatile char key_pressed;      // tells which key is being pressed, -1 if none.
extern volatile uint32_t row_event_flags;	// Tells if an event has occurred in a row

void init_UART0();
// inits UART3
void init_UART3();


// Initalises the GPIO pins required for the keypad
void keypad_init(void);

// Polls the keypad for inputs
uint32_t get_keypad_input(void);



// handle a new note index (from keypad, UART, etc.)
// reset phase will make the waveform restart cleanly @ the beginning of its cycle every time you press a new note
void handle_note_input(uint8_t note_index, bool reset_phase);

// displays what key is presed
void display_keypad();

// scans the keypad every 30ms
void scan_keypad();

// void init_timer0a(void);
// void TIMER0A_Handler(void);

#endif
