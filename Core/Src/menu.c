#include "menu.h"

#define FIRSTLINE_Y 8

/**
 * showCalScreen()
 * @details: show calculation mode screen, call updateDisplay() and modifies DisplayMap[] in this function
 */
void showCalScreen()
{
    char mode[] = "mode1";
    clearDisplay(WHITE);
    setStr(mode, FIRSTLINE_Y, 39, BLACK);
    calData.cursorState = CURSOR_ST_1;
    updateDisplay();
}
/**
 * showSolvXScreen()
 * @details: show solve X mode screen, call updateDisplay() and modifies DisplayMap[] in this function
 */
void showSolvXScreen()
{
    char mode[] = "mode2";
    clearDisplay(WHITE);
    setStr(mode, FIRSTLINE_Y, 39, BLACK);
    calData.cursorState = CURSOR_ST_1;
    updateDisplay();
}
/**
 * showSolveXYScreen()
 * @details: show solve XY mode screen, call updateDisplay() and modifies DisplayMap[] in this function
 */
void showSolvXYScreen()
{
    char mode[] = "mode3";
    clearDisplay(WHITE);
    setStr(mode, 59, FIRSTLINE_Y, BLACK);
    calData.cursorState = CURSOR_ST_1;
    updateDisplay();
}
void showMenu()
{
    clearDisplay(WHITE);
    // MENU 20+3=23,(84-23)/2 =30
    char MENU[] = "MENU";
    char mode1[] = "1.CALCULATION";
    char mode2[] = "2.SOLVE F(X)";
    char mode3[] = "3.SOLVE F(X,Y)";
    setStr(MENU, 29, FIRSTLINE_Y, BLACK);
    setStr(mode1, 1, FIRSTLINE_Y + 3 + 9, BLACK);
    setStr(mode2, 1, FIRSTLINE_Y + 3 + 9 * 2, BLACK);
    setStr(mode3, 1, FIRSTLINE_Y + 3 + 9 * 3, BLACK);
    calData.cursorState = CURSOR_ST_NOTSHOW;
    updateDisplay();
}

