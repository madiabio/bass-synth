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

#define WAVEFORM_AREA_X 0
#define WAVEFORM_AREA_Y 160
#define WAVEFORM_AREA_HEIGHT 120
#define WAVEFORM_AREA_WIDTH WAVE_BUF_LEN
#define WAVEFORM_COLOR ILI9341_CYAN

static inline uint16_t sample_to_screen_y(uint16_t sample)
{
    if (sample > 4095U) {
        sample = 4095U;
    }

    uint32_t scaled = ((uint32_t)sample * (WAVEFORM_AREA_HEIGHT - 1U)) / 4095U;
    return (uint16_t)(WAVEFORM_AREA_Y + (WAVEFORM_AREA_HEIGHT - 1U) - scaled);
}

static void draw_waveform(const uint16_t *samples, size_t count)
{
    if ((samples == NULL) || (count == 0U)) {
        return;
    }

    fillRect(WAVEFORM_AREA_X, WAVEFORM_AREA_Y, WAVEFORM_AREA_WIDTH, WAVEFORM_AREA_HEIGHT, ILI9341_BLACK);
    drawRect(WAVEFORM_AREA_X, WAVEFORM_AREA_Y, WAVEFORM_AREA_HEIGHT, WAVEFORM_AREA_WIDTH, ILI9341_LIGHTGREY);

    uint16_t prev_y = sample_to_screen_y(samples[0]);
    drawPixel(WAVEFORM_AREA_X, prev_y, WAVEFORM_COLOR);

    for (size_t i = 1; i < count; ++i) {
        uint16_t x = (uint16_t)(WAVEFORM_AREA_X + i);
        uint16_t y = sample_to_screen_y(samples[i]);

        if (y == prev_y) {
            drawPixel(x, y, WAVEFORM_COLOR);
        } else {
            uint16_t top = (y < prev_y) ? y : prev_y;
            uint16_t bottom = (y > prev_y) ? y : prev_y;

            for (uint16_t row = top; row <= bottom; ++row) {
                drawPixel(x, row, WAVEFORM_COLOR);
            }
        }

        prev_y = y;
    }
}




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
	setCharConfig(ILI9341_YELLOW,1,1,ILI9341_BLACK,1);
	drawString("Bass Synth",18);
	moveCursor(0,30);
	setCharConfig(ILI9341_LIGHTGREY,1,1,ILI9341_BLACK,1);
	drawString("Waveform",16);

	drawRect(WAVEFORM_AREA_X, WAVEFORM_AREA_Y, WAVEFORM_AREA_HEIGHT, WAVEFORM_AREA_WIDTH, ILI9341_LIGHTGREY);

	init_SSI3(); // for I2S DAC
	init_timer0a(); // for function generator
	while(true)
	{
					if (function_gen_waveform_ready()) {
									uint16_t samples[WAVE_BUF_LEN];
									size_t copied = function_gen_copy_waveform(samples, WAVE_BUF_LEN);

									if (copied > 0U) {
													draw_waveform(samples, copied);
									}
					}
	}

	



	return 0;
}
