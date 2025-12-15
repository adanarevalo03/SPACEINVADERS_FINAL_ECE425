#include <stdint.h>

#define MCP4725_I2C_ADDRESS 0x60

void MCP4725_Init();

void MCP4725_SetValue(uint16_t value);