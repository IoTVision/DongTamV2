#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"

typedef struct BoardParameter{
    char ID[12];
    float Pressure;
    char Time[20];
    uint32_t VanState;
    uint32_t VanData;
}BoardParameter;

#endif
