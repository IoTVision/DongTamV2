
#include <stdio.h>
#include "main.h"
// #include "74HC595.h"
// #include "PressureIndicator.c"
#include "UART.c"
#include "driver/dac.h"
// static const char *TAG= "main";

void PressureIndicator_Init();

void app_main(void)
{
    UARTConfig();
    // PressureIndicator_Init();
    xTaskCreate(UARTToSTM32_event_task, "UARTToSTM32_event_task", 2048, NULL, 4, NULL);
    xTaskCreate(log_uart_event_task, "log_uart_event_task", 2048, NULL, 4, NULL);
    while (1) {
    vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void PressureIndicator_Init()
{
    // PI_ConfigPin();
    // for(uint8_t i=0;i<10;i++)PI_TestShowLevel_Decrease();
    // for(uint8_t i=0;i<10;i++)PI_TestShowLevel_Increase();
    // PI_SetLevel(0);
}














