#ifndef SHARE_VAR_H
#define SHARE_VAR_H

#include "LCD_I2C.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "cJSON.h"
#include "RTC_Format.h"
#include "GUI/GUI.h"
#include "nvs_flash.h"
#include "nvs.h"

#define USE_SCROLL_SCREEN 

#define EVT_GET_PRESSURE (1<<GET_PRESSURE)
#define EVT_GET_TIME (1<<GET_TIME)
#define EVT_GET_VAN_VALUE (1<<GET_VAN_VALUE)
#define EVT_GET_FULL_PARAM (1<<GET_FULL_PARAM)

#define EVT_SET_TIME (1<<SET_TIME)
#define EVT_SET_VAN (1<<SET_VAN)
#define EVT_CLEAR_VAN (1<<CLEAR_VAN)
#define EVT_TRIG_VAN (1<<TRIG_VAN)

#define EVT_NVS_SAVE_VAN (1<<NVS_SAVE_VAN)
#define EVT_NVS_GET_VAN (1<<NVS_GET_VAN)

#define EVT_UART_DATA_SEND_QUEUE (1<<UART_READY_TO_SEND_QUEUE)

#define EVT_UART_TASK_BIG_SIZE (1<<0)
#define EVT_UART_DELETE_TASK_BIG_SIZE (1<<1)
#define EVT_UART_OVERSIZE_HW_FIFO (1<<2)

typedef enum EventCommand{
    GET_PRESSURE,
    GET_TIME,
    GET_VAN_VALUE,
    GET_FULL_PARAM,
    SET_TIME,
    SET_VAN,
    CLEAR_VAN,
    TRIG_VAN,
    NVS_SAVE_VAN,
    NVS_GET_VAN,
    UART_READY_TO_SEND_QUEUE,
}EventCommand;

typedef struct BoardParameter{
    char ID[12];
    float Pressure;
    RTC_t Time;
    char VanState[16];
    uint32_t VanData;
    uint32_t SetVan;
    uint32_t ClearVan;
}BoardParameter;




extern LCDI2C lcdI2C;
extern QueueHandle_t qUartHandle,qSTM32Tx,qLogTx;
extern cJSON *cjsMain;
extern EventGroupHandle_t evg1,evgJson,evgGUI;
extern BoardParameter brdParam;
extern TaskHandle_t taskGUIHandle;
#endif