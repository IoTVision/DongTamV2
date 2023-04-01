/*
 * 74HC165.c
 *
 *  Created on: Mar 27, 2023
 *      Author: hoanganh
 */

#include "74HC165.h"

HAL_StatusTypeDef HC165_ConfigOnePin(HC165* _hc165,GPIO_TypeDef *port,uint16_t pin, pinName pinName)
{
	if(!_hc165) return HAL_ERROR;
	switch(pinName){
	case HC165_CLK:
		_hc165->clk.port = port;
		_hc165->clk.pin = pin;
		break;
	case HC165_DS:
		_hc165->ds.port = port;
		_hc165->ds.pin = pin;
		break;
	case HC165_LATCH:
		_hc165->latch.port = port;
		_hc165->latch.pin = pin;
		break;
	}
	return HAL_OK;
}
HAL_StatusTypeDef HC165_TestPin(HC165* _hc165,pinName pinName)
{
	if(!_hc165) return HAL_ERROR;
	HC165_WRITE(pinName,1);
	HAL_Delay(1000);
	HC165_WRITE(pinName,0);
	HAL_Delay(1000);
	return HAL_OK;
}

HAL_StatusTypeDef HC165_Receive_Data(HC165* _hc165,uint16_t *dt)
{
	if(!_hc165) return HAL_ERROR;
	*dt = 0;
	// Step 1 : Sample
	HC165_WRITE(HC165_LATCH,0);
	HC165_WRITE(HC165_LATCH,1);

	// Step 2 : Shift
	for(int i = 8;i>0;i--)
	{
		GPIO_PinState bit = HC165_READ();
		*dt |= (bit << (i-1));
		HC165_WRITE(HC165_CLK,1); // Shift out the next bit
		HC165_WRITE(HC165_CLK,0);
	}

	return HAL_OK;
}
