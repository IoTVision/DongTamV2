#include "MessageHandle.h"
#include "RTC_Format.h"
#include "../GUI/PressureIndicator.h"
#include "../GUI/LedButton.h"
#include "../BoardParameter.h"

uint8_t PI_HandlePressureFromUART();

const char* strRxKey[] = {
	// Receive message
	" ",//START_INT_VALUE
	"VanState",
	"TotalVan",
	"dpHigh",
	"dpLow",
	"dpWarn",
	"PulseTime",
	"IntervalTime",
	"CycleIntervalTime",
	"ReadFlash",
	"SaveFlash",
	" ",//START_FLOAT_VALUE
	"P",
	" ",//START_TIME_FORMAT
	"Time: ",
};

/**
 *  Tập lệnh gửi đi thông số board để phản hồi 
*/
const char* strTxKey[] = {
	// Transmit message
	"DoNothing",
	"SetVan: ",
	"SetMultiVan",
	"ClearVan: ",
	"SetPulseTime: ",
	"SetTotalVan: ",
	"SetCycleIntervalTime: ",
	"SetIntervalTime: ",
	"TrigVan:",
	"SetTime: ",
	"GetTime: ",
};

/**	
 * @brief Xử lý chuỗi nhận được, so sánh với các chuỗi đã có trong strRxKey
 * @param inputStr chuỗi cần xử lý
 * @param outputStr chuỗi trả về để thông báo kết quả xử lý
 * @return HAL_OK nếu xử lý thành công, HAL_ERROR nếu có lỗi xảy ra
 */
esp_err_t MessageRxHandle(char *inputStr, char* outputStr)
{
	ESP_LOGI("RxHandle","PassHere");
	uint8_t indexKey = sizeof(strRxKey)/sizeof(char*);
	for(uint8_t i=1;i < indexKey;i++){
		if(strstr(inputStr,strRxKey[i])){
			ESP_LOGI("RxHandle","i:%u",i);
			return MesgGetValue(i,inputStr,outputStr);
		}
	}
	return ESP_OK;
}



esp_err_t MesgGetValue(MesgValRX mesgValRX, char*inputStr,char *outputStr)
{
	RTC_t t;
	uint32_t val=0;
	float pVal=0;
	uint8_t itemConverted = 0;
	if(mesgValRX > RX_START_INT_VALUE && mesgValRX < RX_START_FLOAT_VALUE){
		itemConverted = sscanf(inputStr,MESG_PATTERN_KEY_VALUE_INT,&val);
		ESP_LOGI("GetValInt","val:%lu,mesg:%d",val,mesgValRX);	
	} else if (mesgValRX > RX_START_FLOAT_VALUE && mesgValRX < RX_START_TIME_FORMAT){
		itemConverted = sscanf(inputStr,MESG_PATTERN_KEY_VALUE_FLOAT,&pVal);
		ESP_LOGI("GetValFloat","pVal:%.2f,mesg:%d",pVal,mesgValRX);	
	}
	// check if value can be obtained from string
	if(itemConverted != 1) {
		if(outputStr) strcpy(outputStr,"--->Cannot parse value\n");
		return ESP_ERR_INVALID_ARG;
	}
	switch (mesgValRX)
	{
	case RX_VANSTATE:
	if(!val) {
		LedErrorWrite(1);
		LedStatusWrite(0);
		HC595_ShiftOut(NULL,2,1);
	}
	else {
		LedErrorWrite(0);
		LedStatusWrite(1);
		HC595_ShiftOut(NULL,2,1);
	}
	break;
	case RX_TOTAL_VAN:
	Brd_SetParamInt(INDEX_TOTAL_VAN,val,NULL);
	break;
	case RX_PULSE_TIME:
	Brd_SetParamInt(INDEX_PULSE_TIME,val,NULL);
	break;
	case RX_CYC_INTV_TIME:
	Brd_SetParamInt(INDEX_CYCLE_INTERVAL_TIME,val,NULL);
	break;
	case RX_DP_HIGH:
	Brd_SetParamInt(INDEX_DP_HIGH,val,NULL);
	break;
	case RX_DP_LOW:
	Brd_SetParamInt(INDEX_DP_LOW,val,NULL);
	break;
	case RX_DP_WARN:
	Brd_SetParamInt(INDEX_DP_WARN,val,NULL);
	break;
	case RX_INTERVAL_TIME:
	break;
	case RX_TIME:
	break;
	case RX_PRESSURE: 
	PI_SetLevel(PI_HandlePressureFromUART(pVal));
	break;
	case RX_READ_FLASH:
	if(val) {
		Brd_ReadParamFromFlash();
		Brd_PrintAllParameter();
	}
	break;
	case RX_SAVE_FLASH:
	if(val) {
		esp_err_t err = ESP_OK;
		err = Brd_WriteParamToFlash();
		if(err != ESP_OK) ESP_LOGE("GetVal","err:%d",err);
		else ESP_LOGI("GetVal","err:%d",err);
	}
	else ESP_LOGI("GetVal","Are you kidding me?");
	break;
	default:
		break;
	}
	return ESP_ERR_INVALID_ARG;
}


uint8_t PI_HandlePressureFromUART(uint32_t val)
{
	uint32_t dpHigh = Brd_GetParamIntValue(INDEX_DP_HIGH);
	uint32_t dpLow = Brd_GetParamIntValue(INDEX_DP_LOW);
	uint32_t dpStep = (uint32_t)((dpHigh - dpLow)/10); 
	uint32_t a = dpLow + dpStep;
	for(uint8_t i=1; i <= 10;i++){
		if(a > val){
			return i; // return level of pressure indicator
		} else {
			a += dpStep;
		}
		if(a > dpHigh) return i;
		else if(a < dpLow) return 1;
	} 
	return 1;
}