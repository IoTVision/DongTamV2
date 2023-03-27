/*
 * 74HC595.h
 *
 *  Created on: Mar 23, 2023
 *      Author: hoanganh
 */

#ifndef INC_74HC595_H_
#define INC_74HC595_H_

#include "stm32f1xx.h"

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
}pinConfig;

typedef struct{
	pinConfig ds;
	pinConfig clk;
	pinConfig latch;
} HC595;

typedef enum{
	HC595_LATCH,
	HC595_CLK,
	HC595_DS
}pinName;

#define latchPort _hc595->latch.port
#define latchPin _hc595->latch.pin
#define clkPort _hc595->clk.port
#define clkPin _hc595->clk.pin
#define dsPort _hc595->ds.port
#define dsPin _hc595->ds.pin

#define HC595_WRITE(PIN,LOGIC) ( 	((PIN) == HC595_LATCH)? 	HAL_GPIO_WritePin(latchPort,latchPin,(LOGIC)) 	:	\
									((PIN) == HC595_CLK) 	?	HAL_GPIO_WritePin(clkPort,clkPin,(LOGIC)) 		: 	\
									((PIN) == HC595_DS) 	?	HAL_GPIO_WritePin(dsPort,dsPin,(LOGIC)) 		: 0	\
								)

HAL_StatusTypeDef HC595_Send_Data(HC595* _hc595,uint16_t dt);
HAL_StatusTypeDef HC595_TestPin(HC595* _hc595,pinName pin);
HAL_StatusTypeDef HC595_ConfigOnePin(HC595* _hc595,GPIO_TypeDef *port,uint16_t pin, pinName pinName);
#endif /* INC_74HC595_H_ */
