extern void(*Timer_1A_Task)(void);

void Timer_1A_Interrupt_Init(void(*task)(void));

void TIMER1A_Handler(void);