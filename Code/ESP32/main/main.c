
#include <stdio.h>
#include "main.h"
#include "74HC595.h"
#include "PressureIndicator.c"
#include "UART.c"
#include "driver/dac.h"
#include "esp_err.h"
#include "GUI.c"
#include "LCD_I2C.h"
// static const char *TAG= "main";

#define HC595_OE_MASK (1ULL<<GPIO_NUM_4)
#define HC595_LATCH_MASK (1ULL<<GPIO_NUM_5)
#define HC595_CLK_MASK (1ULL<<GPIO_NUM_18)
#define HC595_DS_MASK (1ULL<<GPIO_NUM_23)

HC595 hc595_pi;//74HC595 for Pressure Indicator

void HC595_ConfigPin();



void app_main(void)
{
    HC595_ConfigPin();
    PressureIndicator_Test();
    UARTConfig();
    xTaskCreate(UARTToSTM32_event_task, "UARTToSTM32_event_task", 2048, NULL, 4, NULL);
    xTaskCreate(log_uart_event_task, "log_uart_event_task", 2048, NULL, 4, NULL);
    while (1) {
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}


void HC595_ConfigPin()
{
    gpio_config_t cfg = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = HC595_DS_MASK |
                        HC595_OE_MASK |
                        HC595_CLK_MASK|
                        HC595_LATCH_MASK,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&cfg);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_5,HC595_LATCH);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_4,HC595_OE);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_18,HC595_CLK);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_23,HC595_DS);
    HC595_SetTarget(&hc595_pi);
    HC595_Enable();
}


















