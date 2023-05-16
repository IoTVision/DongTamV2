
#include <stdio.h>
#include "main.h"
#include "UART.c"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
// static const char *TAG= "main";

QueueHandle_t qLogTx;
cJSON *cjsMain;
void taskJsonHandle(void *pvParameter);

void Setup();

void app_main(void)
{
    Setup();
    char *s = NULL;
    while (1) {
        if(xQueueReceive(qLogTx,&s,10/portTICK_PERIOD_MS)){
            uart_write_bytes(UART_NUM_0,s,strlen(s));
            // ESP_LOGI("MAIN","Rx:%s",s);
        }
    }
}

void taskJsonHandle(void *pvParameter)
{
    cjsMain = cJSON_CreateObject();
    cJSON_AddNumberToObject(cjsMain,"Pressure",0.53136);
    cJSON_AddNumberToObject(cjsMain,"VanState",1024);
    cJSON_AddStringToObject(cjsMain,"Time","10:21:00 16/5/23");
    char *s;
    s = cJSON_Print(cjsMain);
    xQueueSend(qLogTx,(void *)&s,10/portTICK_PERIOD_MS);
    while(1){
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
    qLogTx = xQueueCreate(10,sizeof(char *));
    UARTConfig();
    PressureIndicator_Init();
    xTaskCreate(taskUart, "taskUart", 2048, NULL, 4, NULL);
    xTaskCreate(taskJsonHandle, "taskJsonHandle", 2048, NULL, 4, NULL);
}














