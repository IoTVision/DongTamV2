#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "HTTP_POSTGET.h"
#include "WiFi.h"
#include "DNS_Ping.h"
#include "OnlineStatusEvent.h"
TimerHandle_t tOnl; 
TaskHandle_t taskHandleOnl;

#define TIMER_COUNT_EXPIRE_TRIGGER_RECONNECT 5
#define TIMER_COUNT_EXPIRE_TRIGGER_PING 10
TaskHandle_t *TaskOnl_GetHandle() {return &taskHandleOnl;}

void TimerOnline_Callback(TimerHandle_t xTimer)
{   
    uint32_t count = (uint32_t)pvTimerGetTimerID(xTimer);
    count ++; 
    vTimerSetTimerID(xTimer,(void*)count);
}

void Onl_HandleReconnectSequence(){
    static bool IsTimerStop = 1; // set to 1 because it is stop at the initialize
    uint32_t countTimer = 0;
    if(OnlEvt_CheckBit(ONL_EVT_PING_SUCCESS | ONL_EVT_WIFI_CONNECTED)) {
        if(!IsTimerStop){
            xTimerStop(tOnl,10/portTICK_PERIOD_MS);
            IsTimerStop = 1;
        }
        return;
    }
    if(IsTimerStop){
        xTimerStart(tOnl,10/portTICK_PERIOD_MS);
        IsTimerStop = 0;
    }
    if(OnlEvt_CheckBit(ONL_EVT_WIFI_FAIL)){
        countTimer = (uint32_t)pvTimerGetTimerID(tOnl);
        ESP_LOGI("Reconnect","timer count:%lu",countTimer);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        if(countTimer >= TIMER_COUNT_EXPIRE_TRIGGER_RECONNECT){
            vTimerSetTimerID(tOnl,0); // reset counter
            esp_wifi_connect();
        }
    }
    else if(OnlEvt_CheckBit(ONL_EVT_PING_TIMEOUT) || OnlEvt_CheckBit(ONL_EVT_WIFI_CONNECTED)){
        if(!DNS_GetCurrentHost()) {
            if(DNS_PingToHost(DNS_GOOGLE) != ESP_OK) {
                vTaskDelay(1000/portTICK_PERIOD_MS);
                ESP_LOGE("Network","No internet");
                return;
            }
        }
        countTimer = (uint32_t)pvTimerGetTimerID(tOnl);
        ESP_LOGI("Ping","timer count:%lu",countTimer);
        vTaskDelay(6000/portTICK_PERIOD_MS);
        if(countTimer >= TIMER_COUNT_EXPIRE_TRIGGER_PING){
            vTimerSetTimerID(tOnl,0);
            if(!DNS_IsPinging()) DNS_StartToPing();
        }
    }
}


void TaskOnlManage(void *pvParameter)
{
    ESP_LOGI("TaskOnl","Succesfully created");
    tOnl = xTimerCreate("TimerOnlineManage",pdMS_TO_TICKS(1000),pdTRUE,0,TimerOnline_Callback);
    xTimerStop(tOnl,portMAX_DELAY);
    wifi_init_sta();
    while (1){
        Onl_HandleReconnectSequence();
        Onl_HTTP_SendToServer(1);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}