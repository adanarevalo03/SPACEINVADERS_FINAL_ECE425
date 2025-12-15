#include "MCP4725.h"
#include "I2C.h"
#include "TM4C123GH6PM.h"


void MCP4725_Init()
{
	I2C0_Init();
}

void MCP4725_SetValue(uint16_t value)
{
	value = value >> 4; // chopped off 4 least sig bits
	
	I2C0_Transmit(MCP4725_I2C_ADDRESS, value>>8, value);
	
}