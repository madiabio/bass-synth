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

void drawWaveformSample(void)
{
    clearScreen();

    int centerY = ILI9341_TFTHEIGHT / 2;
    int startX  = 20;
    int width   = 200;
    float scaleY = 50.0f;
		uint16_t color = COLOR_SINE;
    const char *label = "";

    for (int i = 0; i < TABLE_SIZE; i++) {
        float normalized = 0.0f;
        float x = startX + (float)i / TABLE_SIZE * width;

        switch (waveform_mode) {
            case WAVE_SINE:
								normalized = (sine_table[i] / 65535.0f) * 2.0f - 1.0f; // -1 to +1							
								color = COLOR_SINE;
								label = "SINE";

                break;

            case WAVE_SAW:
                normalized = ((float)i / TABLE_SIZE) * 2.0f - 1.0f;
								color = COLOR_SAW;
								label = "SAW";
    
								break;

            case WAVE_TRI:
                if (i < TABLE_SIZE / 2)
                    normalized = (4.0f * i / TABLE_SIZE) - 1.0f;
                else
                    normalized = 3.0f - (4.0f * i / TABLE_SIZE);
								color = COLOR_TRI;
								label = "TRI";
								break;

            case WAVE_SQUARE:
                normalized = (i < TABLE_SIZE / 2) ? 1.0f : -1.0f;
								color = COLOR_SQUARE;
								label = "SQUARE";
								break;

            default:
                normalized = 0;
                break;
        }

        int y = centerY - (int)(normalized * scaleY);
				drawPixel((int)x, y, color);
    }
		setCharConfig(0xFFFF, 2, 1, 0x0000, 2); // white text, black background
    moveCursor(70, ILI9341_TFTHEIGHT - 30);          // near bottom center
    drawString((char*)label, strlen(label));

}
void displayADCValues(uint16_t ain7, uint16_t ain6, uint16_t ain8)
{
    char buf[32];

    // draw background rectangle on right side (to overwrite old text)
    fillRect(180, 40, 60, 80, 0x0000); // x,y,w,h,black

    setCharConfig(0xFFFF, 1, 1, 0x0000, 1); // white text
    moveCursor(185, 50);

    snprintf(buf, sizeof(buf), "A7:%4u", ain7);
    drawString(buf, strlen(buf));

    moveCursor(185, 70);
    snprintf(buf, sizeof(buf), "A6:%4u", ain6);
    drawString(buf, strlen(buf));

    moveCursor(185, 90);
    snprintf(buf, sizeof(buf), "A8:%4u", ain8);
    drawString(buf, strlen(buf));
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
		
		ADC0->PSSI |= (1 << 0);   // start conversion on sequencer 0
		while ((ADC0->RIS & (1 << 0)) == 0) { }  // wait for SS0 complete
		
		uint16_t ain7 = ADC0->SSFIFO0 & 0xFFF;
		uint16_t ain6 = ADC0->SSFIFO0 & 0xFFF;
		uint16_t ain8 = ADC0->SSFIFO0 & 0xFFF;
	
		ADC0->ISC = (1 << 0);  // clear SS0 interrupt
		updateADSR(ain7, ain6, ain8);

		// ES_Uprintf(0, "AIN7=%u AIN6=%u AIN8=%u\n", ain7, ain6, ain8);
		displayADCValues(ain7, ain6, ain8);        
		
		if (waveform_changed)
		{
			if (prev == 0)
			{
				drawWaveformSample();
				prev = 1;
				
			}
			else
			{
				drawWaveformSample();
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