// Handles inputs
#include "notes.h"
#include "function_gen.h"
#include "input.h"
#include "ES.h"
#include <TM4C129.h>
#include <stdint.h>
volatile uint8_t note_on = 0;      // gate flag

#define PK3 (1<<3)
#define PK0 (1<<0)
#define PK1 (1<<1)

#define PA1 (1<<1) // U0
#define PA0 (1<<0)	// U0
#define U3  (1<<3)
#define U0  (1<<0)


void init_UART0()
{
	SYSCTL->RCGCUART |= U0;   
	while((SYSCTL->PRUART & U0) == 0); 
	
	SYSCTL->RCGCGPIO |= (1 << 0);   
	while((SYSCTL->PRGPIO & (1 << 0)) == 0);

	GPIOA_AHB->AFSEL |= PA0 | PA1;  
	
	GPIOA_AHB->PCTL  |= (1<<0*4) | (1<<1*4);
	
	GPIOA_AHB->DEN  |= PA0 | PA1;

	UART0->CTL &= ~(1 << 0);        
	UART0->IBRD = 65;               
	UART0->FBRD = 7;               
	UART0->LCRH = (0x3 << 5);       // 8-bit, no parity, 1 stop
	UART0->CC   = 0;              // use sysclk
	UART0->CTL  = (1 << 9) | (1 << 8) | (1 << 0); // RXE, TXE, UEN

}

/*
void init_timer0a() {
    SYSCTL->RCGCTIMER |= (1<<0); // enable timer 0 clock
		while((SYSCTL->PRTIMER & (1 << 0)) == 0) {}// Wait until ready

    TIMER0->CTL &= ~(1<<0); // disable during setup
    TIMER0->CFG = 0; // 32-bit timer
    
		TIMER0->TAMR = 0x1; // periodic down counter mode
    TIMER0->TAILR = TIMER0A_RELOAD; // load value (defined in config.h)
    TIMER0->IMR |= (1<<0); // enable timeout interrupt
    
		// enable IRQ 19 to enable interrupts on NVIC for TIMER0 (see NVIC Register Descriptions in datasheet and TIMER0 Handler in regref/course notes)
		// NVIC->IPR[19] = PRIORITY_TIMER0A; // set priority as defined in config header
		NVIC->ISER[0] |= (1<<19); // this is the same thing as NVIC_ENn in the data sheet
		// TIMER0->CTL = 1;       	// enable timer
}

// Timer0A Handler function as defined in header file
// Calculates the next value of the waveform
void TIMER0A_Handler(void) 
{
	TIMER0->ICR = 1; // clear flag
	note_on = 0; // Clear the gate
}
*/

void keypad_init(void)
{
	uint32_t GPIOK_and_GPIOE = ((1<<9) | (1<<4));
	SYSCTL->RCGCGPIO |= GPIOK_and_GPIOE;
	while ( (SYSCTL->PRGPIO & (GPIOK_and_GPIOE)) == 0 ) {}
	
	uint32_t PKs = (1<<1) | (1<<0) | (1<<2);
	uint32_t PEs = (1<<0) | (1<<3) | (1<<2) | (1<<1);
		
	// Set as inputs
	GPIOK->DIR |= PKs;
	GPIOE_AHB->DIR &= ~PEs;
		
	// enable pull down resistor for switches
	GPIOK->DR2R |= PKs; // enable pins output to drive 2mA
	GPIOE_AHB->PDR |= PEs; // Enable pull up resistor
	
	// enable digital
	GPIOK->DEN  |= PKs;
	GPIOE_AHB->DEN |= PEs;
}


void scan_keypad()
{
	char last_raw = 0;
	char stable_key =  0;
	uint8_t count = 0;
	
	const char keyMap[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
	};

	while(true)
	{
		
		for (int col = 0; col < 3; col++) {
			// set all cols low and then set current col high.
			GPIOK->DATA &= ~((1 << 0) | (1 << 1) | (1 << 2));
			GPIOK->DATA |= (1 << col);
			ES_usDelay(100);
			uint32_t rows = GPIOE_AHB->DATA & 0x0F; // PE0 PE3


			// iterate thru each input and check which bit has been set
			// then reference that to the key map to see which key that
			// corresponds to.
			for (int row = 0; row < 4; row++) {
				if ((rows >> row) & 0x1) {
					char key = keyMap[row][col];
					ES_Uprintf(0, "Key Pressed: %c\n", key);
				}	
			}
		}
		ES_msDelay(30);
	}
}