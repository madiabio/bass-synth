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

#include "notes.h"
#include "adc.h"

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



void showWaveformName(void)
{
    clearScreen();
    const uint16_t textColor = 0xFFFF;
    const uint16_t bgColor   = 0x0000;

    // screen constants
    const int16_t screenW = 240;
    const int16_t screenH = 320;

    // bottom band 40 px tall, placed about 20 px above very bottom
    int16_t bandY = screenH - 60;   // start of band (60 px from top edge)
    int16_t bandH = 40;

    // clear that band
    fillRect(0, bandY, screenW, bandH, bgColor);

    // text position (centered roughly)
    setCharConfig(textColor, 2, 1, bgColor, 1);

    switch (waveform_mode) {

        case WAVE_SINE:
            for (int x = 20; x < 220; x += 2) {             // more samples (denser)
                int16_t y = bandY + 25 - (int16_t)(15 * sinf((x - 20) * 0.12f));
                fillRect(x, y, 2, 2, textColor);
            }
            break;

        case WAVE_SAW:
            for (int x = 40; x < 200; x += 8) {
                int y = bandY + 30 - ((x - 40) / 8);
                fillRect(x, y, 2, 2, 0x07E0);
            }
            break;

        case WAVE_TRI:
            for (int x = 40; x < 120; x++)
                fillRect(x, bandY + 30 - (x - 40) / 4, 1, 1, 0x001F);
            for (int x = 120; x < 200; x++)
                fillRect(x, bandY + 10 + (x - 120) / 4, 1, 1, 0x001F);
            break;

        case WAVE_SQUARE:
            fillRect(70,  bandY + 12, 60, 2, 0xF800);  // top
            fillRect(70,  bandY + 12, 2, 16, 0xF800);  // left
            fillRect(128, bandY + 12, 2, 16, 0xF800);  // right
            fillRect(70,  bandY + 26, 60, 2, 0xF800);  // bottom
            break;
    }
}


// ************* main function ***********************
int main(void)
{	
	
	__enable_irq();
	ES_setSystemClk(120000000);
	
	// ADC
	//init_adc();

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
		/*
		ADC0->PSSI |= (1 << 0);   // start conversion on sequencer 0
		while ((ADC0->RIS & (1 << 0)) == 0) { }  // wait for SS0 complete
		
		uint16_t ain7 = ADC0->SSFIFO0 & 0xFFF;
		uint16_t ain6 = ADC0->SSFIFO0 & 0xFFF;
		uint16_t ain8 = ADC0->SSFIFO0 & 0xFFF;
	
		ADC0->ISC = (1 << 0);  // clear SS0 interrupt
		
		// ES_Uprintf(0, "AIN7=%u AIN6=%u AIN8=%u\n", ain7, ain6, ain8);
		*/
		
		if (waveform_changed)
		{
			if (prev == 0)
			{
				showWaveformName();
				prev = 1;
				
			}
			else
			{
				showWaveformName();
				prev = 0;
			}
			waveform_changed = 0;
		}
		if (scan_ready)
		{
			scan_ready = 0;
			// scan_keypad(); // look for keypad presses
			// handle_waveform_state(); // update waveform according to waveform select
		}
		
	}	
	
	return 0;
}