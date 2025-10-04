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


void test_I2S_circuit()
{
	init_sine_table();
	init_SSI3();
	while(true) 
	{
		while ((SSI3->SR & TNF) == 0){
		}
		SSI3->DR = next_sample();
		while (SSI3->SR & (1 << 4)) {
    // wait until BSY = 0
		}

	}  // loop forever
}

void test_SSI1()
{
	init_SSI1();
	while(true) 
	{
		while ((SSI1->SR & TNF) == 0){
		}
		SSI1->DR = 0x55AA;
		while (SSI1->SR & (1 << 4)) {
    // wait until BSY = 0
		}

	}  // loop forever

}

void test_display()
{
	initSPI();
	initLCD();
	setRotation(2);

	clearScreen();
	setCharConfig(ILI9341_DARKGREEN,1,1,ILI9341_BLACK,1);
	drawString("Let's test the LCD",18);
	setCharConfig(ILI9341_YELLOW,1,1,ILI9341_BLUE,1);
	drawString("\r\nWelcome\r\nEveryone!",19);

	drawRect(100,100,50,50,ILI9341_CYAN);
	
	while(true) 
	{
		
	}  // loop forever
}
// ************* main function ***********************
int main(void)
{
	test_I2S_circuit();
	return 0;
}
