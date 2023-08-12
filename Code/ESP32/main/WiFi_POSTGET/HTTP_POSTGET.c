#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "POSTGET.h"
#include "HTTP_POSTGET.h"
#include "esp_event.h"
#include "OnlineStatusEvent.h"

extern EventGroupHandle_t evgOSE;
char data_send[MAX_HTTP_OUTPUT_BUFFER];

HTTP_CODE_e PostDataFromBoardToServer(int a)
{
    HTTP_CODE_e http_code = HTTP_INVALID;
    OnlEvt_WaitBit(ONL_EVT_PING_SUCCESS | ONL_EVT_WIFI_CONNECTED,pdFALSE,pdTRUE,portMAX_DELAY);
    ESP_LOGI("HTTP","ready");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    return http_code;
}