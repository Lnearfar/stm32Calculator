#ifndef __NOKIA5110LCD_H
#define __NOKIA5110LCD_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "spi.h"
#include "stm32f1xx_hal.h"

/* PCD8544-specific defines: */
#define LCD_COMMAND 0
#define LCD_DATA 1

/* 84x48 LCD Defines: */
#define LCD_WIDTH 84  // Note: x-coordinates go wide
#define LCD_HEIGHT 48 // Note: y-coordinates go high
#define WHITE 0       // For drawing pixels. A 0 draws white.
#define BLACK 1       // A 1 draws black.

extern const uint8_t ASCII[][5];
extern uint8_t displayMap[LCD_WIDTH * LCD_HEIGHT / 8];
void LCDWrite(uint8_t data_or_command, uint8_t data);
void setPixel(int x, int y, uint8_t bw);
void clearPixel(int x, int y);
void gotoXY(int x, int y);
void updateDisplay(void);
void setBitmap(const uint8_t *bitArray);
void clearDisplay(uint8_t bw);
void lcdBegin(void);

void setLine(int x0, int y0, int x1, int y1, uint8_t bw);
void setRect(int x0, int y0, int x1, int y1, uint8_t fill, uint8_t bw);
void setCircle(int x0, int y0, int radius, uint8_t bw, int lineThickness);
void setChar(char character, int x, int y, uint8_t bw);
void setStr(char *dString, int x, int y, uint8_t bw);
void setStrWithL(char *dString, int length, int x, int y, uint8_t bw);
void setContrast(uint8_t contrast);
void invertDisplay(void);
void lcdFunTime(void);

/* some fun byte figures*/
extern const uint8_t xkcdSandwich[504];
extern const uint8_t FireLogo[504];

#endif
