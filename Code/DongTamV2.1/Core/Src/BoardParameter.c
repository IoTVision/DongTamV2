/*
 * BoardParameter.c
 *
 *  Created on: Jun 6, 2023
 *      Author: KHOA
 */


#include "BoardParameter.h"
#include "MessageHandle.h"

BoardParameter brdParam;


VanProcedure vanProcState;


void VanOn(char *outputStr,uint8_t VanTrigger)
{
	HC595_SetBitOutput(VanTrigger);
	HC595_ShiftOut(NULL, 2, 1);
	MessageTxHandle(TX_PRESSURE, outputStr);
	vanProcState = BRD_PULSE_TIME;
}

void VanOff(char *outputStr,uint8_t VanTrigger)
{
	HC595_ClearBitOutput(VanTrigger);
	HC595_ShiftOut(NULL, 2, 1);
	MessageTxHandle(TX_VANSTATE, outputStr);
	vanProcState = BRD_INTERVAL_TIME;
}

void CheckCycleIntervalTime(uint16_t *cycleTime)
{
	if(*cycleTime > 0) {
		(*cycleTime) -= 1;
		vanProcState = BRD_CYCLE_INTERVAL_TIME;
	}
	else if(!cycleTime){
		vanProcState = PROC_END;
	}
}

void PulseTimeHandle()
{
	if(Brd_GetTimerArray(1) * TIMER_PERIOD_MS >= Brd_GetPulseTime()){
		Brd_SetTimerArray(1,0);
		Brd_SetVanProcState(BRD_VAN_OFF);
	}
}

uint16_t CheckVanInUsed(uint16_t *currentVanOn)
{
	if(*currentVanOn > 0){
		for(uint8_t i=0;i<MAX_NUM_VAN;i++){
			if((*currentVanOn & 0x01) > 0) {
				return i;
			}
			else *currentVanOn >>=1;
		}
	}
	else if (!*(currentVanOn)) return 0;
	return -1;
}

void IntervalTimeHandle(uint16_t currentVanOn, uint8_t VanToTrigger)
{
	VanToTrigger = CheckVanInUsed(&currentVanOn);
	//if interval time is passed and no van to trigger
	if(Brd_GetTimerArray(1)*TIMER_PERIOD_MS >= Brd_GetIntervalTime() && !VanToTrigger){
		// reset interval time
		Brd_SetTimerArray(1, 0);
		// set it to check cycle interval time
		Brd_SetVanProcState(BRD_CYCLE_INTERVAL_TIME);
	} else if(Brd_GetTimerArray(1)*TIMER_PERIOD_MS >= Brd_GetIntervalTime()){
		// reset interval time
		Brd_SetTimerArray(1, 0);
		Brd_SetVanProcState(BRD_VAN_ON);
	}
}

void ProcedureTriggerVan(char *outputStr)
{
	static uint16_t cycleTime;
	static uint16_t VanToTrigger;
	static uint16_t currentVanOn;
	switch(vanProcState){
	case PROC_START:
		cycleTime = Brd_GetCycleIntervalTime();
		VanToTrigger = 0;
		currentVanOn = Brd_GetVanOn();
		vanProcState = BRD_VAN_ON;
		break;
	case BRD_VAN_ON:
		VanOn(outputStr,VanToTrigger);
		break;
	case BRD_VAN_OFF:
		VanOff(outputStr,VanToTrigger);
		break;
	case BRD_PULSE_TIME:
		PulseTimeHandle();
		break;
	case BRD_INTERVAL_TIME:
		IntervalTimeHandle(currentVanOn,VanToTrigger);
		break;
	case BRD_CYCLE_INTERVAL_TIME:
		CheckCycleIntervalTime(&cycleTime);
		break;
	case PROC_END:
//		CheckCycleIntervalTime(&cycleTime);
		break;
	}
}

uint32_t Brd_GetVanState(){return HC165_ReadState(2);}


uint16_t Brd_GetTotalVan(){return brdParam.totalVan;}
uint16_t Brd_GetVanOn(){return brdParam.currentVanOn;}
uint16_t Brd_GetIntervalTime(){return brdParam.intervalTime;}
uint16_t Brd_GetPulseTime(){return brdParam.pulseTime;}
uint16_t Brd_GetCycleIntervalTime(){return brdParam.cycIntvTime;}
RTC_t Brd_GetRTC()
{
	brdParam.RTCtime = PCF8563_ReadTimeRegisters();
	return brdParam.RTCtime;
}

float Brd_GetPressure()
{
	brdParam.pressure = AMS5915_CalPressure(Brd_GetAddress_AMS5915());
	return brdParam.pressure;
}

int8_t Brd_SetTotalVan(uint8_t val)
{
    if(val > 0 && val <= 16){
        brdParam.totalVan = val;
        // reset all current valve
        brdParam.currentVanOn = 0;
        // fill all output bit from zero to total van
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



int16_t Brd_SetCycleIntervalTime(uint16_t val)
{
    if(val > 2 && val <= 100){
        brdParam.cycIntvTime = val;
    }
    else return -1;
    return 0;
}
uint8_t Brd_GetTimerArray(uint8_t element){return brdParam.timerArray[element];}
int8_t Brd_SetTimerArray(uint8_t element, uint8_t val)
{
	if(element >= sizeof(brdParam.timerArray)) return -1;
	brdParam.timerArray[element] = val;
	return 0;
}

VanProcedure Brd_GetVanProcState(){return vanProcState;}
void Brd_SetVanProcState(VanProcedure state){vanProcState = state;}
HC595* Brd_GetAddress_HC595(){return &brdParam.hc595;}
HC165* Brd_GetAddress_HC165(){return &brdParam.hc165;}
PCF8563_Handle* Brd_GetAddress_PCF8563(){return &brdParam.pcf;}
AMS5915* Brd_GetAddress_AMS5915(){return &brdParam.ams;}




