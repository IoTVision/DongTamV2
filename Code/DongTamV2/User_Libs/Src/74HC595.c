/*
 * 74HC595.c
 *
 *  Created on: Mar 23, 2023
 *      Author: hoanganh
 */

#include "74HC595.h"

void HC595_Send_Data(HC595* _hc595,uint16_t dt)
{
	for(int i = 0;i<8;i++)
	{
		if(dt & 0x80)
			HAL_GPIO_WritePin(_hc595 -> GPIO_DATA_PORT, _hc595 -> GPIO_DATA_PIN, 1);
		else
			HAL_GPIO_WritePin(_hc595 -> GPIO_DATA_PORT, _hc595 -> GPIO_DATA_PIN, 0);


		HAL_GPIO_WritePin(_hc595 -> GPIO_CLK_PORT, _hc595 -> GPIO_CLK_PIN, 0);
		__NOP();
		HAL_GPIO_WritePin(_hc595 -> GPIO_CLK_PORT, _hc595 -> GPIO_CLK_PIN, 1);
		dt <<= 1;
	}
	HAL_GPIO_WritePin(_hc595 -> GPIO_LATCH_PORT, _hc595 -> GPIO_LATCH_PIN, 0);
	__NOP();
	HAL_GPIO_WritePin(_hc595 -> GPIO_LATCH_PORT, _hc595 -> GPIO_LATCH_PIN, 1);
}
