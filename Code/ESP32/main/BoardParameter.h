#ifndef INC_BOARDPARAMETER_H_
#define INC_BOARDPARAMETER_H_

#include "main.h"
#include "RTC_Format.h"

#define MAX_VAN 16

#define PARAM_STRING_ORDER_OFFSET 14

/*
This enum show full index of param in real table but the order is changed to make it easy to manage data.

From INDEX_DP_LOW to INDEX_SERV_RUN_HOURS_ALARM have data type integer, so we can loop these index to assign HighLimit value, LowLimit value and Default value; the other have data type struct or string which have special handle

In BoardParameter.c has paramMaxLimit array and paramMinLimit array which is mapped with this enum ParamIndex

In GUI.c has paramText pointer char* array which is mapped to this enum ParamIndex
*/ 
typedef enum {
    // unit is none
    INDEX_TOTAL_VAN,
    INDEX_DOWN_TIME_CYCLE,
    INDEX_ODC_CLEAN_MODE,
    INDEX_DISPLAY_CONTRAST,
    // unit is Pa
    INDEX_DP_LOW,
    INDEX_DP_HIGH,
    INDEX_DP_WARN,
    INDEX_ODC_HIGH,
    INDEX_ODC_LOW,
    // unit is ms
    INDEX_PULSE_TIME,
    // unit is s
    INDEX_INTERVAL_TIME,
    INDEX_CYCLE_INTERVAL_TIME,
    // unit is h
    INDEX_OPERATE_HOURS,
    INDEX_SERV_RUN_HOURS,
    INDEX_SERV_RUN_HOURS_ALARM,
    // special param to handle
    INDEX_LANGUAGE,
    INDEX_DISPLAY_RANGE,
    INDEX_TEST_MODE,
    INDEX_PARAM_CODE,
    INDEX_TECH_CODE,
    INDEX_DP_MODE,
}ParamIndex;

typedef struct BoardParameter
{
    uint8_t totalVan;
    uint8_t downTimeCycle;
    uint8_t cleanMode;
    uint8_t contrast;
    uint16_t dpHigh;
    uint16_t dpLow;
    uint16_t dpWarn;
    uint16_t odcHigh;
    uint16_t odcLow;
    uint16_t pulseTime;
    uint16_t intervalTime;
    uint16_t cycIntvTime;
    uint16_t servRunHours;
    uint16_t servAlarm;
    char* language;
    char* disRange;
    char* testMode;
    char* paramCode;
    char* techCode;
    char* dpMode;
    float pressure;
    RTC_t RTCtime;
}BoardParameter;


uint16_t Brd_ParamGetMaxLimit(uint8_t index);
uint16_t Brd_ParamGetMinLimit(uint8_t index);
uint16_t Brd_ParamGetValueInt(uint8_t index);
char* Brd_ParamGetValueString(uint8_t index);
char* Brd_ParamGetUnit(uint8_t index);

uint8_t Brd_GetTotalVan();
uint8_t Brd_GetDownTimeCycle();
uint8_t Brd_GetCleanMode();
uint8_t Brd_GetContrast();

uint16_t Brd_GetDPHigh();
uint16_t Brd_GetDPLow();
uint16_t Brd_GetDPWarn();
uint16_t Brd_GetODCLow();
uint16_t Brd_GetODCHigh();

uint16_t Brd_GetPulseTime();
uint16_t Brd_GetIntervalTime();
uint16_t Brd_GetCycleIntervalTime();

uint16_t Brd_GetServiceRunHours();
uint16_t Brd_GetServiceAlarm();

char* Brd_GetLanguage();
char* Brd_GetDisplayRange();
char* Brd_GetTestMode();
char* Brd_GetParamCode();
char* Brd_GetTechCode();
char* Brd_GetDPMode();
RTC_t Brd_GetRTC();
char* Brd_GetUnit(uint8_t index);


int16_t Brd_SetParamInt(uint8_t index,uint16_t val);
int16_t Brd_SetParamString(uint8_t index, char* valStr);
int16_t Brd_SetParamString(uint8_t index, char* valStr);
#endif