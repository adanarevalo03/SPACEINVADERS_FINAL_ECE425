#include "GPTM1.h"
#include "TM4C123GH6PM.h"


void(*Timer_1A_Task)(void);

void Timer_1A_Interrupt_Init(void(*task)(void))
{
	Timer_1A_Task 			= task;
	SYSCTL->RCGCTIMER 	|= 0x02;
	TIMER1->CTL 				&= ~0x01;
	TIMER1->CFG 				|= 0x04;
	TIMER1->TAMR 				|= 0x02;
	TIMER1->TAPR 				&= ~0x000000FF;
	TIMER1->TAPR 				|= 1-1;
	TIMER1->TAILR 			= (4535 - 1); // 11.025kHz
	TIMER1->ICR 				|= 0x01;
	TIMER1->IMR 				|= 0x01;
	NVIC->IPR[5] 				= (NVIC->IPR[5] & 0x1FFFFFFF) | (1 << 29);
	NVIC->ISER[0] 			|= (1 << 21);
	TIMER1->CTL 				|= 0x01;
	
}

void TIMER1A_Handler(void)
{
	if (TIMER1->MIS & 0x01)
	{
		(*Timer_1A_Task)();
		
		TIMER1->ICR |= 0x01;
	}
}