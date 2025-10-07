#ifndef CONFIG_H
#define CONFIG_H

// Core system definitions
#define SYS_CLK_FREQ   250000000u   // system clock (Hz)
#define SAMPLE_FREQ  48000u      // audio sample rate = 48kHz (Hz)
#define REFRESH_RATE 20 // 20Hz refresh raet for the screen

// Derived timer reload value
#define KEYPAD_SCAN_PERIOD 250
#define TIMER0A_RELOAD  ((SYS_CLK_FREQ / KEYPAD_SCAN_PERIOD ) - 1)

// For SSI Configs
#define SSI1_CPSR 254				// Extra
#define SSI1_SCR (245 << 8) // Extra

#define SSI3_SCR (38 << 8)	// I2S DAC
#define SSI3_CPSR 2					// I2S DAC

#define SSI2_CPSR 2 				// LCD Display
#define SSI2_SCR (0<<8) 		// LCD Display


// Priorities
#define PRIORITY_TIMER0A 3 // Function generator interrupt priority
#define PRIORITY_SSI3 0 // DMA ping pong to SSI3 I2S interrupt priority
#define PRIORITY_ADC0 3 // Note check interrupt priority

// DMA Ping-Pong Buffer Frame Count
#define FRAME_COUNT 256
#define SCOPE_BUFFER_SIZE  320 // ring scope buffer for display (mono)

// Wave table settings
#define TABLE_SIZE 512 	// number of samples
#define DAC_MAX    0xFFFF // highest value the DAC can hold
#define DAC_MID 0x8000  // middle value of the DAC

// Display settings
#define COLOR_SINE      0x07E0  // green
#define COLOR_SAW       0xF800  // red
#define COLOR_TRI  0x001F  // blue
#define COLOR_SQUARE    0xFFE0  // yellow

// ADC 
#define ADC_MAX 4096


#endif