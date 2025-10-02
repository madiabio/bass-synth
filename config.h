#ifndef CONFIG_H
#define CONFIG_H

// Core system definitions
#define SYS_CLK_FREQ   250000000u   // system clock (Hz)
#define SAMPLE_FREQ  48000u      // audio sample rate = 48kHz (Hz)
#define REFRESH_RATE 20 // 20Hz refresh raet for the screen


// Derived timer reload value
#define TIMER0A_RELOAD  ((SYS_CLK_FREQ / REFRESH_RATE) - 1)

// Priorities
#define PRIORITY_TIMER0A 2 // Function generator interrupt priority
#define PRIORITY_SSI3 2 // Function generator to SSI interrupt priority
#endif
