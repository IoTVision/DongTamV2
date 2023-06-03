#include "GUI/GUI_Navigation.h"


// #define USE_LOAD_NEW_SCREEN 

/*
#if defined(USE_LOAD_NEW_SCREEN) 
#elif defined(USE_SCROLL_SCREEN) 
#endif
*/

GUI_NAV guiNav = {
    .pX = 0,
    .pY = 0,
    .page = PAGE_SETTING,
    .param = NO_PARAM_CODE,
    .pNow = IS_KEYWORD,
};

static inline void NextPage(){
    guiNav.page++;
    if(guiNav.page == PAGE_END) guiNav.page = PAGE_START + 1;
}
static inline void PointToNextParam(){
    xEventGroupClearBits(evgGUI,EVT_PARAM_SCOLL_UP);
    uint8_t paramNO = GUINAV_GetParamNum();
    int8_t pY = (int8_t) GUINAV_GetPointerPosY();
    paramNO ++;
    pY++;
    #if defined(USE_LOAD_NEW_SCREEN) 
    // if param reach the last parameter, roll back to the first
    if(guiNav.param == NO_PARAM_END) guiNav.param = NO_PARAM_START + 1;
    if(guiNav.pY == LCD_ROWS) guiNav.pY = 0;
    #elif defined(USE_SCROLL_SCREEN) 
    // if param reach the end of the list, keep the same value
    if(paramNO == SETTING_NO_PARAM_END) paramNO = SETTING_NO_PARAM_END - 1;
    // not roll back pointer, keep it at the latest row
    if(pY > LCD_ROWS - 1) {
        pY = LCD_ROWS - 1;
        xEventGroupSetBits(evgGUI,EVT_PARAM_SCOLL_DOWN);
        
    }
    guiNav.param = paramNO; 
    guiNav.pY = (uint8_t)pY;
    #endif
}
static inline void PointToPrevParam(){
    xEventGroupClearBits(evgGUI,EVT_PARAM_SCOLL_DOWN);
    uint8_t paramNO = GUINAV_GetParamNum();
    int pY = (int) GUINAV_GetPointerPosY();
    paramNO --;
    pY--;
    #if defined(USE_LOAD_NEW_SCREEN) 
    // if param reach the the first, roll back to last
    if(guiNav.param == NO_PARAM_START) guiNav.param = NO_PARAM_END - 1;
    // because uint8_t data type so pY will roll back to 255, set it to the last row
    if(guiNav.pY > (LCD_ROWS+1)) guiNav.pY = LCD_ROWS - 1;
    #elif defined(USE_SCROLL_SCREEN) 
    // if param reach the start of the list, keep the same value
    if(paramNO == SETTING_NO_PARAM_START) paramNO = SETTING_NO_PARAM_START + 1;
    // not roll back pointer, keep it at the 0 row
    if(pY < 0) {
        pY = 0;
        xEventGroupSetBits(evgGUI,EVT_PARAM_SCOLL_UP);
        
    }
    guiNav.param = paramNO;
    guiNav.pY = (uint8_t)pY;
    #endif
}
static inline void SetPointerNowIsKeyword(){
    guiNav.pNow = IS_KEYWORD;
    guiNav.pX = 0;
}
static inline void SetPointerNowIsValue(){
    guiNav.pNow = IS_VALUE;
    guiNav.pX = LENGTH_OF_PARAM + 1;
}
static inline void SaveValueToFlash(){ESP_LOGI("GUINAV","send save flash event");xEventGroupSetBits(evgGUI,EVT_SAVE_VALUE_TO_FLASH);}
static inline void GetValueFromFlash(){xEventGroupSetBits(evgGUI,EVT_GET_VALUE_FROM_FLASH);}
static inline void IncreaseValue(){
    EventBits_t e = xEventGroupGetBits(evgGUI);
    // if not happen event above threshold, bit EVT_INCREASE_VALUE will be set
    if(!CHECKFLAG(e,EVT_VALUE_ABOVE_THRESHOLD)) xEventGroupSetBits(evgGUI,EVT_INCREASE_VALUE);
}
static inline void DecreaseValue(){
    EventBits_t e = xEventGroupGetBits(evgGUI);
    // if not happen event below threshold, bit EVT_DECREASE_VALUE will be set
    if(!CHECKFLAG(e,EVT_VALUE_BELOW_THRESHOLD)) xEventGroupSetBits(evgGUI,EVT_DECREASE_VALUE);
}
static inline void DoNothing(){return;}

void HandleEvent(EventBits_t eventToHandle, EventBits_t eventName,void(*fKEY)(),void(*fVALUE)()){
    if(eventToHandle == eventName){
        //function pointer to handle pNow
        if(guiNav.pNow == IS_KEYWORD) fKEY();
        else if(guiNav.pNow == IS_VALUE) fVALUE();
    }
}
/**
 * @brief Get event from button
 * 
 */
void GUINAV_GetEvent(EventBits_t e)
{
    HandleEvent(e,EVT_BTN_MENU,&NextPage,&SetPointerNowIsKeyword);
    HandleEvent(e,EVT_BTN_SET,&SetPointerNowIsValue,&SaveValueToFlash);
    HandleEvent(e,EVT_BTN_UP,&PointToPrevParam,&IncreaseValue);
    HandleEvent(e,EVT_BTN_DOWN_RIGHT,&PointToNextParam,&DecreaseValue);
}


/**
 * @brief indicate that current selected pNow is KEYWORD or VALUE
 * 
 * @return uint8_t IS_KEYWORD or IS_VALUE
 */

uint8_t GUINAV_GetCurrentSelected(){return guiNav.pNow;}
uint8_t GUINAV_GetPage(){return guiNav.page;}
uint8_t GUINAV_GetParamNum(){return guiNav.param;}
uint8_t GUINAV_GetPointerPosX(){return guiNav.pX;}
uint8_t GUINAV_GetPointerPosY(){return guiNav.pY;}