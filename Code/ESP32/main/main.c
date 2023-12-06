
#include "./main.h"

#include <stdio.h>

#include "./ShareVar.h"
#include "./UART.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "RTC_Format.h"
#include "MessageHandle/MessageHandle.h"
#include "GUI/GUI.h"
#include "freertos/FreeRTOS.h" 
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_mac.h"
#include "OnlineHandle/OnlineManage.h"
#include "freertos/timers.h"
#include "esp_timer.h"

QueueHandle_t qLogTx,qSTM32Tx,qUartHandle,qSTM32Ready;
uart_port_t uartTarget;
TaskHandle_t taskCommon,taskUartHandleString;
TimerHandle_t tPingHello;



void SendStringToUART(QueueHandle_t q,char *s);
void InitProcess();
void Setup();
void STM32_Set_Default_Parameter(char *sOutput);
void STM32_Ready_GUI(char *s);
void CheckVanProcedureIsProcessing(char* sOutput);


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
    char sOutput[50] = {0};
    while (1) {
        if(xQueueReceive(qLogTx,&s,10/portTICK_PERIOD_MS)){
            uart_write_bytes(UART_NUM_0,s,strlen(s));
            free(s);
        }
        if(xQueueReceive(qSTM32Ready,&s,100/portTICK_PERIOD_MS) 
        && !CHECKFLAG(xEventGroupGetBits(evgUART),EVT_UART_STM32_READY)){
            if(!strcmp(s, MESG_READY_STM32)){
                xTimerStop(tPingHello,10/portTICK_PERIOD_MS);
                xTimerDelete(tPingHello,10/portTICK_PERIOD_MS);
                xQueueReset(qSTM32Ready);
                STM32_Ready_GUI("Ready");// show this notification to LCD 
                xEventGroupSetBits(evgUART,EVT_UART_STM32_READY);
                CheckVanProcedureIsProcessing(sOutput);
                vTaskDelay(500/portTICK_PERIOD_MS);
                GUI_ShowPointer();
                GUI_LoadPageAtInit();
            } else {
                STM32_Ready_GUI("Not ready");
            } 
        }
        Brd_ESP_CheckResetInNewDay();
    }
}




/**
 * @brief Dùng để thông báo lên màn hình LCD STM32 có thể giao tiếp bình thường 
 * Chuyển giao diện thông báo từ "Wait for STM32 ready" sang giao diện cài đặt thông số
 * 
*/
void STM32_Ready_GUI(char *s){
    LCDI2C_Clear();
    vTaskDelay(20/portTICK_PERIOD_MS);
    LCDI2C_Print(s,0,0);
    LedErrorWrite(0);
    HC595_ShiftOut(NULL,2,1);
}

void TimerHello_Callback(TimerHandle_t xTimer)
{   
    char sOutput[50] = {0};
    MessageTxHandle(TX_HELLO_STM32,sOutput);
    ESP_LOGI("to STM32","Sending: %s",sOutput);
    SendStringToUART(qSTM32Tx,sOutput);
}




/**
 * @brief Dùng để kiểm tra STM32 có đang trong chu trình kích van hay không
 * Nếu STM32 đang trong chu trình kích van thì không gửi thông số board từ ESP32 qua
 * Tính năng này dùng để tránh trường hợp ESP32 reset sau khi hoạt động trong một khoảng thời gian
 * 
 */
void CheckVanProcedureIsProcessing(char *sOutput)
{
    MessageTxHandle(TX_IS_ON_PROCEDURE,sOutput);
    SendStringToUART(qSTM32Tx,sOutput);
    if((xEventGroupWaitBits(evgUART,EVT_UART_IS_ON_PROCEDURE,pdFALSE,pdFALSE,5000/portTICK_PERIOD_MS) & EVT_UART_IS_ON_PROCEDURE) != EVT_UART_IS_ON_PROCEDURE){
        /*
        Should be wait long enough (in this case 5s) after sending message "IsOnProcedure" to STM32 
        to check whether Van Procedure is processing or not.
        If it's processing, it means only ESP32 is reset by something and STM32 is not reset 
        Because Van Procedure is still processing, no need to send BoardParameter to STM32, only send if both of them are reset 
        */
        STM32_Set_Default_Parameter(sOutput);
        ESP_LOGW("CheckProc","No procedure, load param to STM32");
    } else ESP_LOGW("CheckProc","Procedure is processing");
}

/**
 * @brief Task xử lý chuỗi nhận được từ bên file UART.c gửi sang thông qua queue qUARTHandle,
 * các thao tác xử lý chuỗi sẽ thực hiện tại đây 
 * 
 * @param pvParameter Không dùng
 */
void UartHandleString(void *pvParameter)
{
    char *s=NULL;
    char sOutput[50] = {0};
    EventBits_t e;
    e = xEventGroupGetBits(evgUART);
    while(1){
        if(xQueueReceive(qUartHandle,&s,10/portTICK_PERIOD_MS))
        {
            // for debug UART message
            if(uartTarget == UART_NUM_0) ESP_LOGI("PC","%s",s);
            else if (uartTarget == UART_NUM_2) ESP_LOGI("STM32","%s",s);

            if (!CHECKFLAG(e, EVT_UART_STM32_READY)){
                //Send string that received from STM32 (or PC) to queue, this queue will be handle at IDLE task main
                if(qSTM32Ready) xQueueSend(qSTM32Ready,&s,portMAX_DELAY);
                // if flag EVT_UART_STM32_READY is set, stop CHECK this flag and not clear it 
                e = xEventGroupWaitBits(evgUART,EVT_UART_STM32_READY,pdFALSE,pdFALSE,10/portTICK_PERIOD_MS);              
            } 
            if(CHECKFLAG(e, EVT_UART_STM32_READY)){
                if(MessageRxHandle(s,sOutput) == ESP_OK){
                    if(uartTarget == UART_NUM_0) SendStringToUART(qSTM32Tx,sOutput);
                    else if(uartTarget == UART_NUM_2) SendStringToUART(qLogTx,s);
                    memset(sOutput,0,strlen(sOutput));
                } 
            }             
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

    if(readResetTimeFromFlash() != ESP_OK){
        if(writeResetTimeToFlash() == ESP_OK)
        ESP_LOGW("ResetTime","Not found reset time in flash, write it in flash for the first time");
    } else {
        uint16_t resetTime = Brd_GetResetTime();
        resetTime++;
        Brd_SetResetTime(resetTime);
        if(writeResetTimeToFlash() == ESP_OK) 
        ESP_LOGW("ResetTime","resetTime:%u",Brd_GetResetTime());
    }

    Brd_GetStartupTime();
    qLogTx = xQueueCreate(6,sizeof(char *));
    qUartHandle = xQueueCreate(6,sizeof(char *));
    qSTM32Tx = xQueueCreate(4,sizeof(char *));
    qSTM32Ready = xQueueCreate(5, strlen(MESG_READY_STM32));
    tPingHello = xTimerCreate("Timer Hello",pdMS_TO_TICKS(3000),pdTRUE,0,TimerHello_Callback);
    xTimerStart(tPingHello,10/portTICK_PERIOD_MS);
    Brd_LoadDefaultValue();
    Brd_PrintAllParameter();
    UARTConfig();
    GuiInit();
    
}

void STM32_Set_Default_Parameter(char *sOutput){
    MesgValTX paramToSendSTM32[] = {
        TX_PULSE_TIME,
        TX_TOTAL_VAN,
        TX_CYC_INTV_TIME,
        TX_INTERVAL_TIME,
        TX_TRIG_VAN,
    };

    for(uint8_t i = 0; i < sizeof(paramToSendSTM32)/sizeof(MesgValTX); i++){
        MessageTxHandle(paramToSendSTM32[i], sOutput);
        SendStringToUART(qSTM32Tx,sOutput);
        ESP_LOGI("STM32v","%s",sOutput);
    }
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
    i2cdev_init();
    TaskHandle_t *taskGUIHandle = GUI_GetTaskHandle();
    TaskHandle_t *taskOnlManage = TaskOnl_GetHandle();
    InitProcess();
    ESP_LOGI("Notify","pass InitProcess");
    xTaskCreate(TaskUart, "TaskUart", 2048, NULL, 3, NULL);
    xTaskCreate(UartHandleString,"UartHandleString",4096,NULL,2,NULL);
    xTaskCreate(GUITask, "GUITask", 4096, NULL, 2, taskGUIHandle);
    xTaskCreate(TaskScanButton, "TaskScanButton", 2048, NULL, 1, NULL);
    xTaskCreatePinnedToCore(TaskOnlManage,"TaskOnlManage",4096,NULL,3,taskOnlManage,1);
    STM32_Ready_GUI("Not ready");
    
}
