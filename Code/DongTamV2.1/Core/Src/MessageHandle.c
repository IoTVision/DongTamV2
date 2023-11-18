#include "MessageHandle.h"
#include "RTC_Format.h"
#include "BoardParameter.h"
#include <stdlib.h>


const char* strRxKey[] = {
	// Receive message
	"Do nothing",
	"SetVan",
	"SetMultiVan",
	"ClearVan",
	"SetPulseTime",
	"SetTotalVan",
	"SetCycleIntervalTime",
	"SetIntervalTime",
	"TrigVan",
	"GetTime",
	"Do nothing",
	"SetTime",
	"Do nothing",
	"IsOnProcedure",
};

const char* strTxKey[] = {
	// Transmit message
	" ",
	"Van: ",
	"VanState: ",
	"Time: ",
	"Pressure: ",
	"TotalVan: ",
	"PulseTime: ",
	"IntervalTime: ",
	"CycleIntervalTime: ",
	"CurrentTime: ",
	"VanProcState: ",
};

HAL_StatusTypeDef MesgGetValue(MesgValRX mesgValRX, char*inputStr,char *outputStr);

/**
 * @brief Lấy giá trị các thông số trong BoardParameter tương ứng với mesgValTX, đóng gói và trả về chuỗi chứa giá trị các thông số
 * @param mesgValTX thứ tự param trong enum MesgValTX
 * @param outputStr chuỗi trả về để thông báo kết quả xử lý
 * @return HAL_OK nếu xử lý thành công, HAL_ERROR nếu có lỗi xảy ra
 */
HAL_StatusTypeDef MessageTxHandle(MesgValTX mesgValTX,char *outputStr)
{
	char s[40]={0};
	uint16_t len;
	for(uint8_t i = 1; i < (sizeof(strTxKey)/sizeof(char*)); i++){
			// copy key string to send response
			if(i == mesgValTX) {
				strcpy(s,strTxKey[i]);
				break;
			}
	}
	len = strlen(s);
	switch (mesgValTX)
	{
	case TX_VAN:
		itoa(Brd_GetVanOn(),(s+len),2);
		break;
	case TX_VANSTATE:
		sprintf((s+len),"%lu",Brd_GetVanState());
		break;
	case TX_TIME:
		RTC_PackTimeToString(Brd_GetRTC(),(s+len));
		Brd_RTC_SetTickCount(0);
		break;
	case TX_CURRENT_TIME_FROM_TICK:
		RTC_PackTimeToString(Brd_RTC_GetCurrentTimeFromTick(),(s+len));
		break;
	case TX_PRESSURE:
		sprintf((s+len),"%.2f",Brd_GetPressure());
		break;
	case TX_TOTAL_VAN:
		sprintf((s+len),"%u",Brd_GetTotalVan());
		break;
	case TX_PULSE_TIME:
		sprintf((s+len),"%u",Brd_GetPulseTime());
		break;
	case TX_INTERVAL_TIME:
		sprintf((s+len),"%u",Brd_GetIntervalTime());
		break;
	case TX_CYC_INTV_TIME:
		sprintf((s+len),"%u",Brd_GetCycleIntervalTime());
		break;
	case TX_VAN_IS_ON_PROCEDURE:
		sprintf((s+len),"%u",Brd_GetVanProcState());
		break;
	default:
		break;
	}
	strcpy(outputStr,s);
	strcat(outputStr,"\r\n");

	return HAL_OK;
}

HAL_StatusTypeDef MessageTxPMax(float pMax, char *outputStr)
{
	char s[40]={0};
	sprintf(s,"P: %.2f", pMax);
	strcpy(outputStr,s);
	strcat(outputStr,"\r\n");

	return HAL_OK;
}

/**
 * @brief Xử lý chuỗi nhận được, so sánh với các chuỗi đã có trong strRxKey
 * @param inputStr chuỗi cần xử lý
 * @param outputStr chuỗi trả về để thông báo kết quả xử lý
 * @return HAL_OK nếu xử lý thành công, HAL_ERROR nếu có lỗi xảy ra
 */
HAL_StatusTypeDef MessageRxHandle(char *inputStr, char* outputStr)
{
	uint8_t indexKey = sizeof(strRxKey)/sizeof(char*);
	for(uint8_t i=1;i < indexKey;i++){
		if(strstr(inputStr,strRxKey[i]))
			return MesgGetValue(i,inputStr,outputStr);
	}
	if(!strcmp(inputStr,"Hello STM32")){
		strcpy(outputStr,"Hello ESP32");
		Brd_ESP32_SetConnectIsTrue();
		memset(inputStr,0,strlen(inputStr));
		return HAL_OK;
	}
	memset(inputStr,0,strlen(inputStr));
	return HAL_ERROR;
}

/**
 * @brief Tách lấy giá trị từ chuỗi inputStr, gán tương ứng vào các thông số của BoardParameter
 * @param mesgValRX thứ tự param trong enum MesgValRX
 * @param inputStr chuỗi truyền vào để tách lấy giá trị
 * @param outputStr chuỗi trả về để thông báo kết quả xử lý
 * @return HAL_OK nếu tách giá trị thành công, HAL_ERROR nếu có lỗi xảy ra
 */
HAL_StatusTypeDef MesgGetValue(MesgValRX mesgValRX, char*inputStr,char *outputStr)
{
	RTC_t t;
	uint32_t val=0;
	uint8_t itemConverted = 0;
	HAL_StatusTypeDef pValRet = HAL_OK;
	if(mesgValRX > RX_START_INT_VALUE && mesgValRX < RX_START_TIME_FORMAT){
		itemConverted = sscanf(inputStr,MESG_PATTERN_KEY_VALUE_INT,&val);
		// check if value can be obtained from string
		if(itemConverted != 1) {
			if(outputStr) strcpy(outputStr,"--->Cannot parse value\n");
			return HAL_ERROR;
		}
	} else if (mesgValRX > RX_START_TIME_FORMAT && mesgValRX < RX_START_BOOLEAN){
		t = RTC_GetTimeFromString(inputStr);
		if(Brd_SetRTC(t) == HAL_ERROR) return HAL_ERROR;
	} else if (mesgValRX > RX_START_BOOLEAN){
		__NOP();
	}
	switch(mesgValRX){
		case RX_SET_VAN:
			pValRet = Brd_SetVanOn(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_VAN,outputStr);
			break;
		case RX_SET_MULTI_VAN:
			pValRet = Brd_SetMultiVan(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_VAN,outputStr);
			break;
		case RX_CLEAR_VAN:
			pValRet = Brd_SetVanOff(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_VAN,outputStr);
			break;
		case RX_PULSE_TIME:
			pValRet = Brd_SetPulseTime(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_PULSE_TIME,outputStr);
			break;
		case RX_TOTAL_VAN:
			pValRet = Brd_SetTotalVan(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_VAN,outputStr);
			break;
		case RX_CYC_INTV_TIME:
			pValRet = Brd_SetCycleIntervalTime(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_CYC_INTV_TIME,outputStr);
			break;
		case RX_INTERVAL_TIME:
			pValRet = Brd_SetIntervalTime(val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_INTERVAL_TIME,outputStr);
			break;
		case RX_VAN_IS_ON_PROCEDURE:
			MessageTxHandle(TX_VAN_IS_ON_PROCEDURE, outputStr);
			return HAL_OK;
			break;
		case RX_TRIG_VAN:
			if(val) {
				Brd_SetVanProcState(PROC_START);
				sprintf(outputStr,"TrigVan: On");
			}
			else {
				Brd_SetVanProcState(PROC_END);
				sprintf(outputStr,"TrigVan: Off");
			}
			break;
		case RX_SET_TIME:
			if(pValRet == HAL_OK) MessageTxHandle(TX_TIME,outputStr);
			Brd_RTC_SetTickCount(0);
			return HAL_OK;
			break;
		case RX_GET_TIME:
			if(pValRet == HAL_OK) MessageTxHandle(TX_TIME,outputStr);
			return HAL_OK;
		default:
			return HAL_ERROR;
			break;
	}
	if(pValRet != HAL_OK) {
		if(outputStr) strcpy(outputStr,"--->Value beyond limit\n");
		return HAL_ERROR;
	}
	return HAL_OK;
}
