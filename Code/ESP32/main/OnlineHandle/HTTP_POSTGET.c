#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "POSTGET.h"
#include "HTTP_POSTGET.h"
#include "esp_event.h"
#include "OnlineStatusEvent.h"

char data_send[MAX_HTTP_OUTPUT_BUFFER];

HTTP_CODE_e onl_HTTP_SendToServer(int a)
{
    if(!OnlEvt_CheckBit(ONL_EVT_WIFI_CONNECTED) || !OnlEvt_CheckBit(ONL_EVT_PING_SUCCESS)) return 0;
    HTTP_CODE_e http_code = HTTP_INVALID;
    ESP_LOGI("HTTP","ready");
    vTaskDelay(1000/portTICK_PERIOD_MS);
    return http_code;
}