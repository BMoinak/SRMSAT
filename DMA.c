#include <AT91SAM7SE512.H>              /* AT91SAM7SE512 definitions          */
#include "..\AT91SAM7SE-EK.h"           /* AT91SAM7SE-EK board definitions    */
char Buffer[32]; // holds received characters
unsigned long nChars = 0; // counts number of received chars
char *pBuffer = &Buffer[0]; // pointer into Buffer
void foo() {
	
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOB;
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
  AT91C_BASE_PIOA->PIO_OER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
	AT91C_BASE_PIOA->PIO_SODR =AT91B_LED1 |AT91B_LED2 ;
	AT91C_BASE_PIOA->PIO_CODR =AT91B_LED1 |AT91B_LED2 ;
	
	// determine which interrupt has occurred (end-of-receive DMA or end-of-transmit DMA)
if ((AT91C_BASE_US0->US_CSR & AT91C_US_ENDRX) == AT91C_US_ENDRX) {
	
	
		AT91C_BASE_PIOA->PIO_SODR =AT91B_LED1;
		AT91C_BASE_PIOA->PIO_CODR =AT91B_LED2 ;
	
// we have a end-of-receive interrupt (ENDRX)
AT91C_BASE_US0->US_RCR = 10; // restore the receive count - clears ENDRX flag
// point the transmit buffer pointer to beginning of buffer, set count to 10
AT91C_BASE_US0->US_TPR = (AT91_REG)&Buffer[0]; // address of DMA output buffer (use same one)
AT91C_BASE_US0->US_TCR = 10; // we'll transmit 10 chars via DMA
// disable the end-of-receive interrupt, enable the end-of-transmit interrupt
AT91C_BASE_US0->US_IER = AT91C_US_ENDTX; // enable usart0 end-of-transmit interrupt
AT91C_BASE_US0->US_IDR = ~AT91C_US_ENDTX; // disable all interrupts except ENDTX
// enable transmit DMA transfers, disable receive DMA transfers
// note: this will START the transmission of whatever is in the Buffer[32]!
AT91C_BASE_US0->US_PTCR = AT91C_PDC_TXTEN |AT91C_PDC_RXTDIS;  // enable transmit transfer,
// disable receive transfer
} 

else if ((AT91C_BASE_US0->US_CSR & AT91C_US_ENDTX) == AT91C_US_ENDTX) {
	
	AT91C_BASE_PIOA->PIO_SODR =AT91B_LED2;
		AT91C_BASE_PIOA->PIO_CODR =AT91B_LED1 ;
	
// we have a end-of-transmit interrupt (10 characters have clocked out)
AT91C_BASE_US0->US_TCR = 10; // restore the transmit count - clears ENDTX flag
// point the receive buffer pointer to beginning of buffer, set count to 10
AT91C_BASE_US0->US_RPR = (AT91_REG)&Buffer[0]; // address of DMA output buffer (use same one)
AT91C_BASE_US0->US_RCR = 10; // we'll receive 10 chars via DMA
// enable receive interrupt, disable the transmit interrupt
AT91C_BASE_US0->US_IER = AT91C_US_ENDRX; // enable usart0 end-of-receive interrupt
AT91C_BASE_US0->US_IDR = ~AT91C_US_ENDRX; // disable all interrupts except ENDRX
// enable receive DMA transfers, disable transmit DMA transfers
// note: the DMA transfer will start when the first character arrives!
AT91C_BASE_US0->US_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTDIS;// enable receive transfer,
 // disable transmit transfer
}
}
int main(void)
{
	
//to enable the peripheral clock for the uart (usart0)
	AT91C_BASE_PMC->PMC_PCER  = (1 << AT91C_ID_US0);
//configuring the i/o lines for uart
	//to disable the pins for gpio function
	AT91C_BASE_PIOA->PIO_PDR= AT91C_PIO_PA5 | AT91C_PIO_PA6;
	//to select peripheral A by A-Select-Register(pg-37)
	AT91C_BASE_PIOA->PIO_ASR= AT91C_PA5_RXD0 | AT91C_PA6_TXD0;
	//to disable peripheral B
	AT91C_BASE_PIOA->PIO_BSR=0;
	// Open drain
	AT91C_BASE_PIOA->PIO_MDDR=~(AT91C_PA5_RXD0 | AT91C_PA6_TXD0);
	AT91C_BASE_PIOA->PIO_MDER=(AT91C_PA5_RXD0 | AT91C_PA6_TXD0);
	//to reset the transmitter and receiver and then disable them (later we are enabling it)
	AT91C_BASE_US0->US_CR= AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;
	//to configure the mode register(pg-421)
	AT91C_BASE_US0->US_MR= AT91C_US_PAR_NONE | AT91C_US_CHRL_8_BITS;

	//to disable all interupts
	AT91C_BASE_US0->US_IDR= 0xFFFF;//(we need rxrdy and txempty interupts that will be enabled later)

	//to generate baud rate (divisor CD=313) (baud rate=9600)
	AT91C_BASE_US0->US_BRGR= 0x139;

	//to disable unused registers
	AT91C_BASE_US0->US_RTOR= 0;
	AT91C_BASE_US0->US_TTGR= 0;
	AT91C_BASE_US0->US_FIDI= 0;
	AT91C_BASE_US0->US_IF= 0;

	//to enable the transmitter and receiver
	AT91C_BASE_US0->US_CR= AT91C_US_TXEN | AT91C_US_RXEN;
	AT91C_BASE_US0->US_IER= AT91C_US_TXRDY;

	AT91C_BASE_US0->US_RPR = (unsigned int)Buffer;
	AT91C_BASE_US0->US_RCR = 10;
	AT91C_BASE_US0->US_TPR = (unsigned int)Buffer;
   AT91C_BASE_US0->US_TCR = 10;
	AT91C_BASE_US0->US_RNPR = (unsigned int)0;
	AT91C_BASE_US0->US_RNCR = (unsigned int)0;
	AT91C_BASE_US0->US_TNPR = (unsigned int)0;
	AT91C_BASE_US0->US_TNCR = (unsigned int)0;
	AT91C_BASE_US0->US_PTCR = AT91C_PDC_RXTEN |AT91C_PDC_TXTDIS; // enable receive transfer,// disable transmit transfer
 
 // Set up the Advanced Interrupt Controller (AIC) registers for USART0
AT91C_BASE_AIC->AIC_IDCR = (1<<AT91C_ID_US0); // Disable USART0 interrupt in AIC
AT91C_BASE_AIC->AIC_SMR[AT91C_ID_US0] =(AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x4 );  // Set the interrupt source type and priority
AT91C_BASE_AIC->AIC_SVR[AT91C_ID_US0] =(unsigned int)foo;  // Set the USART0 IRQ handler address in AIC Source
AT91C_BASE_AIC->AIC_IECR = (1<<AT91C_ID_US0); // Enable the USART0 interrupt in AIC
AT91C_BASE_AIC->AIC_ISCR=(1<<AT91C_ID_US0);

// enable the USART0 end-of-receive interrupt
AT91C_BASE_US0->US_IER = AT91C_US_ENDRX; // enable ENDRX usart0 end-of-receive interrupt
//AT91C_BASE_US0->US_IDR = ~AT91C_US_ENDRX; // disable all interrupts except ENDRX
while(1);
}

