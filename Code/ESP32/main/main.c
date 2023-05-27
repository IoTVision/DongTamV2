
#include <stdio.h>
#include "main.h"
#include "UART.h"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
#include "JsonHandle/JsonHandle.h"
#include "ShareVar.h"
#include "GUI/GUI.h"
#include "nvs_flash.h"
#include "nvs.h"
// static const char *TAG= "main";
QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
cJSON *cjsMain;
BoardParameter brdParam;
EventGroupHandle_t evg1,evgJson;
TaskHandle_t taskCommon,taskUartHandleString;
nvs_handle_t nvsBrdStorage;

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
    char *s;
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS))
        {
            if(strstr(s,"{") && strstr(s,"}")){ // is JSON format
                // while(!strstr(s,"}")) vTaskDelay(10/portTICK_PERIOD_MS);
                // cjsMain = cJSON_Parse(s); 
                ESP_LOGI("UartHandleString","Detect {}, size: %d",strlen(s));
            } 
            else if(CheckLogCommandList(s)) {
                ESP_LOGI("UartHandleString","Detect command in list");
            }
            else {
                ESP_LOGI("UartHandleString","len of s:%d",strlen(s));
                ESP_LOGI("UartHandleString","content %s",s);
                // SendStringToUART(qLogTx,s);
            }
            ESP_LOGI("UartHandleString","Receive:%p",s);
            free(s);
            ESP_LOGI("UartHandleString","free s");
        }
    }
}

EventBits_t CheckLogCommandList(char *s)
{
#define COMPARE_STRING_SET_EVENT(STRING_COMPARE,EVENT) (strcmp(s,(STRING_COMPARE)) == 0 ? (xEventGroupSetBits(evg1,(EVENT))) : 0)
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_TIME,EVT_GET_TIME)) return EVT_GET_TIME; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_ALL_PARAM,EVT_GET_FULL_PARAM))return EVT_GET_FULL_PARAM; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_PRESSURE,EVT_GET_PRESSURE))return EVT_GET_PRESSURE; 
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_GET_VAN_VALUE,EVT_GET_VAN_VALUE))return EVT_GET_VAN_VALUE;
    if(COMPARE_STRING_SET_EVENT(JSON_KEY_TRIG_VAN,EVT_TRIG_VAN))return EVT_TRIG_VAN; 
    if(COMPARE_STRING_SET_EVENT(NVS_SAVE_VAN_VALUE,EVT_NVS_SAVE_VAN))return EVT_NVS_SAVE_VAN; 
    if(COMPARE_STRING_SET_EVENT(NVS_GET_VAN_VALUE,EVT_NVS_GET_VAN))return EVT_NVS_GET_VAN;
    return 0; 
#undef COMPARE_STRING_SET_EVENT
}

void TaskCommon(void *pvParameter)
{
    while(1){
        SendCommandToSTM32(cjsMain);
        UpdateParamFromParsedJsonItem(cjsMain);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

esp_err_t TestFlashNVS()
{
    esp_err_t err;
    size_t reqSize;
    err = nvs_open("Board", NVS_READWRITE, &nvsBrdStorage);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        // err = nvs_set_str(nvsBrdStorage,"TestFlash","SpiritBoi");
        // err = nvs_set_blob(nvsBrdStorage,"Pressure",(void*)&p,sizeof(p));
        err = nvs_commit(nvsBrdStorage);
    }
    nvs_close(nvsBrdStorage);
    ESP_LOGI("NVS","Write TestFlash and close");
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI("NVS","Begin to read flash");
    err = nvs_open("Board", NVS_READONLY, &nvsBrdStorage);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        err = nvs_get_str(nvsBrdStorage,"TestFlash",NULL,&reqSize);
        char *s = malloc(reqSize);
        err = nvs_get_str(nvsBrdStorage,"TestFlash",s,&reqSize);
        ESP_LOGI("NVSTest","%s",s);

        double p1;
        size_t sz;
        char s1[15] = {0};
        err = nvs_get_blob(nvsBrdStorage,"Pressure",NULL,&sz);
        err = nvs_get_blob(nvsBrdStorage,"Pressure",&p1,&sz);
        sprintf(s1,"%.6f",p1);
        ESP_LOGI("NVSTest","%s",s1);
    }
    nvs_close(nvsBrdStorage);

    return err;
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
    qUartHandle = xQueueCreate(10,sizeof(char *));
    qSTM32Tx = xQueueCreate(4,sizeof(char *));
    evg1 = xEventGroupCreate();
    UARTConfig();
    // GuiSetup();
    // GuiTest();
    // ESP_ERROR_CHECK(TestFlashNVS()); f
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
    xTaskCreate(UartHandleString,"UartHandleString",2048,NULL,2,NULL);
    xTaskCreate(GUITask, "GUITask", 2048, NULL, 1, NULL);
}












