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


// Define storage for your potentiometer values
volatile uint32_t Attack_Value;
volatile uint32_t Decay_Value;
volatile uint32_t Sustain_Value;

void ADC0Seq0_Handler(void) 
{
    // 1. Read all three samples from FIFO0 (AIN7 -> AIN6 -> AIN8)
    Attack_Value = ADC0->SSFIFO0;  
    Decay_Value  = ADC0->SSFIFO0;
    Sustain_Value = ADC0->SSFIFO0;
    
    // 3. Clear the interrupt status bit for Sequencer 0 [1, 2, 16]
    ADC0->ISC = (1 << 0); 
}


