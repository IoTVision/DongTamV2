#include <stdio.h>
#include "main.h"
#include "UART.h"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "WiFi_POSTGET/WiFi.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h" 


void Setup();

void app_main(void)
{
    Setup();
    while (1) {
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}




void Setup()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
}
