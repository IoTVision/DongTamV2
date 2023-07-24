
#include "GUI.h"

#include "../BoardParameter.h"
#include "GUI_Navigation.h"
#include "LedButton.h"
EventGroupHandle_t evgGUI;
LCDI2C lcdI2C;
TaskHandle_t taskGUIHandle;
GUIParam_t param[LCD_ROWS];

TaskHandle_t* GUI_GetTaskHandle(){
    return &taskGUIHandle;
}
const char *paramText[]={
    "StartParam", //not use
    "Total Van :",
    "Down T Cyc:",
    "Clean Mode:",
    "Contrast  :",
    "DP-Low    :",
    "DP-High   :",
    "DP-Alarm  :",
    "OCD High  :",
    "OCD Low   :",
    "Pulse Time:",
    "Inter Time:",
    "Cycle Time:",
    "OperHours :",
    "Serv Run H:",
    "SerH Alarm:",
    // special param to handle
    "StringOffset",//not use
    "Language  :",
    "dpDisRange:",
    "Test Mode :",
    "Param code:",
    "Tech code :",
    "DP Mode   :",
};

// to map ParamIndex with real param setting table
ParamIndex orderToDisplay[21] = {
    INDEX_PARAM_CODE,
    INDEX_DP_LOW,
    INDEX_DP_HIGH,
    INDEX_DP_WARN,
    INDEX_PULSE_TIME,
    INDEX_INTERVAL_TIME,
    INDEX_CYCLE_INTERVAL_TIME,
    INDEX_TOTAL_VAN,
    INDEX_DOWN_TIME_CYCLE,
    INDEX_ODC_HIGH,
    INDEX_ODC_LOW,
    INDEX_OPERATE_HOURS,
    INDEX_LANGUAGE,
    INDEX_DISPLAY_RANGE,
    INDEX_ODC_CLEAN_MODE,
    INDEX_TEST_MODE,
    INDEX_DISPLAY_CONTRAST,
    INDEX_SERV_RUN_HOURS,
    INDEX_SERV_RUN_HOURS_ALARM,
    INDEX_TECH_CODE,
    INDEX_DP_MODE,
};


void PrintNavigation();
uint8_t CountLengthPreviousValue(uint32_t value);
uint8_t CheckValueIsLimit(uint32_t *value, uint32_t valLowLimit, uint32_t valHighLimit, EventGroupHandle_t *eventLimit);
void GUI_ScrollUpDown(uint8_t paramNO);

void GUITask(void *pvParameter)
{
    EventBits_t e;
    while(1){
        if(xTaskNotifyWait(pdFALSE,pdTRUE,&e,10/portTICK_PERIOD_MS)){
            // ESP_LOGI("GUITask","Get notify");
            GUI_ClearPointer();
            GUINAV_GetEvent(e);
            GUI_ShowPointer();
            PrintNavigation();
            GUI_Manage();
        }
    }
}



void GUI_Manage()
{
    // uint8_t pNow = GUINAV_GetCurrentSelected();
    // uint8_t pY = GUINAV_GetPointerPosY();
    // uint8_t pX = GUINAV_GetPointerPosX();
    // uint8_t paramNO = GUINAV_GetParamNum();
    // if(pNow == IS_KEYWORD){
    //     #if defined(USE_LOAD_NEW_SCREEN) 
    //     #elif defined(USE_SCROLL_SCREEN) 
    //     GUI_ScrollUpDown(paramNO);
    //     #endif
    //     GUI_ShowPointer();
    // }
    // else if (pNow == IS_VALUE){
    //     uint16_t valLowLimit = param[pY].lowLimit;
    //     uint16_t valHighLimit = param[pY].highLimit;
    //     uint16_t scale =  param[pY].scaleValue;
    //     uint32_t value = param[pY].Value;// save previous value to clear value slot before print new value to slot
    //     uint8_t lenPrevVal = CountLengthPreviousValue(value);
    //     char unit[4];
    //     if(param[pY].unit) {strcpy(unit,param[pY].unit);lenPrevVal += strlen(unit);}
    //     EventBits_t BitToWait = EVT_INCREASE_VALUE|EVT_DECREASE_VALUE ;
    //     EventBits_t e = xEventGroupWaitBits(evgGUI,BitToWait, pdTRUE,pdFALSE,0);
    //     if(CHECKFLAG(e,EVT_INCREASE_VALUE)) value +=scale;
    //     if(CHECKFLAG(e,EVT_DECREASE_VALUE))value -=scale;
    //     CheckValueIsLimit(&value,valLowLimit,valHighLimit,&evgGUI);
    //     char s[8]={0};
    //     uint8_t i=0;
    //     do{
    //         strcat(s," ");
    //         i++;
    //     } while(i < lenPrevVal);
    //     ESP_ERROR_CHECK(LCDI2C_Print(s,pX+POINTER_SLOT,pY));
    //     if(!param[pY].unit) sprintf(s,"%lu",value);
    //     else sprintf(s,"%lu%s",value,param[pY].unit);
    //     ESP_ERROR_CHECK(LCDI2C_Print(s,pX+POINTER_SLOT,pY));
    //     param[pY].Value = value;
    //     GUI_SetGuiInfoValue(&guiInfo,paramNO,value);
    // }
}


/**
 * @brief Single function section
 *  
 */


void GUI_ScrollUpDown(uint8_t paramNO)
{
    // // if current pointer pointed to the last row and param is not the last param
    // EventBits_t e = xEventGroupGetBits(evgGUI);
    // if((CHECKFLAG(e,EVT_PARAM_SCOLL_DOWN))){
    //     for(int8_t i = (LCD_ROWS -1) ;i > - 1;i--) {
    //         if((paramNO -(LCD_ROWS - 1 - i)) <= SETTING_NO_PARAM_START) break;
    //         ESP_LOGI("DOWN","%d",i);
    //         GUI_GetParam(param+i,paramNO -(LCD_ROWS - 1 - i));
    //     }
    //     GUI_LoadPage();
    //     for(uint8_t i = 0;i < LCD_ROWS;i++){
    //         ESP_LOGW("GUI_Manage","Down:[%u]:txt:%s",i,param[i].text_on_screen);
    //     }
    // }// if current pointer pointed to the first row and param is not the first param
    // else if((CHECKFLAG(e,EVT_PARAM_SCOLL_UP))){
    //     for(int8_t i = 0; i < LCD_ROWS ;i++) {
    //         if(i >= SETTING_NO_PARAM_END) break;
    //         GUI_GetParam(param+i,paramNO + i);
    //     }
    //     GUI_LoadPage();
    //     for(uint8_t i = 0;i < LCD_ROWS;i++){
    //         ESP_LOGW("GUI_Manage","Up:[%u]:txt:%s",i,param[i].text_on_screen);
    //     }
    // }
}

/**
 * @brief Tính toán số ô cần xóa giá trị value trước đó, xóa tới 6 hàng
 * 
 * @param value Giá trị cần xóa trước khi update giá trị mới vào 
 * @return uint8_t Số ô màn hình LCD cần xóa mà giá trị trước đó chiếm dụng
 */
uint8_t CountLengthPreviousValue(uint32_t value){
    uint8_t lenPrevVal = 0;
    if(value < 10) lenPrevVal = 1;
    else if(value < 100) lenPrevVal = 2; // 2 digit to clear
    else if(value < 1000) lenPrevVal = 3; // 3 digit to clear
    else if(value < 10000) lenPrevVal = 4; // 4 digit to clear
    else if(value < 100000) lenPrevVal = 5; // 5 digit to clear
    else if(value < 1000000) lenPrevVal = 6; // 6 digit to clear
    return lenPrevVal;
}

/**
 * @brief Kiểm tra giá trị đầu vào có vượt ngưỡng thấp và cao hay không
 * Nếu có thì gán ngưỡng vào giá trị đầu vào 
 * @param value Giá trị cần kiểm tra ngưỡng
 * @param valLowLimit Ngưỡng thấp của thông số
 * @param valHighLimit Ngưỡng cao của thông số
 * @param eventLimit event group để bật cờ sự kiện giá trị vượt ngưỡng
 * @return uint8_t 0 là không vượt ngưỡng, 1 ngưỡng cao và 2 là ngưỡng thấp
 */
uint8_t CheckValueIsLimit(uint32_t *value, uint32_t valLowLimit, uint32_t valHighLimit, EventGroupHandle_t *eventLimit)
{
    uint32_t temp = *value;
    // if value is above limit, set it to limit and set eventbit in evgGUI
    if(temp >= valHighLimit){
        *value = valHighLimit;
        xEventGroupSetBits(*eventLimit,EVT_VALUE_ABOVE_THRESHOLD);
        //avoid deadlock
        xEventGroupClearBits(*eventLimit,EVT_VALUE_BELOW_THRESHOLD);
        ESP_LOGE("GUI_Manage","high limit");
        return 1;
    } 
    else if(temp <= valLowLimit){
        *value = valLowLimit;
        xEventGroupSetBits(*eventLimit,EVT_VALUE_BELOW_THRESHOLD);
        //avoid deadlock
        xEventGroupClearBits(*eventLimit,EVT_VALUE_ABOVE_THRESHOLD);
        ESP_LOGE("GUI_Manage","low limit");
        return 2;
    }
    else {
        *value = temp;   
        EventBits_t e = xEventGroupGetBits(*eventLimit);
        //if none of conditions above is passed, it mean value is not saturated anymore, so check and clear bit control threshold
        if(CHECKFLAG(e,EVT_VALUE_ABOVE_THRESHOLD)) xEventGroupClearBits(*eventLimit,EVT_VALUE_ABOVE_THRESHOLD);
        else if(CHECKFLAG(e,EVT_VALUE_BELOW_THRESHOLD)) xEventGroupClearBits(*eventLimit,EVT_VALUE_BELOW_THRESHOLD);
        // ESP_LOGI("GUI_Manage","not limit");
    }
    return 0;
}
void GUI_GetParam(GUIParam_t *gp, ParamIndex paramNO)
{
    //Get value integer
    if(paramNO >= INDEX_TOTAL_VAN && paramNO <= INDEX_SERV_RUN_HOURS_ALARM){
        gp->index = paramNO;
    }
}

void GUI_PrintParam(uint8_t index, uint8_t row)
{
    ESP_LOGI("PrintParam","Index is:%u,row:%u",index,row);
    char unit[5] = {0};
    // check if unit is NULL or not, if not then copy it to unit array variable
    if(Brd_GetUnit(index)) {
        strcpy(unit,Brd_GetUnit(index)); 
        ESP_LOGI("PrintParam","Unit: %s",Brd_GetUnit(index));
    }
    LCDI2C_Print(paramText[index],POINTER_SLOT,row);
    char StringValue[20];
    // if no string is copied to unit, strlen would be zero
    if(!strlen(unit)) {
        // handle string value
        if(index > INDEX_STRING_PARAM_OFFSET && index <= INDEX_DP_MODE){
            sprintf(StringValue,"%s",Brd_GetParamString(index));
        }
        // handle int value non unit
        else {
            sprintf(StringValue,"%lu",Brd_GetParamInt(index));
        }
    }
    else sprintf(StringValue,"%lu %s",Brd_GetParamInt(index),unit);
    // pointer slot for pointer of text and slot for point of value
    LCDI2C_Print(StringValue,POINTER_SLOT + LENGTH_OF_PARAM + POINTER_SLOT,row);
}

void GUI_LoadPage()
{
    LCDI2C_Clear();
    vTaskDelay(20/portTICK_PERIOD_MS);
    // each parameter will be placed in each rows of LCD
    // for(uint8_t i=0;i<LCD_ROWS;i++){
    //     //if not NULL of text
    //     if(!(param+i)->text_on_screen) return;
    //     GUI_PrintParam((param+i)->text_on_screen,(param +i)->Value,(param +i)->unit,i);
    // }
    GUI_ShowPointer();
}

/**
 * @brief Hiển thị các thông số lên màn hình LCD theo thứ tự trong bảng, không tuân theo thứ tự của ParamIndex
 * 
 */
void GUI_LoadPageAtInit()
{
    // each parameter will be placed in each rows of LCD
    for(uint8_t i=0;i<LCD_ROWS;i++){
        GUI_PrintParam(orderToDisplay[i],i);
    }
}

void GUI_ShowPointer(){LCDI2C_Print(">",GUINAV_GetPointerPosX(),GUINAV_GetPointerPosY());}
void GUI_ClearPointer(){LCDI2C_Print(" ",GUINAV_GetPointerPosX(),GUINAV_GetPointerPosY());}

/*
    End Single function section
*/


/**
 * @brief Debug section
 * 
 */


void PrintNavigation()
{
    // ESP_LOGI("GUI_NAV","page: %u",GUINAV_GetPage());
    // ESP_LOGI("GUI_NAV","param: %u",GUINAV_GetParamNum());
    // ESP_LOGI("GUI_NAV","pNow: %u",GUINAV_GetCurrentSelected());
    // ESP_LOGI("GUI_NAV","x: %u",GUINAV_GetPointerPosX());
    // ESP_LOGI("GUI_NAV","y: %u",GUINAV_GetPointerPosY());
}
/*
    End Debug section
*/


/**
 * @brief Init section
 * 
 */



void PressureIndicator_Init()
{
    PI_Init();
    PI_SetLevel(0);
}

esp_err_t LCD_init()
{
    esp_err_t err = ESP_OK;
    err = LCDI2C_Config(&lcdI2C);
    LCDI2C_TurnOnBackLight();
    return err;
}

esp_err_t ButtonInit()

{
    gpio_config_t cfg = {
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = BT1_MASK |
                    BT2_MASK |
                    BT3_MASK|
                    BT4_MASK,
    };
    return gpio_config(&cfg);
}

void GuiInit(){
    evgGUI = xEventGroupCreate();
    ButtonInit();
    LCD_init();
    PressureIndicator_Init();
    GUI_ShowPointer();
    GUI_LoadPageAtInit();
}

/*
    End Init section
*/
