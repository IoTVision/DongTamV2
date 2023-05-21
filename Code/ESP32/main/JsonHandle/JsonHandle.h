#ifndef JSON_HANDLE_H
#define JSON_HANDLE_H

#include "main.h"
#include "ShareVar.h"
#include "cJSON.h"
#include "RTC_Format.h"

void UpdateParamFromParsedJsonItem(cJSON *cjs);
void ObtainJsonItem(cJSON *cjs, char *JsonKey,void (*pParamUpdate)(cJSON *item,char *logMessage));
void SendCommandToSTM32(cJSON *cjs);
EventBits_t CheckLogCommandList(char *s);
#endif