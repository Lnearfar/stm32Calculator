/**
 * @file calculatorGP.c
 * @brief 本计算器的“gpu显卡”，Graphical process, 将若干缓冲区buf转化为bitmap显示
 *
 * 修改bitmap逻辑为：
 * bitmap:
 * y
 * ^--------------------------------------------
 * |                                           |
 * |                                           |
 * |                                           |
 * |                                           |
 * |                                           |
 * |                                           |
 * |         (x,y) [x+(y/8)*84]里面第(y%8位)     |
 * |                                           |
 * (0,0)[0]-------------------------------(83,0)[84] ->x
 * setChar(char c,int x,int y, uint8_t BLACK or WHITE)
 * setStr(char *str, int x,int y, uint8_t BLACK or WHITE)
 *
 * 执行updateDisplay()将会显示displayMap内容
 *
 * @author Jingyuan Li (nearfar1jy@gmail.com)
 * @version 1.0
 * @date 2023-12-12
 *
 * @copyright Copyright (c) 2023 Jingyuan Li
 *
 * @usage: 本文件基于Nokia5110LCD.h,在使用前，必须在主函数执行初始化InitCalculatorGP()
 * claculatorGP.h前置文件为"Nokia5110LCD.h","button.h"
 * 执行流程如下：
 * 1. 根据按键键盘的button_type, 运行updateEquationString(button_type) 更新calData内的与equation字符串，answer字符串以及与显示内容相关的信息
 * 2. 执行updateCalDisplay(),调用Nokia5110LCD库修改DisplayMap并输出结果
 *
 */
#include "calculatorGP.h"
/* Private vairable and definition start*/
#define EQ_Y 18
#define FIRSTLINE_Y 8
uint8_t dispMode; // modeState==0 -> in calculation or solving mode; modeState ==1 -> now is showing menu.

// 包含equation,answer,cursor,button
Struct_Data_Buffer calData;
char butValString[7] = 0;
/**光标闪烁设计思路
 * 定时器定时0.5s修改calData.cursorState的值，修改值的同时修改displayMap里面对应X列(SetLine(cursorX,y,cursorX,y+7,BLACK/WHITE)
 */

/* Private vairable and definition end*/

void InitCalculatorGP(void)
{
    calData.frameStart1 = 0xAA;
    calData.frameStart2 = 0xAA;
    calData.frameEnd1 = 0xFF;
    calData.frameEnd2 = 0xFF;
    memset(calData.equationStr, '\0', sizeof(calData.equationStr));
    calData.equationStrLength = 0;
    calData.eqSTptr = 0;
    calData.eqEDptr = 0;
    calData.answer = 0;
    memset(calData.answerStr, '\0', sizeof(calData.answerStr));
    calData.answerStrLength = 0;
    calData.answerNeedClear = 0;
    calData.cursorInEqString = 0;
    calData.cursorScreenX = 0;
    calData.cursorState = CURSOR_ST_1;
    calData.buttonType = BUT_NO_PRESS;

    dispMode = DISPMODE_MENU;
}

/**
 * 如果上位机传过来按键信息，或者按键有输入，获得button_type，用来更新calData.buttonType,这个calData.buttonType，在执行完updateEquationString，就会被置为BUT_NO_PRESS,
 * 在button中应当设有判断button_type有效性的程序
 * 此处接收到button_type，就当成有一次输入，运行程序并执行结果
 */
void updateButtonState(uint8_t button_type)
{
    calData.buttonType = button_type;
}

/**
 * but2str
 * @param uint8_t button_type (=BUT_xxx)
 * @details: 根据button_type，修改字符串 butValString 的值
 * 因为键入平方的时候，应该是^2，会有多字符，因此应该转为字符串.
 */
void but2str(uint8_t button_type)
{
    memset(butValString, '\0', sizeof(butValString));
    if (button_type == BUT_NO_PRESS)
    {
        return;
    }
    else if (button_type <= BUT_9)
    {
        butValString[0] = '0' + button_type - 1; // BUT0-9
    }
    else if (button_type == BUT_ADD)
    {
        butValString[0] = '+';
    }
    else if (button_type == BUT_MINUS)
    {
        butValString[0] = '-';
    }
    else if (button_type == BUT_MUL)
    {
        butValString[0] = '*';
    }
    else if (button_type == BUT_DIV)
    {
        butValString[0] = '/';
    }
    else if (button_type == BUT_DOT)
    {
        butValString[0] = '.';
    }
    else if (button_type == BUT_LBR)
    {
        butValString[0] = '(';
    }
    else if (button_type == BUT_RBR)
    {
        butValString[0] = ')';
    }
    else if (button_type == BUT_SQRT)
    {
        butValString[0] = '^';
        butValString[1] = '(';
        butValString[2] = '1';
        butValString[3] = '/';
        butValString[4] = '2';
        butValString[5] = ')';
    }
    else if (button_type == BUT_SQU)
    {
        butValString[0] = '^';
        butValString[1] = '2';
    }
    else if (button_type == BUT_CUBE)
    {
        butValString[0] = '^';
        butValString[1] = '3';
    }
    else if (button_type == BUT_CBRT)
    {
        butValString[0] = '^';
        butValString[1] = '(';
        butValString[2] = '1';
        butValString[3] = '/';
        butValString[4] = '3';
        butValString[5] = ')';
    }
    else if (button_type == BUT_EQ_NL)
    {
        butValString[0] = '#';
    }
    else if (button_type == BUT_VARIABLE_X)
    {
        butValString[0] = 'x';
    }
    else if (button_type == BUT_VARIABLE_Y)
    {
        butValString[0] = 'y';
    }
    else if (button_type == BUT_EQU)
    {
        butValString[0] = '=';
    }
    else if (button_type == BUT_POW)
    {
        butValString[0] = '^';
    }
    else
    {
        butValString[0] = '?';
    }
}

char but2char(uint8_t button_type)
{
    switch (button_type)
    {
    case BUT_0:
        return '0';
        break;
    case BUT_1:
        return '1';
        break;
    case BUT_2:
        return '2';
        break;
    case BUT_3:
        return '3';
        break;
    case BUT_4:
        return '4';
        break;
    case BUT_5:
        return '5';
        break;
    case BUT_6:
        return '6';
        break;
    case BUT_7:
        return '7';
        break;
    case BUT_8:
        return '8';
        break;
    case BUT_9:
        return '9';
        break;
    case BUT_ADD:
        return '+';
        break;
    case BUT_MINUS:
        return '-';
        break;
    case BUT_MUL:
        return '*';
        break;
    case BUT_DIV:
        return '/';
        break;
    case BUT_DOT:
        return '.';
        break;
    case BUT_LBR:
        return '(';
        break;
    case BUT_RBR:
        return ')';
        break;
    case BUT_SQRT:
        return '@';
        break;
    case BUT_SQU:
        return '^';
        break;
    case BUT_CUBE:
        return '#';
        break;
    case BUT_CBRT:
        return '$';
        break;
    default:
        // 默认情况下返回一个特殊字符或者做其他处理
        return '?'; // 例子中使用问号作为默认返回值
        break;
    }
}

/* 根据按键键盘的CalData.buttontype ，修改相应变量的值，务必对着CalData的定义去做修改，不要遗漏*/
void updateEquationString(void)
{
    // 如果输入不是"=",if(CalData.answerNeedClear){/*clear answer string*/}
    switch (calData.buttonType)
    {
    case BUT_NO_PRESS:
        break;
    case BUT_EQU:
    {
        // cal.answerNeedClear=true
        //  计算的程序,用tinyExpression库
        /** 方根运算处理方法一  replace_str定义在了tinyexpr库中
         *replace_str(calData.equationStr, "^", "^2"); //对特殊的四个运算进行符号替换
         *replace_str(calData.equationStr, "@", "^(1/2)");
         *replace_str(calData.equationStr, "#", "^3");
         *replace_str(calData.equationStr, "$", "^(1/3)");
         */
        if (dispMode == DISPMODE_CAL)
        {
            calData.answer = te_interp(calData.equationStr, 0);
            calData.answer = ((float)((int)((calData.answer + 0.005) * 100))) / 100; // 把运算结果保留小数点后两位
            memset(calData.answerStr, 0, sizeof(calData.answerStr));
            sprintf(calData.answerStr, "%.2f", calData.answer);
            calData.answerStrLength = sizeof(calData.answerStr);
            calData.answerNeedClear = 1;
            calData.calculationTimes++; //  要告知计算机保存数据
        }
        else if (dispMode == DISPMODE_SOLVEX || dispMode == DISPMODE_SOLVEXY)
        {
            but2str(calData.buttonType);
            calData.buttonType = BUT_NO_PRESS;
            insertCharToEqCursor(butValString[0]);
        }
    }
    break;
    case BUT_CLEAR:
    {
        memset(calData.equationStr, '\0', sizeof(calData.equationStr));
        calData.equationStrLength = 0;
        calData.eqSTptr = 0;
        calData.eqEDptr = 0;
        calData.answer = 0;
        memset(calData.answerStr, '\0', sizeof(calData.answerStr));
        calData.answerStrLength = 0;
        calData.answerNeedClear = 1;
        calData.cursorInEqString = 0;
        calData.cursorScreenX = 0;
        calData.buttonType = BUT_NO_PRESS;
    }
    break;
    case BUT_LSHIFT:
    {
        calData.buttonType = BUT_NO_PRESS;
        if (calData.answerNeedClear)
        {
            calData.answer = 0;
            memset(calData.answerStr, '\0', sizeof(calData.answerStr));
            calData.answerStrLength = 0;
            calData.answerNeedClear = 0;
        }
        // 左移的时候不在最左边，直接移动cursorScreenX-=6,cursorInEqString--
        if (calData.cursorScreenX != 0)
        {
            calData.cursorInEqString--;
            calData.cursorScreenX -= (calData.cursorScreenX == 5) ? 5 : 6;
        }
        else // 光标在屏幕最左边
        {
            if (calData.eqSTptr == 0)
            {
                break;
            }
            else
            {
                calData.cursorInEqString--;
                calData.eqSTptr--;
                calData.eqEDptr--;
            }
        }
    }
    break;
    case BUT_RSHIFT:
    {
        calData.buttonType = BUT_NO_PRESS;
        if (calData.answerNeedClear)
        {
            calData.answer = 0;
            memset(calData.answerStr, '\0', sizeof(calData.answerStr));
            calData.answerStrLength = 0;
            calData.answerNeedClear = 0;
        }
        // 如果光标不在最右边
        if (calData.cursorScreenX != LCD_WIDTH - 1)
        {
            calData.cursorInEqString++;
            calData.cursorScreenX += (calData.cursorScreenX == 0) ? 5 : 6;
        }
        else
        {
            // 光标在最右边而且字符串最右边已经没有字母了
            if (calData.eqEDptr == calData.equationStrLength)
            {
                break;
            }
            else
            {
                calData.cursorInEqString++;
                calData.eqSTptr++;
                calData.eqEDptr++;
            }
        }
    }
    break;
    case BUT_DEL:
    {
        calData.buttonType = BUT_NO_PRESS;
        if (calData.answerNeedClear)
        {
            calData.answer = 0;
            memset(calData.answerStr, '\0', sizeof(calData.answerStr));
            calData.answerStrLength = 0;
            calData.answerNeedClear = 0;
        }
        if (calData.cursorScreenX == 0)
        {
            if (calData.cursorInEqString == 0)
            {
                break;
            }
            for (int i = calData.cursorInEqString; i < calData.equationStrLength; i++)
            {
                calData.equationStr[i - 1] = calData.equationStr[i];
            }
            calData.equationStr[calData.equationStrLength - 1] = '\0';
            calData.eqSTptr--;
            calData.eqEDptr--;
            calData.cursorInEqString--;
            calData.equationStrLength--;
        }
        else
        {
            for (int i = calData.cursorInEqString; i < calData.equationStrLength; i++)
            {
                calData.equationStr[i - 1] = calData.equationStr[i];
            }
            calData.equationStr[calData.equationStrLength - 1] = '\0';
           if (calData.cursorScreenX == 83  && calData.equationStrLength > 14)
            {
                calData.eqSTptr--;  
            }
            else 
            {
                calData.cursorScreenX -= (calData.cursorScreenX == 5) ? 5 : 6;
            }
            calData.eqEDptr--;
            calData.cursorInEqString--;
            calData.equationStrLength--;
        }
    }
    break;
    case BUT_0:
    case BUT_1:
    case BUT_2:
    case BUT_3:
    case BUT_4:
    case BUT_5:
    case BUT_6:
    case BUT_7:
    case BUT_8:
    case BUT_9:
    case BUT_ADD:
    case BUT_MINUS:
    case BUT_MUL:
    case BUT_DIV:
    case BUT_DOT:
    case BUT_LBR:
    case BUT_RBR:
    case BUT_SQRT:
    case BUT_SQU:
    case BUT_CUBE:
    case BUT_CBRT:
    case BUT_EQ_NL:
    case BUT_VARIABLE_X:
    case BUT_VARIABLE_Y:
    case BUT_POW:
    {
        /*在cursorInEqString处插入字符c,然后光标后移*/
        // char c = but2char(calData.buttonType);
        but2str(calData.buttonType);
        calData.buttonType = BUT_NO_PRESS;
        for (int i = 0; butValString[i] != '\0'; i++)
        {
            insertCharToEqCursor(butValString[i]);
        }
    }
    break;
    case BUT_SHIFT:
    {
        dispMode = DISPMODE_MENU;
    }
    break;
    case BUT_SOLVE:
    {
        if (dispMode == DISPMODE_SOLVEX)
        {
            equation_x(calData.equationStr, calData.answerStr);
            calData.answerStrLength = sizeof(calData.answerStr);
            calData.answerNeedClear = 1;
            calData.calculationTimes++;
        }
        if (dispMode == DISPMODE_SOLVEXY)
        {
            equation_xy(calData.equationStr, calData.answerStr);
            calData.answerStrLength = sizeof(calData.answerStr);
            calData.answerNeedClear = 1;
            calData.calculationTimes++;
        }
        calData.buttonType = BUT_NO_PRESS;
    }
    break;
    default:
        break;
    }
}

// 根据calData里面的信息，更新bitmap
void updateCalDisplayMap(void)
{
    clearDisplay(WHITE);
    setStrWithL(calData.equationStr + calData.eqSTptr, LCD_WIDTH / 6, 0, EQ_Y, BLACK);
    setStrWithL(calData.answerStr, calData.answerStrLength, 1, 39, BLACK);
    // 根据cursorState画线
    if (calData.cursorState == CURSOR_ST_1)
    {
        setLine(calData.cursorScreenX, EQ_Y, calData.cursorScreenX, EQ_Y + 7, BLACK);
    }
    else if (calData.cursorState == CURSOR_ST_0)
    {
        setLine(calData.cursorScreenX, EQ_Y, calData.cursorScreenX, EQ_Y + 7, WHITE);
    }

    switch (dispMode)
    {
    case DISPMODE_CAL:
    {
        char mode[] = "mode1";
        setStr(mode, 39, FIRSTLINE_Y, BLACK);
    }
    break;
    case DISPMODE_SOLVEX:
    {
        char mode[] = "mode2";
        setStr(mode, 39, FIRSTLINE_Y, BLACK);
    }
    break;
    case DISPMODE_SOLVEXY:
    {
        char mode[] = "mode3";
        setStr(mode, 39, FIRSTLINE_Y, BLACK);
    }
    break;
    default:
        break;
    }
}

void openingVideo(void)
{
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
    clearDisplay(WHITE); // Start fresh
    HAL_Delay(200);
}

/**
 * @param char c需要加的单字符，注意此函数调用了较多calculatorGP内的全局变量，谨慎使用
 * @details 在equationstr光标位置处插入字符c
 */
void insertCharToEqCursor(char c)
{
    // 如果光标在屏幕中间，向后移动一位
    if (calData.cursorScreenX != LCD_WIDTH - 1)
    {
        for (int i = calData.equationStrLength; i > calData.cursorInEqString; i--)
        {
            calData.equationStr[i] = calData.equationStr[i - 1];
        }
        calData.equationStr[calData.cursorInEqString] = c;
        // 下面是对各个变量的修改
        //  calData.equation
        // calData.eqEDptr
        if (calData.eqEDptr == calData.equationStrLength)
        {
            calData.eqEDptr++;
        }
        calData.equationStrLength++;
        // calData.eqSTptr

        if (calData.answerNeedClear)
        {
            calData.answer = 0;
            memset(calData.answerStr, '\0', sizeof(calData.answerStr));
            calData.answerStrLength = 0;
            calData.answerNeedClear = 0;
        }
        calData.cursorInEqString++;
        calData.cursorScreenX += (calData.cursorScreenX == 0) ? 5 : 6;
    }
    // 光标在屏幕最右端
    else
    {
        for (int i = calData.equationStrLength; i > calData.cursorInEqString; i--)
        {
            calData.equationStr[i] = calData.equationStr[i - 1];
        }
        calData.equationStr[calData.cursorInEqString] = c;
        // 下面是对各个变量的修改
        //  calData.equation
        calData.equationStrLength++;
        calData.eqSTptr++;
        calData.eqEDptr++;
        if (calData.answerNeedClear)
        {
            calData.answer = 0;
            memset(calData.answerStr, '\0', sizeof(calData.answerStr));
            calData.answerStrLength = 0;
            calData.answerNeedClear = 0;
        }
        calData.cursorInEqString++;
        // calData.cursorScreenX;
    }
}
