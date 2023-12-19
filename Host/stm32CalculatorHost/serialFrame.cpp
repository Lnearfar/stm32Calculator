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

enum {
    WAITING_FOR_START,
    RECEIVED_START,
    RECEIVING_DATA,
    RECEIVED_END
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
    switch(state) {

    case WAITING_FOR_START:
        if(data == FRAME_START) {
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
        if(index >= sizeof(Struct_Data_Buffer)) {
            state = RECEIVED_END;
        }
        break;

    case RECEIVED_END:
        if(data == FRAME_END) {
            memcpy(&m_frameData, buffer, sizeof(Struct_Data_Buffer));
            // process frameData
            state = WAITING_FOR_START;
        } else {
            state = WAITING_FOR_START;
        }
        break;
    }
}

