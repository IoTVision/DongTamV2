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

// typedef struct {
//     uint16_t ex_works_setting;
//     uint16_t max_range;
//     uint16_t min_range;
//     uint8_t unit;
// }GUI_function;



typedef struct {
    uint8_t para_no;
    char text_on_screen[20];
    uint16_t param;    
}parameter_UI;

typedef struct {
    char text_on_screen[20];
    uint8_t value;
}Run_menu;

void GUITask(void *pvParameter);
void GuiSetup();

#endif