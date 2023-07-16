
#include "./main.h"

#include <stdio.h>

#include "./ShareVar.h"
#include "./UART.h"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
#include "MessageHandle/MessageHandle.h"
#include "GUI/GUI.h"
QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
cJSON *cjsMain;
EventGroupHandle_t evg1,evgJson;
TaskHandle_t taskCommon,taskUartHandleString;


void Setup();
void SendStringToUART(QueueHandle_t q,char *s);
void InitProcess();
esp_err_t TestFlashNVS();

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

void UartHandleString(void *pvParameter)
{
    char *s=NULL;
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS))
        {
            ESP_LOGI("UartHandleString","len of s:%d",strlen(s));
            SendStringToUART(qLogTx,s);
            free(s);
        }
    }
}


void InitProcess()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );


    cjsMain = cJSON_CreateObject();
    qLogTx = xQueueCreate(3,sizeof(char *));
    qUartHandle = xQueueCreate(6,sizeof(char *));
    qSTM32Tx = xQueueCreate(4,sizeof(char *));
    evg1 = xEventGroupCreate();
    UARTConfig();
    GuiInit();
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


void Setup()
{
    TaskHandle_t *taskGUIHandle = GUI_GetTaskHandle();
    InitProcess();
    ESP_LOGI("Notify","pass InitProcess");
    xTaskCreate(TaskUart, "TaskUart", 2048, NULL, 3, NULL);
    xTaskCreate(UartHandleString,"UartHandleString",4096,NULL,2,NULL);
    xTaskCreate(GUITask, "GUITask", 2048, NULL, 2, taskGUIHandle);
    xTaskCreate(TaskScanButton, "TaskScanButton", 2048, NULL, 1, NULL);
}














