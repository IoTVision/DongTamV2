#include "HTTP_POSTGET.h"
#include "POSTGET.h"
#include "esp_event.h"
#include "wifi_manager.h"
#include "esp_log.h"
#include "OnlineStatusEvent.h"

extern EventGroupHandle_t evgOSE;
char data_send[MAX_HTTP_OUTPUT_BUFFER];
HTTP_CODE_e PostDataFromBoardToServer(int a)
{
    HTTP_CODE_e http_code = HTTP_INVALID;
    ESP_LOGI("HTTP","evt:%lu",xEventGroupGetBits(evgOSE));
    if(!OnlEvt_WaitBit(ONL_EVT_PING_SUCCESS | ONL_EVT_WIFI_CONNECTED, pdFALSE, pdTRUE, 1000/portTICK_PERIOD_MS))
    {
        ESP_LOGI("HTTP","Ready");
        // snprintf(data_send, MAX_HTTP_OUTPUT_BUFFER,"{'IMEI':'AC67B2F6E568', 'Power':1, 'FAN':1, 'ODCMode':5, 'ValveError':5, 'VanKich':5, 'DeltaPH':500, 'DeltaP':200,'DeltaPL':5,'LED10Bar':7}",a);
        // http_code = http_post(URL_POST_DataFromBoardToServer, data_send);
        // esp_log_write(ESP_LOG_DEBUG, "TAG","Status Code: %d",http_code);
    }
    return http_code;
}



