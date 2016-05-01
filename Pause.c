#include <stdio.h>
#include <stdint.h>
#include "ST7735TestMain.h"
#include "ST7735.h"
#include "SysTick.h"
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
	else{
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
		SysTick_Wait10ms(50);
		while((GPIO_PORTE_DATA_R&0x10) == 0x00){};
		while((GPIO_PORTE_DATA_R&0x10) == 0x10){};
		ST7735_FillRect(80, 0, 128, 160, 0xFFFF);
		for(int i = 0; i < 12; i++){
			boxes[i] = mirror[i];
			zs[i] = 2;
		}
		SysTick_Wait10ms(5);		
		color = 0;
		bgColor = 0xFFFF;
		score = 0;
		NVIC_ST_CTRL_R += 0x02;
		
	}
}
