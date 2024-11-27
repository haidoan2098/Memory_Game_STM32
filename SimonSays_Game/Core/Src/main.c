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
#include "BUZZER.h"
#include "Dou7SegLED.h"
#include "LCD_I2C.h"
#include "stdlib.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MAX_LEVEL 100

#define ON GPIO_PIN_SET
#define OFF GPIO_PIN_RESET

#define PRESS GPIO_PIN_RESET
#define RELEASE	GPIO_PIN_SET

#define TRUE 1
#define FALSE 0

#define TIMEOUT_MS 5000

#define NUM_BUTTONS 4
#define NUM_LEDS 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

const uint16_t ledPins[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_1};	// GPIOB

const uint16_t buttonPins[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6};	// GPIOA

const int gameTones[] = {NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5};

const uint16_t startTones[] = {NOTE_G4, NOTE_A4, 0, NOTE_C5, 0, 0, NOTE_G4, 0, 0, 0, NOTE_E4, 0, NOTE_D4, NOTE_E4, NOTE_G4, 0, NOTE_D4, NOTE_E4, 0, NOTE_G4, 0, 0, NOTE_D4, 0, NOTE_E4, 0, NOTE_G4, 0, NOTE_A4, 0, NOTE_C5, 0};
const uint16_t levelUpTones[] = {NOTE_E4, NOTE_G4, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_G5};
const uint16_t overTones[] = {NOTE_DS5, NOTE_D5, NOTE_CS5};


uint8_t gameSequence[MAX_LEVEL] = {0};
uint8_t gameIndex = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/*---------------------------- RANDOM LED ----------------------*/
void generateRandomSeed(void);
uint8_t randomLED(void);
/*--------------------------------------------------------------*/

/*----------------------- SOUND CONFIGURATION ------------------*/
void melodyGameStart(void);
void melodyLevelUp(void);
void melodyGameOver(void);
/*--------------------------------------------------------------*/

/*------------------ LED, BUTTON, SOUND CONFIGURATION ------------------*/
//LED On and Sound
void lightLedAndPlayTone(uint8_t index, uint16_t timeDelay);
//Wait for the Player to Press the Button and Return the Position of the Pressed Button
uint8_t readButtons();
/*----------------------------------------------------------------------*/

/*--------------------- GAME CONFIGURATION ----------------- */
// LED String
void playSequence(void);
// Test LED String with Player Button
uint8_t checkUserSequence(void);
// Game Start
void gameStart(void);
// Game Over
void gameOver(void);
/*----------------------------------------------------------- */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

Seg7LED_t Seg7LED;

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
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  Seg7LED_Int(&Seg7LED, GPIOA, GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_10);
  displayScore(0, &Seg7LED);

  HAL_Delay(1000);
  lcd_init();

  generateRandomSeed();

  gameStart();



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  char tmp[8] = {0};
	  displayScore(gameIndex, &Seg7LED);

	  lcd_goto_XY(1, 3);
	  lcd_send_string("Game Level:");

	  sprintf(tmp, "%d", gameIndex+1);
	  lcd_goto_XY(2, 7);
	  lcd_send_string(tmp);

	  gameSequence[gameIndex] = randomLED();
	  gameIndex++;

	  if (gameIndex >= MAX_LEVEL)
	  {
		  gameIndex = MAX_LEVEL - 1;
	  }

	  playSequence();


	  if (checkUserSequence() == 0)
	  {
		  lcd_clear_display();
		  HAL_Delay(5);
		  gameOver();
	  }

	  HAL_Delay(300);

	  if(gameIndex > 0)
	  {
		  melodyLevelUp();
		  HAL_Delay(300);
	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL10;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
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
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA10 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/*---------------- RANDOM LED ---------------------*/
void generateRandomSeed(void)
{
    uint32_t seed = 0;
    for (int i = 0; i < 32; i++)
    {
        seed = (seed << 1) | (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) & 0x01);
        HAL_Delay(1);
    }
    srand(seed);
}

uint8_t randomLED(void)
{
    return rand() % NUM_BUTTONS;
}
/*--------------------------------------------------*/

/*----------------------- SOUND CONFIGURATION ------------------*/
void melodyGameStart(void)
{
	for(uint8_t i = 0; i < 32; i++)
	{
		playTone(startTones[i], &htim2);
		HAL_Delay(150);
	}
	noTone(&htim2);
}

void melodyLevelUp(void)
{
	for(uint8_t i = 0; i < 6; i++)
	{
		playTone(levelUpTones[i], &htim2);
		HAL_Delay(150);
	}
	noTone(&htim2);
}

void melodyGameOver(void)
{
	for(uint8_t i = 0; i < 3; i++)
	{
		playTone(overTones[i], &htim2);
		HAL_Delay(300);
	}
	for (uint8_t i = 0; i < 10; i++) {
	  for (int pitch = -10; pitch <= 10; pitch++) {
		playTone(NOTE_C5 + pitch, &htim2);
	    HAL_Delay(5);
	  }
	}
	noTone(&htim2);

	HAL_Delay(100);
}
/*--------------------------------------------------------------*/


/*------------------ LED, BUTTON, SOUND CONFIGURATION ------------------*/
void lightLedAndPlayTone(uint8_t index, uint16_t timeDelay)
{
	HAL_GPIO_WritePin(GPIOB, ledPins[index], ON);
	HAL_Delay(timeDelay);
	playTone(gameTones[index], &htim2);
	HAL_GPIO_WritePin(GPIOB, ledPins[index], OFF);
	HAL_Delay(timeDelay);
	noTone(&htim2);
}

uint8_t readButtons()
{
	uint32_t timeOut = HAL_GetTick();
	while (1)
	{
		for(int index = 0; index < NUM_BUTTONS; index++)
		{
			if (HAL_GPIO_ReadPin(GPIOA, buttonPins[index]) == PRESS)
			{
				uint32_t timeOut = HAL_GetTick();
				HAL_Delay(40);
				lightLedAndPlayTone(index, 300);
				while(HAL_GPIO_ReadPin(GPIOA, buttonPins[index]) == PRESS)
				{
					if(HAL_GetTick() - timeOut > TIMEOUT_MS)
					{
						break;
					}
				}
				return index;
			}
		}

		// Time Out
		if (HAL_GetTick() - timeOut >= TIMEOUT_MS)
		{
			return -1;
		}
	}
}
/*---------------------------------------------------------------------------*/


/*--------------------- GAME CONFIGURATION ----------------- */
void playSequence(void)
{
	for(uint8_t i = 0; i < gameIndex; i++)
	{
		uint8_t ledString = gameSequence[i];
		lightLedAndPlayTone(ledString, 300);
		HAL_Delay(50);
	}
}

uint8_t checkUserSequence(void)
{
	for(uint8_t i = 0; i < gameIndex; i++)
	{
		uint8_t expectedButton = gameSequence[i];
		uint8_t actualButton = readButtons();
		if (actualButton != expectedButton)
		{
			return FALSE;
		}
	}
	return TRUE;
}

void gameStart(void)
{
	lcd_goto_XY(1, 1);
	lcd_send_string("VKU MEMORY GAME");
	lcd_goto_XY(2, 3);
	lcd_send_string("Using STM32");
	HAL_Delay(500);

	melodyGameStart();

	lcd_clear_display();
	HAL_Delay(5);

	lcd_goto_XY(1, 2);
	lcd_send_string("Press & Hold");
	lcd_goto_XY(2, 1);
	lcd_send_string("the Red Button");

	uint32_t timeOut = HAL_GetTick();
	while(HAL_GPIO_ReadPin(GPIOA, buttonPins[0]) == RELEASE)
	{

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		HAL_Delay(50);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
		HAL_Delay(50);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		HAL_Delay(50);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
		HAL_Delay(50);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

		if (HAL_GPIO_ReadPin(GPIOA, buttonPins[0]) == PRESS)
		{
			HAL_Delay(40);
			uint32_t timePess = HAL_GetTick();
			while ((HAL_GPIO_ReadPin(GPIOA, buttonPins[0]) == PRESS) )
			{
				if (HAL_GetTick() - timePess >= 800)
				{
					break;
				}
			}
		}

		// Time Out
		if (HAL_GetTick() - timeOut >= TIMEOUT_MS)
		{
			break;
		}
	}

	lcd_clear_display();

	HAL_Delay(5);

	lcd_goto_XY(2, 3);
	lcd_send_string("Game Start");

	for(uint8_t i = 0; i < 2; i++)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_1, ON);
		HAL_Delay(300);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_1, OFF);
		HAL_Delay(300);
	}

	lcd_clear_display();
}

void gameOver(void)
{
	gameIndex = 0;

	lcd_goto_XY(2, 3);
	lcd_send_string("Game Over");

	melodyGameOver();

	lcd_clear_display();
}
/*---------------------------------------------------------------------*/



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
