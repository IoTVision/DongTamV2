/*
 * sdkconfig.h
 *
 *  Created on: Mar 30, 2023
 *      Author: SpiritBoi
 */

#ifndef _SDKCONFIG_H_
#define _SDKCONFIG_H_

//#define CONFIG_USE_FLAG
//#define CONFIG_USE_STRING_HANDLE
#define CONFIG_USE_74HC595
#define CONFIG_USE_AMS5915

#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#else
#error "You're not define library use, use macro above to include proper library"
#endif
#endif
