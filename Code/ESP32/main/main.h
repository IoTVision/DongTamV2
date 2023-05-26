#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/queue.h"
// #include "RTC_Format.h"


#define CHECKFLAG(FlagGroup,FlagBit) ((((FlagGroup) & (FlagBit)) == (FlagBit)) ? 1 : 0) 


void SendStringToUART(QueueHandle_t q,char *s);
void UartHandleString(void *pvParameter);
#endif