// Workshop 2 Template
// Remember to start and configure TeraTerm
#include <TM4C129.h>
#include <stdint.h>

#include "ES.h"
#include "function_gen.h" // for timer0a_init(), waveform_mode
#include "input.h" // for handle_note_input(), scan_keypad(), init_UART3()
#include "waveforms.h" // for init_sine_table()

#include "LCD_Display.h"
#include "SSI.h" // for initSPI(), init_SSI0()
#include "dma.h"

#include "adc.h"
#include "display_utils.h"

void config_I2S_circuit()
{
	init_wave_tables();
	init_dma();
	init_SSI3();
}

void config_button()
{
    init_button_PD7();
    init_PG1();
		init_PK4();
}

// ************* main function ***********************
int main(void)
{	
	ES_setSystemClk(120);

	__enable_irq();
	
	// ADC
	init_adc();

	// Display
	initSPI();
	initLCD();
	setRotation(2);
	clearScreen();
	drawWaveformSample();
	
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
	config_button();
	init_timer0a();
	static uint8_t skip = 0;
	
	ES_Uprintf(0, "sysclk: %d\n", ES_getSystemClk());
	
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