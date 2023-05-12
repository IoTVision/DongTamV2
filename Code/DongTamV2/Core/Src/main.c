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
#include "74HC595.h"
#include "74HC165.h"
#include "PCF8563.h"
#include "DS3231.h"
#include "string.h"
#include "AMS5915.h"
#include "Flag.h"
#include "cJSON.h"
#include "stdio.h"
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
#define FLAG_JSON_GET_VAN_VALUE (1<<4)

#define FLAG_GET_PRESSURE (1<<0)
#define FLAG_SET_TIME (1<<1)
#define FLAG_GET_TIME (1<<2)
#define FLAG_SET_VAN (1<<3)
#define FLAG_CLEAR_VAN (1<<4)
#define FLAG_TRIG_VAN (1<<5)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
HC595 hc595;
HC165 hc165;
PCF8563_Handle pcfHandle;
AMS5915 ams;
cJSON *cjsCommon;
FlagGroup_t fUART,f1,fJS;
double p;
char TimeString[30];
int SetVan,ClearVan;
bool TrigVan = false;
char uartEsp32Buffer[MAX_MESSAGE],uartLogBuffer[MAX_MESSAGE];
uint16_t uartEsp32RxSize,uartLogRxSize;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void SetUp();
void GetUartJson();
void HandleFlagCommand();
void PackageMessage(cJSON *cjs);
void UnpackMessage(cJSON *cjs);
//void UpdateStatusVan();
//HAL_StatusTypeDef ControlVanAndCheck();
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

void JSON_LOG(char * monitor)
{

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
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
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
	  HandleFlagCommand();
	  PackageMessage(cjsCommon);
	  UnpackMessage(cjsCommon);
//	  if(TrigVan == TRUE){
//		  HAL_StatusTypeDef ControlVan();
//	  }
//	  if(!f1 && !fJS && CHECKFLAG(fJS,FLAG_JSON_NEW_MESSAGE)){
//		  cJSON_Delete(cjsCommon);
//	  }
//	AMS5915_ReadRaw(&ams);
//	p = AMS5915_CalPressure(&ams);
//	HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOB, _74HC595_STORE_Pin|LED_Pin|MCP41010_CS_Pin|_74HC165_CLK_Pin
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
  GPIO_InitStruct.Pin = _74HC595_STORE_Pin|LED_Pin|MCP41010_CS_Pin|_74HC165_CLK_Pin;
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
void HandleFlagCommand()
{
	if(CHECKFLAG(f1,FLAG_GET_PRESSURE)){
		AMS5915_ReadRaw(&ams);
		p = AMS5915_CalPressure(&ams);
		SETFLAG(fJS,FLAG_JSON_PACK_PRESSURE);
		CLEARFLAG(f1,FLAG_GET_PRESSURE);
	}
	if(CHECKFLAG(f1,FLAG_SET_VAN)){
		HC595_SetBitOutput(SetVan);
		CLEARFLAG(f1,FLAG_SET_VAN);
	}
	if(CHECKFLAG(f1,FLAG_CLEAR_VAN)){
		HC595_ClearBitOutput(ClearVan);
		CLEARFLAG(f1,FLAG_CLEAR_VAN);
	}
	if(CHECKFLAG(f1,FLAG_TRIG_VAN)){
		HC595_ShiftOut(NULL, 2, 1);
		CLEARFLAG(f1,FLAG_TRIG_VAN);
	}
	if(CHECKFLAG(f1,FLAG_SET_TIME)){
		pcfHandle.t = RTC_GetTimeFromString(TimeString);
		PCF8563_WriteTimeRegisters(pcfHandle.t);
		HAL_UART_Transmit(&huart3, (uint8_t*)"SetTimeOK", sizeof("SetTimeOK"), HAL_MAX_DELAY);
		SETFLAG(f1,FLAG_GET_TIME);
		memset(TimeString,0,strlen(TimeString));
		CLEARFLAG(f1,FLAG_SET_TIME);
	}
	if(CHECKFLAG(f1,FLAG_GET_TIME)){
		pcfHandle.t = PCF8563_ReadTimeRegisters();
		RTC_PackTimeToString(&pcfHandle.t, TimeString);
		SETFLAG(fJS,FLAG_JSON_PACK_TIME);
		CLEARFLAG(f1,FLAG_GET_TIME);
	}
}

void PackageMessage(cJSON *cjs)
{
	if(CHECKFLAG(fJS,FLAG_JSON_PACK_PRESSURE)){
		cJSON_AddNumberToObject(cjs, "Pressure", p);
		char s[50];
		strcpy(s,"Pressure:");
		strcat(s,cJSON_Print(cJSON_GetObjectItemCaseSensitive(cjs, "Pressure")));
		strcat(s,"\n");
		HAL_UART_Transmit(&huart3, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "Pressure");
		p=0;
		CLEARFLAG(fJS,FLAG_JSON_PACK_PRESSURE);
	}
	if(CHECKFLAG(fJS,FLAG_JSON_PACK_TIME)){
		cJSON_AddStringToObject(cjs, "Time", TimeString);
		char s[50];
		strcpy(s,"Time:");
		strcat(s,cJSON_Print(cJSON_GetObjectItemCaseSensitive(cjs, "Time")));
		strcat(s,"\n");
		HAL_UART_Transmit(&huart3, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "Time");
		memset(TimeString,0,strlen(TimeString));
		CLEARFLAG(fJS,FLAG_JSON_PACK_TIME);
	}
	if(CHECKFLAG(fJS,FLAG_JSON_GET_VAN_VALUE)){
		cJSON_AddNumberToObject(cjs, "VanValue", hc595.data);
		char s[50];
		strcpy(s,"VanValue:");
		strcat(s,cJSON_Print(cJSON_GetObjectItemCaseSensitive(cjs, "VanValue")));
		strcat(s,"\n");
		HAL_UART_Transmit(&huart3, (uint8_t*)s, strlen(s), HAL_MAX_DELAY);
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "VanValue");
		CLEARFLAG(fJS,FLAG_JSON_GET_VAN_VALUE);
	}
}



void UnpackMessage(cJSON *cjs)
{
	if(!CHECKFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE)) return;
	CLEARFLAG(fJS,FLAG_JSON_UNPACK_MESSAGE);
	if(cJSON_HasObjectItem(cjs,"GPressure")) {
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "GPressure");
		SETFLAG(f1,FLAG_GET_PRESSURE);
	}
	if(cJSON_HasObjectItem(cjs,"SVan")) {
		SetVan = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(cjs, "SVan"));
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "SVan");
		SETFLAG(f1,FLAG_SET_VAN);
	}
	if(cJSON_HasObjectItem(cjs,"CVan")) {
		ClearVan = cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(cjs, "CVan"));
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "CVan");
		SETFLAG(f1,FLAG_CLEAR_VAN);
	}
	if(cJSON_HasObjectItem(cjs,"TrigVan")){
		if(cJSON_IsTrue((cJSON_GetObjectItemCaseSensitive(cjs, "TrigVan")))) SETFLAG(f1,FLAG_TRIG_VAN);
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "TrigVan");
	}
	if(cJSON_HasObjectItem(cjs,"STime")) {
		strcpy(TimeString,cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(cjs, "STime")));
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "STime");
		SETFLAG(f1,FLAG_SET_TIME);
	}
	if(cJSON_HasObjectItem(cjs,"GTime")) {
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "GTime");
		SETFLAG(f1,FLAG_GET_TIME);
	}
	if(cJSON_HasObjectItem(cjs,"VanValue")) {
		cJSON_DeleteItemFromObjectCaseSensitive(cjs, "VanValue");
		SETFLAG(fJS,FLAG_JSON_GET_VAN_VALUE);
	}
}

void PerformTrigVanSequence()
{

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
	HC595_AssignPin(&hc595, GPIOA, GPIO_PIN_5, HC595_CLK);
	HC595_AssignPin(&hc595, GPIOA, GPIO_PIN_7, HC595_DS);
	HC595_AssignPin(&hc595, GPIOB, GPIO_PIN_0, HC595_LATCH);
	HC595_AssignPin(&hc595, GPIOA, GPIO_PIN_12,HC595_OE);
	HC595_Enable();
}

void hc165_SetUp(){
	HC165_AssignPin(&hc165, GPIOA, GPIO_PIN_11, HC165_PL);
	HC165_AssignPin(&hc165, GPIOB, GPIO_PIN_3, HC165_DATA);
	HC165_AssignPin(&hc165, GPIOA, GPIO_PIN_12, HC165_CE);
	HC165_AssignPin(&hc165, GPIOB, GPIO_PIN_4, HC165_CP);
}

void SetUp()
{
	AMS5915_Init(&ams,&hi2c1);
	PCF8563_Init(&pcfHandle, &hi2c1);
	PCF8563_StartClock();
	UartIdle_Init();
	hc165_SetUp();
	hc595_SetUp();
}

//void UpdateStatusVan(){
//	if(CHECKFLAG(f1, FLAG_SET_VAN)){
//		hc595.data |= (0x01 << SetVan);
//	}
//	if(CHECKFLAG(f1,FLAG_CLEAR_VAN)){
//		hc595.data &= ClearVan;
//	}
//}
//HAL_StatusTypeDef ControlVanAndCheck(){
//
//
//	return HAL_OK;
//}

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
