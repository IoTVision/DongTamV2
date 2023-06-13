#ifndef INC_BOARDPARAMETER_H_
#define INC_BOARDPARAMETER_H_

#include "main.h"
#include "RTC_Format.h"

#define MAX_VAN 16
typedef enum {
    INDEX_DP_LOW,
    INDEX_DP_HIGH,
    INDEX_DP_WARN,
    INDEX_PULSE_TIME,
    INDEX_INTERVAL_TIME,
    INDEX_CYCLE_INTERVAL_TIME,
    INDEX_ODC_HIGH,
    INDEX_ODC_LOW,
    INDEX_OPERATE_HOURS,
    INDEX_CLEAN_MODE,
    INDEX_SERV_RUN_HOURS,
}ParamIndex;

typedef struct BoardParameter
{
    uint16_t intervalTime;
    uint16_t cycIntvTime;
    uint16_t cycleTime;
    uint16_t pulseTime;
    uint16_t currentVanOn;
    uint16_t dpHigh;
    uint16_t dpLow;
    uint16_t dpWarn;
    uint8_t totalVan;
    float pressure;
    RTC_t RTCtime;
}BoardParameter;

#endif