#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// Core system definitions
#define SYS_CLK_HZ   16000000u   // system clock (Hz)
#define SAMPLE_RATE  20000u      // audio sample rate (Hz)

// Derived timer reload value
#define TIMER0A_RELOAD  ((SYS_CLK_HZ / SAMPLE_RATE) - 1)

#endif
