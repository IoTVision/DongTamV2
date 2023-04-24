
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
#include "Text_GUI.h"
//  static const char *TAG= "main";
LCDI2C lcdI2C;
#define HC595_OE_MASK (1ULL<<GPIO_NUM_4)
#define HC595_LATCH_MASK (1ULL<<GPIO_NUM_5)
#define HC595_CLK_MASK (1ULL<<GPIO_NUM_18)
#define HC595_DS_MASK (1ULL<<GPIO_NUM_23)


HC595 hc595_pi;//74HC595 for Pressure Indicato
parameter_UI param[3];
Run_menu run_menu[3];
char test[6]="Hello";
void HC595_ConfigPin();
void Button_ConfigPin();
// void gan_chuoi(parameter_UI a, char *l);
// void sort_parameter();

// uint16_t Sort_para[3][3];

void app_main(void)
{
    
    param[0].para_no = 1;
    strcpy(param[0].text_on_screen ,param1);
    param[0].param = 242;

    param[1].para_no = 2;
    strcpy(param[1].text_on_screen ,param2);
    param[1].param = 700;

    param[2].para_no = 3;
    strcpy(param[2].text_on_screen ,param3);
    param[2].param = 1300;
    
    strcpy(run_menu[0].text_on_screen, valve);
    strcpy(run_menu[1].text_on_screen, testmode);
    strcpy(run_menu[2].text_on_screen, run);
    LCD_init();
    // sort_parameter();
    HC595_ConfigPin();
    Button_ConfigPin();
    //PressureIndicator_Test();
    UARTConfig();
    xTaskCreate(UARTToSTM32_event_task, "UARTToSTM32_event_task", 2048, NULL, 4, NULL);
    xTaskCreate(log_uart_event_task, "log_uart_event_task", 2048, NULL, 4, NULL);
    xTaskCreate(Scan_button, "Button_event_task", 2048, NULL, 4, NULL);


    // LCDI2C_Print("SpiritBoi",0,0);
    // LCDI2C_Print("50",15,0);
    while (1) {
        vTaskDelay(2000/portTICK_PERIOD_MS);
        uart_write_bytes(UART_NUM_2,strcat(test,"500"),6);
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

// void sort_parameter(void){
//     for (int i = 0; i < 3 ; i++){
//         Sort_para[0][i] = param[i].fnc.ex_works_setting;
//         Sort_para[1][i] = param[i].fnc.max_range;
//         Sort_para[2][i] = param[i].fnc.min_range;
//     }
// }















