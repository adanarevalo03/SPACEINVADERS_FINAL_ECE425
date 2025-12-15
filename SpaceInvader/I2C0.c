#include "I2C0.h"

#include "TM4C123GH6PM.h"

void I2C0_Init(){
	SYSCTL->RCGCI2C |= 0x01;
	SYSCTL->RCGCGPIO |= 0x2;
	
	GPIOB->AFSEL |= 0x0C;
	GPIOB->DEN |= 0x0C;
	GPIOB->PCTL = (GPIOB->PCTL & 0x0000FF00)| 0x00003300;
	GPIOB->ODR |= 0x08;
	
	I2C0->MCR |= 0x10;
	I2C0->MTPR|= 0x05;
	
}

// uint8_t I2C0_Recieve (uint8_t address , uint8_t registerAddress){
// }

uint8_t I2C0_Transmit(uint8_t address, uint8_t registerAddress,uint8_t data){
	while(I2C0->MCS & 0x01); // busy wait for I2C0 not busy 
	
	I2C0->MSA = address << 1;
	I2C0->MDR = registerAddress;
	I2C0->MCS = 0x3;// generates START and set RUN to start transmission
	
	while(I2C0->MCS & 0x01);
	
	I2C0->MDR = data;
	I2C0->MCS = 0x5; // generate STOP and set RUN to start transmission
	
	while(I2C0->MCS & 0x01);
	
	return I2C0->MCS & 0x2; //return error bit (==0 if no error)
}