#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "POSTGET.h"
#include "HTTP_POSTGET.h"
#include "esp_event.h"
#include "OnlineStatusEvent.h"
#include "JsonHandle/JsonHandle.h"
char data_send[MAX_HTTP_OUTPUT_BUFFER];


uint8_t dpIndicatorBar_CalPressureLevel(int val)
{
    uint32_t dpHigh = jsHandle_Get_dpHigh();  
	uint32_t dpLow = jsHandle_Get_dpLow();
	uint32_t dpStep = (uint32_t)((dpHigh - dpLow)/10); 
	uint32_t a = dpLow + dpStep;
	for(uint8_t i=1; i <= 10;i++){
		if(a > val){
			return i; // return level of pressure indicator
		} else {
			a += dpStep;
		}
		if(a > dpHigh) return i;
		else if(a < dpLow) return 1;
	} 
	return 1;
}

HTTP_CODE_e onl_HTTP_SendToServer(int a)
{
    if(!OnlEvt_CheckBit(ONL_EVT_WIFI_CONNECTED) || !OnlEvt_CheckBit(ONL_EVT_PING_SUCCESS)) return 0;
    char s[300] = {0};
    HTTP_CODE_e http_code = HTTP_INVALID;
    int dpPressure = jsHandle_GetDP();
    uint8_t dpIndicatorLevel = 0;
    dpPressure +=100;
    if(dpPressure >= 4000){
        dpPressure = 250;
    }
    jsHandle_SetDP(dpPressure);
    dpIndicatorLevel = dpIndicatorBar_CalPressureLevel(dpPressure);
    jsHandle_Set_dpIndicatorBar(dpIndicatorLevel);
    strcpy(s,jsHandle_PrintObject());
    ESP_LOGI("HTTP_POST","dpPressure: %d,dpIndicatorBar:%d",dpPressure,dpIndicatorLevel);   
    // ESP_LOGI("HTTP_POST","%s",s);
    http_code = http_post(URL_POST_IOTVISION_DONGTAM,s);
    vTaskDelay(100/portTICK_PERIOD_MS);
    return http_code;
}