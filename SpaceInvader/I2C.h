#include <stdint.h>

//PB2 = SCL
//PB3 = SDA

void I2C0_Init();

uint8_t I2C0_Transmit(uint8_t address, uint8_t registerAddress, uint8_t data);
