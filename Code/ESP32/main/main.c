<<<<<<< HEAD

#include "./main.h"

#include <stdio.h>

#include "./ShareVar.h"
#include "./UART.h"
=======
#include <stdio.h>
#include "main.h"
>>>>>>> ESP32_JSON_POSTGET
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
#include "MessageHandle/MessageHandle.h"
#include "GUI/GUI.h"
QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
uart_port_t uartTarget;
cJSON *cjsMain;
EventGroupHandle_t evg1,evgJson;
TaskHandle_t taskCommon,taskUartHandleString;


void Setup();
void SendStringToUART(QueueHandle_t q,char *s);
void InitProcess();
esp_err_t TestFlashNVS();
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

/**
 * @brief Vì dùng ESP_LOG tốn tài nguyên CPU và bộ nhớ, ảnh hưởng đến tốc độ chạy của task nên gửi sang task IDLE để 
 * giảm thiểu xử lý, sử dụng hàm uart_write_bytes để gửi,
 * Item trong Queue qLogTx là một con trỏ char* trỏ tới chuỗi cần gửi lên máy tính, 
 * sau đó free bộ nhớ mà con trỏ char* đó trỏ tới 
 * 
 */
void app_main(void)
{
    Setup();
    while (1) {
        // PostDataFromBoardToServer(1);     
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
    }
}

/**
 * @brief Task xử lý chuỗi từ bên file UART.c gửi sang thông qua qUARTHandle,
 * các thao tác xử lý chuỗi sẽ thực hiện tại đây 
 * 
 * @param pvParameter Không dùng
 */
void UartHandleString(void *pvParameter)
{
    char *s=NULL;
    char sOutput[50] = {0};
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS))
        {
            if(uartTarget == UART_NUM_0){
                ESP_LOGI("PC","%s",s);
            }
            else if (uartTarget == UART_NUM_2){
                ESP_LOGI("STM32","%s",s);
            }
            if(MessageRxHandle(s,sOutput) == ESP_OK){
                if(uartTarget == UART_NUM_0){
                    SendStringToUART(qSTM32Tx,sOutput);
                    memset(sOutput,0,strlen(sOutput));
                } else if(uartTarget == UART_NUM_2) {
                    SendStringToUART(qLogTx,s);
                    memset(sOutput,0,strlen(sOutput));
                }
            } 
            free(s);
        }
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


<<<<<<< HEAD
/**
 * @brief Khởi tạo bộ nhớ flash, khởi tạo vùng nhớ Queue, 
 * khởi tạo các ngoại vi liên quan tới GUI như LCD I2C, 
 * cấu hình chân HC595 cho LED bar hiển thị áp suất, 
 * cấu hình chân giao tiếp UART
 * Load các thông số board mặc định vào struct BoardParamter
 */
void InitProcess()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
=======

void Setup()
{
    jsHandle_Init(NULL);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
>>>>>>> ESP32_JSON_POSTGET
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

<<<<<<< HEAD

    cjsMain = cJSON_CreateObject();
    qLogTx = xQueueCreate(3,sizeof(char *));
    qUartHandle = xQueueCreate(6,sizeof(char *));
    qSTM32Tx = xQueueCreate(4,sizeof(char *));
    evg1 = xEventGroupCreate();
    Brd_LoadDefaultValue();
    Brd_PrintAllParameter();
    UARTConfig();
    GuiInit();
}


/**
 * @brief Cấp phát vùng nhớ chứa chuỗi s cần gửi qua UART
 * 
 * @param q Hàng đợi tương ứng với bộ UART cần gửi, ví dụ qLogTx gửi lên UART để log data, 
 * qSTM32Tx để gửi xuống STM32
 * @param s Con trỏ trỏ tới chuỗi ký tự cần gửi đi
 */
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
    TaskHandle_t *taskGUIHandle = GUI_GetTaskHandle();
    InitProcess();
    ESP_LOGI("Notify","pass InitProcess");
    xTaskCreate(TaskUart, "TaskUart", 2048, NULL, 3, NULL);
    xTaskCreate(UartHandleString,"UartHandleString",4096,NULL,2,NULL);
    xTaskCreate(GUITask, "GUITask", 2048, NULL, 2, taskGUIHandle);
    xTaskCreate(TaskScanButton, "TaskScanButton", 2048, NULL, 1, NULL);


}














=======
    ESP_ERROR_CHECK(ret);
    TaskHandle_t *taskOnlManage = TaskOnl_GetHandle();
    xTaskCreatePinnedToCore(TaskOnlManage,"TaskOnlManage",4096,NULL,3,taskOnlManage,1);
}
>>>>>>> ESP32_JSON_POSTGET
