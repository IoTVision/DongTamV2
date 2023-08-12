#ifndef _ONLINE_STATUS_EVENT_H
#define _ONLINE_STATUS_EVENT_H
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
typedef enum{
    ONL_EVT_WIFI_CONNECTED,
    ONL_EVT_WIFI_FAIL,
    ONL_EVT_PING_SUCCESS,
    ONL_EVT_PING_TIMEOUT,
    EVT_END, // not use
}OnlineEventBit;

esp_err_t OnlEvt_SetBit(OnlineEventBit bit);
esp_err_t OnlEvt_ClearBit(OnlineEventBit bit);
bool OnlEvt_WaitBit(OnlineEventBit bit, BaseType_t ClearBit, BaseType_t WaitAll, TickType_t wait);
void OnlEvt_CreateEventGroup();
bool OnlEvt_CheckBit(OnlineEventBit bit);
#endif