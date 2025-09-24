#ifndef CONFIG_H
#define CONFIG_H

// Core system definitions
#define SYS_CLK_FREQ   16000000u   // system clock (Hz)
#define SAMPLE_FREQ  48000u      // audio sample rate = 48kHz (Hz)

// Derived timer reload value
#define TIMER0A_RELOAD  ((SYS_CLK_FREQ / SAMPLE_FREQ) - 1)

// Priorities
#define PRIORITY_TIMER0A 2 // Function generator interrupt priority

#endif
