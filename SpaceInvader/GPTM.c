#include "GPTM0.h"
#include "TM4C123GH6PM.h"


void(*Timer_0A_Task)(void);

void Timer_0A_Interrupt_Init(void(*task)(void))
{
	Timer_0A_Task 			= task;
	SYSCTL->RCGCTIMER 	|= 0x01;
	TIMER0->CTL 				&= ~0x01;
	TIMER0->CFG 				|= 0x04;
	TIMER0->TAMR 				|= 0x02;
	TIMER0->TAPR 				&= ~0x000000FF;
	TIMER0->TAPR 				|= 250-1;
	TIMER0->TAILR 			= (20000 - 1); 
	TIMER0->ICR 				|= 0x01;
	TIMER0->IMR 				|= 0x01;
	NVIC->IPR[4] 				= (NVIC->IPR[4] & 0x1FFFFFFF) | (1 << 29);
	NVIC->ISER[0] 			|= (1 << 19);
	TIMER0->CTL 				|= 0x01;
	
}

void TIMER0A_Handler(void)
{
	if (TIMER0->MIS & 0x01)
	{
		(*Timer_0A_Task)();
		
		TIMER0->ICR |= 0x01;
	}
}