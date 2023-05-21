#include "JsonHandle.h"

void UpdateGetTime(cJSON *item,char *logMessage)
{
    char T[20];
    strcpy(T,cJSON_GetStringValue(item));
    brdParam.Time = RTC_GetTimeFromString(T);
    RTC_PackTimeToString(brdParam.Time,T);
    sprintf(logMessage,"GetTime update:\t%s\n",T);
}

void UpdateSetTime(cJSON *item,char *logMessage)
{
    char T[20];
    strcpy(T,cJSON_GetStringValue(item));
    brdParam.Time = RTC_GetTimeFromString(T);
    RTC_PackTimeToString(brdParam.Time,T);
    sprintf(logMessage,"SetTime update:\t%s\n",T);
    xEventGroupSetBits(evg1,EVT_SET_TIME);
}

void UpdateTrigVan(cJSON *item,char *logMessage)
{
    sprintf(logMessage,"TrigVan is send\n");
    xEventGroupSetBits(evg1,EVT_TRIG_VAN);
}

void UpdatePressure(cJSON *item,char *logMessage)
{
    brdParam.Pressure = cJSON_GetNumberValue(item);
    sprintf(logMessage,"Pressure update: %.2f",brdParam.Pressure);
}

void UpdateSetVan(cJSON *item,char *logMessage)
{
    brdParam.SetVan = cJSON_GetNumberValue(item);
    sprintf(logMessage,"SetVan update:\t%ld\n",brdParam.SetVan);
    xEventGroupSetBits(evg1,EVT_SET_VAN);
}

void UpdateClearVan(cJSON *item,char *logMessage)
{
    brdParam.ClearVan = cJSON_GetNumberValue(item);
    sprintf(logMessage,"ClearVan update:\t%ld\n",brdParam.ClearVan);
    xEventGroupSetBits(evg1,EVT_CLEAR_VAN);
}

void UpdateVanValue(cJSON *item,char *logMessage)
{
    brdParam.VanData = cJSON_GetNumberValue(item);
    sprintf(logMessage,"VanValue update:\t%ld\n",brdParam.VanData);
}

void UpdateVanState(cJSON *item,char *logMessage)
{
    strcpy(brdParam.VanState,cJSON_GetStringValue(item));
    sprintf(logMessage,"VanState update:\t%s\n",brdParam.VanState);
}

void UpdatePrintParam(cJSON *item,char *logMessage)
{

}

void ObtainJsonItem(cJSON *cjs, char *JsonKey,void (*pParamUpdate)(cJSON *item,char *logMessage))
{
    char s[35];
    if(cJSON_HasObjectItem(cjs,JsonKey)){
        cJSON *item = cJSON_GetObjectItemCaseSensitive(cjs,JsonKey);
        pParamUpdate(item,s);
        SendStringToUART(qLogTx,s);
        cJSON_DeleteItemFromObject(cjs,JsonKey); 
    }
}



void UpdateParamFromParsedJsonItem(cJSON *cjs)
{
    // Section: Parse Item to get value from STM32
    ObtainJsonItem(cjs,JSON_PARSE_KEY_PRESSURE,&UpdatePressure);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_TIME,&UpdateGetTime);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_VAN_VALUE,&UpdateVanValue);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_VANSTATE,&UpdateVanState);
    // PrintPram;
    // Section end: Parse Item to get value from STM32
    // Section: Parse item to set value to STM32
    ObtainJsonItem(cjs,JSON_PARSE_KEY_SET_VAN,&UpdateSetVan);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_CLEAR_VAN,&UpdateClearVan);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_TRIG_VAN,&UpdateTrigVan);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_SET_TIME,&UpdateSetTime);
    // End section
}

char* PrintJsonGetItem(cJSON *object, char *jsonKey)
{
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddStringToObject(object,jsonKey,"");   
    else cJSON_SetValuestring(cJSON_GetObjectItem(object,jsonKey),""); 
    return cJSON_Print(object);
}

char* PrintJsonSetTime(cJSON *object, char *jsonKey)
{
    char s[20];
    RTC_PackTimeToString(brdParam.Time,s);
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddStringToObject(object,jsonKey,s);   
    else cJSON_SetValuestring(cJSON_GetObjectItem(object,jsonKey),s);
    return cJSON_Print(object);
}

char* PrintJsonSetVan(cJSON *object, char *jsonKey)
{
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddNumberToObject(object,jsonKey,brdParam.SetVan);   
    else cJSON_SetIntValue(cJSON_GetObjectItem(object,jsonKey),brdParam.SetVan);
    return cJSON_Print(object);
}

char* PrintJsonClearVan(cJSON *object, char *jsonKey)
{
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddNumberToObject(object,jsonKey,brdParam.ClearVan);   
    else cJSON_SetIntValue(cJSON_GetObjectItem(object,jsonKey),brdParam.ClearVan);
    return cJSON_Print(object);
}

char* PrintJsonTrigVan(cJSON *object, char *jsonKey)
{
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddTrueToObject(object,jsonKey);  
    return cJSON_Print(object);
}

char* PrintJsonGetPressure(cJSON *object, char *jsonKey)
{
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddNumberToObject(object,jsonKey,brdParam.Pressure);   
    else cJSON_SetNumberValue(cJSON_GetObjectItem(object,jsonKey),brdParam.Pressure);
    return cJSON_Print(object);
}



void CheckEvent_PackJsonData_Send( cJSON *cjs, 
                                char *jsonKey,
                                EventBits_t e, 
                                uint32_t event, 
                                char* (*pJsPrint)(cJSON * object, char *jsonKey))
{
    if(CHECKFLAG(e,event)){
        SendStringToUART(qLogTx,pJsPrint(cjs,jsonKey));
        cJSON_DeleteItemFromObjectCaseSensitive(cjs,jsonKey);
    }
}

void SendCommandToSTM32(cJSON *cjs)
{
    EventBits_t BitsToWaitFor = EVT_GET_TIME 
                                | EVT_GET_VAN_VALUE 
                                | EVT_GET_PRESSURE 
                                | EVT_GET_FULL_PARAM
                                | EVT_SET_TIME 
                                | EVT_SET_VAN 
                                | EVT_CLEAR_VAN
                                | EVT_TRIG_VAN;
    EventBits_t e = xEventGroupWaitBits(evg1,BitsToWaitFor,pdTRUE,pdFALSE,0);
    if(!e) return;
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_GET_TIME,e,      EVT_GET_TIME,        &PrintJsonGetItem);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_GET_VAN_VALUE,e, EVT_GET_VAN_VALUE,   &PrintJsonGetItem);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_GET_PRESSURE,e,  EVT_GET_PRESSURE,    &PrintJsonGetItem);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_GET_ALL_PARAM,e, EVT_GET_FULL_PARAM,  &PrintJsonGetItem);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_CLEAR_VAN,e,     EVT_CLEAR_VAN,       &PrintJsonClearVan);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_SET_VAN,e,       EVT_SET_VAN,         &PrintJsonSetVan);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_TRIG_VAN,e,      EVT_TRIG_VAN,        &PrintJsonTrigVan);
    CheckEvent_PackJsonData_Send(cjs,JSON_KEY_SET_TIME,e,      EVT_SET_TIME,        &PrintJsonSetTime);
}

EventBits_t CheckLogCommandList(char *s)
{
#define COMPARE_STRING_SET_EVENT(STRING_COMPARE,EVENT) (strcmp(s,(STRING_COMPARE)) == 0 ? (xEventGroupSetBits(evg1,(EVENT))) : 0)
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_TIME,EVT_GET_TIME)) return EVT_GET_TIME; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_ALL_PARAM,EVT_GET_FULL_PARAM))return EVT_GET_FULL_PARAM; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_PRESSURE,EVT_GET_PRESSURE))return EVT_GET_PRESSURE; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_VAN_VALUE,EVT_GET_VAN_VALUE))return EVT_GET_VAN_VALUE;
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_TRIG_VAN,EVT_TRIG_VAN))return EVT_TRIG_VAN; 
    return 0; 
#undef COMPARE_STRING_SET_EVENT
}