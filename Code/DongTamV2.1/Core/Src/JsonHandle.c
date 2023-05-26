/*
 * JsonHandle.c
 *
 *  Created on: May 23, 2023
 *      Author: SpiritBoi
 */

#include "JsonHandle.h"

void SendJsonStringToUart(cJSON *cjs, char *keyName, UART_HandleTypeDef *huartx)
{
	if(keyName){
		char s[40]={0};
		strcpy(s,"{\n\t\"");
		strcat(s,keyName);
		strcat(s,"\":\t");
		strcat(s,cJSON_Print(cJSON_GetObjectItemCaseSensitive(cjs, keyName)));
		strcat(s,"\n}\n");
		HAL_UART_Transmit(huartx, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
	else {
		char *s = cJSON_Print(cjs);
		strcat(s,"\r\n");
		HAL_UART_Transmit(huartx, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
}








void PackJsonPressure(cJSON *object, cJSON *item, void* pvParam)
{
	float *p = (float*)pvParam;
	if(item != NULL) cJSON_SetNumberHelper(item,*p);
	else cJSON_AddNumberToObject(object,JSON_STRING_PACK_PRESSURE,*p);
}

void PackJsonVanValue(cJSON *object, cJSON *item, void* pvParam)
{
	uint32_t *data = (uint32_t*)pvParam;
	if(item != NULL)  cJSON_SetNumberHelper(item,*data);
	else cJSON_AddNumberToObject(object,JSON_STRING_PACK_VAN_VALUE,*data);
}

void PackJsonTime(cJSON *object, cJSON *item, void* pvParam)
{
	RTC_t *t = (RTC_t*)pvParam;
	char TimeString[20] = {0};
	RTC_PackTimeToString(t, TimeString);
	if(item != NULL) cJSON_SetValuestring(item,TimeString);
	else cJSON_AddStringToObject(object,JSON_STRING_PACK_TIME,TimeString);
}

void PackJsonVanState(cJSON *object, cJSON *item, void* pvParam)
{
	uint32_t* data = (uint32_t*)pvParam;
	char s[33] = {0};
	itoa(*data, s, 2);
	if(item != NULL) cJSON_SetValuestring(item,s);
	else cJSON_AddStringToObject(object,JSON_STRING_PACK_VAN_STATE,s);
	memset(data,0,strlen(s));
}

void PackJsonItem(cJSON *cjs, char* jsonKey, FlagGroup_t *f ,uint32_t FlagBit,void(*pSetJsonValue)(cJSON *object,cJSON *item,void* pvParam), void* inputData)
{
	if(CHECKFLAG(*f,FlagBit)){
		if(cJSON_HasObjectItem(cjs, jsonKey)){
			cJSON *item = cJSON_GetObjectItemCaseSensitive(cjs, jsonKey);
			pSetJsonValue(cjs,item,(void*)inputData);
		} else pSetJsonValue(cjs,NULL,(void*)inputData);
		SendJsonStringToUart(cjs, jsonKey, uartTarget);
		cJSON_DeleteItemFromObject(cjs,jsonKey);
		CLEARFLAG(*f,FlagBit);
	}
}


void PackageMessage(cJSON *cjs)
{
	FlagGroup_t fTemp =   FLAG_JSON_PRINT_PARAM
						| FLAG_JSON_PACK_VAN_VALUE
						| FLAG_JSON_PACK_TIME
						| FLAG_JSON_PACK_PRESSURE;

	if(!CHECKFLAG(fJS,FLAG_JSON_PRINT_PARAM)){
		PackJsonItem(cjs,JSON_STRING_PACK_PRESSURE,&fJS,FLAG_JSON_PACK_PRESSURE,&PackJsonPressure,&brdParam.pressure);
		PackJsonItem(cjs,JSON_STRING_PACK_TIME,&fJS,FLAG_JSON_PACK_TIME,&PackJsonTime,&brdParam.pcf.t);
		PackJsonItem(cjs,JSON_STRING_PACK_VAN_VALUE,&fJS,FLAG_JSON_PACK_VAN_VALUE,&PackJsonVanValue,&brdParam.hc595.data);
		PackJsonItem(cjs,JSON_STRING_PACK_VAN_STATE,&fJS,FLAG_JSON_READ_HC165,&PackJsonVanState,&brdParam.VanState);
	}
	if(CHECKFLAG(fJS,fTemp)){
//		PackJsonItem(cjs,JSON_STRING_PACK_PRESSURE,&fJS,FLAG_JSON_PACK_PRESSURE,&PackJsonPressure,&brdParam.pressure);
//		PackJsonItem(cjs,JSON_STRING_PACK_TIME,&fJS,FLAG_JSON_PACK_TIME,&PackJsonTime,&brdParam.pcf.t);
//		PackJsonItem(cjs,JSON_STRING_PACK_VAN_VALUE,&fJS,FLAG_JSON_PACK_VAN_VALUE,&PackJsonVanValue,&brdParam.hc595.data);
		CLEARFLAG(fJS,fTemp);
	}
}







void SetVanHandle(cJSON *object)
{
	uint8_t SetVan;
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, JSON_STRING_SET_VAN);
	if(!cJSON_IsNumber(item)) return;
	SetVan = (uint8_t)cJSON_GetNumberValue(item);
	if(SetVan < MAX_NUM_VAN) HC595_SetBitOutput(SetVan);
	else {
		char *s = NULL;
		sprintf(s,"Van num must be less than %d",MAX_NUM_VAN);
		HAL_UART_Transmit(uartTarget, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
	SETFLAG(fJS,FLAG_JSON_PACK_VAN_VALUE);
}
void ClearVanHandle(cJSON *object)
{
	uint8_t ClearVan;
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, JSON_STRING_CLEAR_VAN);
	if(!cJSON_IsNumber(item)) return;
	ClearVan = (uint8_t)cJSON_GetNumberValue(item);
	if(ClearVan < MAX_NUM_VAN) HC595_ClearBitOutput(ClearVan);
	else {
		char *s = NULL;
		sprintf(s,"Van num must be less than %d",MAX_NUM_VAN);
		HAL_UART_Transmit(uartTarget, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
	SETFLAG(fJS,FLAG_JSON_PACK_VAN_VALUE);
}

void TrigVanHandle(cJSON *object)
{
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, JSON_STRING_TRIG_VAN);
	if(!cJSON_IsTrue(item)) return;
	SETFLAG(f1, FLAG_TRIG_VAN_PROCEDURE);
}

void SetTimeHandle(cJSON *object)
{
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, JSON_STRING_SET_TIME);
	if(!cJSON_IsString(item)) return;
	brdParam.pcf.t = RTC_GetTimeFromString(cJSON_GetStringValue(item));
	PCF8563_WriteTimeRegisters(brdParam.pcf.t);
	SETFLAG(fJS,FLAG_JSON_PACK_TIME); // to read back time value for double check
}

void GetTimeHandle(cJSON *object)
{
	brdParam.pcf.t = PCF8563_ReadTimeRegisters();
	SETFLAG(fJS,FLAG_JSON_PACK_TIME);
}
void GetAsm5915Pressure(cJSON *object)
{
	brdParam.pressure = AMS5915_CalPressure(&brdParam.ams);
	SETFLAG(fJS,FLAG_JSON_PACK_PRESSURE);
}

void CheckItem_GetValue_SetFlag(cJSON *cjs,char* jsonKey,FlagGroup_t *f, uint32_t FlagBit,void (*pGetValue)(cJSON *object))
{
	if(cJSON_HasObjectItem(cjs,jsonKey)) {
		if(pGetValue) pGetValue(cjs);
		cJSON_DeleteItemFromObject(cjs, jsonKey);
		SETFLAG(*f,FlagBit);
	}
}

void UnpackMessage(cJSON *cjs)
{
	if(!CHECKFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE)) return;
	CLEARFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE);
	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_PACK_VAN_VALUE,&fJS,FLAG_JSON_PACK_VAN_VALUE,NULL);
	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_GET_ALL_PARAM,&fJS,FLAG_JSON_PACK_PRESSURE
																| FLAG_JSON_PACK_TIME
																| FLAG_JSON_PRINT_PARAM
																| FLAG_JSON_PACK_VAN_VALUE,NULL);

	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_GET_PRESSURE, &f1, FLAG_GET_PRESSURE,&GetAsm5915Pressure);
	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_GET_TIME,&f1,FLAG_GET_TIME,&GetTimeHandle);
	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_SET_TIME,&f1,FLAG_SET_TIME,&SetTimeHandle);
	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_SET_VAN,&f1,FLAG_SET_VAN,&SetVanHandle);
	CheckItem_GetValue_SetFlag(cjs, JSON_STRING_CLEAR_VAN,&f1,FLAG_CLEAR_VAN,&ClearVanHandle);
	//No need to set any flag to trigger, SetTrigVanFlag automatically set itself if true
	CheckItem_GetValue_SetFlag(cjs, "TrigVan",&f1,0,&TrigVanHandle);
}
