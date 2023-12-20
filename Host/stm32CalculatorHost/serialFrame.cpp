#include "serialFrame.h"

uint8_t m_buttonType;
uint8_t m_hostPressCnt;

st_serialHost2StmFrame m_serialHost2StmFrame;

void initSerialFrame(void){
    m_serialHost2StmFrame.frameStart1=0xAA;
    m_serialHost2StmFrame.frameStart2=0xAA;
    m_serialHost2StmFrame.frameEnd1=0xFF;
    m_serialHost2StmFrame.frameEnd2=0xFF;
    m_serialHost2StmFrame.serialHostPressCnt=0;
}
enum
{
    WAITING_FOR_START,
    RECEIVING_FRAME_START2,
    RECEIVING_DATA,
    RECEIVING_END1,
    RECEIVING_END2,
};

/**
 * 解析来自stm32的显示屏显示数据帧
 * Struct_Data_Buffer为在stm32里面定义的结构体，如在stm32更新，需要在此处同步更新，同时更新相应的解析函数
*/
Struct_Data_Buffer m_frameData;
uint8_t state = WAITING_FOR_START;
uint8_t buffer[sizeof(Struct_Data_Buffer)];
uint8_t index = 0;

void hostGetOneByte(uint8_t data){
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
        if (index >= sizeof(Struct_Data_Buffer) - 2)
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
            memcpy(&m_frameData, &buffer, sizeof(buffer));
            memset(&buffer, '\0', sizeof(buffer));
            //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
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

