/*
 * Dou7SegLED.h
 *
 *  Created on: Nov 5, 2024
 *      Author: ADMIN
 */

#ifndef INC_DOU7SEGLED_H_
#define INC_DOU7SEGLED_H_

#include "stm32f1xx_hal.h"

typedef struct
{
	GPIO_TypeDef* RCLK_Port;
	uint16_t RCLK_Pin;

	GPIO_TypeDef* SRCLK_Port;
	uint16_t SRCLK_Pin;

	GPIO_TypeDef* SER_Port;
	uint16_t SER_Pin;

} Seg7LED_t;

// Khởi tạo LED 7 đoạn
void Seg7LED_Int(Seg7LED_t* Seg7LED, GPIO_TypeDef* COMMON_Port, uint16_t RCLK_Pin, uint16_t SRCLK_Pin, uint16_t SER_Pin);

// Đẩy data vào thanh ghi
void shitfOut(uint8_t digit, Seg7LED_t *Seg7LED);

// Chốt và hiện dữ liệu
void displayScore(uint8_t number, Seg7LED_t *Seg7LED);

#endif /* INC_DOU7SEGLED_H_ */
