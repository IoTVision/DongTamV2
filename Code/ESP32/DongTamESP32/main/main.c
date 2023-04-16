
#include <stdio.h>
#include "main.h"
#include "74HC595.h"
#include "PressureIndicator.c"
#include "UARTToSTM32.c"
#include "driver/dac.h"
static const char *TAG= "main";
void app_main(void)
{
    UARTConfig();
    PI_ConfigPin();
    char* test_str = "This is a test string.\n";
    uart_write_bytes(UART_NUM_2, (const char*)test_str, strlen(test_str));
    while (1) {
    // PI_TestShowLevel_Increase();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
