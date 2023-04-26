/*
 * ESP32_UART.c
 *
 *  Created on: Mar 27, 2023
 *      Author: SpiritBoi
 */
#include "StringUtility.h"
#include "UART_Utility.h"

#define MATCH_KEYWORD(KEYWORD) (StrUtil_SearchKey((char*)UART_ESP_buf,KEYWORD)==STRING_OK ? 1 : 0)
typedef enum{
	CMD_DS3231_SETTIME,
	CMD_DS3231_GETTIME,
	CMD_VALVE_CONTROL,
	CMD_VALVE_STATE,
	CMD_AMS5915_GET_VALUE,
}CMD;
void GetESP32Command(UART_Utility_t *Util,uint8_t *MessageData)
{
	if(!UART_Util_CheckGetMessageComplete(Util,true)) return;
	StrUtil_TokenMessage((char*)MessageData," {},");
	if(MATCH_KEYWORD("DSSETTIME")) SETFLAG(dsFlag,FLAG_DS3231_SET_TIME);
	if(MATCH_KEYWORD("PCFSTART")) SETFLAG(dsFlag,FLAG_PCF8563_START_CLOCK);
	if(MATCH_KEYWORD("DSGETTIME")) SETFLAG(dsFlag,FLAG_DS3231_GET_TIME);
	memset(MessageData,0,sizeof(MessageData));

}

void GetLOGCommand()
{
	if(!UART_Util_CheckGetMessageComplete(&logUtil,true)) return;
	StrUtil_TokenMessage((char*)UART_ESP_buf," {},");
	if(MATCH_KEYWORD("DSSETTIME")) SETFLAG(dsFlag,FLAG_DS3231_SET_TIME);
	if(MATCH_KEYWORD("PCFSTART")) SETFLAG(dsFlag,FLAG_PCF8563_START_CLOCK);
	if(MATCH_KEYWORD("DSGETTIME")) SETFLAG(dsFlag,FLAG_DS3231_GET_TIME);
	memset(UART_ESP_buf,0,sizeof(UART_ESP_buf));

}

