#ifndef _NOKIA5110LCD_H
#define _NOKIA5110LCD_H

#include <stdint.h>
#include"spi.h"

/* PCD8544-specific defines: */
#define LCD_COMMAND  0
#define LCD_DATA     1

/* 84x48 LCD Defines: */
#define LCD_WIDTH   84 // Note: x-coordinates go wide
#define LCD_HEIGHT  48 // Note: y-coordinates go high
#define WHITE       0  // For drawing pixels. A 0 draws white.
#define BLACK       1  // A 1 draws black.

extern const uint8_t ASCII[][5];
extern uint8_t displayMap[LCD_WIDTH * LCD_HEIGHT / 8];
void LCDWrite(uint8_t data_or_command, uint8_t data);
void setPixel(int x, int y,uint8_t bw);
void clearPixel(int x, int y);
void gotoXY(int x, int y);
void updateDisplay(void);
void clearDisplay(uint8_t bw);
void lcdBegin(void);

#endif
