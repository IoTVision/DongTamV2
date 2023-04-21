#ifndef GUI
#define GUI
#include "esp_err.h"
#include "i2cdev.h"
#include "LCD_I2C.h"
#include "esp_check.h"
#include "PressureIndicator.c"
#include "74HC595.h"
#include "GUI.h"
LCDI2C lcdI2C;

#define BTN_MODE GPIO_NUM_35
#define BTN_SET GPIO_NUM_34
#define BTN_UP GPIO_NUM_5
#define BTN_DOWN_RIGHT GPIO_NUM_4
#define LED_ERROR_MASK (1U << 14)
#define LED_STATUS_MASK (1U << 15)

esp_err_t LCD_init()
{
    esp_err_t err = ESP_OK;
    err = LCDI2C_Config(&lcdI2C);
    LCDI2C_TurnOnBackLight();
    LCDI2C_Print("SpiritBoi",0,0);
    return err;
}

void PressureIndicator_Test()
{
    PI_Test();
}

void Button_Test()
{

}
#endif