
#include <stdio.h>
#include "main.h"
#include "UART.h"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
#include "MessageHandle/MessageHandle.h"
#include "ShareVar.h"
#include "GUI/GUI.h"
QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
cJSON *cjsMain;
BoardParameter brdParam;
EventGroupHandle_t evg1,evgJson;
TaskHandle_t taskCommon,taskUartHandleString;


void Setup();
EventBits_t CheckLogCommandList(char *s);
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
            if(CheckLogCommandList(s)) {
                ESP_LOGI("UartHandleString","Detect command in list");
            }
            else {
                ESP_LOGI("UartHandleString","len of s:%d",strlen(s));
                SendStringToUART(qLogTx,s);
            }
            free(s);
        }
    }
}

EventBits_t CheckLogCommandList(char *s)
{
    
}

void TaskCommon(void *pvParameter)
{
    while(1){
        vTaskDelay(10/portTICK_PERIOD_MS);
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
    GuiTestFull();
    ESP_ERROR_CHECK(TestFlashNVS()); 
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
    InitProcess();
    ESP_LOGI("Notify","pass InitProcess");
    
    xTaskCreate(TaskCommon, "TaskCommon", 2048, NULL, 1, &taskCommon);
    xTaskCreate(TaskUart, "TaskUart", 2048, NULL, 3, NULL);
    xTaskCreate(UartHandleString,"UartHandleString",4096,NULL,2,NULL);
    xTaskCreate(GUITask, "GUITask", 2048, NULL, 2, &taskGUIHandle);
    xTaskCreate(TaskScanButton, "TaskScanButton", 2048, NULL, 1, NULL);
}

esp_err_t TestFlashNVS()
{
    esp_err_t err = ESP_OK;
    size_t reqSize;
    nvs_handle_t nvsBrdStorage;
    // err = nvs_open("Board", NVS_READWRITE, &nvsBrdStorage);
    // if (err != ESP_OK) {
    //     printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    // } 
    // else {
    //     // err = nvs_set_str(nvsBrdStorage,"TestFlash","SpiritBoi");
    //     // err = nvs_set_blob(nvsBrdStorage,"Pressure",(void*)&p,sizeof(p));
    //     // err = nvs_commit(nvsBrdStorage);
    // }
    // nvs_close(nvsBrdStorage);
    // ESP_LOGI("NVS","Write TestFlash and close");
    // vTaskDelay(100/portTICK_PERIOD_MS);
    
    ESP_LOGI("NVS","Begin to read flash");
    err = nvs_open("Board", NVS_READONLY, &nvsBrdStorage);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        err = nvs_get_str(nvsBrdStorage,"TestFlash",NULL,&reqSize);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
        char *s = malloc(reqSize);
        err = nvs_get_str(nvsBrdStorage,"TestFlash",s,&reqSize);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
        ESP_LOGI("NVSTest","%s",s);

        double p1;
        size_t sz;
        char s1[15] = {0};
        err = nvs_get_blob(nvsBrdStorage,"Pressure",NULL,&sz);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
        err = nvs_get_blob(nvsBrdStorage,"Pressure",&p1,&sz);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
        sprintf(s1,"%.6f",p1);
        ESP_LOGI("NVSTest","%s",s1);
    }
    nvs_close(nvsBrdStorage);

    return err;
}












