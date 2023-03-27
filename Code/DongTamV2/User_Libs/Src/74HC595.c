/*
 * 74HC595.c
 *
 *  Created on: Mar 23, 2023
 *      Author: hoanganh
 */

#include "74HC595.h"


HAL_StatusTypeDef HC595_ConfigOnePin(HC595* _hc595,GPIO_TypeDef *port,uint16_t pin, pinName pinName)
{
	if(!_hc595) return HAL_ERROR;
	switch(pinName){
	case HC595_CLK:
		_hc595->clk.port = port;
		_hc595->clk.pin = pin;
		break;
	case HC595_DS:
		_hc595->ds.port = port;
		_hc595->ds.pin = pin;
		break;
	case HC595_LATCH:
		_hc595->latch.port = port;
		_hc595->latch.pin = pin;
		break;
	}
	return HAL_OK;
}
HAL_StatusTypeDef HC595_TestPin(HC595* _hc595,pinName pinName)
{
	if(!_hc595) return HAL_ERROR;
	HC595_WRITE(pinName,1);
	HAL_Delay(1000);
	HC595_WRITE(pinName,0);
	HAL_Delay(1000);
	return HAL_OK;
}

HAL_StatusTypeDef HC595_Send_Data(HC595* _hc595,uint16_t dt)
{
	if(!_hc595) return HAL_ERROR;
	for(int i = 0;i<8;i++)
	{
		if(dt & 0x80)
			HC595_WRITE(HC595_DS,1);
		else
			HC595_WRITE(HC595_DS,0);

		HC595_WRITE(HC595_CLK,0);
		HC595_WRITE(HC595_CLK,1);
		dt <<= 1;
	}
	HC595_WRITE(HC595_LATCH,0);
	HC595_WRITE(HC595_LATCH,1);
	return HAL_OK;
}
