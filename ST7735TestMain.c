// ST7735TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Daniel Valvano
// Last Modified: 3/6/2015 

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

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

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "ST7735.h"
#include "Random.h"
#include "SysTick.h"
#include "Pause.h"
#include "PLL.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"

void DelayWait10ms(uint32_t n);
void EnableInterrupts(void);
void DisableInterrupts(void);
// test image
// [blue] [green]
// [red ] [white]
double boxes[12] = {14,12,10,8,6,4,-2,-4,-6,-8,-10,-12};
uint8_t index_val = 0;
double zs[12] = {2,2,2,2,2,2,2,2,2,2,2,2};
uint32_t ADCMail;
uint32_t ADCStatus;
const uint16_t Test[] = {
  0x001F, 0xFFFF, 0xF800, 0x07E0
};

uint32_t const TestData[16] ={
  0,7,34,199,321,654,4789,9999,10000,21896,65535,
  123456,1234567,12345678,123456789,0xFFFFFFFF};
void IO_Init(void);
void IO_HeartBeat(void);
void IO_Touch(void);
void LCD_OutDec(uint32_t num);
void LCD_OutFix(uint32_t num); 	
void line_bresenham(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	uint8_t quad2 = 0;
	int eps = 0;
	if((y2-y1) > (x2-x1)){ // Switch because of the equation
		quad2 = 1;
		uint16_t x = x1;
		x1 = y1;
		y1 = x;
		x = x2;
		x2 = y2;
		y2 = x;
	}
	int dx = x2 - x1;
	int	dy = y2 - y1;
	uint16_t	y = y1;
	for(int x = x1; x <= x2; x++ ){
		if(quad2)
			ST7735_DrawPixel(y,x,0);
		else
			ST7735_DrawPixel(x,y,0);
		eps += dy;
		if ((eps << 1) >= dx){
			y++;  
			eps -= dx;
		}
	}
}
int absolute(int i){
	return (i>0) ? i : -i;
}
void line_bresenham3d(double x1, double y1, double z1, double x2, double y2, double z2, uint16_t color){
	int eps = 0;
	int rx1, rx2, ry1, ry2;
	if(z1 >= 0 && z2 >= 0){
		//double distance1 = sqrt((x1 * x1) + (y1 * y1) + (z1 - 16) * (z1 - 16));
		//double distance2 = sqrt((x2 * x2) + (y2 * y2) + (z2 - 16) * (z2 - 16));
		//x1 = 64 + x1 + .5*z1*cos(angle*rad);
		//y1 = 80 + y1 + .5*z1*sin(angle*rad);
		//x2 = 64 + x2 + .5*z2*cos(angle*rad);
		//y2 = 80 + y2 + .5* z2*sin(angle*rad); // Cavalier projections
		//x1 = 32 + (x1 * cos(rad*angle)) - (y1 * cos(rad*angle));
		//y1 = 40 + z1 + (x1 * sin(rad*angle)) + (y1* sin(rad*angle));
		//x2 = 32 + (x2 * cos(rad*angle)) - (y2 * cos(rad*angle));
		//y2 = 40 + z2 + (x2 * sin(rad*angle)) + (y2 * sin(rad*angle)); // Isometric view
		rx1 = 96 + (x1*16/(z1));
		ry1 = 80 + (y1*8/(z1));
		rx2 = 96 + (x2*16/(z2));
		ry2 = 80 + (y2*8/(z2)); // Vanishing points
	}
	else 
		return;
	if(rx1 < -2 || rx1 > 130 || ry1 < -2 || ry1 > 162){
			boxes[index_val] = -99;
			zs[index_val] = 2;
			return;
	}
	uint8_t swapped = 0;
	if(absolute(rx2-rx1) < absolute(ry2 - ry1)){
		swapped = 1;
		int x = rx1;
		rx1 = ry1;
		ry1 = x;
		x = rx2;
		rx2 = ry2;
		ry2 = x;
	}
	if(rx2 - rx1 < 0){
		int x = rx1;
		rx1 = rx2;
		rx2 = x;
		x = ry1;
		ry1 = ry2;
		ry2 = x;
	}
	int dx = rx2 - rx1;
	int	dy = ry2 - ry1;
	uint16_t	y = ry1;
	for(int x = rx1; x <= rx2; x++ ){
		if(swapped)
			ST7735_DrawPixel(y,x,color);
		else
			ST7735_DrawPixel(x,y,color);
		eps += dy;
		if(dy > 0){
			if (eps  > dx){
				y++;  
				eps -= dx;
			}
		}
		else if(eps < -dx){
			y--;  
			eps += dx;
		}
	}
}
void makeArrow(int color){
	line_bresenham3d(-93, -10, 16, -78, 0, 16, color);
	line_bresenham3d(-93, 10, 16, -78, 0, 16, color);
	line_bresenham3d(-93, -10, 16, -88, 0, 16, color);
	line_bresenham3d(-93, 10, 16, -88, 0, 16, color);
	line_bresenham3d(-88, 0, 16, -78, 0, 16, color);
}
//
// Takes an x and y value as inputs and generates a square based on their positions
//
void makeSquare(int x, double y, double z){
	if(y <= 0){
		line_bresenham3d(x, y, z, x, y, z+.125, 0);
		line_bresenham3d(x-1, y, z, x-1, y, z+.125, 0);
		line_bresenham3d(x, y-2, z, x, y-2, z+.125, 0);
		line_bresenham3d(x, y, z+.125, x, y-2, z+.125, 0);
		line_bresenham3d(x, y, z+.125, x-1, y, z+.125, 0);
	}
	else{
		line_bresenham3d(x, y, z, x, y, z+.125, 0);
		line_bresenham3d(x, y-2, z, x, y-2, z+.125, 0);
		line_bresenham3d(x-1, y-2, z, x-1, y-2, z+.125, 0);
		line_bresenham3d(x, y, z+.125, x, y-2, z+.125, 0);
		line_bresenham3d(x, y-2, z+.125, x-1, y-2, z+.125, 0);
	}
	line_bresenham3d(x, y, z, x-1, y, z, 0);
	line_bresenham3d(x, y, z, x, y-2, z, 0);
	line_bresenham3d(x-1, y-2, z, x-1, y, z, 0);
	line_bresenham3d(x-1, y-2, z, x, y-2, z, 0);
}
int32_t Convert(uint32_t input){
	int32_t result;
	result = (55459000+(943790*input))/1000000;
  return result;
}
void SysTick_Handler(void){
	if((GPIO_PORTB_DATA_R&0x02) == 0x02){
		NVIC_ST_CTRL_R -= 0x02; // Disable Systick
		Pause(0, 0, 0xFFFF);
	}
	ADCMail = ADC_In();  // sample 12-bit channel 1
  double shift_factor = ((double) Convert(ADCMail))/1000; 
	shift_factor -= 1;
	shift_factor *= .5;
	ST7735_FillScreen(0xFFFF);	
	for(index_val = 0; index_val < 12; index_val++){
		if(boxes[index_val] == -99)
			Random_Gen(NVIC_ST_CURRENT_R);
		boxes[index_val] += shift_factor;
		makeSquare(-2, boxes[index_val], zs[index_val]);
		if(zs[index_val] < .6){
			if(boxes[index_val] > -.75 && boxes[index_val] < 2.75){
				NVIC_ST_CTRL_R -= 0x02; // Disable Systick
				Pause(1, 0, 0xFFFF);
			}
		}
		zs[index_val] -= .05;
	}
	makeArrow(0);
}
int main(void){  
//  uint8_t red, green, blue;
  PLL_Init();                           // set system clock to 80 MHz
	ADC_Init();
  IO_Init();
	volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000002;  
  delay = SYSCTL_RCGCGPIO_R; 
	GPIO_PORTB_DIR_R &= ~0x06;          // Output = 1 Input = 0
  GPIO_PORTB_AFSEL_R &= 0x00;               
  GPIO_PORTB_DEN_R |= 0x06;
	SysTick_Init();
  // test DrawChar() and DrawCharS()
  ST7735_InitR(INITR_REDTAB);
  
  ST7735_OutString("Lab 7!\nWelcome to EE319K");
	IO_Touch();
	NVIC_ST_RELOAD_R = 1333333;
	NVIC_ST_CTRL_R += 0x02;
	EnableInterrupts();
	while(1){
	}
	/*while(1){
		IO_Touch();
		ST7735_FillScreen(0xFFFF); 
		z -= .125;
		makeSquare(-2, -5);
	}*/  
}
