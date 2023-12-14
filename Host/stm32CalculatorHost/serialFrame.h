#ifndef SERIALFRAME_H
#define SERIALFRAME_H

#include <QByteArray>

/**
 * 定义数据帧
 * 发送给stm32时应 0xAA,0xAA,按键对应枚举值(uint8_t),按键次数(uint8_t<256),自定义一位控制信号,0xFF,0xFF
 *
 * 接受的时候要解析数据帧
 * 首先
 *
 *
 *
 *
/* ! 本文件枚举应与Button.h(stm32)的顺序一致!*/
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
}KEY_ENUM;

QByteArray* host2stm32FrameCvt(int button_key);




#endif // SERIALFRAME_H
