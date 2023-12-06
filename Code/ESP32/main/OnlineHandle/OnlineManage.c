#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "HTTP_POSTGET.h"
#include "WiFi.h"
#include "DNS_Ping.h"
#include "OnlineStatusEvent.h"
#include "RTC_Format.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "BoardParameter.h"
TimerHandle_t tOnl,tRTC; 
TaskHandle_t taskHandleOnl;

RTC_t rtc;


#define TIMER_COUNT_EXPIRE_TRIGGER_RECONNECT 5
#define TIMER_COUNT_EXPIRE_TRIGGER_PING 10
TaskHandle_t *TaskOnl_GetHandle() {return &taskHandleOnl;}

void TimerOnline_Callback(TimerHandle_t xTimer)
{   
    uint32_t count = (uint32_t)pvTimerGetTimerID(xTimer);
    count ++; 
    vTimerSetTimerID(xTimer,(void*)count);
}

void onl_Handle_Reconnect_Sequence(){
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
        vTaskDelay(3000/portTICK_PERIOD_MS);
        if(countTimer >= TIMER_COUNT_EXPIRE_TRIGGER_RECONNECT){
            vTimerSetTimerID(tOnl,0); // reset counter
            esp_wifi_connect();
        }
    }
    else if(OnlEvt_CheckBit(ONL_EVT_PING_TIMEOUT) || OnlEvt_CheckBit(ONL_EVT_WIFI_CONNECTED)){
        if(!DNS_GetCurrentHost()) {
            if(DNS_PingToHost(DNS_GOOGLE) != ESP_OK) {
                vTaskDelay(3000/portTICK_PERIOD_MS);
                ESP_LOGE("Network","No internet");
                return;
            }
        }
        countTimer = (uint32_t)pvTimerGetTimerID(tOnl);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        if(countTimer >= TIMER_COUNT_EXPIRE_TRIGGER_PING){
            ESP_LOGI("CheckDNS","Pinging");
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
    uint8_t i = 0;
    while(!OnlEvt_CheckBit(ONL_EVT_WIFI_CONNECTED)){
        if(i >= wifi_GetSizeOfArrayWiFiID()) i = 0;
        else {
            WiFi_ID wid = wifi_GetID(i);
            wifi_SwitchToWiFiID(wid);
            i++; 
        }
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    while (1){
        onl_Handle_Reconnect_Sequence();
        onl_HTTP_SendToServer(Brd_GetResetTime());
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}