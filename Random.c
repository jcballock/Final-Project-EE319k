#include <stdio.h>
#include <stdint.h>
#include "ST7735TestMain.h"
#include "tm4c123gh6pm.h"

void Random_Gen(int key){
	int value = 1664525*key+1013904223;
	double rand = ((value&0xFF000000)/0x01000000)+1;
	rand -= 14;
	boxes[index_val] = rand;
}
