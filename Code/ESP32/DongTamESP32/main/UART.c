
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

QueueHandle_t qUART_STM32_event,qLOG_event;

#define UART_RX GPIO_NUM_16
#define UART_TX GPIO_NUM_17
#define RD_BUF_SIZE 10
// static const char *TAG_UART= "UART";

void UARTToSTM32_event_task(void *pvParameters)
{
    uart_event_t event;
    for(;;) {
        if(xQueueReceive(qUART_STM32_event, (void * )&event, (TickType_t)portMAX_DELAY)) {
            switch(event.type) {
                case UART_DATA:
                    char *dtmp = (char *) malloc(event.size + 1);
                    uart_read_bytes(UART_NUM_2, dtmp, event.size, portMAX_DELAY);
                    dtmp[event.size] = '\0';
                    uart_write_bytes(UART_NUM_0, dtmp, strlen(dtmp));
                    free(dtmp);
                    break;
                case UART_BREAK:
                    break;
                //Others
                default:
                    break;
            }
        }
    }
}

void log_uart_event_task(void *pvParameters){
    uart_event_t event;
    while(1){
        if(xQueueReceive(qLOG_event, (void * )&event, (TickType_t)portMAX_DELAY)) {
            switch(event.type) {
                case UART_DATA:{
                    char *dtmp = (char *) malloc(event.size + 1);
                    uart_read_bytes(UART_NUM_0, dtmp, event.size, portMAX_DELAY);
                    dtmp[event.size] = '\0';
                    uart_write_bytes(UART_NUM_2, dtmp, strlen(dtmp));
                    free(dtmp);
                }
				break;
                case UART_BREAK:
				break;
                default:
				break;
            }
        }
    }
}


void UARTConfig()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, 200, 200, 5, &qUART_STM32_event, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uart_driver_install(UART_NUM_0, 200, 200, 20, &qLOG_event, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    
}

