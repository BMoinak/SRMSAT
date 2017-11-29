#include <AT91SAM7SE512.H>             
#include "..\AT91SAM7SE-EK.h"
#include "uart.h"
int datalength = 10;
int x;
//void transmit(void);
unsigned int temp,b[7];int i;
unsigned int read(int);
void write();
char buf[4096];
void blink(void);
void delay(void)
{int i;
	for(i=1;i<=2918572;i++);
}





int main(void)
{
	init_uart0();
  AT91C_BASE_PMC->PMC_PCER  =  (1 << AT91C_ID_PIOA) | (1 << AT91C_ID_TWI);
	AT91C_BASE_PIOA -> PIO_PDR = AT91C_PA3_TWD | AT91C_PA4_TWCK ; //Disabling the receiver and transmitter pins
  AT91C_BASE_PIOA -> PIO_ASR = AT91C_PIO_PA3| AT91C_PIO_PA4 ; //multiplexing 
  AT91C_BASE_PIOA -> PIO_BSR = 0;
	//AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PA4_TWCK|AT91C_PA3_TWD;
	//AT91C_BASE_PIOA->PIO_MDDR=~(AT91C_PA4_TWCK | AT91C_PA3_TWD);
	//AT91C_BASE_PIOA->PIO_MDER=(AT91C_PA4_TWCK | AT91C_PA3_TWD);
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
	AT91C_BASE_TWI->TWI_CWGR = (0xF0 << 0) | (0xF0 << 8) | (0x01 << 16);
	AT91C_BASE_TWI->TWI_CR = (AT91C_TWI_MSEN) | (AT91C_TWI_SVDIS);
	
	//AT91C_BASE_PIOA -> PIO_PER = AT91C_PA3_TWD | AT91C_PA4_TWCK ;
	
	uart0_puts("hello world!\n");

	//snprintf(buf, 255, "cr: %u\n", AT91C_BASE_TWI->TWI_CR);
	//uart0_puts(buf);
	write();
	for(;;){
		uart0_puts("read: ");
		for(i=0;i<7;i++) {
		//	delay();
			//write(i);
						//rt0_putchar('0' + i);
       //uart0_puts("\n");
       b[i]=read(i);
			

								

		}
		snprintf(buf, 255, " %x/%x/%x \t %x  \t %x:%x:%x \n", b[4], b[5],b[6], b[3], b[2],b[1],b[0]);
	    uart0_puts(buf);
			      
		delay();
		uart0_putchar('\n');
		
		
	//transmit();
	}
	return 0;
}

/*void transmit(void)
{
	AT91C_BASE_TWI->TWI_MMR = (AT91C_TWI_DADR & (0x68));
	AT91C_BASE_TWI->TWI_MMR = (AT91C_TWI_MREAD & 0x000000);
	AT91C_BASE_TWI->TWI_THR = 0x012;
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
	AT91C_BASE_TWI->TWI_IER = AT91C_TWI_TXRDY_MASTER;
	AT91C_BASE_TWI->TWI_IDR = ~AT91C_TWI_TXRDY_MASTER;
  while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
  AT91C_BASE_TWI->TWI_THR = 0x0200;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
}*/
unsigned int read(int ad)
{ 
	//uart0_puts("bkpt1\n");
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_MREAD | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = ad;
	//uart0_puts("bkpt2\n");
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;
//snprintf(buf, 255, "cr: %u\n", AT91C_BASE_TWI->TWI_CR);
	//uart0_puts(buf);
	//snprintf(buf, 255, "cwgr: %x\n", AT91C_BASE_TWI->TWI_CWGR);
	//uart0_puts(buf);
  //uart0_puts("bkpt3\n");
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_RXRDY));//rxdy==1
	//uart0_puts("bkpt4 \n");
 temp=AT91C_BASE_TWI->TWI_RHR; //blink();
	//uart0_putchar(temp);
	//uart0_puts("bkpt5\n");
	//snprintf(buf, 255, "sr: %x\n", AT91C_BASE_TWI->TWI_SR);
	//uart0_puts(buf);
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
 //uart0_puts("bkpt6\n");
	return temp;
}
void blink()
{
	AT91C_BASE_PIOA->PIO_PER = AT91B_LED1;
	AT91C_BASE_PIOA->PIO_OER = AT91B_LED1;
	AT91C_BASE_PIOA->PIO_SODR = AT91B_LED1;
	delay();
	AT91C_BASE_PIOA->PIO_CODR = AT91B_LED1;
	delay();
}

void write()
{
	int r,n;
	char v[100];
	uart0_gets(v);
	r=((int)v[0]-48);n=((int)v[1]-48);
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 0;
	AT91C_BASE_TWI->TWI_THR =((r & 0x7) <<4) | (n & 0xf);
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	
	
	
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 1;
	AT91C_BASE_TWI->TWI_THR = 0x59;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	
	
	
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 2;
	AT91C_BASE_TWI->TWI_THR = 0x23;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	
	
	
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 3;
	AT91C_BASE_TWI->TWI_THR = 0x7;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	
	
	
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 4;
	AT91C_BASE_TWI->TWI_THR = 0x31;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	
	
	
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 5;
	AT91C_BASE_TWI->TWI_THR = 0x12;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
	
	
	
	AT91C_BASE_TWI->TWI_MMR = (0x68 << 16) | AT91C_TWI_IADRSZ_1_BYTE ;
	AT91C_BASE_TWI->TWI_IADR = 6;
	AT91C_BASE_TWI->TWI_THR = 0x16;
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY_MASTER));
	while (!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP_MASTER));
}