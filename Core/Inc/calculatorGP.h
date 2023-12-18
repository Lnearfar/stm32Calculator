#ifndef __CALCULATORGP_H
#define __CALCULATORGP_H

#include "Nokia5110LCD.h"
#include "button.h"
#include "tinyexpr.h"
#include <stdbool.h>
#include <string.h>

typedef enum
{
    CURSOR_ST_1,
    CURSOR_ST_0,
    CURSOR_ST_NOTSHOW
}enum_cursor_state;

typedef struct{
    //equation
    char equationStr[100];
    uint8_t equationStrLength;
    uint8_t eqSTptr;//equation开始显示位的下标
    uint8_t eqEDptr;//equation结束显示位的下标
    //answer
    float answer;
    char answerStr[100];
    uint8_t answerStrLength;
    bool answerNeedClear;
    uint8_t calculationTimes;//表示计算过程执行了几次，用于告知上位机要保存数据
    //cursor
    uint8_t cursorInEqString;//表示光标当前值在equationStr中的位置
    uint8_t cursorScreenX;//光标在屏幕(84*48)中的X坐标
    uint8_t cursorState;//光标的状态
    //button
    uint8_t buttonType;
    //与上位机通信相关的内容
    uint8_t ctrSignal;
}Struct_Data_Buffer;

extern Struct_Data_Buffer calData;

char but2char(uint8_t button_type);
void InitCalculatorGP(void);
void updateButtonState(uint8_t button_type);
void updateEquationString(void);
void updateCalDisplayMap(void);
void openingVideo(void);

#endif

