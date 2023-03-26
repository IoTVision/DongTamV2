/*
 * 74HC595.h
 *
 *  Created on: Mar 23, 2023
 *      Author: hoanganh
 */

#ifndef INC_74HC595_H_
#define INC_74HC595_H_

#include "stm32f1xx.h"
typedef struct{
	GPIO_TypeDef* GPIO_CLK_PORT;
	uint16_t GPIO_CLK_PIN;
	GPIO_TypeDef* GPIO_DATA_PORT;
	uint16_t GPIO_DATA_PIN;
	GPIO_TypeDef* GPIO_LATCH_PORT;
	uint16_t GPIO_LATCH_PIN;
} HC595;
void Send_Data(HC595* _hc595,uint8_t dt);
#endif /* INC_74HC595_H_ */
