#include "BoardParameter.h"

uint16_t paramMaxLimit[] = {
        //total
        16,
        //down cyc
        32,
        //clean mode
        5,
        //contrast
        200, 
        //dp low
        4000,
        //dp high
        4000,
        //warn
        5000,
        //odc high
        4000,
        //odc low
        4000,
        //pulse
        300,
        //interval
        500,
        //cyc
        100,
        //oper h
        25000,
        //serv run
        25000,
        //serv alarm
        25000,
};

uint16_t paramMinLimit[] ={
        //total
        0,
        //down cyc
        0,
        //clean mode
        1,
        //contrast
        10,
        //dp low
        250,
        //dp high
        250,
        //warn
        300,
        //odc high
        250,
        //odc low
        250,
        //pulse
        30,
        //interval
        4,
        //cyc
        2,
        //oper h
        0,
        //serv run
        0,
        //serv alarm
        0,
};

uint16_t paramValInt[]= {
        //total
        0,
        //down cyc
        6,
        // clean mode
        3,
        // contrast
        50,
        //dp low
        700,
        //dp high
        1300,
        //warn
        2300,
        //odc high
        1000,
        //odc low
        250,
        //pulse
        60,
        //interval
        10,
        //cyc
        6,
        //oper h
        0,
        // serv run
        3000,
        // serv alarm
        0,
};

char* paramValString[]={
        "Eng",
        "F2",
        "Pa",
        "InWc"
        "F0",
        "On",
        "Off",
};

char* paramUnit[] = {
        " ",
        "Pa",
        "ms",
        "s",
        "h",
};


BoardParameter brdParam;

uint16_t Brd_ParamGetMaxLimit(uint8_t index){return paramMaxLimit[index];}
uint16_t Brd_ParamGetMinLimit(uint8_t index){return paramMinLimit[index];}
uint16_t Brd_ParamGetValueInt(uint8_t index){return paramValInt[index];}
char* Brd_ParamGetValueString(uint8_t index){return paramValString[index];}
char* Brd_ParamGetUnit(uint8_t index){return paramUnit[index];}

uint8_t Brd_GetTotalVan(){return brdParam.totalVan;}
uint8_t Brd_GetDownTimeCycle(){return brdParam.downTimeCycle;}
uint8_t Brd_GetCleanMode(){return brdParam.cleanMode;}
uint8_t Brd_GetContrast(){return brdParam.contrast;}

uint16_t Brd_GetDPHigh(){return brdParam.dpHigh;}
uint16_t Brd_GetDPLow(){return brdParam.dpLow;}
uint16_t Brd_GetDPWarn(){return brdParam.dpWarn;}
uint16_t Brd_GetODCLow(){return brdParam.odcLow;}
uint16_t Brd_GetODCHigh(){return brdParam.odcHigh;}

uint16_t Brd_GetPulseTime(){return brdParam.pulseTime;}
uint16_t Brd_GetIntervalTime(){return brdParam.intervalTime;}
uint16_t Brd_GetCycleIntervalTime(){return brdParam.cycIntvTime;}

uint16_t Brd_GetServiceRunHours(){return brdParam.servRunHours;}
uint16_t Brd_GetServiceAlarm(){return brdParam.servAlarm;}

char* Brd_GetLanguage(){return brdParam.language;}
char* Brd_GetDisplayRange(){return brdParam.disRange;}
char* Brd_GetTestMode(){return brdParam.testMode;}
char* Brd_GetParamCode(){return brdParam.paramCode;}
char* Brd_GetTechCode(){return brdParam.techCode;}
char* Brd_GetDPMode(){return brdParam.dpMode;}

RTC_t Brd_GetRTC(){return brdParam.RTCtime;}

char* Brd_GetUnit(uint8_t index){
        if(index <= INDEX_DISPLAY_CONTRAST){
                return paramUnit[0];
        } else if (index <= INDEX_ODC_LOW){
                return paramUnit[1];
        } else if (index == INDEX_PULSE_TIME){
                return paramUnit[2];
        } else if (index <= INDEX_CYCLE_INTERVAL_TIME){
                return paramUnit[3];
        } else if (index <= INDEX_SERV_RUN_HOURS_ALARM){
                return paramUnit[4];
        }
        return 0;
}

int16_t Brd_SetParamInt(uint8_t index,uint16_t val){
        if(index >= INDEX_TOTAL_VAN && index <= INDEX_SERV_RUN_HOURS_ALARM){
                
                return 0;
        }       
        return -1; 
}

int16_t Brd_SetParamString(uint8_t index, char* valStr){
        if(index >= INDEX_LANGUAGE && index <= INDEX_DP_MODE){
                index -= PARAM_STRING_ORDER_OFFSET;
                char* sTemp = &brdParam.language;
                strcpy((sTemp+index),valStr);
                return 0;
        }       
        return -1; 
}