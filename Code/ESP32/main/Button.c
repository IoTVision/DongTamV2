
#include <stdio.h>
#include "main.h"
#include "driver/dac.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "LCD_I2C.h"
#include "GUI.h"
#include "Text_GUI.h"
static const char *TAG_1= "Button";

// extern uint16_t Sort_para[3][3];
extern parameter_UI param[3];
extern Run_menu run_menu[3];

#define BT1 GPIO_NUM_35
#define BT2 GPIO_NUM_34
#define BT3 GPIO_NUM_39
#define BT4 GPIO_NUM_36

#define BT1_MASK (1ULL<<GPIO_NUM_35)
#define BT2_MASK (1ULL<<GPIO_NUM_34)
#define BT3_MASK (1ULL<<GPIO_NUM_39)
#define BT4_MASK (1ULL<<GPIO_NUM_36)

gpio_num_t BT[4]={BT1,BT2,BT3,BT4};
uint16_t Count[4]={0};
char str[20];
uint8_t pRow;
bool flag_print;
void Print_Setting_Menu(void);
void Print_Run_Menu(void);
void limit_value(void);
void Scan_button (void *pvParameters)
{
    while (1)
    {
        for (int i = 0; i<4; i++){
            if (gpio_get_level(BT[i])==0)
            {
                vTaskDelay(200/portTICK_PERIOD_MS);
                flag_print = 1;
                if (gpio_get_level(BT[i])==1)
                {
                    Count[i]++;  
                    if (Count[3]>1) Count[3]=0;
                    if (Count[2]>1) Count[2]=0;
                    if (i == 3)
                    {
                        Count[2] =0;
                        pRow = 0;
                    }  
                    if (Count[3]==1)
                    {
                        if (Count[2]==0)
                        {
                            if (i==0)
                            {
                                pRow++;
                                if (pRow > 2) pRow = 2; 
                            }
                            if (i==1)
                            {
                                pRow--;
                                if (pRow == 255) pRow = 0; 
                            }
                        }
                        if (Count[2]==1)
                        {
                            if (i == 1)
                            {
                                param[pRow].param+=100;
                            }
                            if (i == 0)
                            {
                                param[pRow].param-=100;
                            };
                        }
                        Print_Setting_Menu();   
                    }
                    if (Count[3]==0)
                    {
                        // Count[2]=0;
                        limit_value();
                        if (Count[2]==0)
                        {
                            if (i==0)
                            {
                                pRow++;
                                if (pRow > 2) pRow = 2; 
                            }
                            if (i==1)
                            {
                                pRow--;
                                if (pRow == 255) pRow = 0; 
                            }
                        }
                        if (Count[2]==1)
                        {
                            if (i == 1)
                            {
                                run_menu[pRow].value++;
                            }
                            if (i == 0)
                            {
                                run_menu[pRow].value--;
                            };
                        }
                        Print_Run_Menu();
                    }  
                           
                }
            }
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void Print_Setting_Menu(void)
{
    if (flag_print == 1)
    {
        LCDI2C_Clear();
        for (int i = 0; i<3; i++)
        {
            LCDI2C_Print(param[i].text_on_screen,1,i);
            sprintf(str,"%d",param[i].param);
            LCDI2C_Print(str,14,i);
            if (Count[2]==0)
            {
                LCDI2C_Print(" ",0,i);
                LCDI2C_Print(P_user,0,pRow);
                LCDI2C_Print(" ",13,pRow);
            }
            if (Count[2]==1)
            {
                LCDI2C_Print(" ",0,i);
                LCDI2C_Print(P_user,13,pRow);
            }
        }
        ESP_LOGE(TAG_1,"        ");
        ESP_LOGE(TAG_1,"        ");
        
        // LCDI2C_Print("-",0,Temp);
        for (int i = 0; i<4 ; i++){
            ESP_LOGI(TAG_1,"số lần nhấn nut %d %d",i,Count[i]);
        }
        ESP_LOGE(TAG_1,"pRow     %d", pRow);
        flag_print = 0;
    }
}

void Print_Run_Menu(void)
{
    if (flag_print == 1)
    {
        LCDI2C_Clear();
        // LCDI2C_Print(runmode,1,0);
        // LCDI2C_Print(testmode,1,1);
        for (int i = 0; i<3; i++)
        {
            LCDI2C_Print(run_menu[i].text_on_screen,1,i);
            sprintf(str,"%d",run_menu[i].value);
            LCDI2C_Print(str,14,i);
            if (Count[2]==0)
            {
                LCDI2C_Print(" ",0,i);
                LCDI2C_Print(P_user,0,pRow);
                LCDI2C_Print(" ",13,pRow);
            }
            if (Count[2]==1)
            {
                LCDI2C_Print(" ",0,i);
                LCDI2C_Print(P_user,13,pRow);
            }
        }
        ESP_LOGE(TAG_1,"        ");
        ESP_LOGE(TAG_1,"        ");
        for (int i = 0; i<4 ; i++){
            ESP_LOGI(TAG_1,"số lần nhấn nut %d %d",i,Count[i]);
        }
        flag_print = 0;
    }
}

void limit_value(void)
{
    if (run_menu[0].value > 16) run_menu[0].value=16;
    if (run_menu[0].value < 0) run_menu[0].value=0;
}
// void test_4_BT(void){
//     for (int i = 0; i<4; i++){
//             if (gpio_get_level(BT[i])==0)
//             {
//                 vTaskDelay(100/portTICK_PERIOD_MS);
//                 flag_print = 1;
//                 if (gpio_get_level(BT[i])==1)
//                 {
//                     Count[i]++;
//                 }
//                 Print_Count();
//             }
//         }
// }

