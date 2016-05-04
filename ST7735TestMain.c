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
#include "Sound.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"

void DelayWait10ms(uint32_t n);
void EnableInterrupts(void);
void DisableInterrupts(void);

double boxes[12] = {14,12,10,8,6,4,-2,-4,-6,-8,-10,-12};
uint8_t index_val = 0;
double zs[12] = {2,2,2,2,2,2,2,2,2,2,2,2};

uint32_t ADCMail;
uint32_t ADCStatus;

int score = 0;
uint16_t color = 0;
uint16_t bgColor = 0xFFFF;

uint8_t shield = 2;
uint8_t makeShieldCount = 0;
uint8_t immuneframes = 0;

uint8_t reset = 0;
double speed = .05;

void IO_Init(void);
void IO_HeartBeat(void);
void IO_Touch(void);

int absolute(int i){
	return (i>0) ? i : -i;
}
void line_bresenham(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	uint8_t swapped = 0;
	if(absolute(x2-x1) < absolute(y2 - y1)){
		swapped = 1;
		int x = x1;
		x1 = y1;
		y1 = x;
		x = x2;
		x2 = y2;
		y2 = x;
	}
	if(x2 - x1 < 0){
		int x = x1;
		x1 = x2;
		x2 = x;
		x = y1;
		y1 = y2;
		y2 = x;
	}
	int eps = 0;
	int dx = x2 - x1;
	int	dy = y2 - y1;
	uint16_t	y = y1;
	for(int x = x1; x <= x2; x++ ){
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
void line_bresenham3d(double x1, double y1, double z1, double x2, double y2, double z2, uint16_t color){
	int eps = 0;
	int rx1, rx2, ry1, ry2;
	if(z1 >= 0 && z2 >= 0){
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
void makePointer(int color, uint8_t position){
	line_bresenham(24 + position, 32, 21 + position, 28, color);
	line_bresenham(24 + position, 32, 27 + position, 28, color);
}
void makeShield(){
	line_bresenham(20, 74, 20, 86,0xFE00);
	line_bresenham(20, 86, 10, 92, 0xFE00);
	line_bresenham(20, 74, 10, 68, 0xFE00);
}
//
// Takes an x and y value as inputs and generates a square based on their positions
//
void makeSquare(int x, double y, double z, uint16_t color){
	if(y <= 0){
		line_bresenham3d(x, y, z, x, y, z+.125, color);
		line_bresenham3d(x-1, y, z, x-1, y, z+.125, color);
		line_bresenham3d(x, y-2, z, x, y-2, z+.125, color);
		line_bresenham3d(x, y, z+.125, x, y-2, z+.125, color);
		line_bresenham3d(x, y, z+.125, x-1, y, z+.125, color);
	}
	else{
		line_bresenham3d(x, y, z, x, y, z+.125, color);
		line_bresenham3d(x, y-2, z, x, y-2, z+.125, color);
		line_bresenham3d(x-1, y-2, z, x-1, y-2, z+.125, color);
		line_bresenham3d(x, y, z+.125, x, y-2, z+.125, color);
		line_bresenham3d(x, y-2, z+.125, x-1, y-2, z+.125, color);
	}
	line_bresenham3d(x, y, z, x-1, y, z, color);
	line_bresenham3d(x, y, z, x, y-2, z, color);
	line_bresenham3d(x-1, y-2, z, x-1, y, z, color);
	line_bresenham3d(x-1, y-2, z, x, y-2, z, color);
}
int32_t Convert(uint32_t input){
	int32_t result;
	result = (55459000+(943790*input))/1000000;
  return result;
}
void toString(int value){
	uint8_t offset = 0;
	while(value > 0){
		uint8_t digit = value%10 + 0x30;
		ST7735_DrawCharS2(153 - offset, 119, digit, color, bgColor, 1);
		offset += 6;
		value /= 10;
	}
}
void SysTick_Handler(void){
	score += 10;
	if(score == 10){
		color = 0;
		bgColor = 0xFFFF;
	}
	else if(score == 2500){
		bgColor = 0;
		color = 0x07E0;	
		ST7735_FillRect(82, 0, 128, 164, bgColor);
	}
	else if(score >= 7500){
		if(reset == 0)
			color = 0x6732;
		bgColor = 0;
		if(color < 0x0400 || color > 0xF000){
			if(reset == 2)
				color = 0x0100;
			reset = 1;
			color += 0x001F;
		}
		else{
			if(reset == 1)
				color = 0x6732;
			reset = 2;
			color -= 0xF060;
		}
	}
	if((GPIO_PORTE_DATA_R&0x10) == 0x10){
		NVIC_ST_CTRL_R -= 0x02; // Disable Systick
		Pause(0, color, bgColor);
	}
	if((GPIO_PORTE_DATA_R&0x20) == 0x20){
		// Trying to shield
		if(shield > 0 && makeShieldCount == 0){
			makeShieldCount = 30;
			shield--;
		}
	}
	if(makeShieldCount > 0){
		makeShieldCount--;
		makeShield();
	}
	ADCMail = ADC_In();  // sample 12-bit channel 1
  double shift_factor = ((double) Convert(ADCMail))/1000; 
	shift_factor -= 1;
	shift_factor *= .5;
	toString(score);
	ST7735_FillRect(0, 0, 82, 164, bgColor);	
	makeArrow(color);
	for(index_val = 0; index_val < 12; index_val++){
		if(boxes[index_val] == -99)
			Random_Gen(NVIC_ST_CURRENT_R);
		
		boxes[index_val] += shift_factor;
		makeSquare(-2, boxes[index_val], zs[index_val], color);
		if(zs[index_val] < .6){
			if(boxes[index_val] > -.75 && boxes[index_val] < 2.75 && immuneframes == 0){
				if(makeShieldCount > 0){
					makeShieldCount = 0;
					immuneframes = 10;
				}
				else{
					NVIC_ST_CTRL_R -= 0x02; // Disable Systick
					Pause(1, color, bgColor);
				}
			}
			if(immuneframes > 0)
				immuneframes--;
		}
		zs[index_val] -= speed;
	}
}
int main(void){  
  PLL_Init();  	// set system clock to 80 MHz
	ADC_Init();
	volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000030;  
  delay = SYSCTL_RCGCGPIO_R; 
	GPIO_PORTE_DIR_R &= ~0x30;          // Output = 1 Input = 0
  GPIO_PORTE_AFSEL_R &= 0x00;               
  GPIO_PORTE_DEN_R |= 0x30;
	GPIO_PORTF_DIR_R |= 0x0A;          // Output = 1 Input = 0
  GPIO_PORTF_AFSEL_R &= 0x00;               
  GPIO_PORTF_DEN_R |= 0x0A;
	SysTick_Init();
	Sound_Init();
	// Init's
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0xFFFF); 
	//Clear screen
	NVIC_ST_RELOAD_R = 2666666;
	//Start with Main Menu
	Pause(2, color, bgColor);
	EnableInterrupts();
	while(1){
	}
}
