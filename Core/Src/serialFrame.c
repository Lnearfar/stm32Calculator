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

char msg1[] = "start1\n";
char msg2[] = "start2\n";
char msg3[] = "data\n";
char msg4[] = "end1\n";
char msg5[] = "end2\n";

void initHostButtonFrame(void)
{
    hostButtonFrame.frameStart1 = 0xAA;
    hostButtonFrame.frameStart2 = 0xAA;
    hostButtonFrame.frameEnd1 = 0xFF;
    hostButtonFrame.frameEnd2 = 0xFF;
    hostButtonFrame.serialButtonType = BUT_NO_PRESS;
    hostButtonFrame.serialHostPressCnt = 0;
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
#define FRAME_END 0xFF

enum
{
    WAITING_FOR_START,
    RECEIVING_FRAME_START2,
    RECEIVING_DATA,
    RECEIVING_END1,
    RECEIVING_END2,
};

uint8_t state = WAITING_FOR_START;
uint8_t buffer[sizeof(hostButtonFrame)];
uint8_t index = 0;
void stm32GetOneByte(uint8_t data)
{
    HAL_UART_Transmit(&huart3, &data, sizeof(data), 10);
    switch (state)
    {
    case WAITING_FOR_START:
    {
        if (data == FRAME_START)
        {
            index = 0;
            buffer[index++] = data;
            state = RECEIVING_FRAME_START2;
            //HAL_UART_Transmit(&huart3, (uint8_t *)msg1, sizeof(msg1), 10);
        }
        // else state = WAITING_FOR_START
    }
    break;
    case RECEIVING_FRAME_START2:
    {
        if (data == FRAME_START)
        {
            buffer[index++] = data;
            state = RECEIVING_DATA;
            //HAL_UART_Transmit(&huart3, (uint8_t *)msg2, sizeof(msg2), 10);
        }
        else
        {
            state = WAITING_FOR_START;
            memset(buffer, '\0', sizeof(buffer));
        }
    }
    break;
    case RECEIVING_DATA:
    {
        buffer[index++] = data;
        //HAL_UART_Transmit(&huart3, (uint8_t *)msg3, sizeof(msg3), 10);
        if (index >= sizeof(hostButtonFrame) - 2)
        {
            state = RECEIVING_END1;
        }
    }
    break;
    case RECEIVING_END1:
    {
        if (data == FRAME_END)
        {
            buffer[index++] = data;
            // process frameData
            state = RECEIVING_END2;
            //HAL_UART_Transmit(&huart3, (uint8_t *)msg4, sizeof(msg4), 10);
        }
        else
        {
            state = WAITING_FOR_START;
            memset(buffer, '\0', sizeof(buffer));
        }
    }
    break;
    case RECEIVING_END2:
    {
        if (data == FRAME_END)
        {
            buffer[index] = data;
            state = WAITING_FOR_START;
            memcpy(&hostButtonFrame, &buffer, sizeof(buffer));
            memset(&buffer, '\0', sizeof(buffer));
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            //HAL_UART_Transmit(&huart3, (uint8_t *)msg5, sizeof(msg5), 10);
        }
        else
        {
            memset(&buffer, '\0', sizeof(buffer));
            state = WAITING_FOR_START;
        }
    }
    break;
    }
}
