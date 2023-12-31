/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t rxbuf; // huart1 receive buffer
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  lcdBegin();
  InitCalculatorGP();
  initHostButtonFrame();
  openingVideo();
  showMenu();
  // lcdFunTime();
  HAL_UART_Receive_IT(&huart1, &rxbuf, 1);
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Obtain button type, most of time the buttonPressedType is BUT_NO_PRESS
    buttonPressedType = checkButton();
    // debug:
    if (hostButtonFrame.serialHostPressCnt != hostButtonCnt)
    {
      buttonPressedType = hostButtonFrame.serialButtonType;
      hostButtonCnt = hostButtonFrame.serialHostPressCnt;
    }
    // end bebug

    if (buttonPressedType != BUT_NO_PRESS)
    {
      // 1.if button == SHIFT
      if (buttonPressedType == BUT_SHIFT)
      {
        dispMode = DISPMODE_MENU;
        showMenu();
      }
      // button != SHIFT, if dispMode == MENU, check if button == 1,2,3 or do nothing
      else if (dispMode == DISPMODE_MENU)
      {
        if (buttonPressedType == BUT_1)
        {
          dispMode = DISPMODE_CAL;
          calData.cursorState = CURSOR_ST_1;
        }
        else if (buttonPressedType == BUT_2)
        {
          dispMode = DISPMODE_SOLVEX;
          calData.cursorState = CURSOR_ST_1;
        }
        else if (buttonPressedType == BUT_3)
        {
          dispMode = DISPMODE_SOLVEXY;
          calData.cursorState = CURSOR_ST_1;
        }
      }
      // dispMode == cal or solve, button == number or operator
      else
      {
        updateButtonState(buttonPressedType);
        updateEquationString();
        // updateCalDisplayMap() draws the string in calData(and other information struct) on DisplayMap[] array, then updateDisplay print displayMap[] on LCD screen
        updateCalDisplayMap();
        updateDisplay();
      }
    }
    if (buttonPressedType == BUT_NO_PRESS)
    {
      // in Menu mode, show menu
      // in calculation mode , refresh screen to show cursor
      if (dispMode == DISPMODE_MENU)
      {
        showMenu();
      }
      else
      {
        updateCalDisplayMap();
        updateDisplay();
      }
    }
    buttonPressedType = BUT_NO_PRESS;
    // 通过串口传给上位机Host
    UART_Transmit_Struct(&huart1, &calData, sizeof(calData));
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1)
  {
    /*rxbuf == data;*/
    stm32GetOneByte(rxbuf);
    HAL_UART_Receive_IT(&huart1, &rxbuf, 1);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim2)
  {
    if (calData.cursorState == CURSOR_ST_0)
    {
      calData.cursorState = CURSOR_ST_1;
    }
    else if (calData.cursorState == CURSOR_ST_1)
    {
      calData.cursorState = CURSOR_ST_0;
    }
    else
    {
      // do nothing calData.cursorState == CURSOR_ST_NOTSHOW
    }
  }
  // �????测到state标志�????1，如果计时器�????5ms溢出�????次，每溢出一次，time++
  if (htim == &htim3)
  {
    if (m_but_state == 1)
    {
      m_but_time++;
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
