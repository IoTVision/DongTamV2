#include "74HC595.h"
#define INDICATOR_MAX_BITMASK 0x03ff
#define INDICATOR_MIN_BITMASK 0x0200
#define INDICATOR_MAX_LEVEL 10

#define HC595_OE_MASK (1ULL<<GPIO_NUM_4)
#define HC595_LATCH_MASK (1ULL<<GPIO_NUM_5)
#define HC595_CLK_MASK (1ULL<<GPIO_NUM_18)
#define HC595_DS_MASK (1ULL<<GPIO_NUM_23)

HC595 hc595_pi;

void PI_SetLevel(uint8_t level){
    if(level > 10) return;
    uint16_t indicator = INDICATOR_MAX_BITMASK;
    indicator <<= (INDICATOR_MAX_LEVEL-level);
    indicator &=INDICATOR_MAX_BITMASK;
    uint8_t data[2] = {data[0]=indicator >> 8,data[1]=indicator,};
    // HC595_Send_Data(data,2);
}

void PI_ConfigPin()
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
    
    PI_SetLevel(1);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(3);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(5);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(7);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(9);
    vTaskDelay(500/portTICK_PERIOD_MS);
}
void PI_TestShowLevel_Increase(){
    static uint16_t dataOriginal= INDICATOR_MIN_BITMASK;
    static uint8_t shft=0;
    uint8_t data[2];
    shft++;
    dataOriginal>>=1;
    dataOriginal|=INDICATOR_MIN_BITMASK;
    if(shft==INDICATOR_MAX_LEVEL) {
        shft = 0;
        dataOriginal = INDICATOR_MIN_BITMASK;
    }
    data[0]=dataOriginal >> 8;
    data[1]=dataOriginal;
    // HC595_Send_Data(data,2);
    DELAY_MS(50);
}
void PI_TestShowLevel_Decrease(){
    static uint16_t dataOriginal= INDICATOR_MAX_BITMASK;
    static uint8_t shft=0;
    uint8_t data[2];
    shft++;
    dataOriginal<<=1;
    dataOriginal&=INDICATOR_MAX_BITMASK;
    if(shft==INDICATOR_MAX_LEVEL) {
        shft = 0;
        dataOriginal = INDICATOR_MAX_BITMASK;
    }
    data[0]=dataOriginal >> 8;
    data[1]=dataOriginal;
    // HC595_Send_Data(data,2);
    DELAY_MS(50);
}

