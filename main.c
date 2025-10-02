// Workshop 2 Template
// Remember to start and configure TeraTerm
#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ES.h"

#include "function_gen.h" // for timer0a_init(), waveform_mode
#include "input.h" // for handle_note_input(), scan_keypad()
#include "waveforms.h" // for init_sine_table()

#include "LCD_Display.h"
#include "SSI.h" // for initSPI(), init_SSI0()


// ************* main function ***********************
int main(void)
{
	ES_setSystemClk(120000000); // set sysclk to 120MHz
	// UART0_Init();
	ES_Serial(0,"19200,8,N,1") ;
	ES_Uprintf(0, "\n****************************************\n") ;
	ES_Uprintf(0, "\n Uart 0 : 19200 baud, 8bits, 1 stop bit \n") ;
	ES_Uprintf(0, "\n****************************************\n") ;
	__enable_irq(); // enable interrupts on CPU lvl
	//init_sine_table(); // lookup table for sine wave
	// keypad_init(); // enable the GPIO pins required for the keypad
	init_PG1(); // for testing
	init_PK4();
	initSPI() ;	// These contain the SPI functions
	initLCD() ; // This function turns on and initialises the LCD
	setRotation(2);

	clearScreen();
	setCharConfig(ILI9341_DARKGREEN,1,1,ILI9341_BLACK,1);
	drawString("Let's test the LCD",18);
	setCharConfig(ILI9341_YELLOW,1,1,ILI9341_BLUE,1);
	drawString("\r\nWelcome\r\nEveryone!",19);

	drawRect(100,100,50,50,ILI9341_CYAN);

	// init_SSI3(); // for I2S DAC
	//init_timer0a(); // for function generator
	while(true)
	{
		
	}
	
	// init_PG1(); // for testing
	
	
		
	return 0;
}
