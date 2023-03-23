/*
 * 74HC595.c
 *
 *  Created on: Mar 23, 2023
 *      Author: hoanganh
 */

#include "74HC595.h"
void HC595_Send_Data(HC595* _hc595,uint8_t dt)
{
	for (int c = 0;c<8;c++)
	{
		uint8_t bit_trans = (dt & (0x80>>c))>>(7-c);
		HAL_GPIO_WritePin(_hc595->GPIO_CLK_PORT,_hc595->GPIO_CLK_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(_hc595->GPIO_DATA_PORT, _hc595->GPIO_DATA_PIN, bit_trans);
		HAL_GPIO_WritePin(_hc595->GPIO_CLK_PORT,_hc595->GPIO_CLK_PIN, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(_hc595->GPIO_LATCH_PORT, _hc595->GPIO_LATCH_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(_hc595->GPIO_LATCH_PORT, _hc595->GPIO_LATCH_PIN, GPIO_PIN_RESET);
}
