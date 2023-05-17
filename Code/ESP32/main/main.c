
#include <stdio.h>
#include "main.h"
#include "UART.c"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
// static const char *TAG= "main";

QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
cJSON *cjsMain;
BoardParameter brdParam;
EventGroupHandle_t evg1,evgJson;

#define EVT_GET_PRESSURE (1<<0)
#define EVT_GET_TIME (1<<1)
#define EVT_GET_VANSTATE (1<<3)
#define EVT_GET_FULL_PARAM (1<<4)

#define EVT_SET_TIME (1<<5)
#define EVT_SET_VAN (1<<6)
#define EVT_CLEAR_VAN (1<<7)
#define EVT_TRIG_VAN (1<<8)

#define CHECKFLAG(FlagGroup,FlagBit) ((((FlagGroup) & (FlagBit)) == (FlagBit)) ? 1 : 0) 

void Setup();

void app_main(void)
{
    Setup();
    char *s = NULL;
    while (1) {
        if(xQueueReceive(qLogTx,&s,10/portTICK_PERIOD_MS)){
            uart_write_bytes(UART_NUM_0,s,strlen(s));
        }
    }
}

void JsonStringHandle()
{
    
}

void GetParamFromSTM32()
{
    EventBits_t BitsToWaitFor = EVT_GET_TIME 
                                | EVT_GET_VANSTATE 
                                | EVT_GET_PRESSURE 
                                | EVT_GET_FULL_PARAM; 
    EventBits_t e = xEventGroupWaitBits(evg1,BitsToWaitFor,pdTRUE,pdFALSE,0);
    if(!e) return;
    if(CHECKFLAG(e,EVT_GET_TIME)){

    }
    if(CHECKFLAG(e,EVT_GET_VANSTATE)){
        
    }
    if(CHECKFLAG(e,EVT_GET_FULL_PARAM)){
        
    }
    if(CHECKFLAG(e,EVT_GET_PRESSURE)){
        
    }
}

void SendingCommandToSTM32()
{
    EventBits_t BitsToWaitFor = EVT_SET_TIME 
                                | EVT_SET_VAN 
                                | EVT_CLEAR_VAN
                                | EVT_TRIG_VAN; 
    EventBits_t e = xEventGroupWaitBits(evg1,BitsToWaitFor,pdTRUE,pdFALSE,0);
    if(!e) return;
    if(CHECKFLAG(e,EVT_CLEAR_VAN)){

    }
    if(CHECKFLAG(e,EVT_SET_TIME)){

    }
    if(CHECKFLAG(e,EVT_SET_VAN)){
        
    }
    if(CHECKFLAG(e,EVT_TRIG_VAN)){
        
    }
}

void UnpackeMessage()
{

}

void UartHandleString(void *pvParameter)
{
    char *s;
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS)){
            ESP_LOGI("MAIN","Receive from UART task %s",s);
            free(s);
        }
    }
}

void TaskCommon(void *pvParameter)
{
    
    while(1){
        SendingCommandToSTM32();
        GetParamFromSTM32();
        JsonStringHandle();
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














