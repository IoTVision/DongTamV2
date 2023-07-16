#include "./BoardParameter.h"

#include <stdint.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
BoardParameter brdParam;
nvs_handle_t brdNVS_Storage;
extern const char *paramText[];

char *Brd_NVS_Key[] = {
    "TotalVan",
    "DownCycT",
    "CleanMode",
    "DisContrast",
    "DPlow",
    "DPhigh",
    "DPwarn",
    "ODChigh",
    "ODClow",
    "PulseTime",
    "IntvTime",
    "CycIntvTime",
    "OperHour",
    "TestMode",
    "SevRunHour",
    "SevRunHAlarm",
};

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
        //test mode
        7,
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
        //test mode
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

const char* paramValString[]={
        "S", // combine with number to form a start of boundary for pararmeter; Ex: S1, S2, S3 ...
        "E", // combine with number to form an end of boundary for pararmeter; Ex: E1, E2, E3 ...

        // Language
        "S1", //start
        "Eng",
        "E1", // end

        // Display range
        "S2", //start
        "Pa",
        "InWc",
        "E2", // end


        // Param code
        "S3", //start
        "F2",
        "E3", // end

        // Tech code
        "S4", //start
        "F0",
        "E4", // end

        //DP mode
        "S5", // start
        "On",
        "Off",
        "E5", // end
};

const char* paramUnit[] = {
        " ",
        "Pa",
        "ms",
        "s",
        "h",
};

/**
 * @brief Lấy đơn vị của thông số board
 * 
 * @param index thứ tự thông số trong ParamIndex
 * @return const char*
 */
const char* Brd_GetUnit(uint8_t index){
        if(index <= INDEX_DISPLAY_CONTRAST){
                return 0;
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

void Brd_SendResponeInt(uint8_t index, uint32_t val, char* outputStr)
{
    strcpy(outputStr,paramText[index]);
    sprintf((outputStr+strlen(outputStr)),"%lu",val);
    strcat(outputStr,Brd_GetUnit(index));
}

void Brd_SendResponeString(uint8_t index, char *valStr, char* outputStr)
{
    strcpy(outputStr,paramText[index]);
    sprintf((outputStr+strlen(outputStr)),"%s",valStr);
}

esp_err_t Brd_SetParamInt(ParamIndex index,uint32_t val,char *outputStr){
    if(val < paramMinLimit[index] || val > paramMaxLimit[index]) {
        strcpy(outputStr,"Value beyond threshold");
        return ESP_ERR_INVALID_ARG;
    }
        if(index >= INDEX_TOTAL_VAN && index <= INDEX_SERV_RUN_HOURS_ALARM){
                switch (index)
                {
                case INDEX_TOTAL_VAN:
                    brdParam.totalVan = (uint8_t)val;
                    break;
                case INDEX_DOWN_TIME_CYCLE:
                    brdParam.downTimeCycle = (uint8_t)val;
                    break;
                case INDEX_ODC_CLEAN_MODE:
                    brdParam.cleanMode =(uint8_t)val;
                    break;
                case INDEX_DISPLAY_CONTRAST:
                    brdParam.contrast =(uint8_t)val;
                break;
                case INDEX_DP_LOW:
                    brdParam.dpLow =(uint16_t)val;
                break;
                case INDEX_DP_HIGH:
                    brdParam.dpHigh =(uint16_t)val;
                break;
                case INDEX_DP_WARN:
                    brdParam.dpWarn =(uint16_t)val;
                break;
                case INDEX_ODC_HIGH:
                    brdParam.odcHigh =(uint16_t)val;
                break;
                case INDEX_ODC_LOW:
                    brdParam.odcLow =(uint16_t)val;
                break;
                case INDEX_PULSE_TIME:
                    brdParam.pulseTime =(uint16_t)val;
                break;
                case INDEX_INTERVAL_TIME:
                    brdParam.intervalTime =(uint16_t)val;
                break;
                case INDEX_CYCLE_INTERVAL_TIME:
                    brdParam.cycIntvTime =(uint16_t)val;
                break;
                case INDEX_OPERATE_HOURS:
                    brdParam.operateHours =(uint16_t)val;
                break;
                case INDEX_SERV_RUN_HOURS:
                    brdParam.servRunHours =(uint16_t)val;
                break;
                case INDEX_SERV_RUN_HOURS_ALARM:
                    brdParam.servAlarm =(uint16_t)val;
                break;
                default:
                    break;
                }
                if(outputStr) Brd_SendResponeInt(index,val,outputStr);
                return ESP_OK;
        }       
        return ESP_ERR_INVALID_ARG; 
}

uint32_t Brd_GetParamInt(ParamIndex index)
{
        if(index >= INDEX_TOTAL_VAN && index <= INDEX_SERV_RUN_HOURS_ALARM){
            switch (index)
            {
            case INDEX_TOTAL_VAN:
                return brdParam.totalVan;
                break;
            case INDEX_DOWN_TIME_CYCLE:
                return brdParam.downTimeCycle ;
                break;
            case INDEX_ODC_CLEAN_MODE:
                return brdParam.cleanMode ;
                break;
            case INDEX_DISPLAY_CONTRAST:
                return brdParam.contrast ;
                break;
            case INDEX_DP_LOW:
                return brdParam.dpLow ;
                break;
            case INDEX_DP_HIGH:
                return brdParam.dpHigh ;
                break;
            case INDEX_DP_WARN:
                return brdParam.dpWarn ;
                break;
            case INDEX_ODC_HIGH:
                return brdParam.odcHigh ;
                break;
            case INDEX_ODC_LOW:
                return brdParam.odcLow ;
                break;
            case INDEX_PULSE_TIME:
                return brdParam.pulseTime ;
                break;
            case INDEX_INTERVAL_TIME:
                return brdParam.intervalTime ;
                break;
            case INDEX_CYCLE_INTERVAL_TIME:
                return brdParam.cycIntvTime ;
                break;
            case INDEX_OPERATE_HOURS:
                return brdParam.operateHours ;
                break;
            case INDEX_SERV_RUN_HOURS:
                return brdParam.servRunHours ;
                break;
            case INDEX_SERV_RUN_HOURS_ALARM:
                return brdParam.servAlarm ;
                break;
            default:
                break;
            }
        }       
        return ESP_ERR_INVALID_ARG; 
}


esp_err_t Brd_SetParamString(uint8_t index, char* valStr, char *outputStr)
{
        if (!valStr) {
            strcpy(outputStr,"valStr NULL");
            return ESP_ERR_INVALID_ARG;
        } 
        if(index >= INDEX_LANGUAGE && index <= INDEX_DP_MODE){
                index -= STRING_PARAM_OFFSET; // offset it to 1
                char start[5] = {0};
                char end[5] = {0};
                uint8_t startIndex = 0;
                strcpy(start,paramValString[0]); // Get the "S"
                strcpy(end,paramValString[1]); // Get the "E"
                sprintf(start+strlen(start),"%d",index); // combine with number, Ex: S1 and E1
                sprintf(end+strlen(end),"%d",index);
                for(uint8_t i = 0; i < sizeof(paramValString);i++){
                    if(!strcmp(start,paramValString[i])){
                        startIndex = i;
                    }  
                    if(!strcmp(valStr,paramValString[i]) && startIndex){
                        index += STRING_PARAM_OFFSET; // return it to original index
                        switch (index)
                        {
                        case INDEX_LANGUAGE:
                            brdParam.language = valStr;
                            break; 
                        case INDEX_DISPLAY_RANGE:
                            brdParam.disRange = valStr;
                            break;
                        case INDEX_TEST_MODE:
                            brdParam.testMode = valStr;
                            break;
                        case INDEX_PARAM_CODE:
                            brdParam.language = valStr;
                            break;
                        case INDEX_TECH_CODE:
                            brdParam.techCode = valStr;
                            break;
                        case INDEX_DP_MODE:
                            brdParam.dpMode = valStr;
                            break;
                        default:
                        return ESP_ERR_INVALID_ARG;
                            break;
                        }
                        if(outputStr) Brd_SendResponeString(index,"Set OK\n",outputStr);
                        return ESP_OK;
                    }
                    if(!strcmp(end,paramValString[i])){
                        index += STRING_PARAM_OFFSET; // return it to original index
                        if(outputStr) Brd_SendResponeString(index,"Set Error\n",outputStr);
                        return ESP_ERR_INVALID_ARG; // if searching to the end, return error
                    }
                }
                return ESP_OK;
        }       
        return ESP_ERR_INVALID_ARG; 
}

char* Brd_GetParamString(ParamIndex index)
{
        if(index >= INDEX_LANGUAGE && index <= INDEX_DP_MODE){
            switch(index){
            case INDEX_LANGUAGE:
                return brdParam.language;
                break;
            case INDEX_DISPLAY_RANGE:
                return brdParam.disRange ;
                break;
            case INDEX_PARAM_CODE:
                return brdParam.paramCode ;
                break;
            case INDEX_TECH_CODE:
                return brdParam.techCode ;
                break;
            case INDEX_DP_MODE:
                return brdParam.dpMode ;
                break;
            default:
                return 0;
                break;
            }
        }
        return NULL;
}

void Brd_PrintAllParameter()
{
	for(uint8_t i = INDEX_TOTAL_VAN; i <= INDEX_SERV_RUN_HOURS_ALARM; i++){
		ESP_LOGI("brdParamPrint","[%d]:%lu",i,Brd_GetParamInt(i));
	}
}

esp_err_t Brd_WriteParamToFlash(){
	esp_err_t err;
	err = nvs_open("Board", NVS_READWRITE, &brdNVS_Storage);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        err = nvs_set_blob(brdNVS_Storage,"Parameter",(void*)&brdParam,sizeof(BoardParameter));
        err = nvs_commit(brdNVS_Storage);
    }
    nvs_close(brdNVS_Storage);
    ESP_LOGI("BoardWriteFlash","Write parameter and close");
    vTaskDelay(10/portTICK_PERIOD_MS);
	return err;
}

esp_err_t Brd_ReadParamFromFlash()
{
        esp_err_t err;
    err = nvs_open("Board", NVS_READONLY, &brdNVS_Storage);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else {
        size_t sz;
        err = nvs_get_blob(brdNVS_Storage,"Parameter",NULL,&sz);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
        err = nvs_get_blob(brdNVS_Storage,"Parameter",&brdParam,&sz);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    }
	ESP_LOGI("BoardReadFlash","Read success");
    nvs_close(brdNVS_Storage);
    return err;

}

RTC_t Brd_GetRTC(){return brdParam.RTCtime;}


uint8_t Brd_GetTotalVan(){return brdParam.totalVan;}
uint8_t Brd_GetDownTimeCycle(){return brdParam.downTimeCycle;}
uint8_t Brd_GetCleanMode(){return brdParam.cleanMode;}
uint8_t Brd_GetContrast(){return brdParam.contrast;}

uint16_t Brd_ParamGetMaxLimit(uint8_t index){return paramMaxLimit[index];}
uint16_t Brd_ParamGetMinLimit(uint8_t index){return paramMinLimit[index];}
uint16_t Brd_ParamGetValueInt(uint8_t index){return paramValInt[index];}
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

const char* Brd_ParamGetValueString(uint8_t index){return paramValString[index];}
const char* Brd_GetParamText(uint8_t index){return paramText[index];}
char* Brd_GetLanguage(){return brdParam.language;}
char* Brd_GetDisplayRange(){return brdParam.disRange;}
char* Brd_GetTestMode(){return brdParam.testMode;}
char* Brd_GetParamCode(){return brdParam.paramCode;}
char* Brd_GetTechCode(){return brdParam.techCode;}
char* Brd_GetDPMode(){return brdParam.dpMode;}
