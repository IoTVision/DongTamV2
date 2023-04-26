/*
 * FlagDefinition.h
 *
 *  Created on: Apr 26, 2023
 *      Author: KHOA
 */

#ifndef INC_FLAGDEFINITION_H_
#define INC_FLAGDEFINITION_H_


#define FLAG_RTC_SET_TIME (1<<RTC_SET_TIME)
#define FLAG_RTC_GET_TIME (1<<RTC_GET_TIME)
#define FLAG_RTC_START_CLOCK (1<<RTC_START_CLOCK)
#define FLAG_RTC_STOP_CLOCK (1<<RTC_STOP_CLOCK)

#define FLAG_VALVE_ON (1<<VALVE_ON)
#define FLAG_VALVE_OFF (1 << VALVE_OFF)

#define FLAG_GET_PRESSURE_VALUE (1<<GET_PRESSURE_VALUE)
#define FLAG_SET_PRESSURE_THRESHOLD (1<<SET_PRESSURE_THRESHOLD)

#define FLAG_UART_RECEIVE_DATA (1<<UART_RECEIVE_DATA)

typedef enum RTC_CMD{
	RTC_SET_TIME,
	RTC_GET_TIME,
	RTC_START_CLOCK,
	RTC_STOP_CLOCK,
}RTC_CMD;

typedef enum RTC_CMD{
	VALVE_ON,
	VALVE_OFF,
}VALVE;

typedef enum Pressure{
	GET_PRESSURE_VALUE,
	SET_PRESSURE_THRESHOLD,
}Pressure;

typedef enum UARTMessage{
	UART_RECEIVE_DATA,
}UARTMessage;

typedef struct RTC_DataStructure{

}RTC_DataStructure;

typedef struct Valve_DataStructure{
	uint16_t OutputBit;

}Valve_DataStructure;

typedef struct UART_DataStructure{

}Valve_DataStructure;

#endif /* INC_FLAGDEFINITION_H_ */
