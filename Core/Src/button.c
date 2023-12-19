/**
 * @file button.c
 * @brief File brief description
 *
 * Detailed description of the file.
 *
 * @author Author Name (author@email.com)
 * @version 1.0
 * @date 2023-02-20
 *
 * @copyright Copyright (c) 2023 Author Name
 *
 */
#include "button.h"

/* private variable and definition start */

uint8_t hostButtonType = BUT_NO_PRESS;
uint8_t hostButtonCnt = 0;
//假设serialFrame.h里面hostButtonFrame是暴露给此文件的，并且认为此次信息是实时的
//hostButtonFrame.serialButtonType
//hostButtonFrame.serialButtonCnt

// 初始时没有任何按键按下,button = BUT_NO_PRESS
uint8_t buttonPressedType = BUT_NO_PRESS;

// 检查按键状态，返回对应字符BUT_xxx
uint8_t checkButton(void)
{
    //
    return BUT_NO_PRESS;
}

/* private variable and definition end */
