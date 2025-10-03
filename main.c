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


// ************* main function ***********************
int main(void)
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
	return 0;
}
