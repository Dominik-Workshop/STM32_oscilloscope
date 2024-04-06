/*
 * lcd.h
 *
 *  Created on: Apr 6, 2024
 *      Author: Dominik
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"
#include "ili9488.h"
#include "xpt2046.h"


#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define LCD_BRIGHTNESS 800 // 0-1000

#define MEMORY_DEPTH  480//512

#define CANVA_MIDDLE_V 170

void drawGrid();


#endif