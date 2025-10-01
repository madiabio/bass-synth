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
	__enable_irq(); // enable interrupts on CPU lvl
	//init_sine_table(); // lookup table for sine wave
	// keypad_init(); // enable the GPIO pins required for the keypad

	
	init_i2c_0(); //
	//mcp4725_test_connection();
	// init_PG1(); // for testing
	// timer0a_init(); // init timer0a for the function generator
	
	
	while(true)
	{
		// Write midscale value ~1.65V
    mcp4725_write(0x0FFF);
		msDelay(30);
		mcp4725_write(0x0000);
		msDelay(30);
	}
	
		
	return 0;
}
