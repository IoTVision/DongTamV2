
#include "74HC595.h"
#define INDICATOR_MAX_BITMASK 0x03ff
#define INDICATOR_MIN_BITMASK 0x0200
#define INDICATOR_MAX_LEVEL 10
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
    HC595_Send_Data(data,2);
    DELAY_MS(20);
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
    HC595_Send_Data(data,2);
    DELAY_MS(30);
}

void PI_SetLevel(uint8_t level){
    // if(level<0 && level > 10) return;
    uint16_t indicator = INDICATOR_MAX_BITMASK;
    indicator <<= (INDICATOR_MAX_LEVEL-level);
    indicator &=INDICATOR_MAX_BITMASK;
    uint8_t data[2];
    data[0]=indicator >> 8;
    data[1]=indicator;
    HC595_Send_Data(data,2);
}