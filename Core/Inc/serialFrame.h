#ifndef __SERIALFRAME_H
#define __SERIALFRAME_H

#include <stdint.h>
#include "usart.h"

void UART_Transmit_Struct(UART_HandleTypeDef *huart, void *data, uint16_t size);

/**
 * Host发过来的数据帧格式
*/
typedef struct{
    uint8_t frameStart1;
    uint8_t frameStart2;
    uint8_t serialButtonType;
    uint8_t serialHostPressCnt;
    uint8_t frameEnd1;
    uint8_t frameEnd2;
}st_serialHost2StmFrame;

void stm32GetOneByte(uint8_t data);

extern st_serialHost2StmFrame hostButtonFrame;

#endif




