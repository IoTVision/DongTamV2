
#include <stdio.h>
#include "main.h"
#include "driver/dac.h"
#include "esp_err.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG_1= "Button";

extern uint16_t Sort_para[3][3];

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
uint8_t Temp;
bool flag_print;
void Print_Count(void);
void Scan_button (void *pvParameters)
{
    while (1)
    {
        for (int i = 0; i<4; i++){
            if (gpio_get_level(BT[i])==0)
            {
                vTaskDelay(100/portTICK_PERIOD_MS);
                flag_print = 1;
                if (gpio_get_level(BT[i])==1)
                {
                    Count[i]++;  
                    if (Count[3]>2) Count[3]=0;
                    if (Count[2]>1) Count[2]=0;    
                    if (Count[2]==0)
                    {
                        if (i==3)
                        {
                            Temp++;
                            if (Temp > 2) Temp = 0; 
                        }
                    }
                    if (i == 2){
                        Count[3]=0;
                    }
                    if (Count[2]==1)
                    {
                        switch (Count[3])
                            {
                            case 0:
                                
                                if (i == 1)
                                {
                                    Sort_para[0][Temp]++;
                                    
                                }
                                if (i == 0)
                                {
                                    Sort_para[0][Temp]--;
                                };
                                break;
                            case 1:
                                
                                if (i == 1)
                                {
                                    Sort_para[1][Temp]++;
                                    
                                }
                                if (i == 0)
                                {
                                    Sort_para[1][Temp]--;
                                };
                                break;
                            case 2:         
                                if (i == 1)
                                {
                                    Sort_para[2][Temp]++;
                                    
                                }
                                if (i == 0)
                                {
                                    Sort_para[2][Temp]--;
                                };
                                break;
                            } 
                    }           
                }
                
                Print_Count();
                
            }
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void Print_Count(void)
{
    if (flag_print == 1)
    {
        // for (int i = 0; i<4 ; i++){
        //     ESP_LOGI(TAG_1,"số lần nhấn nut %d %d",i+1,Count[i]);
        // //     ESP_LOGE(TAG_1,"para no %d",param[i].para_no);
        // //     ESP_LOGE(TAG_1,"max range %d",param[i].fnc.max_range);
        // //     ESP_LOGE(TAG_1,"min range %d",param[i].fnc.min_range);
        // }
        for (int i = 0; i<3; i++)
        {
            ESP_LOGE(TAG_1,"\t %d \t %d \t %d \t",Sort_para[0][i],Sort_para[1][i],Sort_para[2][i]);
            
        }
        ESP_LOGE(TAG_1,"        ");
        ESP_LOGE(TAG_1,"        ");
        for (int i = 0; i<4 ; i++){
            ESP_LOGI(TAG_1,"số lần nhấn nut %d %d",i,Count[i]);
        //     ESP_LOGE(TAG_1,"para no %d",param[i].para_no);
        //     ESP_LOGE(TAG_1,"max range %d",param[i].fnc.max_range);
        //     ESP_LOGE(TAG_1,"min range %d",param[i].fnc.min_range);
        }
        ESP_LOGE(TAG_1,"Temp     %d", Temp);
        flag_print = 0;
    }
}
void test_4_BT(void){
    for (int i = 0; i<4; i++){
            if (gpio_get_level(BT[i])==0)
            {
                vTaskDelay(100/portTICK_PERIOD_MS);
                flag_print = 1;
                if (gpio_get_level(BT[i])==1)
                {
                    Count[i]++;
                }
                Print_Count();
            }
        }
}

