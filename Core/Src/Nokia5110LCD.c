/**
 * @file Nokia5110LCD.c
 * @brief Nokia5110LCD函数库（stm32f103）
 *
 * ! This file is grabbed from "github.com/sparkfun/GraphicLCD_Nokia_5110" and I replace some functions with HAL lib to enable stm32f103 to use Nokia5110LCD.
 * 本Nokia的驱动文件由Arduino版移植而来，修改了相应的函数，以便供stm32使用,注意本文件具有通用性，具体相关功能可以include"Nokia5110LCD.h" 并添加
 *
 * @author Jingyuan Li (nearfar1jy@email.com)
 * @version 1.0
 * @date 2023-12-12
 *
 * @copyright Copyright (c) 2023 Jingyuan Li
 *
 * @usage: 函数库包含3个文件，Nokia5110LCD.h,Nokia5110LCD.c,Nokia5110LDCbitFig.c
 * .h文件前置文件为<stdint.h>, <stdlib.h>, <math.h>, "spi.h" (by STM32CubeMX), "stm32f1xx_hal.h" (by STM32CubeMX)
 * Nokia5110LCD工作流程：详见 lcdbegin()
 *
 */
#include "Nokia5110LCD.h"

#define PI 3.1415926

/* Pin definitions:
Most of these pins can be moved to any digital or analog pin.
DN(MOSI)and SCLK should be left where they are (SPI pins). The
LED (backlight) pin should remain on a PWM-capable pin. */

// PB13: SPI2_SCK
// PB14: SPI2_MISO
// PB15: SPI2_MOSI
GPIO_TypeDef *lcdPort = GPIOB;
const uint16_t scePin = GPIO_PIN_12;  // SCE - Chip select, pin 3 on LCD.
const uint16_t rstPin = GPIO_PIN_8;  // RST - Reset, pin 4 on LCD.
const uint16_t dcPin = GPIO_PIN_7;   // DC - Data/Command, pin 5 on LCD.
const uint16_t sdinPin = GPIO_PIN_15; // DN(MOSI) - Serial data, pin 6 on LCD.
const uint16_t sclkPin = GPIO_PIN_13; // SCLK - Serial clock, pin 7 on LCD.
const uint16_t blPin = GPIO_PIN_9;    // LED - Backlight LED, pin 8 on LCD.

/* Font table:
This table contains the hex values that represent pixels for a
font that is 5 pixels wide and 8 pixels high. Each byte in a row
represents one, 8-pixel, vertical column of a character. 5 bytes
per character. */
static const uint8_t ASCII[][5] = {
    // First 32 characters (0x00-0x19) are ignored. These are
    // non-displayable, control characters.
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 0x20
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // 0x21 !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 0x22 "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // 0x23 #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // 0x24 $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 0x25 %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 0x26 &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 0x27 '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // 0x28 (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // 0x29 )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // 0x2a *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // 0x2b +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 0x2c ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 0x2d -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 0x2e .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 0x2f /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0x30 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 0x31 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 0x32 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 0x33 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 0x34 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 0x35 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 0x36 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 0x37 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 0x38 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 0x39 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 0x3a :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 0x3b ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 0x3c <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 0x3d =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 0x3e >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 0x3f ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // 0x40 @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // 0x41 A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // 0x42 B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // 0x43 C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // 0x44 D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // 0x45 E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // 0x46 F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // 0x47 G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // 0x48 H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // 0x49 I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // 0x4a J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // 0x4b K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // 0x4c L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // 0x4d M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // 0x4e N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // 0x4f O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // 0x50 P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // 0x51 Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // 0x52 R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 0x53 S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // 0x54 T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // 0x55 U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // 0x56 V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // 0x57 W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 0x58 X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 0x59 Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 0x5a Z
    {0x00, 0x7f, 0x41, 0x41, 0x00}, // 0x5b [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // 0x5c \ (keep this to escape the backslash)
    {0x00, 0x41, 0x41, 0x7f, 0x00}, // 0x5d ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // 0x5e ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // 0x5f _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // 0x60 `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 0x61 a
    {0x7f, 0x48, 0x44, 0x44, 0x38}, // 0x62 b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // 0x63 c
    {0x38, 0x44, 0x44, 0x48, 0x7f}, // 0x64 d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // 0x65 e
    {0x08, 0x7e, 0x09, 0x01, 0x02}, // 0x66 f
    {0x0c, 0x52, 0x52, 0x52, 0x3e}, // 0x67 g
    {0x7f, 0x08, 0x04, 0x04, 0x78}, // 0x68 h
    {0x00, 0x44, 0x7d, 0x40, 0x00}, // 0x69 i
    {0x20, 0x40, 0x44, 0x3d, 0x00}, // 0x6a j
    {0x7f, 0x10, 0x28, 0x44, 0x00}, // 0x6b k
    {0x00, 0x41, 0x7f, 0x40, 0x00}, // 0x6c l
    {0x7c, 0x04, 0x18, 0x04, 0x78}, // 0x6d m
    {0x7c, 0x08, 0x04, 0x04, 0x78}, // 0x6e n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 0x6f o
    {0x7c, 0x14, 0x14, 0x14, 0x08}, // 0x70 p
    {0x08, 0x14, 0x14, 0x18, 0x7c}, // 0x71 q
    {0x7c, 0x08, 0x04, 0x04, 0x08}, // 0x72 r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 0x73 s
    {0x04, 0x3f, 0x44, 0x40, 0x20}, // 0x74 t
    {0x3c, 0x40, 0x40, 0x20, 0x7c}, // 0x75 u
    {0x1c, 0x20, 0x40, 0x20, 0x1c}, // 0x76 v
    {0x3c, 0x40, 0x30, 0x40, 0x3c}, // 0x77 w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 0x78 x
    {0x0c, 0x50, 0x50, 0x50, 0x3c}, // 0x79 y
    {0x44, 0x64, 0x54, 0x4c, 0x44}, // 0x7a z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // 0x7b {
    {0x00, 0x00, 0x7f, 0x00, 0x00}, // 0x7c |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // 0x7d }
    {0x10, 0x08, 0x08, 0x10, 0x08}, // 0x7e ~
    {0x78, 0x46, 0x41, 0x46, 0x78}, // 0x7f DEL
};

/* The displayMap variable stores a buffer representation of the
pixels on our display. There are 504 total bits in this array,
same as how many pixels there are on a 84 x 48 display.

Each byte in this array covers a 8-pixel vertical block on the
display. Each successive byte covers the next 8-pixel column over
until you reach the right-edge of the display and step down 8 rows.

To update the display, we first have to write to this array, then
call the updateDisplay() function, which sends this whole array
to the PCD8544.*/

// V=0 horizonal mode -> ^
uint8_t displayMap[504] = {0};

// There are two memory banks in the LCD, data/RAM and commands.
// This function sets the DC pin high or low depending, and then
// sends the data byte
void LCDWrite(uint8_t data_or_command, uint8_t data)
{
  // Tell the LCD that we are writing either to data or a command
  // D/~C = 1 data
  HAL_GPIO_WritePin(lcdPort, dcPin, data_or_command ? GPIO_PIN_SET : GPIO_PIN_RESET);

  // Send the data
  HAL_GPIO_WritePin(lcdPort, scePin, GPIO_PIN_RESET);
  // SPI.transfer(data); // shiftOut(sdinPin, sclkPin, MSBFIRST, data);
  HAL_SPI_Transmit(&hspi2, &data, 1, 100);
  HAL_GPIO_WritePin(lcdPort, scePin, GPIO_PIN_SET);
}

// This function sets a pixel on displayMap to your preferred
// color. 1=Black, 0= white.
void setPixel(int x, int y, uint8_t bw)
{
  // First, double check that the coordinate is in range.
  if ((x >= 0) && (x < LCD_WIDTH) && (y >= 0) && (y < LCD_HEIGHT))
  {
    uint8_t shift = y % 8;

    if (bw) // If black, set the bit.
      displayMap[x + (y / 8) * LCD_WIDTH] |= 1 << shift;
    else // If white clear the bit.
      displayMap[x + (y / 8) * LCD_WIDTH] &= ~(1 << shift);
  }
}

void clearPixel(int x, int y)
{
  setPixel(x, y, WHITE); // call setPixel with bw set to white
}

// setLine draws a line from x0,y0 to x1,y1 with the set color.
// This function was grabbed from the SparkFun ColorLCDShield
// library.
void setLine(int x0, int y0, int x1, int y1, uint8_t bw)
{
  int dy = y1 - y0; // Difference between y0 and y1
  int dx = x1 - x0; // Difference between x0 and x1
  int stepx, stepy;

  if (dy < 0)
  {
    dy = -dy;
    stepy = -1;
  }
  else
    stepy = 1;

  if (dx < 0)
  {
    dx = -dx;
    stepx = -1;
  }
  else
    stepx = 1;

  dy <<= 1;             // dy is now 2*dy
  dx <<= 1;             // dx is now 2*dx
  setPixel(x0, y0, bw); // Draw the first pixel.

  if (dx > dy)
  {
    int fraction = dy - (dx >> 1);
    while (x0 != x1)
    {
      if (fraction >= 0)
      {
        y0 += stepy;
        fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      setPixel(x0, y0, bw);
    }
  }
  else
  {
    int fraction = dx - (dy >> 1);
    while (y0 != y1)
    {
      if (fraction >= 0)
      {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      setPixel(x0, y0, bw);
    }
  }
}

// setRect will draw a rectangle from x0,y0 top-left corner to
// a x1,y1 bottom-right corner. Can be filled with the fill
// parameter, and colored with bw.
// This function was grabbed from the SparkFun ColorLCDShield
// library.
void setRect(int x0, int y0, int x1, int y1, uint8_t fill, uint8_t bw)
{
  // check if the rectangle is to be filled
  if (fill == 1)
  {
    int xDiff;

    if (x0 > x1)
      xDiff = x0 - x1; // Find the difference between the x vars
    else
      xDiff = x1 - x0;

    while (xDiff > 0)
    {
      setLine(x0, y0, x0, y1, bw);

      if (x0 > x1)
        x0--;
      else
        x0++;

      xDiff--;
    }
  }
  else
  {
    // best way to draw an unfilled rectangle is to draw four lines
    setLine(x0, y0, x1, y0, bw);
    setLine(x0, y1, x1, y1, bw);
    setLine(x0, y0, x0, y1, bw);
    setLine(x1, y0, x1, y1, bw);
  }
}

// setCircle draws a circle centered around x0,y0 with a defined
// radius. The circle can be black or white. And have a line
// thickness ranging from 1 to the radius of the circle.
// This function was grabbed from the SparkFun ColorLCDShield
// library.
void setCircle(int x0, int y0, int radius, uint8_t bw, int lineThickness)
{
  for (int r = 0; r < lineThickness; r++)
  {
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    setPixel(x0, y0 + radius, bw);
    setPixel(x0, y0 - radius, bw);
    setPixel(x0 + radius, y0, bw);
    setPixel(x0 - radius, y0, bw);

    while (x < y)
    {
      if (f >= 0)
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;

      setPixel(x0 + x, y0 + y, bw);
      setPixel(x0 - x, y0 + y, bw);
      setPixel(x0 + x, y0 - y, bw);
      setPixel(x0 - x, y0 - y, bw);
      setPixel(x0 + y, y0 + x, bw);
      setPixel(x0 - y, y0 + x, bw);
      setPixel(x0 + y, y0 - x, bw);
      setPixel(x0 - y, y0 - x, bw);
    }
    radius--;
  }
}

// This function will draw a char (defined in the ASCII table
// near the beginning of this sketch) at a defined x and y).
// The color can be either black (1) or white (0).
void setChar(char character, int x, int y, uint8_t bw)
{
  uint8_t column;             // temp byte to store character's column bitmap
  for (int i = 0; i < 5; i++) // 5 columns (x) per character
  {
    column = ASCII[character - 0x20][i];
    for (int j = 0; j < 8; j++) // 8 rows (y) per character
    {
      if (column & (0x01 << j)) // test bits to set pixels
        setPixel(x + i, y + j, bw);
      else
        setPixel(x + i, y + j, !bw);
    }
  }
}

// setStr draws a string of characters, calling setChar with
// progressive coordinates until it's done.
// This function was grabbed from the SparkFun ColorLCDShield
// library.
void setStr(char *dString, int x, int y, uint8_t bw)
{
  while (*dString != 0x00) // loop until null terminator
  {
    setChar(*dString++, x, y, bw);
    x += 5;
    for (int i = y; i < y + 8; i++)
    {
      setPixel(x, i, !bw);
    }
    x++;
    if (x > (LCD_WIDTH - 5)) // Enables wrap around
    {
      x = 0;
      y += 8;
    }
  }
}

// setStrWithL draws a string of characters with length L
void setStrWithL(char *dString, int length, int x, int y, uint8_t bw)
{
  for (int l = 0; (*dString != 0x00) && (l < length); l++)
  {
    setChar(*dString++, x, y, bw);
    x += 5;
    for (int i = y; i < y + 8; i++)
    {
      setPixel(x, i, !bw);
    }
    x++;
    if (x > (LCD_WIDTH - 5)) // Enables wrap around
    {
      x = 0;
      y += 8;
    }
  }
}

// Helpful function to directly command the LCD to go to a specific x,y coordinate.
void gotoXY(int x, int y)
{
  LCDWrite(0, 0x80 | x); // Column.
  LCDWrite(0, 0x40 | y); // Row.  ?
}

// This will actually draw on the display, whatever is currently
// in the displayMap array.
void updateDisplay()
{
  gotoXY(0, 0);
  for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    LCDWrite(LCD_DATA, displayMap[i]);
  }
}

// This function will draw an array over the screen. (For now) the
// array must be the same size as the screen, covering the entirety
// of the display.
// Also, the array must reside in FLASH and declared with PROGMEM.
void setBitmap(const uint8_t *bitArray)
{
  for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    uint8_t c = bitArray[i];
    displayMap[i] = c;
  }
}

// This function clears the entire display either white (0) or
// black (1).
// The screen won't actually clear until you call updateDisplay()!
void clearDisplay(uint8_t bw)
{
  for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    if (bw)
      displayMap[i] = 0xFF;
    else
      displayMap[i] = 0;
  }
}

// Set contrast can set the LCD Vop to a value between 0 and 127.
// 40-60 is usually a pretty good range.
void setContrast(uint8_t contrast)
{
  LCDWrite(LCD_COMMAND, 0x21);            // Tell LCD that extended commands follow
  LCDWrite(LCD_COMMAND, 0x80 | contrast); // Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  LCDWrite(LCD_COMMAND, 0x20);            // Set display mode
}

/* There are two ways to do this. Either through direct commands
to the display, or by swapping each bit in the displayMap array.
We'll leave both methods here, comment one or the other out if
you please. */
void invertDisplay(void)
{
  /* Direct LCD Command option
  LCDWrite(LCD_COMMAND, 0x20); //Tell LCD that extended commands follow
  LCDWrite(LCD_COMMAND, 0x08 | 0x05); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  LCDWrite(LCD_COMMAND, 0x20); //Set display mode  */

  /* Indirect, swap bits in displayMap option: */
  for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    displayMap[i] = ~displayMap[i] & 0xFF;
  }
  updateDisplay();
}

// This sends the magical commands to the PCD8544
void lcdBegin(void)
{
  // Configure control pins
  // analogWrite(blPin, 255);

  // SPI.begin();
  // SPI.setDataMode(SPI_MODE0);
  // SPI.setBitOrder(MSBFIRST);

  // Reset the LCD to a known state
  HAL_GPIO_WritePin(lcdPort, rstPin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(lcdPort, rstPin, GPIO_PIN_SET);

  LCDWrite(LCD_COMMAND, 0x21); // Tell LCD extended commands follow
  LCDWrite(LCD_COMMAND, 0xc0); // Set LCD Vop (Contrast)
  LCDWrite(LCD_COMMAND, 0x04); // Set Temp coefficent
  LCDWrite(LCD_COMMAND, 0x13); // LCD bias mode 1:48 (try 0x13)
  // We must send 0x20 before modifying the display control mode
  LCDWrite(LCD_COMMAND, 0x20);
  LCDWrite(LCD_COMMAND, 0x0C); // Set display control, normal mode.

  updateDisplay();
}

/* This function serves as a fun demo of the graphics driver
functions below. */
void lcdFunTime(void)
{
  clearDisplay(WHITE); // Begin by clearing the display
  // srand((unsigned)time(NULL));

  /* setPixel Example */
  const int pixelCount = 100;
  for (int i = 0; i < pixelCount; i++)
  {
    // setPixel takes 2 to 3 parameters. The first two params
    // are x and y variables. The third optional variable is
    // a "color" boolean. 1 for black, 0 for white.
    // setPixel() with two variables will set the pixel with
    // the color set to black.
    // clearPixel() will call setPixel with with color set to
    // white.
    setPixel(rand() % LCD_WIDTH, rand() % LCD_HEIGHT, BLACK);
    // After drawing something, we must call updateDisplay()
    // to actually make the display draw something new.
    updateDisplay();
    HAL_Delay(10);
  }
  setStr("full of stars", 0, LCD_HEIGHT - 8, BLACK);
  updateDisplay();
  HAL_Delay(1000);
  // Seizure time!!! Err...demoing invertDisplay()
  for (int i = 0; i < 5; i++)
  {
    invertDisplay(); // This will swap all bits in our display
    HAL_Delay(200);
    invertDisplay(); // This will get us back to where we started
    HAL_Delay(200);
  }
  HAL_Delay(2000);

  /* setLine Example */
  clearDisplay(WHITE); // Start fresh
  int x0 = LCD_WIDTH / 2;
  int y0 = LCD_HEIGHT / 2;
  for (float i = 0; i < 2 * PI; i += PI / 8)
  {
    // Time to whip out some maths:
    const int lineLength = 24;
    int x1 = x0 + lineLength * sin(i);
    int y1 = y0 + lineLength * cos(i);

    // setLine(x0, y0, x1, y1, bw) takes five variables. The
    // first four are coordinates for the start and end of the
    // line. The last variable is the color (1=black, 0=white).
    setLine(x0, y0, x1, y1, BLACK);
    updateDisplay();
    HAL_Delay(100);
  }
  // Demo some backlight tuning
  for (int j = 0; j < 2; j++)
  {
    for (int i = 255; i >= 0; i -= 5)
    {
      // analogWrite(blPin, i); // blPin is ocnnected to BL LED
      // HAL_Delay(20);
      HAL_Delay(2);
    }
    for (int i = 0; i < 256; i += 5)
    {
      // analogWrite(blPin, i);
      // HAL_Delay(20);
      HAL_Delay(2);
    }
  }

  /* setRect Example */
  clearDisplay(WHITE); // Start fresh

  // setRect takes six parameters (x0, y0, x1, y0, fill, bw)
  // x0, y0, x1, and y0 are the two diagonal corner coordinates
  // fill is a boolean, which determines if the rectangle is
  // filled in. bw determines the color 0=white, 1=black.
  for (int x = 0; x < LCD_WIDTH; x += 8)
  { // Swipe right black
    setRect(0, 0, x, LCD_HEIGHT, 1, BLACK);
    updateDisplay();
    HAL_Delay(10);
  }
  for (int x = 0; x < LCD_WIDTH; x += 8)
  { // Swipe right white
    setRect(0, 0, x, LCD_HEIGHT, 1, WHITE);
    updateDisplay();
    HAL_Delay(10);
  }
  for (int x = 0; x < 12; x++)
  { // Shutter swipe
    setRect(0, 0, x, LCD_HEIGHT, 1, 1);
    setRect(11, 0, x + 12, LCD_HEIGHT, 1, BLACK);
    setRect(23, 0, x + 24, LCD_HEIGHT, 1, BLACK);
    setRect(35, 0, x + 36, LCD_HEIGHT, 1, BLACK);
    setRect(47, 0, x + 48, LCD_HEIGHT, 1, BLACK);
    setRect(59, 0, x + 60, LCD_HEIGHT, 1, BLACK);
    setRect(71, 0, x + 72, LCD_HEIGHT, 1, BLACK);
    updateDisplay();
    HAL_Delay(10);
  }
  // 3 Dee!
  setRect(25, 10, 45, 30, 0, WHITE);
  setRect(35, 20, 55, 40, 0, WHITE);
  setLine(25, 10, 35, 20, WHITE);
  setLine(45, 30, 55, 40, WHITE);
  setLine(25, 30, 35, 40, WHITE);
  setLine(45, 10, 55, 20, WHITE);
  updateDisplay();
  HAL_Delay(2000);

  /* setCircle Example */
  clearDisplay(WHITE);
  // setCircle takes 5 parameters -- x0, y0, radius, bw, and
  // lineThickness. x0 and y0 are the center coords of the circ.
  // radius is the...radius. bw is the color (0=white, 1=black)
  // lineThickness is the line width of the circle, 1 = smallest
  // thickness moves in towards center.
  for (int i = 0; i < 20; i++)
  {
    int x = rand() % LCD_WIDTH;
    int y = rand() % LCD_HEIGHT;

    setCircle(x, y, i, BLACK, 1);
    updateDisplay();
    HAL_Delay(100);
  }
  HAL_Delay(2000);

  /* setChar & setStr Example */
  // setStr takes 4 parameters: an array of characters to print,
  // x and y coordinates for the top-left corner. And a color
  setStr("Modern Art", 0, 10, WHITE);
  updateDisplay();
  HAL_Delay(2000);

  /* setBitmap Example */
  // setBitmap takes one parameter, an array of the same size
  // as our screen.
  setBitmap(xkcdSandwich);
  updateDisplay();
}
