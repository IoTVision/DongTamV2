
#include "LCD_I2C.h"
#include "freertos/queue.h"
#include "cJSON.h"
extern LCDI2C lcdI2C;
extern QueueHandle_t qUartJson;
extern cJSON *cjsMain;

#define rxBUFFER rxData