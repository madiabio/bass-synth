#include <stdint.h>

extern volatile uint16_t Attack_ms;
extern volatile uint16_t Decay_ms;
extern volatile uint16_t Sustain_lv;  // scaled ×1000

void updateADSR(uint16_t ain7, uint16_t ain6, uint16_t ain8);
void init_adc();

