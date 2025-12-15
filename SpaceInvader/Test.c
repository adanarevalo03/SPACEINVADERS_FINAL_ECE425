#include "Test.h"
#include "Nokia5110.h"
#include "Sprites.h"
#include "ADC0.h"
#include "MCP4725.h"
#include "GPTM1.h"

void Test_Display_Init() {
	Nokia5110_Init();
	ADC_Init();
}
void Test_Display() {
	Nokia5110_Clear();
	
	while(1) {
		Nokia5110_ClearBuffer();
		Nokia5110_DrawImage(ADC_Sample()*84/4095, 38, 10, 10, rocketship, 12);
		Nokia5110_DisplayBuffer();
	}
	
	
//	for (int i = 0; i < 48; i++) {
//		Nokia5110_ClearBuffer();
//		Nokia5110_DrawImage(i, i, 10, 16, rocketship, 0);
//		Nokia5110_DisplayBuffer();
//		
//		for (int j = 0; j < 500000; j++);
//	}
	
//	Nokia5110_SetCursor(0, 1);
//	Nokia5110_OutString("Hello!");
}

void Test_ADC_Init() {
	ADC_Init();
}
void Test_ADC() {
	ADC_Sample();
}

void Test_DAC_Init() {
	MCP4725_Init();
}
void Test_DAC() {
	//MCP4725_SetValue(0x80);
	
	for (int i = 0; i < 0xFF; i++) {
		MCP4725_SetValue(i);
	}
}

void Test_GPTM1_Init() {
	Timer_1A_Interrupt_Init(Test_GPTM1);
}
void Test_GPTM1() {
	int i = 0;
	i++;
}