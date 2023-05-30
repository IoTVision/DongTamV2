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
}GUI_Info;

typedef struct {
    char* text_on_screen;
    uint16_t Value;    
}Param_t;

#define PARAM_CODE_TEXT      "Param code:"
#define DP_LOW_TEXT          "DP-Low    :"
#define DP_HIGH_TEXT         "DP-High   :"
#define DP_ALARM_TEXT        "DP-Alarm  :"

#define PARAM_CODE_TEXT      "Param code:"
#define DP_LOW_TEXT          "DP-Low    :"
#define DP_HIGH_TEXT         "DP-High   :"
#define DP_ALARM_TEXT        "DP-High   :"
#define LENGTH_OF_PARAM      strlen(PARAM_CODE_TEXT)

#define LCD_COLS 20
#define LCD_ROWS 4

void LedErrorWrite(bool ledState);
void LedStatusWrite(bool ledState);
void ReadGuiButton(gpio_num_t gpio, EventBits_t e);
void GUITask(void *pvParameter);
void TaskScanButton(void *pvParameter);
void GuiInit();
void GuiTestFull();

#endif