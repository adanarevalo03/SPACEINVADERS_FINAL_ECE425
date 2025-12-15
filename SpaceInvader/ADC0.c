#include "TM4C123GH6PM.h"

#include "ADC0.h"

// PE3 CONNECTS TO MIDDLE , THE OUTER PINS GROUND &POWER , CHECK TO INVERT 

void ADC_Init(){
		SYSCTL->RCGCADC |= 0x01;
	SYSCTL->RCGCGPIO |= 0x10;
	
	GPIOE->DIR &= ~0x08;
	GPIOE->DEN &= ~0x08;
	GPIOE-> AMSEL |= 0x08;
	GPIOE->AFSEL |= 0x08;
	
	ADC0->ACTSS &= ~0x1;
	ADC0->EMUX &= ~0xF;
	ADC0->SSMUX0 &= ~0XFFFFFFFF;
	ADC0->SSMUX0 |= 0x00000000;
	ADC0->SSCTL0 |= 0x00000002;
	ADC0->SSCTL0 |= 0x00000004;
	ADC0->ACTSS |= 0x1;
	
}

// return a value from 0-4095
uint16_t ADC_Sample(){
	ADC0->PSSI |= 0x01;
	while((ADC0->RIS & 0x01)== 0);
	uint16_t result = ADC0->SSFIFO0;
	ADC0->ISC |= 0x01;
	return result;
	
	
}