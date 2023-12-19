#ifndef SERIALFRAME_H
#define SERIALFRAME_H

#include <QByteArray>

#define FRAME_START 0xAA
#define FRAME_END 0xBB

/**
 * 定义发送给stm32的按键信息数据帧
 * 发送给stm32时应 0xAA,0xAA,按键对应枚举值(uint8_t),按键次数(uint8_t<256),自定义一位控制信号,0xFF,0xFF
 *
 * 接受的时候要解析数据帧
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
    BUT_EQ_NL,
}KEY_ENUM;

typedef struct{
    uint8_t frameStart1;
    uint8_t frameStart2;
    uint8_t serialButtonType;
    uint8_t serialHostPressCnt;
    uint8_t frameEnd1;
    uint8_t frameEnd2;
}st_serialHost2StmFrame;

extern st_serialHost2StmFrame m_serialHost2StmFrame;

void initSerialFrame(void);


/**
 * 解析来自stm32的显示屏显示数据帧
 * Struct_Data_Buffer为在stm32里面定义的结构体，如在stm32更新，需要在此处同步更新，同时更新相应的解析函数
*/
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

void hostGetOneByte(uint8_t data);
extern Struct_Data_Buffer m_frameData;



#endif // SERIALFRAME_H
