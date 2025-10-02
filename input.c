// Handles inputs
#include "notes.h"
#include "function_gen.h"
#include "input.h"
#include "ES.h"
#include <TM4C129.h>
#include <stdint.h>

void handle_note_input(uint8_t note_index, bool reset_phase) {
    if (note_index < CHROMATIC_LEN) {
        phase_step = chromatic[note_index].step; // get 32 bit phase increment for that note
        if (reset_phase) {
            phase_acc = 0;
        }
    }
}

void keypad_init(void)
{
	uint32_t GPIOK_and_GPIOE = ((1<<9) | (1<<4));
	SYSCTL->RCGCGPIO |= GPIOK_and_GPIOE;
	while ( (SYSCTL->PRGPIO & (GPIOK_and_GPIOE)) == 0 ) {}
	
	uint32_t PKs = (1<<1) | (1<<0) | (1<<2);
	uint32_t PEs = (1<<0) | (1<<3) | (1<<2) | (1<<1);
		
	// Set as inputs
	GPIOK->DIR |= PKs;
	GPIOE_AHB->DIR &= ~PEs;
		
	// enable pull down resistor for switches
	GPIOK->DR2R |= PKs; // enable pins output to drive 2mA
	GPIOE_AHB->PDR |= PEs; // Enable pull up resistor
	
	// enable digital
	GPIOK->DEN  |= PKs;
	GPIOE_AHB->DEN |= PEs;
}


void scan_keypad()
{
	char last_raw = 0;
	char stable_key =  0;
	uint8_t count = 0;
	
	const char keyMap[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
	};

	while(true)
	{
		
		for (int col = 0; col < 3; col++) {
			// set all cols low and then set current col high.
			GPIOK->DATA &= ~((1 << 0) | (1 << 1) | (1 << 2));
			GPIOK->DATA |= (1 << col);
			uint32_t rows = GPIOE_AHB->DATA & 0x0F; // PE0 PE3


			// iterate thru each input and check which bit has been set
			// then reference that to the key map to see which key that
			// corresponds to.
			for (int row = 0; row < 4; row++) {
				if ((rows >> row) & 0x1) {
					char key = keyMap[row][col];
					ES_Uprintf(0, "Key Pressed: %c\n", key);
				}	
			}
		}
		ES_msDelay(30);
	}
}
