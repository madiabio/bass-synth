#ifndef CONFIG_H
#define CONFIG_H

// Core system definitions
#define SYS_CLK_FREQ   250000000u   // system clock (Hz)
#define SAMPLE_FREQ  48000u      // audio sample rate = 48kHz (Hz)
#define REFRESH_RATE 20 // 20Hz refresh raet for the screen


// Derived timer reload value
#define TIMER0A_RELOAD  ((SYS_CLK_FREQ / REFRESH_RATE) - 1)

// For SSI Configs
#define SSI1_CPSR 254				// Extra
#define SSI1_SCR (245 << 8) // Extra
#define SSI3_SCR (38 << 8)	// I2S DAC
#define SSI3_CPSR 2					// I2S DAC
#define SSI2_CPSR 10 				// LCD Display
#define SSI2_SCR (5<<8) 		// LCD Display


// Priorities
#define PRIORITY_TIMER0A 2 // Function generator interrupt priority
#define PRIORITY_SSI3 2 // Function generator to SSI interrupt priority
#endif
