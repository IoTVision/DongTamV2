/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS485_CTR_Pin GPIO_PIN_1
#define RS485_CTR_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_2
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_3
#define RS485_RX_GPIO_Port GPIOA
#define _74HC595_SCK_Pin GPIO_PIN_5
#define _74HC595_SCK_GPIO_Port GPIOA
#define _74HC595_MOSI_Pin GPIO_PIN_7
#define _74HC595_MOSI_GPIO_Port GPIOA
#define _74HC595_STORE_Pin GPIO_PIN_0
#define _74HC595_STORE_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOB
#define MCP41010_CS_Pin GPIO_PIN_12
#define MCP41010_CS_GPIO_Port GPIOB
#define _74HC165_SCK_Pin GPIO_PIN_13
#define _74HC165_SCK_GPIO_Port GPIOB
#define _74HC165_MISO_Pin GPIO_PIN_14
#define _74HC165_MISO_GPIO_Port GPIOB
#define MCP41010_MOSI_Pin GPIO_PIN_15
#define MCP41010_MOSI_GPIO_Port GPIOB
#define _74HC165_LOAD_Pin GPIO_PIN_8
#define _74HC165_LOAD_GPIO_Port GPIOA
#define STM32_TX_Pin GPIO_PIN_9
#define STM32_TX_GPIO_Port GPIOA
#define STM32_RX_Pin GPIO_PIN_10
#define STM32_RX_GPIO_Port GPIOA
#define OE_Pin GPIO_PIN_12
#define OE_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
