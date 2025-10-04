#include <stdint.h>
#include "dma.h"
#include <TM4C129.h>
#include "config.h"
#include "function_gen.h"
#define CH15_MASK_CLEAR 0x0FFFFFFF
#define CH15_SHIFT 28

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

	// 3) Write the 1024-byte aligned base address of the uDMAControlTable to DMACTLBASE [1, 2]
	UDMA->CTLBASE = (uint32_t)uDMAControlTable; 
		
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
	UDMA->CHMAP1 = (UDMA->CHMAP1 & CH15_MASK_CLEAR) | (SSI3_TX_UDMA_ENCODING << CH15_SHIFT); // uDMA channel 15 source select.
	UDMA->PRIOCLR |= (1 << SSI3_TX_UDMA_CHANNEL);   // default priority
	UDMA->ALTCLR |= (1 << SSI3_TX_UDMA_CHANNEL);   // start with primary contorl structure
	UDMA->USEBURSTCLR |= (1 << SSI3_TX_UDMA_CHANNEL); // alow both burst and clearing
	UDMA->REQMASKCLR |= (1 << SSI3_TX_UDMA_CHANNEL); // unmask channel requests

	fillPingBuffer(pingBuffer, FRAME_COUNT);	// fill logic handled in function_gen.c
	fillPongBuffer(pongBuffer, FRAME_COUNT);	// fill logic handled in function_gen.c

	UDMA->ENASET |= (1<<SSI3_TX_UDMA_CHANNEL); // enable to transfer for SSI3 channel
}

