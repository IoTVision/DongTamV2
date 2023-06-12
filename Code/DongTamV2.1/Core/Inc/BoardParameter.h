/*
 * BoardParameter.h
 *
 *  Created on: Jun 6, 2023
 *      Author: KHOA
 */

#ifndef INC_BOARDPARAMETER_H_
#define INC_BOARDPARAMETER_H_
#include "main.h"
#include "RTC_Format.h"
#include "74HC595.h"
#include "74HC165.h"
#include "PCF8563.h"
#include "AMS5915.h"
#include "ShareVar.h"
#include "Flag.h"

#define MAX_MESSAGE 50
#define MAX_NUM_VAN 16
#define TIMER_PERIOD_MS 10

typedef struct BoardParameter
{
	HC595 hc595;
	HC165 hc165;
	AMS5915 ams;
	PCF8563_Handle pcf;
    uint16_t intervalTime;
    uint16_t cycIntvTime;
    uint16_t pulseTime;
    uint16_t currentVanOn;
    float pressure;
    uint8_t totalVan;
    uint8_t timerArray[3];// 1 for pulse time, 2 for interval time
    RTC_t RTCtime;
}BoardParameter;




typedef enum {
	PROC_START,
	BRD_VAN_ON,
	BRD_PULSE_TIME,
	BRD_VAN_OFF,
	BRD_INTERVAL_TIME,
	BRD_CYCLE_INTERVAL_TIME,
	PROC_END,
}VanProcedure;

float Brd_GetPressure();
uint16_t Brd_GetTotalVan();
uint16_t Brd_GetVanOn();
uint16_t Brd_GetIntervalTime();
uint16_t Brd_GetPulseTime();
RTC_t Brd_GetRTC();
uint16_t Brd_GetCycleIntervalTime();
uint8_t Brd_GetTimerArray(uint8_t element);

int8_t Brd_SetTotalVan(uint8_t val);
int16_t Brd_SetVanOn(uint16_t val);
int16_t Brd_SetVanOff(uint16_t val);
uint32_t Brd_GetVanState();
int16_t Brd_SetIntervalTime(uint16_t val);
int16_t Brd_SetPulseTime(uint16_t val);
int8_t Brd_SetRTC(RTC_t t);
int16_t Brd_SetCycleTime(uint16_t val);
int8_t Brd_SetTimerArray(uint8_t element, uint8_t val);
int16_t Brd_SetCycleIntervalTime(uint16_t val);

HC595* Brd_GetAddress_HC595();
HC165* Brd_GetAddress_HC165();
PCF8563_Handle* Brd_GetAddress_PCF8563();
AMS5915* Brd_GetAddress_AMS5915();

void ProcedureTriggerVan();

VanProcedure Brd_GetVanProcState();
void Brd_SetVanProcState(VanProcedure state);

#endif /* INC_BOARDPARAMETER_H_ */
