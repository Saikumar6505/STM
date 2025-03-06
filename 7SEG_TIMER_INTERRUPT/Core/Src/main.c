/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint8_t current_number = 0;
static volatile uint32_t last_button_press_time = 0; // For debouncing
static const uint32_t DEBOUNCE_DELAY = 200; // 200 ms debounce delay

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void display_number(uint8_t number) {
    // Adjust GPIO states to display the number on a 7-segment display
    // Update GPIO configuration here according to your hardware
	switch (number) {
	        case 0:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, SET);
	            break;
	        case 1:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, SET);
	            break;
	        case 2:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
	        case 3:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
	        case 4:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
	        case 5:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
	        case 6:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
	        case 7:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, SET);
	            break;
	        case 8:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
	        case 9:
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, SET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, RESET);
	            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, RESET);
	            break;
    }
}

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
  MX_USART2_UART_Init();
  MX_TIM2_Init();

  /* Start Timer 2 in interrupt mode */
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief This function is executed when the timer period elapses.
  * @param htim: TIM handle pointer
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        // Update or handle timer interrupt logic
        // Example: Toggle LED or update display number
        current_number = (current_number + 1) % 10;  // Increment and wrap around
        display_number(current_number);
    }
}

/**
  * @brief This function is executed when an external interrupt occurs.
  * @param GPIO_Pin: GPIO pin number
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) {
        uint32_t current_time = HAL_GetTick();
        if (current_time - last_button_press_time > DEBOUNCE_DELAY) {
            last_button_press_time = current_time;
            current_number = (current_number + 1) % 10;
            display_number(current_number);
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8399; // Adjust this value based on your clock settings
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999; // 1 second interval with 84 MHz clock
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pin : PC13 (Button) */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* Configure GPIO pins for 7-segment display */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
  * @brief This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    // Optional: Toggle an LED or send error message via UART
  }
}

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
