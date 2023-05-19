
#include "LCD_I2C.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "cJSON.h"

extern LCDI2C lcdI2C;
extern QueueHandle_t qUartHandle,qSTM32Tx;
extern cJSON *cjsMain;
extern EventGroupHandle_t evg1,evgJson;
