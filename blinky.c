#include "AT91SAM7SE512.H"              /* AT91SAM7SE512 definitions          */
#include "..\AT91SAM7SE-EK.h"           /* AT91SAM7SE-EK board definitions    */
int main(void)
{
	int j=1;
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOA;
	AT91C_BASE_PMC->PMC_PCER  = 1 << AT91C_ID_PIOB;
	AT91C_BASE_PIOA->PIO_PER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
  AT91C_BASE_PIOA->PIO_OER  = AT91B_LED1 | AT91B_LED2 | AT91B_POWERLED;
	AT91C_BASE_PIOA->PIO_SODR =AT91B_LED1 | AT91B_POWERLED;
	for(;;){
		long int i;
		if(j==1){
				for(i=1;i<320000;i++){}
	AT91C_BASE_PIOA->PIO_CODR =AT91B_LED1;
	AT91C_BASE_PIOA->PIO_SODR =AT91B_LED2;
		}
	if(j==2){	for(i=1;i<320000;i++){}

	AT91C_BASE_PIOA->PIO_CODR =AT91B_LED2;
	AT91C_BASE_PIOA->PIO_CODR =AT91B_POWERLED;
	}
		if(j==3){	for(i=1;i<320000;i++){}

	AT91C_BASE_PIOA->PIO_SODR =AT91B_POWERLED;
		AT91C_BASE_PIOA->PIO_SODR =AT91B_LED1 ;
		}
		j++;
		if(j==4)
			j=1;
	}
}
