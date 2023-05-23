/*
 * JsonHandle.h
 *
 *  Created on: May 23, 2023
 *      Author: SpiritBoi
 */

#ifndef INC_JSONHANDLE_H_
#define INC_JSONHANDLE_H_

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "RTC_Format.h"
#include "74HC595.h"
#include "74HC165.h"
#include "PCF8563.h"
#include "string.h"
#include "AMS5915.h"
#include "Flag.h"
#include "cJSON.h"
#include "ShareVar.h"



void UnpackMessage(cJSON *cjs);
void CheckItem_GetValue_SetFlag(cJSON *cjs,char* jsonKey,FlagGroup_t *f, uint32_t FlagBit,void (*pGetValue)(cJSON *object));
void PackJsonItem(cJSON *cjs, char* jsonKey, FlagGroup_t *f ,uint32_t FlagBit,void(*pSetJsonValue)(cJSON *object,cJSON *item,void* pvParam), void* inputData);
void SendJsonStringToUart(cJSON *cjs, char *keyName, UART_HandleTypeDef *huartx);

#endif /* INC_JSONHANDLE_H_ */
