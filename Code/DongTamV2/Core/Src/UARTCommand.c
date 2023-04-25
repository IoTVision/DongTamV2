/*
 * ESP32_UART.c
 *
 *  Created on: Mar 27, 2023
 *      Author: SpiritBoi
 */
#include "StringUtility.h"
#include "UART_Utility.h"

#define MATCH_KEYWORD(KEYWORD) (StrUtil_SearchKey(UART_ESP_buf,KEYWORD)==STRING_OK ? 1 : 0)
extern uint8_t UART_ESP_buf[15];
extern UART_Utility_t espUtil;

typedef enum{
	CMD_DS3231_SETTIME,
	CMD_DS3231_GETTIME,
	CMD_VALVE_CONTROL,
	CMD_VALVE_STATE,
	CMD_AMS5915_GET_VALUE,
}ESP_CMD;


void GetESP32Command()
{
	if(!UART_Util_CheckGetMessageComplete(&espUtil,true)) return;
	StrUtil_TokenMessage((char*)UART_ESP_buf," {},");
	memset(UART_ESP_buf,0,sizeof(UART_ESP_buf));

}

