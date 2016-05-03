#include <stdio.h>
#include <stdint.h>
#include "ST7735TestMain.h"
#include "ST7735.h"
#include "SysTick.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"

int mirror[12] = {14,12,10,8,6,4,-2,-4,-6,-8,-10,-12};
void Pause(int menu, uint16_t color, uint16_t bgcolor){
	if(menu == 0){
		ST7735_DrawCharS2(48, 80, 'P', color, bgcolor, 2);
		ST7735_DrawCharS2(60, 80, 'a', color, bgcolor, 2);
		ST7735_DrawCharS2(72, 80, 'u', color, bgcolor, 2);
		ST7735_DrawCharS2(84, 80, 's', color, bgcolor, 2);
		ST7735_DrawCharS2(96, 80, 'e', color, bgcolor, 2);
		ST7735_DrawCharS2(108, 80, 'd', color, bgcolor, 2);
		SysTick_Wait10ms(50);
		while((GPIO_PORTE_DATA_R&0x10) == 0x00){};
		while((GPIO_PORTE_DATA_R&0x10) == 0x10){};
		ST7735_FillRect(80, 0, 128, 160, bgcolor);
		NVIC_ST_CTRL_R += 0x02;
	}
	else if(menu == 1){
		ST7735_FillScreen(bgcolor);
		ST7735_DrawCharS2(24, 80, 'G', color, bgcolor, 2);
		ST7735_DrawCharS2(36, 80, 'a', color, bgcolor, 2);
		ST7735_DrawCharS2(48, 80, 'm', color, bgcolor, 2);
		ST7735_DrawCharS2(60, 80, 'e', color, bgcolor, 2);
		ST7735_DrawCharS2(72, 80, ' ', color, bgcolor, 2);
		ST7735_DrawCharS2(84, 80, 'O', color, bgcolor, 2);
		ST7735_DrawCharS2(96, 80, 'v', color, bgcolor, 2);
		ST7735_DrawCharS2(108, 80, 'e', color, bgcolor, 2);
		ST7735_DrawCharS2(120, 80, 'r', color, bgcolor, 2);
		ST7735_DrawCharS2(132, 80, '!', color, bgcolor, 2);
		//
		ST7735_DrawCharS2(36, 60, 'S', color, bgcolor, 1);
		ST7735_DrawCharS2(42, 60, 'c', color, bgcolor, 1);
		ST7735_DrawCharS2(48, 60, 'o', color, bgcolor, 1);
		ST7735_DrawCharS2(54, 60, 'r', color, bgcolor, 1);
		ST7735_DrawCharS2(60, 60, 'e', color, bgcolor, 1);
		ST7735_DrawCharS2(66, 60, ':', color, bgcolor, 1);
		//
		ST7735_DrawCharS2(36, 40, 'P', color, bgcolor, 1);
		ST7735_DrawCharS2(42, 40, 'l', color, bgcolor, 1);
		ST7735_DrawCharS2(48, 40, 'a', color, bgcolor, 1);
		ST7735_DrawCharS2(54, 40, 'y', color, bgcolor, 1);
		ST7735_DrawCharS2(60, 40, ' ', color, bgcolor, 1);
		ST7735_DrawCharS2(66, 40, 'A', color, bgcolor, 1);
		ST7735_DrawCharS2(72, 40, 'g', color, bgcolor, 1);
		ST7735_DrawCharS2(78, 40, 'a', color, bgcolor, 1);
		ST7735_DrawCharS2(84, 40, 'i', color, bgcolor, 1);
		ST7735_DrawCharS2(90, 40, 'n', color, bgcolor, 1);
		ST7735_DrawCharS2(96, 40, '!', color, bgcolor, 1);
		//
		ST7735_DrawCharS2(36, 20, 'M', color, bgcolor, 1);
		ST7735_DrawCharS2(42, 20, 'a', color, bgcolor, 1);
		ST7735_DrawCharS2(48, 20, 'i', color, bgcolor, 1);
		ST7735_DrawCharS2(54, 20, 'n', color, bgcolor, 1);
		ST7735_DrawCharS2(60, 20, ' ', color, bgcolor, 1);
		ST7735_DrawCharS2(66, 20, 'M', color, bgcolor, 1);
		ST7735_DrawCharS2(72, 20, 'e', color, bgcolor, 1);
		ST7735_DrawCharS2(78, 20, 'n', color, bgcolor, 1);
		ST7735_DrawCharS2(84, 20, 'u', color, bgcolor, 1);
		uint8_t offset, count = 0;
		int value = score;
		int size=1;
		while(value > 9){ 
			size++; 
			value/=10; 
		}
		char digits[size];
		value = score;
		while(value > 0){
			uint8_t digit = value%10 + 0x30;
			digits[count] = digit;
			count++;
			value /= 10;
		}
		for(int i = (size - 1); i >= 0; i--){
			ST7735_DrawCharS2(78 + offset, 60, digits[i], color, bgColor, 1);
			offset += 6;
		}
		uint8_t position = 0;
		while((GPIO_PORTE_DATA_R&0x10) == 0x00){
			SysTick_Wait10ms(50);
			ST7735_FillRect(15, 28, 36, 6, bgcolor);
			makePointer(color, position);
			value = ADC_In();
			uint32_t conversion = Convert(value);
			if(conversion > 1500){
				position = 0;
			}
			else if(conversion < 500){
				position = 20;
			}
		}
		while((GPIO_PORTE_DATA_R&0x10) == 0x10){};
		ST7735_FillRect(80, 0, 128, 160, 0xFFFF);
		for(int i = 0; i < 12; i++){
			boxes[i] = mirror[i];
			zs[i] = 2;
		}
		SysTick_Wait10ms(15);		
		score = 0;
		if(position == 20){
			color = 0;
			bgColor = 0xFFFF;
			NVIC_ST_CTRL_R += 0x02;
		}
		else
			Pause(2, color, bgcolor);
	}
	else{
		ST7735_FillScreen(bgcolor);
		ST7735_DrawCharS2(24, 90, 'M', color, bgcolor, 2);
		ST7735_DrawCharS2(36, 90, 'a', color, bgcolor, 2);
		ST7735_DrawCharS2(48, 90, 'i', color, bgcolor, 2);
		ST7735_DrawCharS2(60, 90, 'n', color, bgcolor, 2);
		ST7735_DrawCharS2(72, 90, ' ', color, bgcolor, 2);
		ST7735_DrawCharS2(84, 90, 'M', color, bgcolor, 2);
		ST7735_DrawCharS2(96, 90, 'e', color, bgcolor, 2);
		ST7735_DrawCharS2(108, 90, 'n', color, bgcolor, 2);
		ST7735_DrawCharS2(120, 90, 'u', color, bgcolor, 2);
		//
		ST7735_DrawCharS2(36, 60, 'E', color, bgcolor, 1);
		ST7735_DrawCharS2(42, 60, 'a', color, bgcolor, 1);
		ST7735_DrawCharS2(48, 60, 's', color, bgcolor, 1);
		ST7735_DrawCharS2(54, 60, 'y', color, bgcolor, 1);
		//
		ST7735_DrawCharS2(36, 40, 'M', color, bgcolor, 1);
		ST7735_DrawCharS2(42, 40, 'e', color, bgcolor, 1);
		ST7735_DrawCharS2(48, 40, 'd', color, bgcolor, 1);
		ST7735_DrawCharS2(54, 40, 'i', color, bgcolor, 1);
		ST7735_DrawCharS2(60, 40, 'u', color, bgcolor, 1);
		ST7735_DrawCharS2(66, 40, 'm', color, bgcolor, 1);
		//
		ST7735_DrawCharS2(36, 20, 'H', color, bgcolor, 1);
		ST7735_DrawCharS2(42, 20, 'a', color, bgcolor, 1);
		ST7735_DrawCharS2(48, 20, 'r', color, bgcolor, 1);
		ST7735_DrawCharS2(54, 20, 'd', color, bgcolor, 1);
		int value = 0;
		uint8_t position = 0;
		while((GPIO_PORTE_DATA_R&0x10) == 0x00){
			SysTick_Wait10ms(75);
			ST7735_FillRect(15, 28, 56, 6, bgcolor);
			makePointer(color, position);
			value = ADC_In();
			uint32_t conversion = Convert(value);
			if(conversion > 1500){
				if(position != 0)
					position -= 20;
				else
					position = 0;
			}	
			else if(conversion < 500){
				if(position != 40)
					position += 20;
				else
					position = 40;
			}
		}
		while((GPIO_PORTE_DATA_R&0x10) == 0x10){};
		ST7735_FillRect(80, 0, 128, 160, 0xFFFF);
		for(int i = 0; i < 12; i++){
			boxes[i] = mirror[i];
			zs[i] = 2;
		}
		SysTick_Wait10ms(15);		
		color = 0;
		bgColor = 0xFFFF;
		score = 0;
		if(position == 40)
			speed = 0.03;
		else if (position == 20)
			speed = 0.05;
		else
			speed = 0.07;
		NVIC_ST_CTRL_R += 0x02;
		
	}
}
