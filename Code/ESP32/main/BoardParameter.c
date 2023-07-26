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
    "StartParam", //not use
    "TotalVan:",
    "DownCycT:",
    "CleanMode:",
    "TestMode:",
    "DisContrast:",
    "DPlow:",
    "DPhigh:",
    "DPwarn:",
    "ODChigh:",
    "ODClow:",
    "PulseTime:",
    "IntvTime:",
    "CycIntvTime:",
    "OperHour:",
    "SevRunHour:",
    "SevRunHAlarm:",
    "StringOffset",//not use
    "Language:",
    "DisplayRange:",
    "ParamCode:",
    "TechCode:",
    "DPMode:",

};

uint16_t paramMaxLimit[] = {
    0,// nothing
    16,//total
    32,//down cyc
    5,//clean mode
    7,//test mode
    200, //contrast
    4000,//dp low
    4000,//dp high
    5000,//warn
    4000,//odc high
    4000,//odc low
    300,//pulse
    500,//interval
    100,//cyc
    25000,//oper h
    25000,//serv run
    25000,//serv alarm
};

uint16_t paramMinLimit[] ={
    0,// nothing
    0,//total
    0,//down cyc
    1,//clean mode
    0,//test mode
    10,//contrast
    250,//dp low
    250,//dp high
    300,//dp warn
    250,//odc high
    250,//odc low
    30,//pulse time
    4,//interval time
    2,//cyc time
    0,//oper h
    0,//serv run
    0,//serv alarm
};

uint16_t paramValInt[]= {
    0,// Nothing
    0,//total
    6,//down cyc
    3,// clean mode
    3,// test mode
    50,// contrast
    700,//dp low
    1300,//dp high
    2300,//warn
    1000,//odc high
    250,//odc low
    60,//pulse
    10,//interval
    6,//cyc
    0,//oper h
    3000,// serv run
    0,// serv alarm
};

/**
 * @brief Khi nhấn nút UP, DOWN trên màn hình thì giá trị thông số sẽ tăng và giảm
 * paramStepChange là các nấc thay đổi giá trị của thông số tương ứng
 * 
 */
const uint32_t paramStepChange[] = {
    0, // nothing
    1, // total
    1, // down cyc
    1, // clean mode
    1, // test mode
    5, //constrast
    50,//dp low
    50,//dp high
    50,//dp warn
    50, //odc high
    50, //odc low
    10,//pulse time
    2, //interval time
    1, //cycle time
    100, //operate hours
    100, //serv run
    100, //serv run alarm
    0, // offset
    1,// language
    1,// display range
    1,// param code
    1,// tech code
    1,// dp mode
};

const char* paramValString[]={
        "S", // combine with number to form a start of boundary for pararmeter; Ex: S1, S2, S3 ...
        "E", // combine with number to form an end of boundary for pararmeter; Ex: E1, E2, E3 ...

        // Language
        "S1", //start
        "Eng",
        "Jan",
        "Fra",
        "Vie",
        "US", 
        "UK",
        "E1", // end

        // Display range
        "S2", //start
        "Pa",
        "InWc",
        "mmHg",
        "E2", // end


        // Param code
        "S3", //start
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
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
                /*
                    Lưu ý đây là trả về 0, nếu giá trị là NULL thì không thể truy cập và sẽ báo lỗi
                    vì vậy cần phải kiểm tra giá trị trả về có khác 0 hay không trước khi sử dụng
                */ 
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
    if(index >= INDEX_STRING_PARAM_OFFSET){
        strcpy(outputStr,"Not index int range\n");
        return;
    }
    strcpy(outputStr,Brd_NVS_Key[index]);
    sprintf((outputStr+strlen(outputStr)),"%lu ",val);
    // if parameter has NULL unit, must be returned
    if(!Brd_GetUnit(index)) return;
    strcat(outputStr,Brd_GetUnit(index));
}

void Brd_SendResponeString(uint8_t index, char *valStr, char* outputStr)
{
    if(index < INDEX_STRING_PARAM_OFFSET) {
        strcpy(outputStr,"Not index string range\n");
        return;
    }
    strcpy(outputStr,Brd_NVS_Key[index]);
    sprintf((outputStr+strlen(outputStr)),"%s ",valStr);
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
                case INDEX_TEST_MODE:
                    brdParam.testMode = (uint8_t)val;
                break;
                default:
                    break;
                }
                if(outputStr) Brd_SendResponeInt(index,val,outputStr);
                return ESP_OK;
        }       
        return ESP_ERR_INVALID_ARG; 
}

uint32_t Brd_GetParamIntValue(ParamIndex index)
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
            case INDEX_TEST_MODE:
                return brdParam.testMode;
                break;
            default:
                break;
            }
        }       
        return ESP_ERR_INVALID_ARG; 
}


esp_err_t Brd_SetParamString(ParamIndex index,uint8_t indexStringValue, char *outputStr)
{
        if(index >= INDEX_LANGUAGE && index <= INDEX_DP_MODE){
                index -= INDEX_STRING_PARAM_OFFSET; // offset it to 1
                char start[5] = {0};
                char end[5] = {0};
                uint8_t startIndex = 0;
                uint8_t endIndex = 0;
                strcpy(start,paramValString[0]); // Get the "S"
                strcpy(end,paramValString[1]); // Get the "E"
                sprintf(start+strlen(start),"%d",index); // combine with number index, Ex: S1 and E1
                sprintf(end+strlen(end),"%d",index); 
                // Find startIndex (Sx) and endIndex(Ex) using for loop
                for(uint8_t i = 0; i < sizeof(paramValString);i++){
                    if(!strcmp(start,paramValString[i])){
                        startIndex = i;
                    }
                    if(!strcmp(end,paramValString[i])){
                        endIndex = i;
                        break;
                    }
                }
                if((startIndex && endIndex) && (startIndex < endIndex)){
                    uint8_t resultIndex = startIndex + indexStringValue;
                    // Checking limit and wrapping selection
                    if(resultIndex >= endIndex){
                        ESP_LOGW("SetString","reaching end, wrap around");
                        resultIndex = startIndex + 1;
                    } else if (resultIndex <= startIndex){
                        ESP_LOGW("SetString","reaching start, wrap around");
                        resultIndex = endIndex - 1;
                    }
                    index += INDEX_STRING_PARAM_OFFSET; // return it to original index to use for switch case 
                    switch (index){
                        case INDEX_LANGUAGE:
                            brdParam.language = (char*)paramValString[resultIndex];
                            break; 
                        case INDEX_DISPLAY_RANGE:
                            brdParam.disRange = (char*)paramValString[resultIndex];
                            break;
                        case INDEX_PARAM_CODE:
                            brdParam.paramCode = (char*)paramValString[resultIndex];
                            break;
                        case INDEX_TECH_CODE:
                            brdParam.techCode = (char*)paramValString[resultIndex];
                            break;
                        case INDEX_DP_MODE:
                            brdParam.dpMode = (char*)paramValString[resultIndex];
                            break;
                        default:
                        return ESP_ERR_INVALID_ARG;
                            break;
                    }
                    Brd_SendResponeString(index,"Set OK\n",outputStr);
                } else {
                    index += INDEX_STRING_PARAM_OFFSET; // return it to original index to use for switch case 
                    Brd_SendResponeString(index,"Set Error\n",outputStr);
                    return ESP_ERR_INVALID_ARG;
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
		ESP_LOGI("brdParamPrint","%s[%d]:%lu",Brd_NVS_Key[i],i,Brd_GetParamIntValue(i));
	}
    for(uint8_t i = INDEX_LANGUAGE; i <= INDEX_DP_MODE + 1; i++){
		if(Brd_GetParamString(i)) ESP_LOGI("brdParamPrint","[%d]:%s",i,Brd_GetParamString(i));
        else ESP_LOGE("brdParamPrint","Not found string");
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
    nvs_close(brdNVS_Storage);
	ESP_LOGI("BoardReadFlash","Read success");
    return err;

}

void Brd_LoadDefaultValue()
{
	esp_err_t err = ESP_OK;
	uint32_t valArray[] = {
        0,//nothing, it is the begin of param
		10,
		8,
		2,
		6,
		55,
        // unit is Pa
		550,
		1210,
		2250,
		2080,
		2240,
		160,
		400,
		6,
		18000,
		18650,
		24540,
	};
	uint8_t valueStr[]={
        2,
        2,
        2,
        2,
        2,
        2,
    };
	char s[50];
	for(uint8_t i = INDEX_TOTAL_VAN; i <= INDEX_SERV_RUN_HOURS_ALARM; i++){
		err = Brd_SetParamInt(i,valArray[i],s);
		if(err == ESP_OK) ESP_LOGI("LoadValueBoardInt","%s",s);
		else ESP_LOGE("LoadValueBoardInt","%s",s);
		memset(s,0,strlen(s));
	}
    for(uint8_t i = INDEX_LANGUAGE; i <= INDEX_DP_MODE; i++){
		err = Brd_SetParamString(i,valueStr[i - INDEX_STRING_PARAM_OFFSET],s);
		if(err == ESP_OK) ESP_LOGI("LoadValueBoardString","%s",s);
		else ESP_LOGE("LoadValueBoardString","%s",s);
		memset(s,0,strlen(s));
	}
}

RTC_t Brd_GetRTC(){return brdParam.RTCtime;}




uint16_t Brd_GetMaxLimit(uint8_t index){return paramMaxLimit[index];}
uint16_t Brd_GetMinLimit(uint8_t index){return paramMinLimit[index];}
uint32_t Brd_GetParamStepChange(uint8_t index){return paramStepChange[index];}

