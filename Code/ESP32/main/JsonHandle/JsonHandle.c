#include "JsonHandle.h"

#define MAX_NUM_VAN 16

void UpdateGetTime(cJSON *item,char *logMessage)
{
    if(!cJSON_IsString(item)) return;
    char T[20];
    strcpy(T,cJSON_GetStringValue(item));
    brdParam.Time = RTC_GetTimeFromString(T);
    RTC_PackTimeToString(brdParam.Time,T);
    sprintf(logMessage,"GetTime update:\t%s\n",T);
}

void UpdateSetTime(cJSON *item,char *logMessage)
{
    if(!cJSON_IsString(item)) return;
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
    if(!cJSON_IsNumber(item) && !cJSON_IsArray(item)) return;
    if(cJSON_IsNumber(item)){
        brdParam.Pressure[0] = cJSON_GetNumberValue(item);
        sprintf(logMessage,"Pressure update: %.2f\n",brdParam.Pressure[0]);
    }
    else if(cJSON_IsArray(item)){
        size_t arraySize = cJSON_GetArraySize(item);
        for(uint8_t i=0;i<arraySize;i++){
            if(i>PRESSURE_BUFFER){
                ESP_LOGE("PArray","item have more than PRESSURE_BUFFER");
                break;
            }
            brdParam.Pressure[i] = (float)cJSON_GetNumberValue(cJSON_GetArrayItem(item,i));
        }
        sprintf(logMessage,"Pressure array update\n");
        for(uint8_t i=0;i<PRESSURE_BUFFER;i++){
            ESP_LOGI("PArray","P[%u]%.2f",i,brdParam.Pressure[i]);
        }
    }
}

void UpdateSetVan(cJSON *item,char *logMessage)
{
    if(!cJSON_IsNumber(item)) return;
    uint32_t a = (uint32_t) cJSON_GetNumberValue(item);
    if(a > MAX_NUM_VAN - 1) {
        sprintf(logMessage,"Input van > MAX_NUM_VAN:%d",MAX_NUM_VAN);
        return;
    }
    brdParam.SetVan = a;
    sprintf(logMessage,"SetVan update:\t%ld\n",brdParam.SetVan);
    xEventGroupSetBits(evg1,EVT_SET_VAN);
}

void UpdateClearVan(cJSON *item,char *logMessage)
{
    if(!cJSON_IsNumber(item)) return;
    uint32_t a = (uint32_t) cJSON_GetNumberValue(item);
    if(a > MAX_NUM_VAN - 1) {
        sprintf(logMessage,"Input van > MAX_NUM_VAN:%d",MAX_NUM_VAN);
        return;
    }
    brdParam.ClearVan = a;
    sprintf(logMessage,"ClearVan update:\t%ld\n",brdParam.ClearVan);
    xEventGroupSetBits(evg1,EVT_CLEAR_VAN);
}

void UpdateVanValue(cJSON *item,char *logMessage)
{
    if(!cJSON_IsNumber(item)) return;
    brdParam.VanData = cJSON_GetNumberValue(item);
    sprintf(logMessage,"VanValue update:\t%ld\n",brdParam.VanData);
}

void UpdateVanState(cJSON *item,char *logMessage)
{
    if(!cJSON_IsString(item)) return;
    strcpy(brdParam.VanState,cJSON_GetStringValue(item));
    sprintf(logMessage,"VanState update:\t%s\n",brdParam.VanState);
}

void SendPrintParamEvt(cJSON *item,char *logMessage)
{
    xEventGroupSetBits(evg1,EVT_GET_FULL_PARAM);
    strcpy(logMessage,"Trig GET_FUL_PARAM evt");
}



void ObtainJsonItem(cJSON *cjs, char *JsonKey,void (*pParamUpdate)(cJSON *item,char *logMessage))
{
    char s[35];
    if(cJSON_HasObjectItem(cjs,JsonKey)){
        ESP_LOGI("JSON Parse","%s",JsonKey);
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
    ObtainJsonItem(cjs,JSON_PARSE_KEY_PRESSURE_ARRAY,&UpdatePressure);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_TIME,&UpdateGetTime);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_VAN_VALUE,&UpdateVanValue);
    ObtainJsonItem(cjs,JSON_PARSE_KEY_VANSTATE,&UpdateVanState);
    // End section
    // Section: Parse item to set value to STM32
    ObtainJsonItem(cjs,JSON_PARSE_KEY_GET_ALL_PARAM,&SendPrintParamEvt);
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
    if(!cJSON_HasObjectItem(object,jsonKey)) cJSON_AddNumberToObject(object,jsonKey,brdParam.Pressure[0]);   
    else cJSON_SetNumberValue(cJSON_GetObjectItem(object,jsonKey),brdParam.Pressure[0]);
    return cJSON_Print(object);
}



void CheckEvent_PackJsonData_Send( cJSON *cjs, 
                                char *jsonKey,
                                EventBits_t e, 
                                uint32_t event, 
                                char* (*pJsPrint)(cJSON * object, char *jsonKey))
{
    if(CHECKFLAG(e,event)){
        SendStringToUART(qSTM32Tx,pJsPrint(cjs,jsonKey));
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

