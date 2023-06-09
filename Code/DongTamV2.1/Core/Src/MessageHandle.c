#include "MessageHandle.h"
const char* strRxKey[] = {
	// Receive message
	"DoNothing",
	"SetVan",
	"ClearVan",
	"SetPulseTime",
	"SetCycleTime",
	"SetTotalVan",
	"SetIntervalTime",
	"TrigVan",
	"SetTime",
	"GetTime",
};

const char* strTxKey[] = {
	// Transmit message
	"DoNothing",
	"Van: ",
	"Time: ",
	"Pressure: ",
	"TotalVan: ",
	"PulseTime: ",
	"IntervalTime: ",
	"CycleTime: ",
};

HAL_StatusTypeDef MessageTxHandle(MesgValTX mesgValTX,char *outputStr)
{
	char s[40], sTemp[30];
	for(uint8_t i = 1; i < (sizeof(strTxKey)/sizeof(char*)); i++){
			// copy key string to send response
			if(i == mesgValTX) {
				strcpy(s,strTxKey[i]);
				break;
			}
	}
	switch (mesgValTX)
	{
	case TX_VAN:
		sprintf(sTemp,"%u",Brd_GetVanOn());
		break;
	case TX_TIME:
		RTC_PackTimeToString(Brd_GetRTC(),sTemp);
		break;
	case TX_PRESSURE:
		sprintf(sTemp,"%.2f",Brd_GetPressure());
		break;
	case TX_TOTAL_VAN:
		sprintf(sTemp,"%u",Brd_GetTotalVan());
		break;
	case TX_PULSE_TIME:
		sprintf(sTemp,"%u",Brd_GetPulseTime());
		break;
	case TX_INTERVAL_TIME:
		sprintf(sTemp,"%u",Brd_GetIntervalTime());
		break;
	case TX_CYCLE_TIME:
		sprintf(sTemp,"%u",Brd_GetCycleTime());
		break;
	default:
		break;
	}
	strcat(s,sTemp);
	strcpy(outputStr,s);
	strcat(outputStr,"\n");
	return HAL_OK;
}

HAL_StatusTypeDef MesgRxHandle(char *inputStr, char* outputStr)
{
	uint8_t indexKey = sizeof(strRxKey)/sizeof(char*);
	for(uint8_t i=0;i < indexKey;i++){
		if(strstr(inputStr,strRxKey[i]))
			return MesgGetValue(i,inputStr,outputStr);
	}
	return HAL_OK;
}

HAL_StatusTypeDef Mesg_SetVan(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetVanOn(*val) == -1) return HAL_ERROR;
	return HAL_OK;

}

HAL_StatusTypeDef Mesg_ClearVan(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetVanOff(*val) == -1) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef Mesg_TotalVan(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetTotalVan(*val) ==  -1) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef Mesg_PulseTime(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetPulseTime(*val) == -1) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef Mesg_IntervalTime(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetIntervalTime(*val) == -1) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef Mesg_CycleTime(void *pvParameter)
{
	uint32_t *val = (uint32_t*)pvParameter;
	if(Brd_SetCycleTime(*val) == -1) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef Mesg_SetTime(void *pvParameter)
{
	RTC_t *t = (RTC_t*)pvParameter;
	if(Brd_SetRTC(*t) == -1) return HAL_ERROR;
	return HAL_OK;
}

HAL_StatusTypeDef MesgGetValue(MesgValRX mesgValRX, char*inputStr,char *outputStr)
{
	RTC_t t;
	uint32_t val;
	uint8_t itemConverted = 0;
	pValueHandle pVal;
	HAL_StatusTypeDef pValRet = HAL_OK;
	if(mesgValRX >= SET_VAN && mesgValRX <= INTERVAL_TIME){
		itemConverted = sscanf(inputStr,MESG_PATTERN_KEY_VALUE_INT,&val);
		// check if value can be obtained from string
		if(itemConverted != 1) {
			strcpy(outputStr,"--->Cannot parse value\n");
			return HAL_ERROR;
		}
	} else if (mesgValRX == SET_TIME){
		t = RTC_GetTimeFromString(inputStr);
		if(Brd_SetRTC(t) == -1) return HAL_ERROR;
	}
	switch(mesgValRX){
		case SET_VAN:
			pVal = &Mesg_SetVan;
			pValRet = pVal((void *)&val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_VAN,outputStr);
			break;
		case CLEAR_VAN:
			pVal = &Mesg_ClearVan;
			pValRet = pVal((void *)&val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_VAN,outputStr);
			break;
		case TOTAL_VAN:
			pVal = &Mesg_TotalVan;
			pValRet = pVal((void *)&val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_TOTAL_VAN,outputStr);
			break;
		case PULSE_TIME:
			pVal = &Mesg_PulseTime;
			pValRet = pVal((void *)&val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_PULSE_TIME,outputStr);
			break;
		case CYCLE_TIME:
			pVal = &Mesg_CycleTime;
			pValRet = pVal((void *)&val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_CYCLE_TIME,outputStr);
			break;
		case INTERVAL_TIME:
			pVal = &Mesg_IntervalTime;
			pValRet = pVal((void *)&val);
			if(pValRet == HAL_OK) MessageTxHandle(TX_INTERVAL_TIME,outputStr);
			break;
		case SET_TIME:
			if(pValRet == HAL_OK) MessageTxHandle(TX_TIME,outputStr);
			return HAL_OK;
			break;
		case GET_TIME:
			if(pValRet == HAL_OK) MessageTxHandle(TX_TIME,outputStr);
			return HAL_OK;
		case TRIG_VAN:
			break;
		default:
			return HAL_OK;
			break;
	}
	if(pValRet != HAL_OK) {
		strcpy(outputStr,"--->Value beyond limit\n");
		return HAL_ERROR;
	}
	return HAL_OK;
}
