/**
 * @file button.c
 * @brief File brief description
 *
 * Detailed description of the file.
 *
 * short-time
 * -----------------
 * |  1  2  3  4  |
 * |  5  6  7  8  |
 * |  9  0  <  >  |
 * |  .  = cl del |
 * -----------------
 * long-time
 * -------------------
 * |  (   )  ^2  ^3  |
 * |  √  3√   X   Y  |
 * |  +   -   *   /  |
 * |  ^  sv  nl  shf |
 * -------------------
 *
 * @author Jiayi Hu (author@email.com)
 * @version 1.0
 * @date 2023-12-19
 * @note PA0-3 推挽输出，PA4-7上拉输入；TIM3->psc=7199,arr=49,USART1, 115200baudrate,8bit,noparity,1 stop bit, t and r enable; use TIM3
 *
 * @copyright Copyright (c) 2023 Author Name
 *
 */
/**
 * TODO: update checkButton() to merge button information in hostButtonFrame, and return ButtonType;
 */

#include "button.h"
/* private variable and definition start */
uint8_t hostButtonType = BUT_NO_PRESS;
uint8_t hostButtonCnt = 0;

typedef struct Button_GPIO_TypeDef
{
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin; 
}Button_GPIO_TypeDef;

Button_GPIO_TypeDef R1 = {GPIOA, GPIO_PIN_0};
Button_GPIO_TypeDef R2 = {GPIOA, GPIO_PIN_1};
Button_GPIO_TypeDef R3 = {GPIOA, GPIO_PIN_2}; 
Button_GPIO_TypeDef R4 = {GPIOA, GPIO_PIN_3};

Button_GPIO_TypeDef C1 = {GPIOA, GPIO_PIN_4};
Button_GPIO_TypeDef C2 = {GPIOA, GPIO_PIN_5};  
Button_GPIO_TypeDef C3 = {GPIOA, GPIO_PIN_6};
Button_GPIO_TypeDef C4 = {GPIOA, GPIO_PIN_7};

// 假设serialFrame.h里面hostButtonFrame是暴露给此文件的，并且认为此次信息是实时的
// hostButtonFrame.serialButtonType
// hostButtonFrame.serialButtonCnt
// 初始时没有任何按键按下,button = BUT_NO_PRESS
uint8_t buttonPressedType = BUT_NO_PRESS;
int m_but_time;
int m_but_state = 0;
// 检查按键状态，返回对应字符BUT_xxx
uint8_t checkButton(void)
{
	//
	HAL_TIM_Base_Start_IT(&htim3); // 开启时钟
	while (1)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // 使第一行置0，其余置1，依次检测第一行的所有列是否为0
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
		{
			m_but_time = 0;									 // 重置计时，方便后续判断长按短按
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0) // 第一列为0，即为第一行第一列被按下
			{
				m_but_state = 1; // 消抖的同时对按键按下的时间进行计时，超过
			}
			m_but_state = 0; // 按键松开，状态位置0，time不再变化
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+0,1, 0xffff);   //短按
				return BUT_1;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+0,1,0xffff);    //长按
				return BUT_LBR;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+1,1, 0xffff);
				return BUT_2;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+1,1,0xffff);
				return BUT_RBR;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+2,1, 0xffff);
				return BUT_3;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+2,1,0xffff);
				return BUT_SQU;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+3,1, 0xffff);
				return BUT_4;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+3,1,0xffff);
				return BUT_CUBE;
			}
		}

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+4,1, 0xffff);
				return BUT_5;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+4,1,0xffff);
				return BUT_SQRT;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+5,1, 0xffff);
				return BUT_6;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+5,1,0xffff);
				return BUT_CBRT;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+6,1, 0xffff);
				return BUT_7;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+6,1,0xffff);
				return BUT_VARIABLE_X;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+7,1, 0xffff);
				return BUT_8;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+7,1,0xffff);
				return BUT_VARIABLE_Y;
			}
		}

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+8,1, 0xffff);
				return BUT_9;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+8,1,0xffff);
				return BUT_ADD;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+9,1, 0xffff);
				return BUT_0;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+9,1,0xffff);
				return BUT_MINUS;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+10,1, 0xffff);
				return BUT_LSHIFT;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_chang + 10, 1, 0xffff);
				return BUT_MUL;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+11,1, 0xffff);
				return BUT_RSHIFT;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_chang + 11, 1, 0xffff);
				return BUT_DIV;
			}
		}

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+12,1, 0xffff);
				return BUT_DOT;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+12,1,0xffff);
				return BUT_POW;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+13,1, 0xffff);
				return BUT_EQU;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+13,1,0xffff);
				return BUT_SOLVE;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+14,1, 0xffff);
				return BUT_CLEAR;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+14,1,0xffff);
				return BUT_EQ_NL;
			}
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
		{
			m_but_time = 0;
			while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 0)
			{
				m_but_state = 1;
			}
			m_but_state = 0;
			if (m_but_time > 3 && m_but_time < 100)
			{
				// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+15,1, 0xffff);
				return BUT_DEL;
			}
			else if (m_but_time > 100)
			{
				// HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+15,1,0xffff);
				return BUT_SHIFT;
			}
		}
		return BUT_NO_PRESS;
	}
}

/* private variable and definition end */
