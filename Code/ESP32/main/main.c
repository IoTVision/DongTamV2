#include <stdio.h>
#include "main.h"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h" 
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_mac.h"
#include "OnlineHandle/OnlineManage.h"
#include "JsonHandle/JsonHandle.h"
#include "UART.h"


#define MAC_ADDR_SIZE 6

uint8_t mac_address[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
uint8_t MAC_WIFI[6];




void Setup();

void app_main(void)
{
    Setup();
    while (1) {
        // PostDataFromBoardToServer(1);     
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
    }
}

void get_mac_wifi_address(){
    uint8_t mac[MAC_ADDR_SIZE];
    esp_read_mac(mac,ESP_MAC_WIFI_STA);
    ESP_LOGI("MAC address", "MAC address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void set_mac_address(uint8_t *mac){
    esp_err_t err = ESP_OK;
    if (err == ESP_OK) {
        ESP_LOGI("MAC address", "MAC address successfully set to %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        ESP_LOGE("MAC address", "Failed to set MAC address");
    }
}



void Setup()
{
    jsHandle_Init(NULL);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    TaskHandle_t *taskOnlManage = TaskOnl_GetHandle();
    xTaskCreatePinnedToCore(TaskOnlManage,"TaskOnlManage",4096,NULL,3,taskOnlManage,1);
}
