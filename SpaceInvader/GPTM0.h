extern void(*Timer_0A_Task)(void);

void Timer_0A_Interrupt_Init(void(*task)(void));

void TIMER0A_Handler(void);