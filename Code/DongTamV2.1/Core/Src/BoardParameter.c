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


void VanOn(char *outputStr,uint8_t currentVanOn)
{
	HC595_SetBitOutput(currentVanOn);
	HC595_ShiftOut(NULL, 2, 1);
	MessageTxHandle(TX_PRESSURE, outputStr);
	vanProcState = BRD_PULSE_TIME;
}

void VanOff(char *outputStr,uint8_t currentVanOn)
{
	HC595_ClearBitOutput(currentVanOn);
	MessageTxHandle(TX_VANSTATE, outputStr);
	HC595_ShiftOut(NULL, 2, 1);
	vanProcState = BRD_INTERVAL_TIME;
}

uint8_t CheckVanInUsed(uint8_t *totalVanOn)
{
	if(*currentVanOn > 0){
		for(uint8_t i=0;i<16;i++){
			if((*totalVanOn & 0x01) > 0) {
				return i;
			}
			else *totalVanOn >>=1;
		}
	}
	else if (!*(totalVanOn)) vanProcState = BRD_CYCLE_TIME;
	return 0;
}

uint8_t CheckCycleTime(uint8_t *cycTime)
{
	if(*cycTime > 0) {
		(*cycTime) -= 1;
		vanProcState = BRD_CYCLE_INTERVAL_TIME;
	}
	else if(!cycleTime){
		vanProcState = PROC_END;
	}
}

void CheckTimer_SetBit(uint16_t *tArr,VanProcedure nextState, uint16_t (*pBrdVal)())
{
	if(*tArr*10 >= pBrdVal()){
		*tArr = 0;
		Brd_SetVanProcState(nextState);
	}
}

void ProcedureTriggerVan(char *outputStr)
{
	static uint32_t delayAfterVanOn = 0;
	uint16_t pulseTime = Brd_GetPulseTime();
	static uint16_t cycleTime = Brd_GetCycleTime();
	static uint16_t currentVanOn = 0;
	static uint16_t totalVanOn = Brd_GetVanOn();
	switch(vanProcState){
	case PROC_START:
		break;
	case BRD_VAN_ON:
		VanOn(outputStr,currentVanOn);
		break;
	case BRD_PULSE_TIME:
		CheckTimer_SetBit(timerArray[0],BRD_VAN_OFF,&Brd_GetPulseTime());
		break;
	case BRD_VAN_OFF:
		VanOff(outputStr,currentVanOn);
		break;
	case BRD_INTERVAL_TIME:
		currentVanOn = CheckVanInUsed(&totalVanOn);
		if(currentVanOn)
		CheckTimer_SetBit(timerArray[1],BRD_VAN_ON,&Brd_GetIntervalTime());
		break;
	case BRD_CYCLE_TIME:
		CheckCycleTime(cycleTime);
		break;
	case BRD_CYCLE_INTERVAL_TIME:
		CheckTimer_SetBit(timerArray[2],BRD_VAN_ON,&Brd_GetPulseTime());
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

int16_t Brd_SetCycleTime(uint16_t val)
{
    if(val > 0 && val <= 100){
        brdParam.cycleTime = val;
    }
    else return -1;
    return 0;
}

int16_t Brd_SetCycleIntervalTime(uint16_t val)
{
    if(val > 2 && val <= 100){
        brdParam.cycIntTime = val;
    }
    else return -1;
    return 0;
}

int8_t Brd_FlagSetBit(BoardFlagBit f)
{
	if(f > FLAG_START && f < FLAG_END){
		SETFLAG(brdParam.f,f);
	} else return -1;
	return 0;
}

int8_t Brd_FlagClearBit(BoardFlagBit f)
{
	if(f > FLAG_START && f < FLAG_END){
		CLEARFLAG(brdParam.f,f);
	} else return -1;
	return 0;
}

int8_t Brd_FlagCheckBit(BoardFlagBit f)
{
	if(f > FLAG_START && f < FLAG_END){
		return CHECKFLAG(brdParam.f,f);
	} else return -1;
	return 0;
}


uint8_t Brd_GetTimerArray(uint8_t element){return brdParam.timerArray[element];}
int8_t Brd_SetTimerArray(uint8_t element, uint8_t val)
{
	if(element >= sizeof(brdParam.timerArray)) return -1;
	brdParam.timerArray[element] = val;
}

VanProcedure Brd_GetVanProcState(){return vanProState;}
VanProcedure Brd_SetVanProcState(VanProcedure state){vanProState = state;}
HC595* Brd_GetAddress_HC595(){return &brdParam.hc595;}
HC165* Brd_GetAddress_HC165(){return &brdParam.hc165;}
PCF8563_Handle* Brd_GetAddress_PCF8563(){return &brdParam.pcf;}
AMS5915* Brd_GetAddress_AMS5915(){return &brdParam.ams;}




