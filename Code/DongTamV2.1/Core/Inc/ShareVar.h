/*
 * ShareVar.h
 *
 *  Created on: May 22, 2023
 *      Author: SpiritBoi
 */

#ifndef INC_SHAREVAR_H_
#define INC_SHAREVAR_H_


typedef struct BoarParam{
	HC595 hc595;
	HC165 hc165;
	AMS5915 ams;
	cJSON *cjsCommon;
	PCF8563_Handle pcf;
	float pressure;
}BoarParam;


#endif /* INC_SHAREVAR_H_ */
