#include "BoardParameter.h"

uint16_t paramMaxLimit[] = {
        4000,
        4000,
        5000,
        500,
        100,
        300,
        4000,
        4000,
        25000,
        25000,
};

uint16_t paramMinLimit[] ={
        4,
        2,
        250,
        250,
        300,
        30,
        250,
        250,
        0,
        1,
        0,
};
uint16_t paramDefaultVal []= {
        700,
        1300,
        2300,
        60,
        10,
        6,
        1000,
        250,
        0,
        3,
        3000,
        0,
        6,
};
BoardParameter brdParam;

uint16_t Brd_GetVanOn(){return brdParam.currentVanOn;}
uint16_t Brd_GetTotalVan(){return brdParam.totalVan;}
uint16_t Brd_GetIntervalTime(){return brdParam.intervalTime;}
uint16_t Brd_GetPulseTime(){return brdParam.pulseTime;}
uint16_t Brd_GetCycleIntervalTime(){return brdParam.cycIntvTime;}
uint16_t Brd_GetCycleTime(){return brdParam.cycleTime;}
uint16_t Brd_GetDPHigh(){return brdParam.dpHigh;}
uint16_t Brd_GetDPLow(){return brdParam.dpLow;}
uint16_t Brd_GetDPWarn(){return brdParam.dpWarn;}
RTC_t Brd_GetRTC(){return brdParam.RTCtime;}

int8_t Brd_SetTotalVan(uint8_t val)
{
    
}

int16_t Brd_SetVanOn(uint16_t val)
{

}

int16_t Brd_SetVanOff(uint16_t val)
{

}

int16_t Brd_SetIntervalTime(uint16_t val)
{

}

int16_t Brd_SetPulseTime(uint16_t val)
{

}

int16_t Brd_SetPulseTime(uint16_t val)
{

}