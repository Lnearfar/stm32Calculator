/**
 * @file serialFrame.c
 * @brief 定义通信的数据帧，做接收数据的解析
 *
 * 本文件实现通信相关的函数，不同设备间应保有相同的收发协议
 *
 * @author Jingyuan Li (nearfar1jy@email.com)
 * @version 1.0
 * @date 2023-12-14
 *
 * @copyright Copyright (c) 2023 Jingyuan Li
 *
 * @usage:
 *
 */

#include "serialFrame.h"

void initHostButtonFrame(void){
    hostButtonFrame.frameStart1==0xAA;
    hostButtonFrame.frameStart2==0xAA;
    hostButtonFrame.frameEnd1==0xFF;
    hostButtonFrame.frameEnd2==0xFF;
    hostButtonFrame.serialButtonType=BUT_NO_PRESS;
    hostButtonFrame.serialHostPressCnt=0;
}

void UART_Transmit_Struct(UART_HandleTypeDef *huart, void *data, uint16_t size)
{
    // Cast the data as a pointer to uint8_t to send its raw bytes
    uint8_t *ptr = (uint8_t *)data;

    // Loop to send each byte of the data over UART
    for (uint16_t i = 0; i < size; i++)
    {
        // Send byte over UART
        HAL_UART_Transmit(huart, &ptr[i], sizeof(ptr[i]), 100);
    }
}

/**
 * 接受来自Host的数据 0xAA,0xAA,按键类型，按键次数, 0xFF,0xFF
 * 配置为中断输入
*/

st_serialHost2StmFrame hostButtonFrame;

#define FRAME_START 0xAA
#define FRAME_END 0xBB

enum
{
    WAITING_FOR_START,
    RECEIVED_START,
    RECEIVING_DATA,
    RECEIVED_END
};

uint8_t state = WAITING_FOR_START;
uint8_t buffer[sizeof(hostButtonFrame)];
uint8_t index = 0;
void stm32GetOneByte(uint8_t data)
{
    switch (state)
    {
    case WAITING_FOR_START:
        if (data == FRAME_START)
        {
            index = 0;
            state = RECEIVED_START;
        }
        break;

    case RECEIVED_START:
        state = RECEIVING_DATA;
        buffer[index++] = data;
        break;

    case RECEIVING_DATA:
        buffer[index++] = data;
        if (index >= sizeof(hostButtonFrame))
        {
            state = RECEIVED_END;
        }
        break;

    case RECEIVED_END:
        if (data == FRAME_END)
        {
            memcpy(&hostButtonFrame, buffer, sizeof(Struct_Data_Buffer));
            // process frameData
            state = WAITING_FOR_START;
        }
        else
        {
            state = WAITING_FOR_START;
        }
        break;
    }
}
