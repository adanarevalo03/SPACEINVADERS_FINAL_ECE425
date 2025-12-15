
#include "TM4C123GH6PM.h"

void EduBase_Button_Init(void)
{
	// Enable the clock to Port D
	SYSCTL->RCGCGPIO |= 0x08;
	
	// Set PD0, PD1, PD2, and PD3 as input GPIO pins
	GPIOD->DIR &= ~0x0F;
	
	// Configure PD0, PD1, PD2, and PD3 to function as GPIO pins
	GPIOD->AFSEL &= ~0x0F;
	
	// Enable digital functionality for PD0, PD1, PD2, and PD3
	GPIOD->DEN |= 0x0F;
}

uint8_t Get_EduBase_Button_Status(void)
{
	// Assign the value of Port D to a local variable
	// and only read the values of the following bits: 3, 2, 1, and 0
	// Then, return the local variable's value
	uint8_t button_status = GPIOD->DATA & 0x0F;
	return button_status;
}

 

