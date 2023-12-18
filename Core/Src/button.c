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
/*#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h" */            //宏定义
/* private variable and definition start */

uint8_t hostButtonType = BUT_NO_PRESS;
uint8_t hostButtonCnt = 0;
//假设serialFrame.h里面hostButtonFrame是暴露给此文件的，并且认为此次信息是实时的
//hostButtonFrame.serialButtonType
//hostButtonFrame.serialButtonCnt

// 初始时没有任何按键按下,button = BUT_NO_PRESS
uint8_t buttonPressedType = BUT_NO_PRESS;

// 检查按键状态，返回对应字符BUT_xxx
KEY_ENUM checkButton(void)
{
    HAL_Init();//HAL 定义
    SystemClock_Config();//
    MX_GPIO_Init();//初始化GPIO，其中，P0-03推挽输出，P4-P7上拉输入
    MX_TIM3_Init();//定义时钟源，内部时钟源，向上计数，psc=7199，arr=49，不分频，自动重装载：使能，勾选定时器中断
    MX_USART1_UART_Init();//USART1,异步通信，波特率115200，8位数据长度，无奇偶检验，停止位1，接收和发送都使能
    HAL_TIM_Base_Start_IT(&htim3);//开启时钟
    //
      while (1)
  {
	  
    /* USER CODE END WHILE */
	  
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);     //使第一行置0，其余置1，依次检测第一行的所有列是否为0
	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
	  {
		time = 0;                                           //重置计时，方便后续判断长按短按
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)      //第一列为0，即为第一行第一列被按下
		  {
			 state = 1;                                     //消抖的同时对按键按下的时间进行计时，超过
		  }
		  state = 0;                                        //按键松开，状态位置0，time不再变化
		  if(time > 3 && time < 100)
		  {
			//HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+0,1, 0xffff);   //短按
            return BUT_1;
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+0,1,0xffff);    //长按
            return BUT_LBR;
		  }
		 
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			//HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+1,1, 0xffff);
            return BUT_2;   
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+1,1,0xffff);
            return BUT_RBR;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			//HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+2,1, 0xffff);  
            return BUT_3; 
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+2,1,0xffff);
            return BUT_SQU;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+3,1, 0xffff);
             return BUT_4;   
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+3,1,0xffff);
            return BUT_CUBE;
		  }
\
	  }
	  
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+4,1, 0xffff);
             return BUT_5;   
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+4,1,0xffff);
            return BUT_SQRT;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+5,1, 0xffff);
             return BUT_6;   
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+5,1,0xffff);
            return BUT_CBRT;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+6,1, 0xffff);
            return BUT_7;
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+6,1,0xffff);
            return BUT_VARIABLE_X;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+7,1, 0xffff);
            return BUT_8;
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+7,1,0xffff);
            return BUT_VARIABLE_Y;
		  }
	  }
	  
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
		  {
			 state = 1;
		  }
		  state=0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+8,1, 0xffff);
            return BUT_9;
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+8,1,0xffff);
            return BUT_ADD;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+9,1, 0xffff);
             return BUT_0;   
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+9,1,0xffff);
            return BUT_MINUS;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
		  {
			state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+10,1, 0xffff);
            return BUT_LSHIFT;
		  }
		  else if(time > 100)
		  {
			HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+10,1,0xffff);
            return BUT_MUL;
		  }

	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			// HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+11,1, 0xffff);
            return BUT_RSHIFT;   
		  }
		  else if(time > 100)
		  {
			HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+11,1,0xffff);
            return BUT_DIV;
		  }
	  }
	  
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+12,1, 0xffff);
            return BUT_DOT;
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+12,1,0xffff);
            return BUT_POW;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+13,1, 0xffff);
             return BUT_EQU;   
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+13,1,0xffff);
            return BUT_SOLVE;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+14,1, 0xffff);
            return BUT_CLEAR;
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+14,1,0xffff);
            return BUT_EQ_NL;
		  }
	  }
	  
	  	  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
	  {
		time = 0;
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) == 0)
		  {
			 state = 1;
		  }
		  state = 0;
		  if(time > 3 && time < 100)
		  {
			 //HAL_UART_Transmit(&huart1, (uint8_t *)button_duan+15,1, 0xffff);  
             return BUT_DEL; 
		  }
		  else if(time > 100)
		  {
			//HAL_UART_Transmit(&huart1,(uint8_t *)button_chang+15,1,0xffff);
            return BUT_SHIFT;
		  }
	  }
    return BUT_NO_PRESS;
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
//原码中程序初始化自带
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
//程序初始化自带
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim3)
	{
		if(state == 1)
		{
			time++;
		}
	}
	
}				//检测到state标志为1，如果计时器每5ms溢出一次，每溢出一次，time++
/* private variable and definition end */
