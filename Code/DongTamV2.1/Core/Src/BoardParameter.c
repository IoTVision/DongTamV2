/*
 * BoardParameter.c
 *
 *  Created on: Jun 6, 2023
 *      Author: KHOA
 */


#include "BoardParameter.h"

BoardParameter brdParam;

uint16_t Brd_GetTotalVan(){return brdParam.totalVan;}
uint16_t Brd_GetVanOn(){return brdParam.currentVanOn;}
uint16_t Brd_GetIntervalTime(){return brdParam.intervalTime;}
uint16_t Brd_GetPulseTime(){return brdParam.pulseTime;}
RTC_t Brd_GetRTC()
{
	brdParam.RTCtime = PCF8563_ReadTimeRegisters();
	return brdParam.RTCtime;
}
uint16_t Brd_GetCycleTime(){return brdParam.cycleTime;}
float Brd_GetPressure()
{
	brdParam.pressure = AMS5915_CalPressure(Brd_GetAddress_AMS5915());
	return brdParam.pressure;
}

int8_t Brd_SetTotalVan(uint8_t val)
{
    if(val > 0 && val <= 16){
        brdParam.totalVan = val;
        brdParam.currentVanOn = 0;
        for(uint8_t i = 0; i < val; i++){
        	brdParam.currentVanOn |=  (1 << (val-1));
        }
    } else return -1;
	return 0;
}

int16_t Brd_SetVanOn(uint16_t val)
{
    if(val > 0 && val <= 16){
        brdParam.currentVanOn |=  (1 << (val-1));
    }
    else return -1;
    return 0;
}

int16_t Brd_SetVanOff(uint16_t val)
{
    if(val > 0 && val <= 16){
        return (int16_t)(brdParam.currentVanOn &=  ~(1 << (val-1)));
    }
    else return -1;
}

int16_t Brd_SetIntervalTime(uint16_t val)
{
    if(val > 0 && val <= 100){
        brdParam.intervalTime = val;
    }
    else return -1;
    return 0;
}

int16_t Brd_SetPulseTime(uint16_t val)
{
    if(val >= 30 && val <= 300) {
        brdParam.pulseTime = val;
        return (int16_t)brdParam.pulseTime;
    }
    return -1;
}

int8_t Brd_SetRTC(RTC_t t){
    uint8_t indexT = sizeof(t) / (sizeof(int8_t));
    int8_t *a = &t.year;
    for(uint8_t i = 0; i < indexT; i++){
        if(*(a+i) == -1) return -1;
    }
    brdParam.RTCtime = t;
    PCF8563_WriteTimeRegisters(brdParam.RTCtime);
    return 0;
}

int16_t Brd_SetCycleTime(uint16_t val)
{
    if(val > 0 && val <= 100){
        brdParam.cycleTime = val;
    }
    else return -1;
    return 0;
}

HC595* Brd_GetAddress_HC595(){return &brdParam.hc595;}
HC165* Brd_GetAddress_HC165(){return &brdParam.hc165;}
PCF8563_Handle* Brd_GetAddress_PCF8563(){return &brdParam.pcf;}
AMS5915* Brd_GetAddress_AMS5915(){return &brdParam.ams;}


//void ProcedureVan(){
//	static uint32_t delayAfterVanOn = 0;
//	uint16_t pulseTime = Brd_GetPulseTime();
//	// turn on valve
//	if(CHECKFLAG(f1, FLAG_TRIG_VAN_PROCEDURE)){
//		if(!CHECKFLAG(f1,FLAG_AFTER_TRIG_VAN_ON)){
////			brdParam.pressure = AMS5915_CalPressure(&brdParam.ams);
////			HC595_SetByteOutput(brdParam.hc595.data);
//			HC595_ShiftOut(NULL, 2, 1);
//			SETFLAG(f1,FLAG_AFTER_TRIG_VAN_ON);
//			delayAfterVanOn = HAL_GetTick();
//			// Delay 500ms, read van state then turn off
//		}
//		else if(CHECKFLAG(f1,FLAG_AFTER_TRIG_VAN_ON | FLAG_GET_PRESSURE_MAX_DONE) &&
//				(HAL_GetTick() - delayAfterVanOn >= pulseTime)){
////			brdParam.VanState = HC165_ReadState(2);
////			HC595_ClearByteOutput(brdParam.hc595.data);
//			HC595_ShiftOut(NULL, 2, 1);
//			CLEARFLAG(f1,FLAG_AFTER_TRIG_VAN_ON);
//			CLEARFLAG(f1, FLAG_TRIG_VAN_PROCEDURE);
//		}
//
//	}
//}
//
//
