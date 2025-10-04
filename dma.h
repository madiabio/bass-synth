#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#define SSI3_TX_UDMA_CHANNEL   15
#define SSI3_TX_UDMA_ENCODING  2
#define UDMA_MODE_PINGPONG 0x3
#define UDMA_MODE_STOP 0x0

#define UDMA_CFG_CTL(count) ( \
		/* XFERSIZE = N-1  
		The transfer size is the number of items, not the number of bytes. If the
		data size is 32 bits, then this value is the number of 32-bit words to
		transfer
		*/ \
    (((count)-1) << 4)       |  \
		\
		/* ARBSIZE = 4 transfers
		This field configures the number of transfers that can occur before the
		µDMA controller re-arbitrates
		*/ \
    (0x2 << 14)              |  \
		\
		/* SRCSIZE = 16-bit 
		This field configures the source item data size.
		*/ \
		(0x1 << 24)              |  \
		\
		/* SRCINC = 16-bit 
		This field configures the source address increment.
		The address increment value must be equal or greater than the value		of the source size (SRCSIZE).
		*/ \
    (0x1 << 26)              |  \
		\
		/* DSTSIZE = 16-bit (half word)
		This field configures the destination item data size.
		*/ \
    (0x1 << 28)              |  \
		\
		/* DSTINC = none  (Address remains set to the value of the Destination Address End Pointer (DMADSTENDP) for the channel)
		This field configures the destination address increment.
		*/ \
    (0x3 << 30)              |  \
		\
		/* MODE = PINGPONG (011b) 
		This field configures the operating mode of the µDMA cycle.
		*/ \
    (UDMA_MODE_PINGPONG) )                     \

typedef struct {
    volatile void *srcEndAddr; 	// points to last (inclusive) addr of the source buffer (ie pingBuf/pongBuf) 
    volatile void *dstEndAddr;	// points to last (inclusive) addr of the dest buffer. For periph transmit, this addr si fixed to point to periph's DR
    volatile uint32_t control; 	// contains primary transfer config (data size, addres incriment, arbitration size, transfer size, transfer mode). the controller updates the transfer size & transfer mod fields as the transfer is performed.
    volatile uint32_t unused;
} uDMAControlStruct;


extern __attribute__((aligned(1024))) uDMAControlStruct uDMAControlTable[64]; // arr of size 64 accounts for all 32 uDMA channels supported.
extern uint16_t pingBuffer[FRAME_COUNT];
extern uint16_t pongBuffer[FRAME_COUNT];

void init_dma();
void dma_configure_ssi3_tx();