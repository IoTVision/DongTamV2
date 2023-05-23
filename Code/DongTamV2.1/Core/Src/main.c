/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "74HC595.h"
#include "74HC165.h"
#include "PCF8563.h"
#include "string.h"
#include "AMS5915.h"
#include "Flag.h"
#include "cJSON.h"
#include "ShareVar.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MAX_MESSAGE 200
#define FLAG_UART_ESP_RX_DONE (1<<0)
#define FLAG_UART_LOG_RX_DONE (1<<1)

#define FLAG_JSON_PACK_TIME (1<<0)
#define FLAG_JSON_PACK_PRESSURE (1<<1)
#define FLAG_JSON_UNPACK_MESSAGE (1<<2)
#define FLAG_JSON_NEW_MESSAGE (1<<3)
#define FLAG_JSON_PACK_VAN_VALUE (1<<4)
#define FLAG_JSON_PRINT_PARAM (1<<5)
#define FLAG_JSON_READ_HC165 (1 << 6)

#define FLAG_GET_PRESSURE (1<<0)
#define FLAG_SET_TIME (1<<1)
#define FLAG_GET_TIME (1<<2)
#define FLAG_SET_VAN (1<<3)
#define FLAG_CLEAR_VAN (1<<4)
#define FLAG_TRIG_VAN (1<<5)
#define FLAG_TRIG_VAN_PROCEDURE (1<<6)
#define FLAG_AFTER_TRIG_VAN_ON (1<<7)

#define MAX_NUM_VAN 16
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart3_rx;

///* USER CODE BEGIN PV */

uint32_t hc165_data = 0;
cJSON *cjsCommon;
FlagGroup_t fUART,f1 = 0,fJS;
char TimeString[30];
int SetVan,ClearVan;
bool TrigVan = false;
char uartEsp32Buffer[MAX_MESSAGE],uartLogBuffer[MAX_MESSAGE];
uint16_t uartEsp32RxSize,uartLogRxSize;
BoarParam brdParam;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void SetUp();
void GetUartJson();
void HandleFlagCommand();
void PackageMessage(cJSON *cjs);
void UnpackMessage(cJSON *cjs);

void ProcedureVan();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == I2C1){
	}
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if(huart->Instance == USART1){
		HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t*)uartEsp32Buffer, MAX_MESSAGE);
		uartEsp32RxSize = Size;
		SETFLAG(fUART,FLAG_UART_ESP_RX_DONE);
	}
	if(huart->Instance == USART3){
		HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t*)uartLogBuffer, MAX_MESSAGE);
		uartLogRxSize = Size;
		SETFLAG(fUART,FLAG_UART_LOG_RX_DONE);
	}
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

	HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  SetUp();
  cjsCommon = cJSON_CreateObject();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  GetUartJson();
	  UnpackMessage(cjsCommon);
	  HandleFlagCommand();
	  ProcedureVan();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  PackageMessage(cjsCommon);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}



/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, _74HC595_CLK_Pin|_74HC595_DATA_Pin|_74HC165_LOAD_Pin|OE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, _74HC595_STORE_Pin|UserLED_Pin|_74HC165_CLK_Pin
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA4 PA6 PA8
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : _74HC595_CLK_Pin _74HC595_DATA_Pin */
  GPIO_InitStruct.Pin = _74HC595_CLK_Pin|_74HC595_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : _74HC595_STORE_Pin LED_Pin MCP41010_CS_Pin _74HC165_CLK_Pin */
  GPIO_InitStruct.Pin = _74HC595_STORE_Pin|UserLED_Pin|_74HC165_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB14 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_14|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : _74HC165_LOAD_Pin */
  GPIO_InitStruct.Pin = _74HC165_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(_74HC165_LOAD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OE_Pin */
  GPIO_InitStruct.Pin = OE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : _74HC165_DATA_Pin */
  GPIO_InitStruct.Pin = _74HC165_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(_74HC165_DATA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void SendJsonStringToUart(cJSON *cjs, char *keyName, UART_HandleTypeDef *huartx)
{
	if(keyName){
		char s[40]={0};
		strcpy(s,"{\n\t\"");
		strcat(s,keyName);
		strcat(s,"\":\t");
		strcat(s,cJSON_Print(cJSON_GetObjectItemCaseSensitive(cjs, keyName)));
		strcat(s,"\n}\n");
		HAL_UART_Transmit(huartx, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
	else {
		char *s = cJSON_Print(cjs);
		strcat(s,"\r\n");
		HAL_UART_Transmit(huartx, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
}








void PackJsonPressure(cJSON *object, cJSON *item, void* pvParam)
{
	float *p = (float*)pvParam;
	if(item != NULL) cJSON_SetNumberHelper(item,*p);
	else cJSON_AddNumberToObject(object,"Pressure",*p);
}

void PackJsonVanValue(cJSON *object, cJSON *item, void* pvParam)
{
	uint32_t *data = (uint32_t*)pvParam;
	if(item != NULL)  cJSON_SetNumberHelper(item,*data);
	else cJSON_AddNumberToObject(object,"VanValue",*data);
}

void PackJsonTime(cJSON *object, cJSON *item, void* pvParam)
{
	RTC_t *t = (RTC_t*)pvParam;
	char TimeString[20] = {0};
	RTC_PackTimeToString(t, TimeString);
	if(item != NULL) cJSON_SetValuestring(item,TimeString);
	else cJSON_AddStringToObject(object,"Time",TimeString);
}

void PackJsonVanState(cJSON *object, cJSON *item, void* pvParam)
{
	char* data = (char*)pvParam;
	if(item != NULL) cJSON_SetValuestring(item,data);
	else cJSON_AddStringToObject(object,"HC165",data);
	memset(data,0,strlen(data));
}

void PackJsonItem(cJSON *cjs, char* jsonKey, FlagGroup_t *f ,uint32_t FlagBit,void(*pSetJsonValue)(cJSON *object,cJSON *item,void* pvParam), void* inputData)
{
	if(CHECKFLAG(*f,FlagBit)){
		if(cJSON_HasObjectItem(cjs, jsonKey)){
			cJSON *item = cJSON_GetObjectItemCaseSensitive(cjs, jsonKey);
			pSetJsonValue(cjs,item,(void*)inputData);
		} else pSetJsonValue(cjs,NULL,(void*)inputData);
		SendJsonStringToUart(cjs, jsonKey, &huart3);
		cJSON_DeleteItemFromObject(cjs,jsonKey);
		CLEARFLAG(*f,FlagBit);
	}
}

void PackageMessage(cJSON *cjs)
{
	FlagGroup_t fTemp =   FLAG_JSON_PRINT_PARAM
						| FLAG_JSON_PACK_VAN_VALUE
						| FLAG_JSON_PACK_TIME
						| FLAG_JSON_PACK_PRESSURE;
	if(!CHECKFLAG(fJS,FLAG_JSON_PRINT_PARAM)){
		PackJsonItem(cjs,"Pressure",&fJS,FLAG_JSON_PACK_PRESSURE,&PackJsonPressure,&brdParam.pressure);
		PackJsonItem(cjs,"Time",&fJS,FLAG_JSON_PACK_TIME,&PackJsonTime,&brdParam.pcf.t);
		PackJsonItem(cjs,"VanValue",&fJS,FLAG_JSON_PACK_VAN_VALUE,&PackJsonVanValue,&brdParam.hc595.data);
		PackJsonItem(cjs,"HC165",&fJS,FLAG_JSON_READ_HC165,&PackJsonVanState,&brdParam.hc165);
	}
	else if(CHECKFLAG(fJS,fTemp)){
		cJSON_AddNumberToObject(cjs, "VanValue", brdParam.hc595.data);
		cJSON_AddNumberToObject(cjs, "Pressure", brdParam.pressure);
		cJSON_AddStringToObject(cjs, "Time", TimeString);
//		JSON_LOG(cjs,NULL);
		cJSON_DeleteItemFromObject(cjs, "VanValue");
		cJSON_DeleteItemFromObject(cjs, "Pressure");
		cJSON_DeleteItemFromObject(cjs, "Time");
		p=0;
		memset(TimeString,0,strlen(TimeString));
		CLEARFLAG(fJS,fTemp);
	}
}







void SetVanHandle(cJSON *object)
{
	uint8_t SetVan;
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, "SVan");
	if(!cJSON_IsNumber(item)) return;
	SetVan = (uint8_t)cJSON_GetNumberValue(item);
	if(SetVan < MAX_NUM_VAN) HC595_SetBitOutput(SetVan);
	else {
		sprintf(s,"Van num must be less than %d",MAX_NUM_VAN);
		HAL_UART_Transmit(&huart3, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
	SETFLAG(fJS,FLAG_JSON_PACK_VAN_VALUE);
}
void ClearVanHandle(cJSON *object)
{
	uint8_t ClearVan;
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, "CVan");
	if(!cJSON_IsNumber(item)) return;
	ClearVan = (uint8_t)cJSON_GetNumberValue(item);
	if(ClearVan < MAX_NUM_VAN) HC595_SetBitOutput(ClearVan);
	else {
		sprintf(s,"Van num must be less than %d",MAX_NUM_VAN);
		HAL_UART_Transmit(&huart3, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
	}
	SETFLAG(fJS,FLAG_JSON_PACK_VAN_VALUE);
}

void TrigVanHandle(cJSON *object)
{
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, "TrigVan");
	if(!cJSON_IsTrue(item)) return;
	SETFLAG(f1, FLAG_TRIG_VAN_PROCEDURE);
}

void SetTimeHandle(cJSON *object)
{
	cJSON *item = cJSON_GetObjectItemCaseSensitive(object, "STime");
	if(!cJSON_IsString(item)) return;
	brdParam.pcf.t = RTC_GetTimeFromString(cJSON_GetStringValue(item));
	PCF8563_WriteTimeRegisters(brdParam.pcf.t);
	SETFLAG(fJS,FLAG_JSON_PACK_TIME); // to read back time value for double check
}

void GetTimeHandle(cJSON *object)
{
	brdParam.pcf.t = PCF8563_ReadTimeRegisters();
	SETFLAG(fJS,FLAG_JSON_PACK_TIME);
}
void GetAsm5915Pressure(cJSON *object)
{
	AMS5915_ReadRaw(&brdParam.ams);
	brdParam.pressure = AMS5915_CalPressure(&brdParam.ams);
	SETFLAG(fJS,FLAG_JSON_PACK_PRESSURE);
}

void CheckItem_GetValue_SetFlag(cJSON *cjs,char* jsonKey,FlagGroup_t *f, uint32_t FlagBit,void (*pGetValue)(cJSON *object))
{
	if(cJSON_HasObjectItem(cjs,jsonKey)) {
		if(pGetValue) pGetValue(cjs);
		cJSON_DeleteItemFromObject(cjs, jsonKey);
		SETFLAG(*f,FlagBit);
	}
}

void UnpackMessage(cJSON *cjs)
{
	if(!CHECKFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE)) return;
	CLEARFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE);
	CheckItem_GetValue_SetFlag(cjs, "VanValue",&f1,FLAG_JSON_PACK_VAN_VALUE,NULL);
	CheckItem_GetValue_SetFlag(cjs, "PrintParam",&f1,FLAG_GET_PRESSURE
												| FLAG_GET_TIME
												| FLAG_JSON_PRINT_PARAM
												| FLAG_JSON_PACK_VAN_VALUE,NULL);

	CheckItem_GetValue_SetFlag(cjs, "GPressure", &f1, FLAG_GET_PRESSURE,&GetAsm5915Pressure);
	CheckItem_GetValue_SetFlag(cjs, "GTime",&f1,FLAG_GET_TIME,&GetTimeHandle);
	CheckItem_GetValue_SetFlag(cjs, "STime",&f1,FLAG_SET_TIME,&SetTimeHandle);
	CheckItem_GetValue_SetFlag(cjs, "SVan",&f1,FLAG_SET_VAN,&SetVanHandle);
	CheckItem_GetValue_SetFlag(cjs, "CVan",&f1,FLAG_CLEAR_VAN,&ClearVanHandle);
	//No need to set any flag to trigger, SetTrigVanFlag automatically set itself if true
	CheckItem_GetValue_SetFlag(cjs, "TrigVan",&f1,0,&TrigVanHandle);


}

void ProcedureVan(){
	static uint32_t delayAfterVanOn = 0;
	// turn on valve
	if(CHECKFLAG(f1, FLAG_TRIG_VAN_PROCEDURE)){
		HC595_ShiftOut(NULL, 2, 1);
		SETFLAG(f1,FLAG_AFTER_TRIG_VAN_ON);
		CLEARFLAG(f1, FLAG_TRIG_VAN_PROCEDURE);
		delayAfterVanOn = HAL_GetTick();
	}
	// Delay 500ms, read van state then turn off
	if(CHECKFLAG(f1,FLAG_AFTER_TRIG_VAN_ON) && (HAL_GetTick() - delayAfterVanOn >= 500)){
		HC165_ReadState(&hc165_data, 2);
		HC595_ClearBitOutput(brdParam.hc595.data);
		HC595_ShiftOut(NULL, 2, 1);
		SETFLAG(fJS, FLAG_JSON_READ_HC165);
		CLEARFLAG(f1,FLAG_AFTER_TRIG_VAN_ON);
	}
}

void GetUartJson()
{
	if(CHECKFLAG(fUART,FLAG_UART_ESP_RX_DONE)){
		cjsCommon = cJSON_Parse(uartEsp32Buffer);
		memset(uartEsp32Buffer,0,uartEsp32RxSize);
		SETFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE);
		CLEARFLAG(fUART,FLAG_UART_ESP_RX_DONE);
	}
	if(CHECKFLAG(fUART,FLAG_UART_LOG_RX_DONE)){
		cjsCommon = cJSON_Parse(uartLogBuffer);
		memset(uartLogBuffer,0,uartLogRxSize);
		SETFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE);
		CLEARFLAG(fUART,FLAG_UART_LOG_RX_DONE);
	}
}



void UartIdle_Init()
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)uartEsp32Buffer, MAX_MESSAGE);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t*)uartLogBuffer, MAX_MESSAGE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_HT);
}

void hc595_SetUp()
{


	HC595_AssignPin(&brdParam.hc595, GPIOA, GPIO_PIN_5, HC595_CLK);
	HC595_AssignPin(&brdParam.hc595, GPIOA, GPIO_PIN_7, HC595_DS);
	HC595_AssignPin(&brdParam.hc595, GPIOB, GPIO_PIN_0, HC595_LATCH);
	HC595_AssignPin(&brdParam.hc595, GPIOA, GPIO_PIN_12,HC595_OE);
	HC595_SetTarget(&brdParam.hc595);

	HC595_ClearByteOutput(0xffffffff, 0);
	HC595_ShiftOut(NULL, 2, 1);
	HC595_Enable();
}

void hc165_SetUp()
{
	HC165_AssignPin(&brdParam.hc165, GPIOA, GPIO_PIN_11, HC165_PL);
	HC165_AssignPin(&brdParam.hc165, GPIOB, GPIO_PIN_3, HC165_DATA);
	HC165_AssignPin(&brdParam.hc165, GPIOA, GPIO_PIN_12, HC165_CE);
	HC165_AssignPin(&brdParam.hc165, GPIOB, GPIO_PIN_4, HC165_CP);
}

void SetUp()
{

	AMS5915_Init(&brdParam.ams,&hi2c1);
	PCF8563_Init(&brdParam.pcf,&hi2c1);
	PCF8563_StartClock();
	UartIdle_Init();
	hc165_SetUp();
	hc595_SetUp();

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
