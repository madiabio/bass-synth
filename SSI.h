#include <stdint.h>

// SSI1 -> 12 bit DAC
#define PB4 (1<<4) // SSI1Fss
#define PB5 (1<<5) // SSI1Clk
#define PE4	(1<<4) // SSI1XDAT0 TX
#define PE5 (1<<5) // SSI1XDAT1 Rx (UNUSED)

// SSI3 -> I2S sound chip
#define PQ0 (1<<0) // SSI3Clk
#define PQ1 (1<<1) // SSI3Fss
#define PQ2 (1<<2) // SSI3XDAT0 (unused) Rx
#define PQ3 (1<<3) // SSI3XDAT1 Tx

// SSI2 -> Display
#define PD0 (1<<0) // SSI2XDAT1 Tx
#define PD1 (1<<1) // SSI2XDAT0 (unused) Rx
#define PD2 (1<<2) // SS2Fss
#define PD3 (1<<3) // SSI2Clk

#define TXIM (1<<3) // QSSI Transmit FIFO Interrupt Mask
#define TXMIS (1<<3) // QSSI Transmit FIFO Masked Interrupt Status
#define SSE (1<<1) // QSSI Synchronous Serial Port Enable
#define TNF (1<<1) // QSSI Transmit FIFO Not Full
#define SPH (1<<7) // QSSI Serial Clock phase
#define SPO (1<<6) // QSSI Serial Clock Polarity
#define RNE (1<<2) // QSSI Receive FIFO Not Empty


void init_SSI1(); // for 12-bit DAC
void init_SSI3(); // for I2S DAC
void initSPI(); // for Display
void spi_Transmit(uint8_t data); // for Display
