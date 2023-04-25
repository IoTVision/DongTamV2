/*
 * DS3231_Handle.c
 *
 *  Created on: Apr 25, 2023
 *      Author: KHOA
 */

#include "string.h"
#include "main.h"
#include "Flag.h"
#include "DS3231.h"

uint16_t dsFlag;
extern DS3231_Time_t dsTime;

void SetTimeDS3231()
{
	if(CHECKFLAG(dsFlag,FLAG_DS3231_SET_TIME)){
		DS3231_SetTime(dsTime);
	}
}

void GetDS3231TimeAndPacketData()
{

}



