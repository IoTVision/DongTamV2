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

typedef struct BoardParameter
{
	HC595 hc595;
	HC165 hc165;
	AMS5915 ams;
	PCF8563_Handle pcf;
    uint16_t intervalTime;
    uint16_t cycleTime;
    uint16_t pulseTime;
    uint16_t currentVanOn;
    float pressure;
    uint8_t totalVan;
    RTC_t RTCtime;
}BoardParameter;

uint16_t Brd_GetTotalVan();
uint16_t Brd_GetVanOn();
uint16_t Brd_GetIntervalTime();
uint16_t Brd_GetPulseTime();
RTC_t Brd_GetRTC();
uint16_t Brd_GetCycleTime();
float Brd_GetPressure();

int8_t Brd_SetTotalVan(uint8_t val);
int16_t Brd_SetVanOn(uint16_t val);
int16_t Brd_SetVanOff(uint16_t val);
int16_t Brd_SetIntervalTime(uint16_t val);
int16_t Brd_SetPulseTime(uint16_t val);
int8_t Brd_SetRTC(RTC_t t);
int16_t Brd_SetCycleTime(uint16_t val);
HC595* Brd_GetAddress_HC595();
HC165* Brd_GetAddress_HC165();
PCF8563_Handle* Brd_GetAddress_PCF8563();
AMS5915* Brd_GetAddress_AMS5915();

#endif /* INC_BOARDPARAMETER_H_ */
