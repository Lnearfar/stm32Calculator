#ifndef __BUTTON_H
#define __BUTTON_H

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"            //宏定义

//按键变量宏定义,请勿随便改变顺序！可以在后方添加相应的按键功能,但不要在中间加入新的定义!
typedef enum
{
    BUT_NO_PRESS=0,
    //0-9
    BUT_0,
    BUT_1,
    BUT_2,
    BUT_3, 
    BUT_4,
    BUT_5,
    BUT_6,
    BUT_7,
    BUT_8,
    BUT_9,
    //+-*/
    BUT_ADD, 
    BUT_MINUS,
    BUT_MUL,
    BUT_DIV,

    BUT_LBR,//left bracket(左括号)
    BUT_RBR,//left bracket(左括号)
    BUT_DOT,
    BUT_SQRT,   // 开平方根 square root (sqrt)
    BUT_SQU,    // 平方 Square
    BUT_CUBE,   // 立方 
    BUT_CBRT,   // 开立方根 Cube Root
    //上面都是单字符，可以简单的加在字符串后面

    BUT_EQU,
    BUT_CLEAR,
    BUT_DEL,

    BUT_LSHIFT, // 左移位
    BUT_RSHIFT, // 右移位
    //解方程部分
    BUT_SOLVE,
    BUT_VARIABLE_X,
    BUT_VARIABLE_Y,
    BUT_EQ_NL,//新的一行显示
    BUT_POW,//^
    BUT_SHIFT,
}KEY_ENUM;

extern uint8_t buttonPressedType;
extern uint8_t hostButtonCnt;
extern int m_but_state;
extern int m_but_time;

uint8_t checkButton(void);

#endif
