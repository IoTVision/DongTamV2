
#include "./main.h"

#include <stdio.h>

#include "./ShareVar.h"
#include "./UART.h"
#include "driver/dac.h"
#include "cJSON.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
#include "MessageHandle/MessageHandle.h"
#include "GUI/GUI.h"
QueueHandle_t qLogTx,qSTM32Tx,qUartHandle;
cJSON *cjsMain;
EventGroupHandle_t evg1,evgJson;
TaskHandle_t taskCommon,taskUartHandleString;


void Setup();
void SendStringToUART(QueueHandle_t q,char *s);
void InitProcess();
esp_err_t TestFlashNVS();

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
    char *s = NULL;
    while (1) {
        if(xQueueReceive(qLogTx,&s,10/portTICK_PERIOD_MS)){
            uart_write_bytes(UART_NUM_0,s,strlen(s));
            free(s);
        }
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
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS))
        {
            ESP_LOGI("UartHandleString","len of s:%d",strlen(s));
            MessageRxHandle(s,NULL);
            // SendStringToUART(qLogTx,s);
            free(s);
        }
    }
}


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
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );


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














