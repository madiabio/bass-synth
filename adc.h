#include <stdint.h>

extern volatile uint32_t Attack_Value;
extern volatile uint32_t Decay_Value;
extern volatile uint32_t Sustain_Value;

void updateADSR(uint16_t ain7, uint16_t ain6, uint16_t ain8);
void init_adc();

