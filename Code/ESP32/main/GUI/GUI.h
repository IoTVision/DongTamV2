#ifndef __GUI_H__
#define __GUI_H__

#include "esp_err.h"
#include "i2cdev.h"
#include "LCD_I2C.h"
#include "esp_check.h"
#include "PressureIndicator.h"
#include "74HC595.h"
#include "GUI.h"
#include "freertos/event_groups.h"
#include "main.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LED_ERROR_MASK 10
#define LED_STATUS_MASK 11

#define EVT_BTN_MENU (1<<0)
#define EVT_BTN_SET (1<<1)
#define EVT_BTN_UP (1<<2)
#define EVT_BTN_DOWN_RIGHT (1<<3)

#define EVT_SAVE_VALUE_TO_FLASH (1<<4)
#define EVT_GET_VALUE_FROM_FLASH (1<<5)
#define EVT_VALUE_ABOVE_THRESHOLD (1<<6)
#define EVT_VALUE_BELOW_THRESHOLD (1<<7)
#define EVT_INCREASE_VALUE (1<<8)
#define EVT_DECREASE_VALUE (1<<9)
#define EVT_PARAM_SCOLL_UP (1<<10)
#define EVT_PARAM_SCOLL_DOWN (1<<1)


#define BTN_MENU GPIO_NUM_36
#define BTN_SET GPIO_NUM_39
#define BTN_UP GPIO_NUM_34
#define BTN_DOWN_RIGHT GPIO_NUM_35

#define BT1_MASK (1ULL<<GPIO_NUM_36)
#define BT2_MASK (1ULL<<GPIO_NUM_39)
#define BT3_MASK (1ULL<<GPIO_NUM_34)
#define BT4_MASK (1ULL<<GPIO_NUM_35)

typedef struct GUI_Info {
    uint16_t dpHigh; // in Pa
    uint16_t dpLow; // in Pa
    uint16_t dpAlarm; // in Pa
    uint16_t pulseTime; // in second
    uint16_t intervalTime; // in second
    uint16_t cycleIntervalTime; // in second
    uint16_t totalVan; // max 16
    uint16_t odcDownTimeCycle;
    uint16_t odcHigh;
    uint16_t odcLow;
    uint16_t operateHours;
    uint16_t odcCleanMode;
    uint16_t serviceRunHoursAlarm;
    bool dpMode;
}GUI_Info;

typedef struct {
    char* text_on_screen;
    char* unit;
    uint16_t Value;    
    uint16_t lowLimit;
    uint16_t highLimit;
    uint16_t scaleValue; // multiply with GUI_NAV.value to get actual result
}Param_t;

#define TEXT_PARAM_CODE      "Param code:"
#define TEXT_DP_LOW          "DP-Low    :"
#define TEXT_DP_HIGH         "DP-High   :"
#define TEXT_DP_ALARM        "DP-Alarm  :"
#define TEXT_CYCLE_TIME      "Cycle Time:"
#define TEXT_INTERVAL_TIME   "Inter Time:"
#define TEXT_PULSE_TIME      "Pulse Time:"
#define TEXT_TOTAL_VAN       "Total Van :"
#define TEXT_ODC_DOWN_CYC    "Down T Cyc:"
#define TEXT_ODC_HIGH        "OCD High  :"
#define TEXT_ODC_LOW         "OCD Low   :"
#define TEXT_OPERATE_H       "OperHours :"
#define TEXT_CLEAN_MODE      "Clean Mode:"
#define TEXT_SER_H_ALARM     "SerH Alarm:"
#define TEXT_DP_MODE         "DP Mode   :"
#define LENGTH_OF_PARAM      strlen(TEXT_PARAM_CODE)



#define LCD_COLS 20
#define LCD_ROWS 4

void LedErrorWrite(bool ledState);
void LedStatusWrite(bool ledState);
void ReadGuiButton(gpio_num_t gpio, EventBits_t e);
void GUITask(void *pvParameter);
void TaskScanButton(void *pvParameter);
void GuiInit();
void GuiTestFull();
void GUI_ClearPointer();
void GUI_ShowPointer();
void GUI_GetParam(Param_t *param, uint8_t paramNO);
void GUI_SetGuiInfoValue(GUI_Info *gi, uint8_t paramNO, uint32_t value);
void GUI_LoadPage();
#endif