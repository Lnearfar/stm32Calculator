#ifndef __CALCULATORGP_H
#define __CALCULATORGP_H

#include "Nokia5110LCD.h"
#include "button.h"
#include "tinyexpr.h"
#include <string.h>

typedef enum
{
    CURSOR_ST_1,
    CURSOR_ST_0,
    CURSOR_ST_NOTSHOW
}enum_cursor_state;

typedef enum{
    DISPMODE_MENU,
    DISPMODE_CAL,
    DISPMODE_SOLVEX,
    DISPMODE_SOLVEXY,
}enum_dispmode;

#pragma pack(1)
typedef struct{
    //数据帧定义
    uint8_t frameStart1;
    uint8_t frameStart2;
    //equation
    char equationStr[100];
    uint8_t equationStrLength;
    uint8_t eqSTptr;//equation开始显示位的下标
    uint8_t eqEDptr;//equation结束显示位的下标
    //answer
    float answer;
    char answerStr[100];
    uint8_t answerStrLength;
    uint8_t answerNeedClear;
    uint8_t calculationTimes;//表示计算过程执行了几次，用于告知上位机要保存数据
    //cursor
    uint8_t cursorInEqString;//表示光标当前值在equationStr中的位置
    uint8_t cursorScreenX;//光标在屏幕(84*48)中的X坐标
    uint8_t cursorState;//光标的状态
    //button
    uint8_t buttonType;
    //与上位机通信相关的内容
    uint8_t ctrSignal;

    //数据帧尾部
    uint8_t frameEnd1;
    uint8_t frameEnd2;
}Struct_Data_Buffer;
#pragma pack()

extern Struct_Data_Buffer calData;
extern uint8_t dispMode;

char but2char(uint8_t button_type);
void InitCalculatorGP(void);
void updateButtonState(uint8_t button_type);
void updateEquationString(void);

void updateCalDisplayMap(void);
void openingVideo(void);
void insertCharToEqCursor(char c);

#endif

