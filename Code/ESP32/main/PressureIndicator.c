#include "74HC595.h"
#include "esp_log.h"
#ifndef PRESSURE_INDICATOR
#define PRESSURE_INDICATOR
#define INDICATOR_MAX_BITMASK 0x03ff
#define INDICATOR_MIN_BITMASK 0x0200
#define INDICATOR_MAX_LEVEL 10


void PI_SetLevel(uint8_t level){
    if(level > 10) return;
    uint16_t indicator = INDICATOR_MAX_BITMASK;
    indicator <<= (INDICATOR_MAX_LEVEL-level);
    indicator &=INDICATOR_MAX_BITMASK;
    uint8_t data[2] = {0};
    data[0]=indicator;
    data[1]=indicator >> 8;
    HC595_Send_Data(data,2);
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
    data[1]=dataOriginal >> 8;
    data[0]=dataOriginal;
    HC595_Send_Data(data,2);
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
    data[1]=dataOriginal >> 8;
    data[0]=dataOriginal;
    HC595_Send_Data(data,2);
    DELAY_MS(50);
}

void PI_Test()
{
    PI_SetLevel(2);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(4);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(6);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(8);
    vTaskDelay(500/portTICK_PERIOD_MS);
    PI_SetLevel(10);
    vTaskDelay(500/portTICK_PERIOD_MS);
    
    for(uint8_t i=0;i<10;i++)PI_TestShowLevel_Increase();
    for(uint8_t i=0;i<10;i++)PI_TestShowLevel_Decrease();
    PI_SetLevel(0);
}
#endif
