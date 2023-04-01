/*
 * 74HC165.h
 *
 *  Created on: Mar 27, 2023
 *      Author: hoanganh
 */

#ifndef INC_74HC165_H_
#define INC_74HC165_H_

#include "stm32f1xx.h"

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
}pinConfig;

typedef struct{
	pinConfig ds;
	pinConfig clk;
	pinConfig latch;
} HC165;

typedef enum{
	HC165_LATCH,
	HC165_CLK,
	HC165_DS
}pinName;

#define latchPort 	_hc165->latch.port
#define latchPin 	_hc165->latch.pin
#define clkPort 	_hc165->clk.port
#define clkPin 		_hc165->clk.pin
#define dsPort 		_hc165->ds.port
#define dsPin 		_hc165->ds.pin

#define HC165_WRITE(PIN,LOGIC) 	( 	((PIN) == HC165_LATCH)	? 	HAL_GPIO_WritePin(latchPort,latchPin,(LOGIC)) 	:	\
									((PIN) == HC165_CLK) 	?	HAL_GPIO_WritePin(clkPort,clkPin,(LOGIC)) 		: 0	\
								)
#define HC165_READ() 			(	HAL_GPIO_ReadPin(dsPort,dsPin)	)

HAL_StatusTypeDef HC165_Receive_Data(HC165* _hc165,uint16_t *dt);
HAL_StatusTypeDef HC165_TestPin(HC165* _hc165,pinName pinName);
HAL_StatusTypeDef HC165_ConfigOnePin(HC165* _hc165,GPIO_TypeDef *port,uint16_t pin, pinName pinName);

#endif /* INC_74HC165_H_ */
