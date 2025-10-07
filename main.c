// Workshop 2 Template
// Remember to start and configure TeraTerm
#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ES.h"
#include "function_gen.h" // for timer0a_init(), waveform_mode
#include "input.h" // for handle_note_input(), scan_keypad(), init_UART3()
#include "waveforms.h" // for init_sine_table()

#include "LCD_Display.h"
#include "SSI.h" // for initSPI(), init_SSI0()
#include "dma.h"

#include "adc.h"
#include "display_utils.h"

#include <math.h>
void config_I2S_circuit()
{
	init_wave_tables();
	init_dma();
	init_SSI3();
}

void test_button()
{
    init_button_PD7();
    init_PG1();
		init_PK4();
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

void test_keypad_with_I2S()
{
	keypad_init();
	// init_timer0a();
	// test_button();
	config_I2S_circuit();
	while(true) 
	{
		scan_keypad();
	}  // loop forever

}

// ************* main function ***********************
int main(void)
{	
	
	__enable_irq();
	ES_setSystemClk(120000000);
	
	// ADC
	init_adc();

	// Display
	
	initSPI();
	initLCD();
	setRotation(2);
	clearScreen();
	
	// Printing to terminal
	init_UART0();
	ES_Serial(0, "115200,8,N,1");     // matches the UART config
	ES_Uprintf(0, "\n=========\nBR = 115200, 8 bit wlen, no parity, 1 stop bit, \n==========\n");
	
	// Synth notes
	init_wave_tables(); // lookup table for waves
	init_chromatic_table(); // calculate the chromatic notes
	
	// DAC
	config_I2S_circuit();

	// Input
	keypad_init();
	test_button();
	init_timer0a();
	static uint8_t skip = 0;
	
	
	int prev = 0;
	while(true) 
	{
		displayADCValues();        

		if (waveform_changed)
		{
			drawWaveformSample();
			waveform_changed = 0;
		}
	}	
	return 0;
}