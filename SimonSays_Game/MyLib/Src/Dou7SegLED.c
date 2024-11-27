/*
 * Dou7SegLED.c
 *
 *  Created on: Nov 5, 2024
 *      Author: ADMIN
 */
#include "Dou7SegLED.h"

const uint8_t digitTable[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09};

void Seg7LED_Int(Seg7LED_t* Seg7LED, GPIO_TypeDef* COMMON_Port, uint16_t RCLK_Pin, uint16_t SRCLK_Pin, uint16_t SER_Pin)
{
	Seg7LED->RCLK_Port = Seg7LED->SER_Port = Seg7LED->SRCLK_Port = COMMON_Port;
	Seg7LED->RCLK_Pin = RCLK_Pin;
	Seg7LED->SRCLK_Pin = SRCLK_Pin;
	Seg7LED->SER_Pin = SER_Pin;
}

void shitfOut(uint8_t digit, Seg7LED_t *Seg7LED)
{
	uint8_t data = digitTable[digit];

	for(uint8_t i = 0; i < 8; i++)
	{
		//SRCLK
		HAL_GPIO_WritePin(Seg7LED->SRCLK_Port, Seg7LED->SRCLK_Pin, GPIO_PIN_RESET);
		HAL_Delay(1);

		//SER
		HAL_GPIO_WritePin(Seg7LED->SER_Port, Seg7LED->SER_Pin, data & 0x01);


		HAL_GPIO_WritePin(Seg7LED->SRCLK_Port, Seg7LED->SRCLK_Pin, GPIO_PIN_SET);
		HAL_Delay(1);

		data = data >> 1;
	}
}


void displayScore(uint8_t number, Seg7LED_t *Seg7LED)
{
	//RCLK CLOCK
	HAL_GPIO_WritePin(Seg7LED->RCLK_Port, Seg7LED->RCLK_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);

	shitfOut(number % 10, Seg7LED);
	shitfOut(number / 10, Seg7LED);

	HAL_GPIO_WritePin(Seg7LED->RCLK_Port, Seg7LED->RCLK_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}
