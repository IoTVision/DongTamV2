
#include <stdio.h>
#include "main.h"
#include "74HC595.h"
#include "PressureIndicator.c"
#include "UART.c"
#include "driver/dac.h"
#include "esp_err.h"
#include "GUI.c"
#include "LCD_I2C.h"
#include "Button.c"
#include "GUI.h"
 static const char *TAG= "main";

#define HC595_OE_MASK (1ULL<<GPIO_NUM_4)
#define HC595_LATCH_MASK (1ULL<<GPIO_NUM_5)
#define HC595_CLK_MASK (1ULL<<GPIO_NUM_18)
#define HC595_DS_MASK (1ULL<<GPIO_NUM_23)


HC595 hc595_pi;//74HC595 for Pressure Indicator

// GUI_function parameter_code = {
//     .ex_works_setting = 242,
//     .max_range = 0,
//     .max_range = 0,
// };

// parameter_UI param_1 = {
//     .para_no = 1,
//     .text_on_screen = "Parameter code",
//     .fnc = &parameter_code,
// };


// GUI_function Delta_low = {
//     .ex_works_setting = 700,
//     .max_range = 250,
//     .max_range = 4000,
// };

// parameter_UI DP_low = {
//     .para_no = 2,
//     .text_on_screen = "DP-Low",
//     .fnc = &Delta_low,
// };

// GUI_function Delta_high = {
//     .ex_works_setting = 1300,
//     .max_range = 250,
//     .max_range = 4000,
// };

// parameter_UI DP_high = {
//     .para_no = 3,
//     .text_on_screen = "DP-High",
//     .fnc = &Delta_high,
// };

parameter_UI param[3];





void HC595_ConfigPin();
void Button_ConfigPin();
void gan_chuoi(parameter_UI a, char *l);
void sort_parameter();

uint16_t Sort_para[3][3];

void app_main(void)
{
   

    param[0].para_no = 1;
// gan_chuoi(param[0],"Parameter code");
    param[0].fnc.ex_works_setting = 242;
    param[0].fnc.max_range = 0;
    param[0].fnc.min_range = 0;
    // ESP_LOGE(TAG,"para no %d",param[0].para_no);
    // ESP_LOGE(TAG,"text on screen  %s",param[0].text_on_screen);
    // ESP_LOGE(TAG,"ex works %d",param[0].fnc.ex_works_setting);
    // ESP_LOGE(TAG,"max range %d",param[0].fnc.max_range);
    // ESP_LOGE(TAG,"min range %d",param[0].fnc.min_range);

    param[1].para_no = 2;
    // gan_chuoi(param[0],"Parameter code");
    param[1].fnc.ex_works_setting = 700;
    param[1].fnc.max_range = 4000;
    param[1].fnc.min_range = 250;


    param[2].para_no = 3;
    // gan_chuoi(param[0],"Parameter code");
    param[2].fnc.ex_works_setting = 1300;
    param[2].fnc.max_range = 4000;
    param[2].fnc.min_range = 250;
    
    sort_parameter();
    HC595_ConfigPin();
    Button_ConfigPin();
    //PressureIndicator_Test();
    UARTConfig();
    xTaskCreate(UARTToSTM32_event_task, "UARTToSTM32_event_task", 2048, NULL, 4, NULL);
    xTaskCreate(log_uart_event_task, "log_uart_event_task", 2048, NULL, 4, NULL);
    xTaskCreate(Scan_button, "Button_event_task", 2048, NULL, 4, NULL);
    while (1) {
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}


void Button_ConfigPin(){
    gpio_config_t cfg = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BT1_MASK |
                        BT2_MASK |
                        BT3_MASK|
                        BT4_MASK,
    };
    gpio_config(&cfg);
}

void HC595_ConfigPin()
{
    gpio_config_t cfg = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = HC595_DS_MASK |
                        HC595_OE_MASK |
                        HC595_CLK_MASK|
                        HC595_LATCH_MASK,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&cfg);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_5,HC595_LATCH);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_4,HC595_OE);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_18,HC595_CLK);
    HC595_AssignPin(&hc595_pi,GPIO_NUM_23,HC595_DS);
    HC595_SetTarget(&hc595_pi);
    HC595_Enable();
}

// void gan_chuoi(parameter_UI a, char *l){
//     for (int i = 0; i< sizeof(l);i++){
//         a.text_on_screen[i] = l[i];
//     }   
// }

void sort_parameter(void){
    for (int i = 0; i < 3 ; i++){
        Sort_para[0][i] = param[i].fnc.ex_works_setting;
        Sort_para[1][i] = param[i].fnc.max_range;
        Sort_para[2][i] = param[i].fnc.min_range;
    }
}















