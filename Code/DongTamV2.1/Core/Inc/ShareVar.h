/*
 * ShareVar.h
 *
 *  Created on: May 22, 2023
 *      Author: SpiritBoi
 */

#ifndef INC_SHAREVAR_H_
#define INC_SHAREVAR_H_


typedef struct BoarParam{
	HC595 hc595;
	HC165 hc165;
	AMS5915 ams;
	cJSON *cjsCommon;
	PCF8563_Handle pcf;
	float pressure;
	uint32_t VanState;
}BoarParam;

#define MAX_MESSAGE 200
#define FLAG_UART_ESP_RX_DONE (1<<0)
#define FLAG_UART_LOG_RX_DONE (1<<1)

#define FLAG_JSON_PACK_TIME (1<<0)
#define FLAG_JSON_PACK_PRESSURE (1<<1)
#define FLAG_JSON_UNPACK_MESSAGE (1<<2)
#define FLAG_JSON_NEW_MESSAGE (1<<3)
#define FLAG_JSON_PACK_VAN_VALUE (1<<4)
#define FLAG_JSON_PRINT_PARAM (1<<5)
#define FLAG_JSON_READ_HC165 (1 << 6)

#define FLAG_GET_PRESSURE (1<<0)
#define FLAG_SET_TIME (1<<1)
#define FLAG_GET_TIME (1<<2)
#define FLAG_SET_VAN (1<<3)
#define FLAG_CLEAR_VAN (1<<4)
#define FLAG_TRIG_VAN (1<<5)
#define FLAG_TRIG_VAN_PROCEDURE (1<<6)
#define FLAG_AFTER_TRIG_VAN_ON (1<<7)

#define JSON_STRING_PACK_TIME "Time"
#define JSON_STRING_PACK_PRESSURE "Pressure"
#define JSON_STRING_PACK_VAN_VALUE "VanValue"
#define JSON_STRING_PACK_VAN_STATE "HC165"
#define JSON_STRING_SET_TIME "STime"
#define JSON_STRING_GET_TIME "GTime"
#define JSON_STRING_SET_VAN "SVan"
#define JSON_STRING_CLEAR_VAN "CVan"
#define JSON_STRING_TRIG_VAN "TrigVan"
#define JSON_STRING_GET_PRESSURE "GPressure"
#define JSON_STRING_GET_ALL_PARAM "PrintParam"

#define MAX_NUM_VAN 16

extern cJSON *cjsCommon;
extern FlagGroup_t fUART,f1,fJS;
extern char uartEsp32Buffer[MAX_MESSAGE],uartLogBuffer[MAX_MESSAGE];
extern uint16_t uartEsp32RxSize,uartLogRxSize;
extern BoarParam brdParam;
extern UART_HandleTypeDef *uartTarget;


#endif /* INC_SHAREVAR_H_ */
