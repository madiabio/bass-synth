#include <TM4C129.h>
#include "ES_Lib.h"
#include "adc.h"
#include "config.h"
#define PD4 (1<<4)
#define PD5 (1<<5)
#define PE5 (1<<5)
#define SS0 (1<<0)
#define SS1 (1<<1)
#define SS3 (1<<3)
#include <stdint.h>
#define IRQ_NUMBER_ADC0_SEQ0 14



void init_adc(void) {
    SYSCTL->RCGCGPIO |= (1<<3) | (1<<4);   // Port D,E clocks
    while ((SYSCTL->PRGPIO & ((1<<3)|(1<<4))) == 0);

    GPIOD_AHB->DIR &= ~(PD4| PD5);    // PD4(AIN7), PD5(AIN6)
    GPIOE_AHB->DIR &= ~PD5;             // PE5(AIN8)
    GPIOD_AHB->AFSEL |= PD4| PD5;
    GPIOE_AHB->AFSEL |= PD5;
    GPIOD_AHB->DEN &= ~(PD4| PD5);
    GPIOE_AHB->DEN &= ~(PD5);
    GPIOD_AHB->AMSEL |= PD4| PD5;
    GPIOE_AHB->AMSEL |= PD5;

    SYSCTL->RCGCADC |= (1<<0);
    while ((SYSCTL->PRADC & (1<<0)) == 0); // wait ready

    ADC0->ACTSS &= ~(1<<0);        // disable SS0
    ADC0->EMUX  &= ~0xF;           // processor trigger
    ADC0->SSMUX0 = (7) | (6<<4) | (8<<8);  // AIN7,6,8
    ADC0->SSCTL0 = (0<<0)|(0<<4)|(0x6<<8); // IE|END on last
    ADC0->IM &= ~(1<<0);           // disable interrupts for now
    ADC0->ISC = (1<<0);            // clear status
    ADC0->ACTSS |= (1<<0);         // enable SS0
}

volatile uint16_t Attack_ms  = 50;
volatile uint16_t Decay_ms   = 100;
volatile uint16_t Sustain_lv = 700;  // scaled ×1000

static inline uint16_t mapU16(uint16_t val, uint16_t out_min, uint16_t out_max)
{
    return ((uint32_t)val * (out_max - out_min) / ADC_MAX) + out_min;
}

void updateADSR(uint16_t ain7, uint16_t ain6, uint16_t ain8)
{
    Attack_ms  = mapU16(ain7, 5, 500);
    Decay_ms   = mapU16(ain6, 10, 1000);
    Sustain_lv = mapU16(ain8, 0, 1000);  // 0–1000 = 0.0–1.0
}
