// Nokia5110.c
// Runs on LM4F120/TM4C123
// Use SSI0 to send an 8-bit code to the Nokia5110 48x84
// pixel LCD to display text, images, or other information.
// Daniel Valvano
// July 2, 2018

// Font table, initialization, and other functions based
// off of Nokia_5110_Example from Spark Fun:
// 7-17-2011
// Spark Fun Electronics 2011
// Nathan Seidle
// http://dlnmh9ip6v2uc.cloudfront.net/datasheets/LCD/Monochrome/Nokia_5110_Example.pde

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// ----------------------------------------------
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "Nokia5110.h"
#include "TM4C123GH6PM.h"

#define PLAYERH 0x08
#define LASER0W 0x02
#define LASER0H 0x09

#define DC                      (*((volatile unsigned long *)0x40004100))
#define DC_COMMAND              0
#define DC_DATA                 0x40
#define RESET                   (*((volatile unsigned long *)0x40004200))
#define RESET_LOW               0
#define RESET_HIGH              0x80

enum typeOfWrite{
  COMMAND,                              // the transmission is an LCD command
  DATA                                  // the transmission is data
};
// The Data/Command pin must be valid when the eighth bit is
// sent.  The SSI module has hardware input and output FIFOs
// that are 8 locations deep.  Based on the observation that
// the LCD interface tends to send a few commands and then a
// lot of data, the FIFOs are not used when writing
// commands, and they are used when writing data.  This
// ensures that the Data/Command pin status matches the byte
// that is actually being transmitted.
// The write command operation waits until all data has been
// sent, configures the Data/Command pin for commands, sends
// the command, and then waits for the transmission to
// finish.
// The write data operation waits until there is room in the
// transmit FIFO, configures the Data/Command pin for data,
// and then adds the data to the transmit FIFO.

// This is a helper function that sends an 8-bit message to the LCD.
// inputs: type     COMMAND or DATA
//         message  8-bit code to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
void static lcdwrite(enum typeOfWrite type, char message){
  if(type == COMMAND){
                                        // wait until SSI0 not busy/transmit FIFO empty
    while((SSI0->SR&0x10)==0x10){};
    DC = DC_COMMAND;
    SSI0->DR = message;                // command out
                                        // wait until SSI0 not busy/transmit FIFO empty
    while((SSI0->SR&0x10)==0x10){};
  } else{
    while((SSI0->SR&0x2)==0){}; // wait until transmit FIFO not full
    DC = DC_DATA;
    SSI0->DR = message;                // data out
  }
}

//********Nokia5110_Init*****************
// Initialize Nokia 5110 48x84 LCD by sending the proper
// commands to the PCD8544 driver.  One new feature of the
// LM4F120 is that its SSIs can get their baud clock from
// either the system clock or from the 16 MHz precision
// internal oscillator.
// inputs: none
// outputs: none
// assumes: system clock rate of 80 MHz
void Nokia5110_Init(void){
  volatile unsigned long delay;
  SYSCTL->RCGCSSI |= 0x1;  // activate SSI0
  SYSCTL->RCGCGPIO |= 0x1; // activate port A
	
  GPIOA->DIR |= 0xC0;             // make PA6,7 out
  GPIOA->AFSEL |= 0x2C;           // enable alt funct on PA2,3,5
  GPIOA->AFSEL &= ~0xC0;          // disable alt funct on PA6,7
  GPIOA->DEN |= 0xEC;             // enable digital I/O on PA2,3,5,6,7
                                        // configure PA2,3,5 as SSI
  GPIOA->PCTL = (GPIOA->PCTL&0xFF0F00FF)+0x00202200;
                                        // configure PA6,7 as GPIO
  GPIOA->PCTL = (GPIOA->PCTL&0x00FFFFFF)+0x00000000;
  GPIOA->AMSEL &= ~0xEC;          // disable analog functionality on PA2,3,5,6,7
  SSI0->CR1 &= ~0x2;           // disable SSI
  SSI0->CR1 &= ~0x4;            // master mode
                                        // configure for system clock/PLL baud clock source
  SSI0->CC = (SSI0->CC&~0xF)+0x0;
                                        // clock divider for 3.33 MHz SSIClk (80 MHz PLL/24)
                                        // SysClk/(CPSDVSR*(1+SCR))
                                        // 50/(15*(1+0)) = 3.33 MHz (slower than 4 MHz)
  SSI0->CPSR = (SSI0->CPSR&~0xFF)+15; // must be even number
  SSI0->CR0 &= ~(0xFF00 |       // SCR = 0 (3.33 Mbps data rate)
                  0x80 |         // SPH = 0
                  0x40);         // SPO = 0
                                        // FRF = Freescale format
  SSI0->CR0 = (SSI0->CR0&~0x30)+0x0;
                                        // DSS = 8-bit data
  SSI0->CR0 = (SSI0->CR0&~0xF)+0x7;
  SSI0->CR1 |= 0x2;            // enable SSI

  RESET = RESET_LOW;                    // reset the LCD to a known state
  for(delay=0; delay<10; delay=delay+1);// delay minimum 100 ns
  RESET = RESET_HIGH;                   // negative logic

  lcdwrite(COMMAND, 0x21);              // chip active; horizontal addressing mode (V = 0); use extended instruction set (H = 1)
                                        // set LCD Vop (contrast), which may require some tweaking:
  lcdwrite(COMMAND, CONTRAST);          // try 0xB1 (for 3.3V red SparkFun), 0xB8 (for 3.3V blue SparkFun), 0xBF if your display is too dark, or 0x80 to 0xFF if experimenting
  lcdwrite(COMMAND, 0x04);              // set temp coefficient
  lcdwrite(COMMAND, 0x14);              // LCD bias mode 1:48: try 0x13 or 0x14

  lcdwrite(COMMAND, 0x20);              // we must send 0x20 before modifying the display control mode
  lcdwrite(COMMAND, 0x0C);              // set display control to normal mode: 0x0D for inverse
}

//********Nokia5110_OutChar*****************
// Print a character to the Nokia 5110 48x84 LCD.  The
// character will be printed at the current cursor position,
// the cursor will automatically be updated, and it will
// wrap to the next row or back to the top if necessary.
// One blank column of pixels will be printed on either side
// of the character for readability.  Since characters are 8
// pixels tall and 5 pixels wide, 12 characters fit per row,
// and there are six rows.
// inputs: data  character to print
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutChar(unsigned char data){
  int i;
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
  for(i=0; i<5; i=i+1){
    lcdwrite(DATA, ASCII[data - 0x20][i]);
  }
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
}

//********Nokia5110_OutString*****************
// Print a string of characters to the Nokia 5110 48x84 LCD.
// The string will automatically wrap, so padding spaces may
// be needed to make the output look optimal.
// inputs: ptr  pointer to NULL-terminated ASCII string
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutString(char *ptr){
  while(*ptr){
    Nokia5110_OutChar((unsigned char)*ptr);
    ptr = ptr + 1;
  }
}

//********Nokia5110_OutUDec*****************
// Output a 16-bit number in unsigned decimal format with a
// fixed size of five right-justified digits of output.
// Inputs: n  16-bit unsigned number
// Outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutUDec(unsigned short n){
  if(n < 10){
    Nokia5110_OutString("    ");
    Nokia5110_OutChar(n+'0'); /* n is between 0 and 9 */
  } else if(n<100){
    Nokia5110_OutString("   ");
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<1000){
    Nokia5110_OutString("  ");
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else if(n<10000){
    Nokia5110_OutChar(' ');
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else {
    Nokia5110_OutChar(n/10000+'0'); /* ten-thousands digit */
    n = n%10000;
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
}

//********Nokia5110_SetCursor*****************
// Move the cursor to the desired X- and Y-position.  The
// next character will be printed here.  X=0 is the leftmost
// column.  Y=0 is the top row.
// inputs: newX  new X-position of the cursor (0<=newX<=11)
//         newY  new Y-position of the cursor (0<=newY<=5)
// outputs: none
void Nokia5110_SetCursor(unsigned char newX, unsigned char newY){
  if((newX > 11) || (newY > 5)){        // bad input
    return;                             // do nothing
  }
  // multiply newX by 7 because each character is 7 columns wide
  lcdwrite(COMMAND, 0x80|(newX*7));     // setting bit 7 updates X-position
  lcdwrite(COMMAND, 0x40|newY);         // setting bit 6 updates Y-position
}

//********Nokia5110_Clear*****************
// Clear the LCD by writing zeros to the entire screen and
// reset the cursor to (0,0) (top left corner of screen).
// inputs: none
// outputs: none
void Nokia5110_Clear(void){
  int i;
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, 0x00);
  }
  Nokia5110_SetCursor(0, 0);
}

uint8_t Screen[SCREENW*SCREENH/8]; // buffer stores the next image to be printed on the screen

uint8_t min(uint8_t a, uint8_t b) {
	return (a < b) ? a : b;
}
uint8_t max(uint8_t a, uint8_t b) {
	return (a > b) ? a : b;
}

//********Nokia5110_DrawImage*****************
// Draw a 1bit image to the screen at the coordinates
// inputs: xpos      horizontal position of top left corner of image, columns from the left edge
//                     must be less than 84
//                     0 is on the left; 82 is near the right
//         ypos      vertical position of top left corner of image, rows from the top edge
//                     must be less than 48
//                     2 is near the top; 47 is at the bottom
//         width     width of the image in pixels
//         height    height of the image in pixels
//         ptr       pointer to a byte array of 1-bit pixel values (1 byte = 8 pixels of data)
//         sizeBytes Length of the array pointed to by ptr
void Nokia5110_DrawImage(unsigned char xpos, unsigned char ypos, unsigned char width, unsigned char height, const unsigned char *ptr, unsigned char sizeBytes) {	
	uint8_t y_offset = ypos % 8;
	uint8_t misalign  = height % 8;
	
	uint8_t x_start = min(xpos, SCREENW); // avoid images where starting coordinate is off the screen
	uint8_t x_stop = min(xpos+width, SCREENW); // clip image at edge of screen
	
	uint8_t y_start = min(ypos, SCREENH)/8; // avoid images where starting coordinate is off the screen
	uint8_t y_stop = (min(ypos+height, SCREENH)+7)/8; // clip image at edge of screen
	
	uint8_t ptrIndex = 0;
	for (uint8_t y = y_start; y < y_stop; y++) {
		for (uint8_t x = x_start; x < x_stop; x++) {
			uint16_t currentIndex = y*SCREENW+x;
			
//			if (y == y_start) { // first row
//				Screen[currentIndex] = (Screen[currentIndex] & (0xFF << y_offset)) | (~ptr[ptrIndex] << y_offset);
//			} else if (y == y_stop - 1) { // lastRow
//				if (y_offset == 0)
//					Screen[currentIndex] = ~ptr[ptrIndex]&(0xFF>>(8-misalign));
//				else
//					Screen[currentIndex] = (~ptr[ptrIndex-width]>>(8-y_offset))&(0xFF>>(8-y_offset));
//			} else { // Middle row
//				Screen[currentIndex] = (~ptr[ptrIndex-width]>>(8-y_offset))&(0xFF>>(8-y_offset)) | (~ptr[ptrIndex]<<y_offset)&(0xFF<<y_offset);
//			}
			
			if (y == y_start) { // first row
				// DOES NOT CORRECTLY MAINTAIN PREVIOUS SCREEN CONTENTS
				uint8_t a = (Screen[currentIndex]);
				uint8_t b = (~ptr[ptrIndex] << y_offset);
				Screen[currentIndex] = a | b;
			} else { // Middle row
				uint8_t a = Screen[currentIndex];
				uint8_t b = (~ptr[ptrIndex-width]>>(8-y_offset))&(0xFF>>(8-y_offset));
				uint8_t c = (~ptr[ptrIndex]<<y_offset)&(0xFF<<y_offset);
				if (ptrIndex < sizeBytes)
					Screen[currentIndex] = a | b | c;
				else
					Screen[currentIndex] = a | b;
			}
			
			ptrIndex++;
		}
	}
}

//********Nokia5110_DrawFullImage*****************
// Fill the whole screen by drawing a 48x84 bitmap image.
// inputs: ptr  pointer to 504 byte bitmap
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DrawFullImage(const uint8_t *ptr){
  int i;
  Nokia5110_SetCursor(0, 0);
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, ptr[i]);
  }
}

//********Nokia5110_PrintBMP*****************
// Bitmaps defined above were created for the LM3S1968 or
// LM3S8962's 4-bit grayscale OLED display.  They also
// still contain their header data and may contain padding
// to preserve 4-byte alignment.  This function takes a
// bitmap in the previously described format and puts its
// image data in the proper location in the buffer so the
// image will appear on the screen after the next call to
//   Nokia5110_DisplayBuffer();
// The interface and operation of this process is modeled
// after RIT128x96x4_BMP(x, y, image);
// inputs: xpos      horizontal position of bottom left corner of image, columns from the left edge
//                     must be less than 84
//                     0 is on the left; 82 is near the right
//         ypos      vertical position of bottom left corner of image, rows from the top edge
//                     must be less than 48
//                     2 is near the top; 47 is at the bottom
//         ptr       pointer to a 16 color BMP image
//         threshold grayscale colors above this number make corresponding pixel 'on'
//                     0 to 14
//                     0 is fine for ships, explosions, projectiles, and bunkers
// outputs: none
void Nokia5110_PrintBMP(unsigned char xpos, unsigned char ypos, const unsigned char *ptr, unsigned char threshold){
  long width = ptr[18], height = ptr[22], i, j;
  unsigned short screenx, screeny;
  unsigned char mask;
  // check for clipping
  if((height <= 0) ||              // bitmap is unexpectedly encoded in top-to-bottom pixel order
     ((width%2) != 0) ||           // must be even number of columns
     ((xpos + width) > SCREENW) || // right side cut off
     (ypos < (height - 1)) ||      // top cut off
     (ypos > SCREENH))           { // bottom cut off
    return;
  }
  if(threshold > 14){
    threshold = 14;             // only full 'on' turns pixel on
  }
  // bitmaps are encoded backwards, so start at the bottom left corner of the image
  screeny = ypos/8;
  screenx = xpos + SCREENW*screeny;
  mask = ypos%8;                // row 0 to 7
  mask = 0x01<<mask;            // now stores a mask 0x01 to 0x80
  j = ptr[10];                  // byte 10 contains the offset where image data can be found
  for(i=1; i<=(width*height/2); i=i+1){
    // the left pixel is in the upper 4 bits
    if(((ptr[j]>>4)&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;
    // the right pixel is in the lower 4 bits
    if((ptr[j]&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;
    j = j + 1;
    if((i%(width/2)) == 0){     // at the end of a row
      if(mask > 0x01){
        mask = mask>>1;
      } else{
        mask = 0x80;
        screeny = screeny - 1;
      }
      screenx = xpos + SCREENW*screeny;
      // bitmaps are 32-bit word aligned
      switch((width/2)%4){      // skip any padding
        case 0: j = j + 0; break;
        case 1: j = j + 3; break;
        case 2: j = j + 2; break;
        case 3: j = j + 1; break;
      }
    }
  }
}
// There is a buffer in RAM that holds one screen
// This routine clears this buffer
void Nokia5110_ClearBuffer(void){int i;
  for(i=0; i<SCREENW*SCREENH/8; i=i+1){
    Screen[i] = 0;              // clear buffer
  }
}

//********Nokia5110_SetPixel*****************
// Turn on the specified pixel in the screen buffer.
// inputs: x - horizontal coordinate of the pixel, must be less than 84
//         y - vertical coordinate of the pixel, must be less than 48
//         Nokia5110_SetPixel(0, 0); turns on the upper left pixel
// outputs: none
// Pixel level functions by Tomas
void Nokia5110_SetPixel(unsigned char x, unsigned char y) {
  unsigned short PixelByte;            // byte# in screen buffer
  unsigned char PixelBit;              // bit# in byte
  if ((x<84) && (y<48)) {              // check screen boundaries
    PixelByte = ((y/8)*84) + x;
    PixelBit = y % 8;
    Screen[PixelByte] |= 1U<<PixelBit;
  }
}

//********Nokia5110_ClearPixel*****************
// Turn off the specified pixel in the screen buffer.
// inputs: x - horizontal coordinate of the pixel, must be less than 84
//         y - vertical coordinate of the pixel, must be less than 48
//         Nokia5110_ClearPixel(0, 0); turns off the upper left pixel
// outputs: none
// Pixel level functions by Tomas
void Nokia5110_ClearPixel(unsigned char x, unsigned char y) {
  unsigned short PixelByte;            // byte# in screen buffer
  unsigned char PixelBit;              // bit# in byte
  if ((x<84) && (y<48)) {              // check screen boundaries
    PixelByte = ((y/8)*84) + x;
    PixelBit = y % 8;
    Screen[PixelByte] &= ~(1U<<PixelBit);
  }
}

//********Nokia5110_DisplayBuffer*****************
// Fill the whole screen by drawing a 48x84 screen image.
// inputs: none
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DisplayBuffer(void){
  Nokia5110_DrawFullImage(Screen);
}

