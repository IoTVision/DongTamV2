
#include <stdio.h>
#include "main.h"
#include "74HC595.h"
#include "PressureIndicator.c"
static const char *TAG = "example";

#define HC595_OE_MASK (1ULL<<GPIO_NUM_4)
#define HC595_LATCH_MASK (1ULL<<GPIO_NUM_5)
#define HC595_CLK_MASK (1ULL<<GPIO_NUM_18)
#define HC595_DS_MASK (1ULL<<GPIO_NUM_23)

HC595 hc595;
void app_main(void)
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
    HC595_AssignPin(&hc595,GPIO_NUM_5,HC595_LATCH);
    HC595_AssignPin(&hc595,GPIO_NUM_4,HC595_OE);
    HC595_AssignPin(&hc595,GPIO_NUM_18,HC595_CLK);
    HC595_AssignPin(&hc595,GPIO_NUM_23,HC595_DS);
    HC595_SetTarget(&hc595);
    HC595_Enable();
    PI_SetLevel(7);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    PI_SetLevel(3);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    PI_SetLevel(5);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    HC595_Disable();
    while (1) {
    PI_TestShowLevel_Increase();
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
