// Workshop 2 Template
// Remember to start and configure TeraTerm

#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ES_Lib.h"

#include "function_gen.h" // for timer0a_init(), waveform_mode
#include "input.h" // for handle_note_input()


// ************* main function ***********************
int main(void)
{
	
	ES_Serial(0,"19200,8,N,1") ;
	ES_printf("\n****************************************\n") ;
	ES_printf("\n Uart 0 : 19200 baud, 8bits, 1 stop bit \n") ;

	ES_printf("\n****************************************\n") ;

	
	timer0a_init(); // init timer0a for the function generator
	__enable_irq();

	while (1) {
			// uint8_t key = get_keypad_input(); // your own keypad/uart function
			handle_note_input(0, true); // hard coded for testing
	}
	while(true) {}
	return 0;
}
