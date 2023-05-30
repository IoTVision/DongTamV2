#include "GUI/GUI_Navigation.h"

GUI_NAV guiNav = {
    .pX = 0,
    .pY = 0,
    .page = PAGE_DEFAULT,
    .param = NO_PARAM_CODE,
    .pNow = IS_KEYWORD,
};

static inline void NextPage(){
    guiNav.page++;
    if(guiNav.page == PAGE_END) guiNav.page = PAGE_DEFAULT;
}
static inline void PointToNextParam(){
    guiNav.param++;
    guiNav.pY++;
    // if param reach the last parameter, roll back to the first
    if(guiNav.param == NO_PARAM_END) guiNav.param = NO_PARAM_START + 1;
    if(guiNav.pY == LCD_ROWS) guiNav.pY = 0;
}
static inline void PointToPrevParam(){
    guiNav.param--;
    guiNav.pY--;
    // if param reach the the first, roll back to last
    if(guiNav.param == NO_PARAM_START) guiNav.param = NO_PARAM_END - 1;
    // because uint8_t data type so pY will roll back to 255
    if(guiNav.pY > (LCD_ROWS+1)) guiNav.pY = LCD_ROWS - 1;
}
static inline void SetPointerNowIsKeyword(){
    guiNav.pNow = IS_KEYWORD;
    guiNav.pX = 0;
}
static inline void SetPointerNowIsValue(){
    guiNav.pNow = IS_VALUE;
    guiNav.pX = LENGTH_OF_PARAM + 1;
}
static inline void SaveValueToFlash(){xEventGroupSetBits(evgGUI,EVT_SAVE_VALUE_TO_FLASH);}
static inline void GetValueFromFlash(){xEventGroupSetBits(evgGUI,EVT_GET_VALUE_FROM_FLASH);}
static inline void IncreaseValue(){
    EventBits_t e = xEventGroupGetBits(evgGUI);
    if(e != EVT_VALUE_ABOVE_THRESHOLD) guiNav.value++;
}
static inline void DecreaseValue(){
    EventBits_t e = xEventGroupGetBits(evgGUI);
    if(e != EVT_VALUE_BELOW_THRESHOLD) guiNav.value--;
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
    // if param come to an end, turn back to the first param
    if(guiNav.param == NO_PARAM_END) guiNav.param = NO_PARAM_CODE;
}


/**
 * @brief indicate that current selected pNow is KEYWORD or VALUE
 * 
 * @return uint8_t IS_KEYWORD or IS_VALUE
 */
uint8_t GUINAV_GetCurrentSelected(){return guiNav.pNow;}
uint8_t GUINAV_GetValue(){return guiNav.value;}
uint8_t GUINAV_GetPage(){return guiNav.page;}
uint8_t GUINAV_GetParam(){return guiNav.param;}
uint8_t GUINAV_GetPointerPosX(){return guiNav.pX;}
uint8_t GUINAV_GetPointerPosY(){return guiNav.pY;}