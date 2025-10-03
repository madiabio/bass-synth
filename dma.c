#include <stdint.h>
#include "dma.h"
#include <TM4C129.h>
#include "config.h"
#define SSI3_TX_UDMA_CHANNEL   15
#define SSI3_TX_UDMA_ENCODING  2
#define UDMA_MODE_PINGPONG 0x3
#include "function_gen.h"

uint16_t pingBuffer[FRAME_COUNT];
uint16_t pongBuffer[FRAME_COUNT];

// DMA control table must be 1024-byte aligned (requirement of TM4C uDMA)
__attribute__((aligned(1024))) uDMAControlStruct uDMAControlTable[64];



void init_dma()
{

	
	// config UDMA perif
	
	// 1 - enable RCGCDMA 
	SYSCTL->RCGCDMA |= (1<<0);
	while ( (SYSCTL->PRDMA &(1<<0)) == 0 ) {}
	
	// 2 - master enable DMA
	UDMA->CFG |= (1<<0);  // controller master enable
	
	dma_configure_ssi3_tx();
}

void dma_configure_ssi3_tx(void) {
	
	// Primary (Ping)
	uDMAControlTable[15].srcEndAddr = &pingBuffer[FRAME_COUNT-1];
	uDMAControlTable[15].dstEndAddr = &SSI3->DR;
	uDMAControlTable[15].control    = UDMA_CFG_CTL(FRAME_COUNT);

	// Alternate (Pong)
	uDMAControlTable[15+32].srcEndAddr = &pongBuffer[FRAME_COUNT-1];
	uDMAControlTable[15+32].dstEndAddr = &SSI3->DR;
	uDMAControlTable[15+32].control    = UDMA_CFG_CTL(FRAME_COUNT);
	
	// config UDMA channel 15 for SSI3TX
	UDMA->PRIOCLR |= (1 << SSI3_TX_UDMA_CHANNEL);   // default priority
	UDMA->ALTCLR |= (1 << SSI3_TX_UDMA_CHANNEL);   // start with primary contorl structure
	UDMA->USEBURSTCLR |= (1 << SSI3_TX_UDMA_CHANNEL); // alow both burst and clearing
	UDMA->REQMASKCLR |= (1 << SSI3_TX_UDMA_CHANNEL); // unmask channel requests

	fillPingBuffer(pingBuffer, FRAME_COUNT);
	fillPongBuffer(pongBuffer, FRAME_COUNT);

	UDMA->ENASET |= (1<<SSI3_TX_UDMA_CHANNEL); // enable to transfer for SSI3 channel
}

