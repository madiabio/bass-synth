// Workshop 2 Template
// Remember to start and configure TeraTerm

#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ES_Lib.h"

#include "function_gen.h" // for timer0a_init(), waveform_mode
#include "input.h" // for handle_note_input(), scan_keypad()
#include "waveforms.h" // for init_sine_table()
#include "i2c.h" // for init_i2c_0()

// ************* main function ***********************
int main(void)
{
	
	ES_Serial(0,"19200,8,N,1") ;
	ES_printf("\n****************************************\n") ;
	ES_printf("\n Uart 0 : 19200 baud, 8bits, 1 stop bit \n") ;

	ES_printf("\n****************************************\n") ;

	init_sine_table(); // lookup table for sine wave
	keypad_init(); // enable the GPIO pins required for the keypad
	
	init_i2c_0(); //
	
	init_PG1(); // for testing
	timer0a_init(); // init timer0a for the function generator
	__enable_irq(); // enable interrupts on CPU lvl
	
	
	
	
	while (true) {
			// uint8_t key = get_keypad_input(); // your own keypad/uart function
			handle_note_input(0, true); // hard coded for testing
			scan_keypad();
	}
	
	
	return 0;
}
