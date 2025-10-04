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
#include "dma.h"

#include "adc.h"
uint8_t prev_button_edge  = 1;   // track previous button state


void test_I2S_circuit()
{
	init_sine_table();
	init_dma();
	init_SSI3();
}

void test_button()
{
    init_button_PD7();
    init_PG1();
		init_PK4();


}

void handle_waveform_state()
{
	uint8_t now = (GPIOD_AHB->DATA & PD7) ? 1 : 0;

	if(prev_button_edge == 1 && now == 0) {   // detect falling edge (press)
		waveform_mode = (waveform_t)((waveform_mode + 1) & 0x3);
		update_LEDs();
		for(volatile int i=0; i<50000; i++); // crude debounce ~5–10ms
	}
	prev_button_edge  = now;

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
	test_button();
	test_I2S_circuit();
	while(true) 
	{
		handle_waveform_state();
	}  // loop forever

	return 0;
}
