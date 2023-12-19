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

// 检查按键状态，返回对应字符
char checkButton(void)
{
    //
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
