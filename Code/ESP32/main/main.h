#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/queue.h"
// #include "RTC_Format.h"
typedef enum EventCommand{
    GET_PRESSURE,
    GET_TIME,
    GET_VAN_VALUE,
    GET_FULL_PARAM,
    SET_TIME,
    SET_VAN,
    CLEAR_VAN,
    TRIG_VAN,
}EventCommand;

#define EVT_GET_PRESSURE (1<<GET_PRESSURE)
#define EVT_GET_TIME (1<<GET_TIME)
#define EVT_GET_VAN_VALUE (1<<GET_VAN_VALUE)
#define EVT_GET_FULL_PARAM (1<<GET_FULL_PARAM)

#define EVT_SET_TIME (1<<SET_TIME)
#define EVT_SET_VAN (1<<SET_VAN)
#define EVT_CLEAR_VAN (1<<CLEAR_VAN)
#define EVT_TRIG_VAN (1<<TRIG_VAN)

// Use to found item match key value after parsing the receive string
#define JSON_PARSE_KEY_PRESSURE "Pressure"
#define JSON_PARSE_KEY_TIME "Time"
#define JSON_PARSE_KEY_VANSTATE "HC165"
#define JSON_PARSE_KEY_VAN_VALUE "VanValue"
#define JSON_PARSE_KEY_SET_VAN "SVan"
#define JSON_PARSE_KEY_CLEAR_VAN "CVan"
#define JSON_PARSE_KEY_TRIG_VAN "TrigVan"
#define JSON_PARSE_KEY_SET_TIME "STime"
#define JSON_PARSE_KEY_GET_ALL_PARAM "PrintParam"


// Command to request STM32
#define JSON_KEY_GET_TIME "GTime"
#define JSON_KEY_GET_PRESSURE "GPressure"
#define JSON_KEY_GET_VAN_VALUE "VanValue"
#define JSON_KEY_GET_ALL_PARAM "PrintParam"
#define JSON_KEY_SET_TIME "STime"
#define JSON_KEY_SET_VAN "SVan"
#define JSON_KEY_CLEAR_VAN "CVan"
#define JSON_KEY_TRIG_VAN "TrigVan"

// reference in cJSON library
#define cJSON_OFFSET_BYTES 5

#define CHECKFLAG(FlagGroup,FlagBit) ((((FlagGroup) & (FlagBit)) == (FlagBit)) ? 1 : 0) 


void SendStringToUART(QueueHandle_t q,char *s);
void UartHandleString(void *pvParameter);
#endif