#ifndef JSON_HANDLE_H
#define JSON_HANDLE_H

#include "main.h"
#include "ShareVar.h"
#include "cJSON.h"
#include "RTC_Format.h"



// Use to found item match key value after parsing the receive string
#define JSON_PARSE_KEY_PRESSURE "Pressure"
#define JSON_PARSE_KEY_PRESSURE_ARRAY "PressureArray"
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



void UpdateParamFromParsedJsonItem(cJSON *cjs);
void ObtainJsonItem(cJSON *cjs, char *JsonKey,void (*pParamUpdate)(cJSON *item,char *logMessage));
void SendCommandToSTM32(cJSON *cjs);
#endif