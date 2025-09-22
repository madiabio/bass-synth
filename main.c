// Workshop 2 Template
// Remember to start and configure TeraTerm

#include <TM4C129.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ES_Lib.h"

void enable_GPIOK()
{
	uint32_t pin2 = (1<<2);
	uint32_t portK = (1<<9);
	SYSCTL->RCGCGPIO |= portK; // Port K
	while ((SYSCTL->PRGPIO & portK) == 0) {} // wait
	
	GPIOK->DIR |= pin2; // Set to output
	GPIOK->DEN |= pin2; // Set to digital
}

void enable_GPION()
{
	SYSCTL->RCGCGPIO |= (1<<12); // clock for pin 1
	while (( SYSCTL->PRGPIO & (1<<12)) == 0) {} // wait
	
	GPION->DIR |= (1<<1); // enable as output
	GPION->DEN |= (1<<1); // enable digitla
}

void init_T1()
{
	uint32_t T1 = (1<<1);
	SYSCTL->RCGCTIMER |= T1;
	while ( (SYSCTL->PRTIMER & T1) != T1 );

	TIMER1->CTL &= ~0x1;   // disable timer
	TIMER1->CFG = 0x4; 	 	 // put into 16 bit modex		
	TIMER1->TAMR &= ~0x3;  // clear bits to 00
	TIMER1->TAMR |= 0x1;	 // set bit to 01 for one-shot mode
	
	TIMER1->TAILR = 50000; // set preload counter to 50,000
	TIMER1->TAPR = 159;		 // set prescalar to 159

	TIMER1->ICR |= 0x01;	 // clear timeout status flag
}

void delay_500ms()
{
    TIMER1->ICR = 0x1;          // clear any prior timeout
    TIMER1->CTL |= 0x1;         // enable timer
    while ((TIMER1->RIS & 0x1) == 0) {
        // wait until timeout
    }
    TIMER1->ICR = 0x1;          // acknowledge timeout
}

// ************* main function ***********************
int main(void)
{
	ES_Serial(0,"19200,8,N,1") ;
	
	
	ES_printf("\n****************************************\n") ;
	ES_printf("\n Uart 0 : 19200 baud, 8bits, 1 stop bit \n") ;

	ES_printf("\n****************************************\n") ;
	enable_GPIOK();
	enable_GPION();
	init_T1();
	while (true)
	{
		delay_500ms();
		GPIOK->DATA ^= (1<<2);
		GPION->DATA ^= (1<<1);
	}
	
	while(true) {}
	return 0;
}
