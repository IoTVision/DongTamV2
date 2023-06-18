#include "MessageHandle.h"
#include "RTC_Format.h"
const char* strRxKey[] = {
	// Receive message
	"DoNothing",
	"Van",
	"VanState",
	"Pressure",
	"TotalVan",
	"PulseTime",
	"IntervalTime",
	"CycleIntervalTime",
	"CycleTime",
	"DP-High",
	"DP-Low",
	"DP-Warn",
	"Time: ",
};

const char* strTxKey[] = {
	// Transmit message
	"DoNothing",
	"SetVan: ",
	"ClearVan: ",
	"SetPulseTime: ",
	"SetCycleTime: ",
	"SetTotalVan: ",
	"SetCycleIntervalTime: ",
	"SetIntervalTime: ",
	"TrigVan:",
	"SetTime: ",
	"GetTime:",
};

esp_err_t MessageRxHandle(char *inputStr, char* outputStr)
{
	uint8_t indexKey = sizeof(strRxKey)/sizeof(char*);
	for(uint8_t i=0;i < indexKey;i++){
		if(strstr(inputStr,strRxKey[i]))
			return MesgGetValue(i,inputStr,outputStr);
	}
	return ESP_OK;
}

esp_err_t Mesg_SetVan(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetVanOn(*val) == -1) return ESP_ERR_INVALID_ARG;
	return ESP_OK;

}

esp_err_t MesgGetValue(MesgValRX mesgValRX, char*inputStr,char *outputStr)
{
	RTC_t t;
	uint32_t val;
	uint8_t itemConverted = 0;
	if(mesgValRX >= RX_VAN && mesgValRX <= RX_INTERVAL_TIME){
		itemConverted = sscanf(inputStr,MESG_PATTERN_KEY_VALUE_INT,&val);
		// check if value can be obtained from string
		if(itemConverted != 1) {
			strcpy(outputStr,"--->Cannot parse value\n");
			return ESP_ERR_INVALID_ARG;
		}
	}
	switch (mesgValRX)
	{
	case RX_VAN:
	break;
	case RX_VANSTATE:
	break;
	case RX_TOTAL_VAN:
	break;
	case RX_PULSE_TIME:
	break;
	case RX_CYC_INTV_TIME:
	break;
	case RX_CYCLE_TIME:
	break;
	case RX_DP_HIGH:
	break;
	case RX_DP_LOW:
	break;
	case RX_DP_WARN:
	break;
	case RX_INTERVAL_TIME:
	break;
	case RX_TIME:
	break;
	case RX_PRESSURE:
	break;
	default:
		break;
	}
}