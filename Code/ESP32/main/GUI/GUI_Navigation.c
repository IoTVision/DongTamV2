#include "GUI/GUI_Navigation.h"
#include "GUI.h"



/**
 * @brief Khởi tạo giá trị ban đầu cho đối tượng điều hướng trên màn hình
 * 
 */
GUI_NAV guiNav = {
    .pX = 0,
    .pY = 0,
    .page = PAGE_SETTING,
    .param = INDEX_TOTAL_VAN,
    .pNow = IS_KEYWORD,
};

static inline void NextPage(){
    guiNav.page++;
    if(guiNav.page == PAGE_END) guiNav.page = PAGE_START + 1;
}

/**
 * @brief Trỏ tới thông số tiếp theo trong bảng, cuộn xuống thông số khi con trỏ 
 * nằm ở hàng dưới cùng
 * 
 */
static inline void PointToNextParam(){
    xEventGroupClearBits(evgGUI,EVT_PARAM_SCROLL_UP);
    ParamIndex paramNO = GUINAV_GetOrderToDisplayIndex();
    int8_t pY = (int8_t) GUINAV_GetPointerPosY();
    paramNO ++;
    if(paramNO == INDEX_END_PARAM) return;
    guiNav.param = paramNO;  
    // if param reach the end of the list, keep the same value
    // not roll back pointer, keep it at the latest row
    if(pY == LCD_ROWS - 1) {
        xEventGroupSetBits(evgGUI,EVT_PARAM_SCROLL_DOWN);
        return;
    } else pY++;
    guiNav.pY = (uint8_t)pY;
}

/**
 * @brief Trỏ tới thông số trước đó trong bảng, cuộn lên thông số khi con trỏ 
 * nằm ở hàng trên cùng
 * 
 */
static inline void PointToPrevParam(){
    xEventGroupClearBits(evgGUI,EVT_PARAM_SCROLL_DOWN);
    uint8_t paramNO = GUINAV_GetOrderToDisplayIndex();
    int pY = (int) GUINAV_GetPointerPosY();
    paramNO --;
    if(paramNO == INDEX_START_PARAM) return;
    guiNav.param = paramNO;
    // if param reach the start of the list, keep the same value
    if(!pY) {
        xEventGroupSetBits(evgGUI,EVT_PARAM_SCROLL_UP);
        return;
    } else pY--;
    guiNav.pY = (uint8_t)pY;
}

static inline void SetPointerNowIsKeyword(){
    guiNav.pNow = IS_KEYWORD;
    guiNav.pX = 0;
}
static inline void SetPointerNowIsValue(){
    guiNav.pNow = IS_VALUE;
    guiNav.pX = LENGTH_OF_PARAM + 1;
}
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


/**
 * @brief Xử lý sự kiện từ nút nhấn nhận được ở TaskScanButton
 * 
 * @param eventToHandle tên của sự kiện nút nhấn nhận được cần xử lý
 * @param eventName danh sách các sự kiện nút nhấn có thể được xử lý bởi hàm này quy định trong GUI.h
 * @param fKEY hàm xử lý sự kiện nếu con trỏ đang trỏ tới tên thông số(key)
 * @param fVALUE hàm xử lý sự kiện nếu con trỏ đang trỏ tới giá trị thông số(value)
 */
void HandleEvent(EventBits_t eventToHandle, EventBits_t eventName,void(*fKEY)(),void(*fVALUE)()){
    if(eventToHandle == eventName){
        //function pointer to handle pNow
        if(guiNav.pNow == IS_KEYWORD) fKEY();
        else if(guiNav.pNow == IS_VALUE) fVALUE();
    }
}
/**
 * @brief Trả về sự kiện khi nhấn nút EventBits_t e
 * Sự kiện nhấn nút xảy ra ở TaskScanButton thuộc file LedButton.c, dùng TaskNotify thông báo cho GUITask và được chứa trong EventBits_t e
 * GUITask sẽ chờ sự kiện nhấn nút trong hàm xTaskNotifyWait, khi nhấn thì GUITask sẽ gọi tới hàm này để kiểm tra sự kiện và truyền 
 * tham số đầu vào là EventBits_t e nhận được từ TaskScanButton
 * 
 */
void GUINAV_GetEvent(EventBits_t e)
{
    HandleEvent(e,EVT_BTN_MENU,&NextPage,&SetPointerNowIsKeyword);
    HandleEvent(e,EVT_BTN_SET,&SetPointerNowIsValue,NULL);
    HandleEvent(e,EVT_BTN_UP,&PointToPrevParam,&IncreaseValue);
    HandleEvent(e,EVT_BTN_DOWN_RIGHT,&PointToNextParam,&DecreaseValue);
}


uint8_t GUINAV_GetCurrentSelected(){return guiNav.pNow;} 
uint8_t GUINAV_GetPage(){return guiNav.page;}
ParamIndex GUINAV_GetOrderToDisplayIndex(){return guiNav.param;} 
uint8_t GUINAV_GetPointerPosX(){return guiNav.pX;}
uint8_t GUINAV_GetPointerPosY(){return guiNav.pY;} 