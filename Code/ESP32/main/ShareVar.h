#ifndef SHARE_VAR_H
#define SHARE_VAR_H

#include "LCD_I2C.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "cJSON.h"
#include "RTC_Format.h"

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
extern EventGroupHandle_t evg1,evgJson;
extern BoardParameter brdParam;
#endif