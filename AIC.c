#include <AT91SAM7SE512.H>
#include "..\AT91SAM7SE-EK.h"
void delay(void)
{int i;
	for(i=1;i<=2918572;i++);
}

char i;
void foo() {
	char x;
	AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;//Enabling the Receiver Ready
	AT91C_BASE_US0->US_IDR = ~AT91C_US_RXRDY;//Disabling all other interrupts
	while(!(AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY));
	x=AT91C_BASE_US0->US_RHR;

	AT91C_BASE_US0->US_IER= AT91C_US_TXRDY;//Enabling the Tx ready
	AT91C_BASE_US0->US_IDR= ~AT91C_US_TXRDY;//Disabling all other interrups
	//no loops and condition actually its working fine without any conditions

		AT91C_BASE_US0->US_THR=x;
	// y u no haz empty loops?
	 //loop runs till rxrdy==1

	if(x=='a')
	{
		AT91C_BASE_PIOA->PIO_SODR =AT91B_LED2;
	AT91C_BASE_PIOA->PIO_CODR =AT91B_LED1;
	}
	if(x=='b')
	{
		AT91C_BASE_PIOA->PIO_CODR =AT91B_LED2;
	AT91C_BASE_PIOA->PIO_SODR =AT91B_LED1;
	}
	/* AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_US0->US_CSR;
	AT91C_BASE_AIC->AIC_ICCR  = (0x1 << AT91C_ID_US0);
	 *AT91C_AIC_EOICR=0;
	AT91C_BASE_US0->US_CR = AT91C_US_RSTSTA;
	*/

}


int main(void)
{
	// Enable the Clock of the PIOA (UART, LED{1,2,POWER})
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;
	// Configure the PIO Lines for LEDs as Outputs
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOB;
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
  AT91C_BASE_PIOA->PIO_OER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
	AT91C_BASE_PIOA->PIO_SODR = AT91B_POWERLED;
	AT91C_BASE_PIOA->PIO_SODR = AT91B_LED1 | AT91B_LED2 ;

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



	 AT91C_BASE_AIC->AIC_IDCR=(1<<AT91C_ID_US0);
	 AT91C_BASE_AIC->AIC_SMR[AT91C_ID_US0] =(AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | AT91C_AIC_PRIOR_HIGHEST);
	 AT91C_BASE_AIC->AIC_SVR[AT91C_ID_US0]=(unsigned long)foo;
	AT91C_BASE_AIC->AIC_IECR=(1<<AT91C_ID_US0);
	AT91C_BASE_AIC->AIC_ISCR=(1<<AT91C_ID_US0);
	AT91C_BASE_US0->US_IER= AT91C_US_ENDRX;
	AT91C_BASE_PIOA->PIO_CODR =AT91B_LED2;
	AT91C_BASE_PIOA->PIO_CODR =AT91B_LED1;
while(1);
}

