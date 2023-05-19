
#include <stdio.h>
#include "main.h"
#include "UART.c"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
// static const char *TAG= "main";
typedef struct BoardParameter{
    char ID[12];
    float Pressure;
    RTC_t Time;
    char VanState[16];
    uint32_t VanData;
    uint32_t SetVan;
    uint32_t ClearVan;
}BoardParameter;


QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
cJSON *cjsMain;
BoardParameter brdParam;
EventGroupHandle_t evg1,evgJson;



#define CHECKFLAG(FlagGroup,FlagBit) ((((FlagGroup) & (FlagBit)) == (FlagBit)) ? 1 : 0) 

void Setup();

void app_main(void)
{
    Setup();
    char *s = NULL;
    while (1) {
        if(xQueueReceive(qLogTx,&s,10/portTICK_PERIOD_MS)){
            uart_write_bytes(UART_NUM_0,s,strlen(s));
            free(s);
        }
    }
}
void SendStringToUART(QueueHandle_t q,char *s)
{
    char *a = (char *) malloc(strlen(s) + cJSON_OFFSET_BYTES);
    if(a) {
        strcpy(a,s);
        xQueueSend(q,(void*)&a,2/portTICK_PERIOD_MS);
    }
    else ESP_LOGI("MAIN","Cannot malloc to send queue");
}


void UpdateParamFromParsedJsonItem(cJSON *cjs)
{
    char s[40];
    if(cJSON_HasObjectItem(cjs,JSON_PARSE_KEY_PRESSURE)){
        cJSON *item = cJSON_GetObjectItemCaseSensitive(cjs,JSON_PARSE_KEY_PRESSURE);
        brdParam.Pressure = cJSON_GetNumberValue(item);
        sprintf(s,"Pressure update: %.2f",brdParam.Pressure);
        SendStringToUART(qLogTx,s);
        cJSON_DeleteItemFromObject(cjs,JSON_PARSE_KEY_PRESSURE); 
    }
    if(cJSON_HasObjectItem(cjs,JSON_PARSE_KEY_TIME)){
        cJSON *item;
        item = cJSON_GetObjectItemCaseSensitive(cjs,JSON_PARSE_KEY_TIME);
        char T[20];
        strcpy(T,cJSON_GetStringValue(item));
        brdParam.Time = RTC_GetTimeFromString(T);
        RTC_PackTimeToString(brdParam.Time,T);
        ESP_LOGI("Time","%u/%u/%u %u:%u:%u",brdParam.Time.day,
                                            brdParam.Time.month,
                                            brdParam.Time.year, 
                                            brdParam.Time.hour, 
                                            brdParam.Time.minute, 
                                            brdParam.Time.second);
        sprintf(s,"Time update: %s",T);
        SendStringToUART(qLogTx,s);
        cJSON_DeleteItemFromObject(cjs,JSON_PARSE_KEY_TIME);
    }
    if(cJSON_HasObjectItem(cjs,JSON_PARSE_KEY_VAN_VALUE)){
        cJSON *item = cJSON_GetObjectItemCaseSensitive(cjs,JSON_PARSE_KEY_VAN_VALUE);
        brdParam.VanData = cJSON_GetNumberValue(item);
        sprintf(s,"VanValue update: %ld",brdParam.VanData);
        SendStringToUART(qLogTx,s);
        cJSON_DeleteItemFromObject(cjs,JSON_PARSE_KEY_VAN_VALUE);
    }
    if(cJSON_HasObjectItem(cjs,JSON_PARSE_KEY_VANSTATE)){
        cJSON *item = cJSON_GetObjectItemCaseSensitive(cjs,JSON_PARSE_KEY_VANSTATE);
        strcpy(brdParam.VanState,cJSON_GetStringValue(item));
        sprintf(s,"VanState update: %s",brdParam.VanState);
        SendStringToUART(qLogTx,s);
        cJSON_DeleteItemFromObject(cjs,JSON_PARSE_KEY_VANSTATE);
    }
    
}

static inline void CheckEventAndPackJsonData(EventBits_t e, 
                                            cJSON *cjs, 
                                            char *jsonKey,
                                            uint32_t event, 
                                            JSON_TYPE jsType, void* value)
{
    if(CHECKFLAG(e,event)){
        switch (jsType)
        {
        case JSON_TYPE_STRING:
            cJSON_AddStringToObject(cjs,jsonKey,(char*)value);    
            break;
        case JSON_TYPE_NUMBER:
            cJSON_AddNumberToObject(cjs,jsonKey,*((double*)value));
            break;
        case JSON_TYPE_BOOL:
            if(value) cJSON_AddTrueToObject(cjs,jsonKey);
            else cJSON_AddFalseToObject(cjs,jsonKey);
            break;
        default:
            cJSON_AddStringToObject(cjs,jsonKey,"");
            break;
        }
        SendStringToUART(qLogTx,cJSON_Print(cjs));
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
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_GET_TIME,      EVT_GET_TIME,0,NULL);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_GET_VAN_VALUE, EVT_GET_VAN_VALUE,0,NULL);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_GET_PRESSURE,  EVT_GET_PRESSURE,0,NULL);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_GET_ALL_PARAM, EVT_GET_FULL_PARAM,0,NULL);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_CLEAR_VAN,     EVT_CLEAR_VAN,JSON_TYPE_NUMBER,&brdParam.ClearVan);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_SET_VAN,       EVT_SET_VAN,JSON_TYPE_NUMBER,&brdParam.SetVan);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_TRIG_VAN,      EVT_TRIG_VAN,JSON_TYPE_BOOL,(void*)1);
    char s[20];
    RTC_PackTimeToString(brdParam.Time,s);
    CheckEventAndPackJsonData(e,cjs,JSON_KEY_SET_TIME,      EVT_SET_TIME,JSON_TYPE_STRING,s);
}

EventBits_t CheckCommandList(char *s)
{
#define COMPARE_STRING_SET_EVENT(STRING_COMPARE,EVENT) (strcmp(s,(STRING_COMPARE)) == 0 ? (xEventGroupSetBits(evg1,(EVENT))) : 0)
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_TIME,EVT_GET_TIME)) return EVT_GET_TIME; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_ALL_PARAM,EVT_GET_FULL_PARAM))return EVT_GET_FULL_PARAM; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_PRESSURE,EVT_GET_PRESSURE))return EVT_GET_PRESSURE; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_VAN_VALUE,EVT_GET_VAN_VALUE))return EVT_GET_VAN_VALUE;
    return 0; 
#undef COMPARE_STRING_SET_EVENT
}

void UartHandleString(void *pvParameter)
{
    char *s;
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS)){
            if(strstr(s,"{") && strstr(s,"}")){ // is JSON format
                cjsMain = cJSON_Parse(s); 
            } 
            else if(CheckCommandList(s)) {
                ESP_LOGI("UartHandleString","Detect command in list");
            }
            else SendStringToUART(qLogTx,"This is Log");
            free(s);
        }
    }
}

void TaskCommon(void *pvParameter)
{
    while(1){
        SendCommandToSTM32(cjsMain);
        UpdateParamFromParsedJsonItem(cjsMain);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void PressureIndicator_Init()
{
    // PI_ConfigPin();
    // for(uint8_t i=0;i<10;i++)PI_TestShowLevel_Decrease();
    // for(uint8_t i=0;i<10;i++)PI_TestShowLevel_Increase();
    // PI_SetLevel(0);
}

void Setup()
{
    cjsMain = cJSON_CreateObject();
    qLogTx = xQueueCreate(3,sizeof(char *));
    qUartHandle = xQueueCreate(10,sizeof(char *));
    qSTM32Tx = xQueueCreate(4,sizeof(char *));
    evg1 = xEventGroupCreate();
    UARTConfig();
    PressureIndicator_Init();
    xTaskCreate(TaskCommon, "TaskCommon", 2048, NULL, 1, NULL);
    xTaskCreate(TaskUart, "TaskUart", 2048, NULL, 3, NULL);
    xTaskCreate(UartHandleString,"UartHandleString",2048,NULL,2,NULL);
}














