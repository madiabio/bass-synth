// Handles inputs
#include "notes.h"
#include "function_gen.h"
#include "input.h"
#include "ES.h"
#include <TM4C129.h>
#include <stdint.h>
#include "config.h"
volatile uint8_t note_on = 0;      // gate flag
volatile char key_pressed = -1;   // current key (-1 if none)
volatile uint32_t row_event_flags = 0;

#define PK3 (1<<3)
#define PK0 (1<<0)
#define PK1 (1<<1)

#define PA1 (1<<1) // U0
#define PA0 (1<<0)	// U0
#define U3  (1<<3)
#define U0  (1<<0)



#define IRQ_NUMBER_GPIOE 4

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
void init_timer0a() {
    SYSCTL->RCGCTIMER |= (1<<0); // enable timer 0 clock
		while((SYSCTL->PRTIMER & (1 << 0)) == 0) {}// Wait until ready

    TIMER0->CTL &= ~(1<<0); // disable during setup
    TIMER0->CFG = 0; // 32-bit timer
    
		TIMER0->TAMR = 0x2; // periodic mode
    TIMER0->TAILR = TIMER0A_RELOAD; // load value (defined in config.h)
    TIMER0->IMR |= (1<<0); // enable timeout interrupt
    
		// enable IRQ 19 to enable interrupts on NVIC for TIMER0 (see NVIC Register Descriptions in datasheet and TIMER0 Handler in regref/course notes)
		NVIC->IPR[19] = PRIORITY_TIMER0A; // set priority as defined in config header
		NVIC->ISER[0] |= (1<<19); // this is the same thing as NVIC_ENn in the data sheet
		TIMER0->CTL = 1;       	// enable timer
}

// Timer0A Handler function as defined in header file
// Calculates the next value of the waveform
void TIMER0A_Handler(void) 
{
	TIMER0->ICR = 0x1;    // clear interrupt flag
	scan_keypad();        // scan
}



void keypad_init(void)
{
	uint32_t GPIOK_and_GPIOE = ((1<<9) | (1<<4));
	SYSCTL->RCGCGPIO |= GPIOK_and_GPIOE;
	while ( (SYSCTL->PRGPIO & (GPIOK_and_GPIOE)) == 0 ) {}
	
		
	// Set as inputs
	GPIOK->DIR |= PKs;
	GPIOE_AHB->DIR &= ~PEs;
		
	// enable pull down resistor for switches
	GPIOK->DR2R |= PKs; // enable pins output to drive 2mA
	GPIOE_AHB->PDR |= PEs; // Enable pull down resistor
	
	// enable digital
	GPIOK->DEN  |= PKs;
	GPIOE_AHB->DEN |= PEs;
		
	/*
		
	// Configure port E rows for interrupts
	GPIOE_AHB->IM &= ~PEs; // turn off interrupts.
	
	GPIOE_AHB->IS &= ~PEs; // set to edge sensitive
	
	GPIOE_AHB->IBE |= PEs; // set to both edges triggering an interrupt
	
	GPIOE_AHB->ICR |= PEs; // Clear ICR
	
	GPIOE_AHB->IM |= PEs; // enable interrupts to NVIC
	
	// NVIC FOR GPIOE
	NVIC->ISER[IRQ_NUMBER_GPIOE / 32] |= (1<<(IRQ_NUMBER_GPIOE %32));
	NVIC->IPR[IRQ_NUMBER_GPIOE] = PRIORITY_SSI3;
	*/

	init_timer0a();
}

/*
void scan_keypad(void) {
    static const char keyMap[4][3] = {
        {'1','2','3'},
        {'4','5','6'},
        {'7','8','9'},
        {'*','0','#'}
    };

    for (int col = 0; col < 3; col++) {
        GPIOK->DATA &= ~0x07;          // all low
        GPIOK->DATA |= (1 << col);     // one high
        ES_usDelay(50);
        uint32_t rows = GPIOE_AHB->DATA & 0x0F;

        for (int row = 0; row < 4; row++) {
            if ((rows >> row) & 1) {
                key_pressed = keyMap[row][col];
								// ES_Uprintf(0, "Key Pressed: %c\n", key_pressed);
                note_on = 1;
                return;  // leave after first press
            }
        }
    }
    // if no key pressed
    note_on = 0;
    key_pressed = -1;
}
*/
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
		int mask = 0;
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
					key_pressed  = keyMap[row][col];
					note_on = 1;
					ES_Uprintf(0, "Key Pressed: %c\n", key_pressed);
					while ((GPIOE_AHB->DATA & (1<<row)) != 0) {
							ES_usDelay(1000); // small hold delay
					}
					note_on = 0; // clear when released
					key_pressed = -1;
					ES_Uprintf(0, "Released\n");

				}	
			}
		}
		ES_msDelay(30);
	}
}

