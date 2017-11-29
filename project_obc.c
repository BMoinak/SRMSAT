#include <AT91SAM7SE512.H>
#include "..\AT91SAM7SE-EK.h"
#include "stdint.h"
#include "string.h"
int i = 0,xa,j=0;
int t_cnt=0;
float M=0;
float e=0.0;
volatile int sendagain_flag=0;
volatile int gnc_flag = 0;
volatile int irq_flag = 0;
volatile int ack_flag = 0;
volatile int health_flag = 0;
int h_print=0;
volatile int buf_count = 0;
int health_count = 0;
unsigned char he='\0';
unsigned char  health[5][32+1];
volatile unsigned char ack_inp[32+1];
volatile unsigned char chk_inp[32+1];
volatile unsigned char buffer[32+1];
volatile unsigned char temp[128]; //For debugging printing data


#define PACKET_SIZE 32
void health_assign()
{
	strcpy(health[health_count++],"612345678909879879879870000000F7");
	strcpy(health[health_count++],"612345678901239879879870000000F7");
	strcpy(health[health_count++],"612345678909871239879870000000F7");
	strcpy(health[health_count++],"612345678909879871239870000000F7");
	strcpy(health[health_count++],"612345678909879879871230000000F7");
	strcpy(ack_inp,"200000000000000000000000000000AC");
	strcpy(chk_inp,"200000000000000000000000000000AC");
}
void delay(int k)
{
	long int i;
	for(i=1;i<=2500*k;i++){};
}

void us0_putchar(unsigned char x)
{
	AT91C_BASE_US0->US_THR = x;
}
void us1_putchar(unsigned char x)
{
	while ( ! ((AT91C_BASE_US1->US_CSR & AT91C_US_TXRDY)==AT91C_US_TXRDY) ) {
		// do nothing
	};
	AT91C_BASE_US1->US_THR = x;
}

void us1_puts(char x[])//String Transmitting function.
{
	unsigned int i = 0;
	while(x[i]!='\0')//transmitts character by character till null character is reached
	{
		while ( ! ((AT91C_BASE_US1->US_CSR & AT91C_US_TXRDY)==AT91C_US_TXRDY) ) {
		// do nothing
	};
		
			AT91C_BASE_US1->US_THR = x[i];//transmitts a character
			i++;
		
	}
}
/*void checksum()
{
}*/
void parse()
{
	//checksum();
	switch(buffer[0])
	{
		if(checksum()==1)
			us1_puts(ack_inp);
		else
			us1_puts(chk_inp);
		case '1':
			
		sendagain_flag=1;
			//Send Data Again
			break;
		case '2':
			//Send Next Packet
			ack_flag=0;
			break;
		case '3':
			for(i=1;i<=5;i++)
			 { 
				 xa=((int)buffer[i]-48);
				 e=e*10+xa;
			 }
			 e=e/10000;
			 for(i=6;i<=10;i++)
			 {xa=((int)buffer[i]-48);
				M=M*10+xa;
			 }
			 M=M/10000;
			 gnc_flag=1;
			break;
		case '5':
		health_flag=1;
			break;
		
	}
	
}
/*void gnc_calc()
{
//For Creating thita and E	
}
*/
void send_health()
{
	if(h_print<health_count && !ack_flag)
	{
		us1_puts(health[h_print++]);
		ack_flag=1;
	}
	if(h_print==health_count)
	{
	health_flag=0;	
	}
	
}

void uart_interrupt(void) {
	/* ignore first irq */
	if (irq_flag == 0) {
		irq_flag = 1;
		AT91C_BASE_US0->US_IDR = 0xFFFFFFFF; //Disabling all other interrupts
		buffer[0] = AT91C_BASE_US0->US_RHR;
		AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;
		return;
	}
	
	AT91C_BASE_US0->US_IDR = 0xFFFFFFFF; //Disabling all other interrupts
	buffer[buf_count] = AT91C_BASE_US0->US_RHR;
	buf_count++;
	AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;

	if (buf_count == PACKET_SIZE) {
		AT91C_BASE_US0->US_IDR = 0xFFFFFFFF;
		
	}

}

void init_uart0()//uart0 initialisation
{
	// Enable the Clock of the PIOA (UART, LED{1,2,POWER})
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;

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
	AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;
	AT91C_BASE_US0->US_IER = ~AT91C_US_RXRDY;
	//AT91C_BASE_US0->US_IDR = 0xFFFFFFFF;
}

void init_uart1()//uart1 initialisation
{
	// Enable the Clock of the PIO
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOB;

	// Configure the PIO Lines for LEDs as Outputs
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED_MASK;
	AT91C_BASE_PIOA->PIO_OER  = AT91B_LED_MASK;

	//to enable the peripheral clock for the uart (usart1)
	AT91C_BASE_PMC->PMC_PCER= (1 << AT91C_ID_US1);

	//configuring the i/o lines for uart

	//to disable the pins for gpio function
	AT91C_BASE_PIOA->PIO_PDR= AT91C_PIO_PA21 | AT91C_PIO_PA22;

	//to select peripheral A by A-Select-Register(pg-37)

	AT91C_BASE_PIOA->PIO_ASR= AT91C_PA21_RXD1 | AT91C_PA22_TXD1;

	//to disable peripheral B

	AT91C_BASE_PIOA->PIO_BSR=0;

	// Open drain
	AT91C_BASE_PIOA->PIO_MDDR=~(AT91C_PA21_RXD1 | AT91C_PA22_TXD1);
	AT91C_BASE_PIOA->PIO_MDER=(AT91C_PA21_RXD1 | AT91C_PA22_TXD1);

	//to reset the transmitter and receiver and then disable them (later we are enabling it)
	AT91C_BASE_US1->US_CR= AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS;

	//to configure the mode register(pg-421)
	AT91C_BASE_US1->US_MR= AT91C_US_PAR_NONE | AT91C_US_CHRL_8_BITS;

	//to disable all interupts
	AT91C_BASE_US1->US_IDR= 0xFFFF;//(we need rxrdy and txempty interupts that will be enabled later)

	//to generate baud rate (divisor CD=313) (baud rate=9600)
	AT91C_BASE_US1->US_BRGR= 0x139;

	//to disable unused registers
	AT91C_BASE_US1->US_RTOR= 0;
	AT91C_BASE_US1->US_TTGR= 0;
	AT91C_BASE_US1->US_FIDI= 0;
	AT91C_BASE_US1->US_IF= 0;

		AT91C_BASE_US1->US_CR = AT91C_US_TXEN | AT91C_US_RXEN;
	AT91C_BASE_US1->US_IDR = 0xFFFFFFFF;
}


void init_aic(void *uart_interrupt(void))
{
	AT91C_BASE_AIC->AIC_IDCR=(1<<AT91C_ID_US0);
	AT91C_BASE_AIC->AIC_SMR[AT91C_ID_US0] =(AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | AT91C_AIC_PRIOR_HIGHEST);
	AT91C_BASE_AIC->AIC_SVR[AT91C_ID_US0]=(unsigned long)uart_interrupt;
	AT91C_BASE_AIC->AIC_IECR=(1<<AT91C_ID_US0);
	AT91C_BASE_AIC->AIC_ISCR=(1<<AT91C_ID_US0);
}
int main(void)
{
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
  AT91C_BASE_PIOA->PIO_OER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
	AT91C_BASE_PIOA->PIO_SODR =AT91B_LED1 | AT91B_POWERLED | AT91B_LED2;
	health_assign();
	init_uart0();
	init_uart1();
	init_aic(uart_interrupt);
	
	while(1)
	{
	  if(buf_count == PACKET_SIZE )
		{
			//us1_puts(buffer);
			parse();
			if( gnc_flag == 1 )
			{
				//gnc_calc();
			}
			if( health_flag == 1 )
			{ 
				send_health();
			}
			 if(sendagain_flag==1)
			{
				us1_puts(buffer);
				sendagain_flag=0;
			}
			else if(ack_flag==1)
			{
				
				
			}
			AT91C_BASE_US0->US_IER = AT91C_US_RXRDY;
			buf_count=0;
			
			
		}
		AT91C_BASE_PIOA->PIO_SODR =AT91B_POWERLED;
		delay(500);
		AT91C_BASE_PIOA->PIO_CODR =AT91B_POWERLED;
			delay(500);
		}
}
