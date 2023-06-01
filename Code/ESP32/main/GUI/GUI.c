
#include "GUI.h"
#include "GUI_Navigation.h"

EventGroupHandle_t evgGUI;
LCDI2C lcdI2C;
TaskHandle_t taskGUIHandle;
Param_t param[LCD_ROWS];

GUI_Info guiInfo = {
    .dpHigh = 1300,
    .dpLow = 700,
    .dpAlarm = 2300,
    .cycleIntervalTime = 6,
    .pulseTime = 60,
    .intervalTime = 10,
    .totalVan = 0,
};

void PrintNavigation();
uint8_t CountLengthPreviousValue(uint32_t value);
esp_err_t GUI_SaveValueToFlash(EventGroupHandle_t *evg,Param_t param);
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

void TaskScanButton(void *pvParameter){
    while(1){
        ReadGuiButton(BTN_MENU,EVT_BTN_MENU);
        ReadGuiButton(BTN_UP,EVT_BTN_UP);
        ReadGuiButton(BTN_DOWN_RIGHT,EVT_BTN_DOWN_RIGHT);
        ReadGuiButton(BTN_SET,EVT_BTN_SET);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void GUI_Manage()
{
    uint8_t pNow = GUINAV_GetCurrentSelected();
    uint8_t pY = GUINAV_GetPointerPosY();
    uint8_t pX = GUINAV_GetPointerPosX();
    uint8_t paramNO = GUINAV_GetParamNum();
    if(pNow == IS_KEYWORD){
        #if defined(USE_LOAD_NEW_SCREEN) 
        #elif defined(USE_SCROLL_SCREEN) 
        GUI_ScrollUpDown(paramNO);
        #endif
        GUI_ShowPointer();
    }
    else if (pNow == IS_VALUE){
        uint16_t valLowLimit = param[pY].lowLimit;
        uint16_t valHighLimit = param[pY].highLimit;
        uint16_t scale =  param[pY].scaleValue;
        uint32_t value = param[pY].Value;// save previous value to clear value slot before print new value to slot
        uint8_t lenPrevVal = CountLengthPreviousValue(value);
        lenPrevVal += strlen(param[pY].unit);
        EventBits_t BitToWait = EVT_INCREASE_VALUE|EVT_DECREASE_VALUE ;
        EventBits_t e = xEventGroupWaitBits(evgGUI,BitToWait, pdTRUE,pdFALSE,0);
        if(CHECKFLAG(e,EVT_INCREASE_VALUE)) value +=scale;
        if(CHECKFLAG(e,EVT_DECREASE_VALUE))value -=scale;
        CheckValueIsLimit(&value,valLowLimit,valHighLimit,&evgGUI);
        ESP_ERROR_CHECK(GUI_SaveValueToFlash(&evgGUI,param[pY]));
        char s[8]={0};
        uint8_t i=0;
        do{
            strcat(s," ");
            i++;
        } while(i < lenPrevVal);
        ESP_ERROR_CHECK(LCDI2C_Print(s,pX+POINTER_SLOT,pY));
        if(!param->unit) sprintf(s,"%lu",value);
        else sprintf(s,"%lu%s",value,param->unit);
        ESP_ERROR_CHECK(LCDI2C_Print(s,pX+POINTER_SLOT,pY));
        param[pY].Value = value;
        GUI_SetGuiInfoValue(&guiInfo,paramNO,value);
    }

}


/**
 * @brief Single function section
 *  
 */

esp_err_t GUI_SaveValueToFlash(EventGroupHandle_t *evg,Param_t param)
{
    EventBits_t e = xEventGroupWaitBits(*evg,EVT_SAVE_VALUE_TO_FLASH,pdTRUE,pdFALSE,0);
    esp_err_t err = ESP_OK;
    nvs_handle_t nvsHandle;
    if(CHECKFLAG(e,EVT_SAVE_VALUE_TO_FLASH)){
        ESP_LOGI("saveFlash","receive event");
        err = nvs_open("Board", NVS_READWRITE, &nvsHandle);
        if (err != ESP_OK) ESP_LOGE("nvs","Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        else {
            // err = nvs_set_str(nvsBrdStorage,"TestFlash","SpiritBoi");
            // err = nvs_set_blob(nvsBrdStorage,"Pressure",(void*)&p,sizeof(p));
            // err = nvs_commit(nvsBrdStorage);
        }
        nvs_close(nvsHandle);
        LCDI2C_Clear();
        vTaskDelay(20/portTICK_PERIOD_MS);
        LCDI2C_Print("Save data",0,0);
        LCDI2C_Print(param.text_on_screen,0,1);
        vTaskDelay(2000/portTICK_PERIOD_MS);
        GUI_LoadPage();
    }
    return err;
}

void GUI_ScrollUpDown(uint8_t paramNO)
{
    // if current pointer pointed to the last row and param is not the last param
    EventBits_t e = xEventGroupGetBits(evgGUI);
    if((CHECKFLAG(e,EVT_PARAM_SCOLL_DOWN))){
        for(int8_t i = (LCD_ROWS -1) ;i > - 1;i--) {
            if((paramNO -(LCD_ROWS - 1 - i)) <= NO_PARAM_START) break;
            ESP_LOGI("DOWN","%d",i);
            GUI_GetParam(param+i,paramNO -(LCD_ROWS - 1 - i));
        }
        GUI_LoadPage();
        for(uint8_t i = 0;i < LCD_ROWS;i++){
            ESP_LOGW("GUI_Manage","Down:[%u]:txt:%s",i,param[i].text_on_screen);
        }
    }// if current pointer pointed to the first row and param is not the first param
    else if((CHECKFLAG(e,EVT_PARAM_SCOLL_UP))){
        for(int8_t i = 0; i < LCD_ROWS ;i++) {
            if(i >= NO_PARAM_END) break;
            GUI_GetParam(param+i,paramNO + i);
        }
        GUI_LoadPage();
        for(uint8_t i = 0;i < LCD_ROWS;i++){
            ESP_LOGW("GUI_Manage","Up:[%u]:txt:%s",i,param[i].text_on_screen);
        }
    }
    
    
}

/**
 * @brief calculate how many columns that value take place on LCD before it's value change to clear, up to 6 digit 
 * 
 * @param value input value
 * @return uint8_t columns that value take place
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
 * @brief Check if value input is above or below limit
 * 
 * @param value input value
 * @param valLowLimit low limit input
 * @param valHighLimit high limit input
 * @param eventLimit event group to set if detect low or high limit occur
 * @return uint8_t 0 is not limit, 1 is high limit and 2 is low limit
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

void BtnHandleWhenHolding(gpio_num_t gpio, EventBits_t e){

#define BTN_HOLD_DELAY_MAX 300
#define BTN_HOLD_DELAY_MIN 50
#define BTN_HOLD_DELAY_DECREASE_STEP 50
#define DELAY_COUNT_LOOP_THRESHOLD 3
#define RESET_DELAY 2
    //Delay each loop when user keep holding button
    static uint16_t Delay = BTN_HOLD_DELAY_MAX;
    /* if this number is bigger than certain threshold, 
    Delay will be decrease to speed up sending notify to GUITask
    Each time call xTaskNotify, DelayCountLoop increase by 1
    */
    static uint16_t DelayCountLoop = 0;
    if(e == RESET_DELAY){
        DelayCountLoop = 0;
        Delay = BTN_HOLD_DELAY_MAX;
        // do nothing after reset, so must be return
        return;
    }
    /*if user keep holding down button, check button is UP or DOWN-RIGHT and if only currently selected is value 
    to make delay shorter, the rest is just delay
    
    */
    if(((gpio == BTN_UP) || (gpio == BTN_DOWN_RIGHT)) 
    && (GUINAV_GetCurrentSelected() == IS_VALUE)) {
        vTaskDelay(Delay/portTICK_PERIOD_MS);
        DelayCountLoop+=1;
        /*
        if user only press BTN_UP and BTN_DOWN_RIGHT just only one, interpret it as normal button and not send notify, if user holding down button and DelayCountLoop increase, this time interpret it as speed up count
        */
        if(DelayCountLoop > DELAY_COUNT_LOOP_THRESHOLD - 1) 
        xTaskNotify(taskGUIHandle,e,eSetValueWithoutOverwrite);
    }
    else {
        // button SET and MENU are nothing to hanle, just delay and return
        vTaskDelay(50/portTICK_PERIOD_MS);
        return;
    }
    if(DelayCountLoop >= DELAY_COUNT_LOOP_THRESHOLD && Delay > BTN_HOLD_DELAY_MIN){
        DelayCountLoop=0;
        int temp = Delay - BTN_HOLD_DELAY_DECREASE_STEP;
        // Saturate low delay speed
        if(temp < BTN_HOLD_DELAY_MIN) Delay = BTN_HOLD_DELAY_MIN;
        else Delay-=BTN_HOLD_DELAY_DECREASE_STEP; 
    }
    
}

void ReadGuiButton(gpio_num_t gpio, EventBits_t e)
{
    if(!gpio_get_level(gpio)){
        vTaskDelay(100/portTICK_PERIOD_MS);
        while (!gpio_get_level(gpio)){
            // do things while holding button, i.e count UP and DOWN continously
            BtnHandleWhenHolding(gpio,e);
        }
        xTaskNotify(taskGUIHandle,e,eSetValueWithoutOverwrite);
        // Send RESET_DELAY to reset Delay and DelayCountLoop
        BtnHandleWhenHolding(0,2);
    }
}

void LedErrorWrite(bool ledState)
{
    if(ledState) HC595_SetBitOutput(LED_ERROR_MASK);
    else HC595_ClearBitOutput(LED_ERROR_MASK);
}

void LedStatusWrite(bool ledState)
{
    if(ledState) HC595_SetBitOutput(LED_STATUS_MASK);
    else HC595_ClearBitOutput(LED_STATUS_MASK);
}

void GUI_ShowPointer()
{
    LCDI2C_Print(">",GUINAV_GetPointerPosX(),GUINAV_GetPointerPosY());
}

void GUI_ClearPointer()
{
    LCDI2C_Print(" ",GUINAV_GetPointerPosX(),GUINAV_GetPointerPosY());
}

static inline void AssignParam(Param_t *param, char*textScreen,uint16_t value, uint16_t lowLimit, uint16_t highLimit, uint16_t scale, char* unit){
    param->text_on_screen = textScreen;
    param->Value = value;
    param->lowLimit = lowLimit;
    param->highLimit = highLimit;
    param->unit = unit;
    param->scaleValue = scale;
}

void GUI_GetParam(Param_t *param, uint8_t paramNO)
{
    switch (paramNO)
    {
    case NO_PARAM_CODE:
        AssignParam(param,TEXT_PARAM_CODE,0,0,0,0,NULL);
        break;
    case NO_DP_LOW:
        AssignParam(param,TEXT_DP_LOW,guiInfo.dpLow,250,4000,50,"Pa");
        break;
    case NO_DP_HIGH:
        AssignParam(param,TEXT_DP_HIGH,guiInfo.dpHigh,250,4000,50,"Pa");
        break;
    case NO_DP_ALARM:
        AssignParam(param,TEXT_DP_ALARM,guiInfo.dpAlarm,300,5000,100,"Pa");
        break;
    case NO_PULSE_TIME:
        AssignParam(param,TEXT_PULSE_TIME,guiInfo.pulseTime,30,300,30,"ms");
        break;
    case NO_INTERVAL_TIME:
        AssignParam(param,TEXT_INTERVAL_TIME,guiInfo.intervalTime,4,500,1,"s");
        break;
    case NO_CYCLE_TIME:
        AssignParam(param,TEXT_CYCLE_TIME,guiInfo.cycleIntervalTime,2,100,1,"s");
        break;
    case NO_TOTAL_VAN:
        AssignParam(param,TEXT_TOTAL_VAN,guiInfo.totalVan,0,15,1,NULL);
        break;
    default:
        param->text_on_screen = NULL;
        param->Value = 0;
        param->highLimit = 0;
        param->lowLimit = 0;
        param->scaleValue = 0;
        param->unit = NULL;
        break;
    }
}

void GUI_SetGuiInfoValue(GUI_Info *gi, uint8_t paramNO, uint32_t value)
{
    switch (paramNO)
    {
    case NO_PARAM_CODE:
        break;
    case NO_DP_LOW:
        gi->dpLow = value;
        break;
    case NO_DP_HIGH:
        gi->dpHigh = value;
        break;
    case NO_DP_ALARM:
        gi->dpAlarm = value;
        break;
    case NO_PULSE_TIME:
        gi->pulseTime = value;
        break;
    case NO_INTERVAL_TIME:
        gi->intervalTime = value;
        break;
    case NO_CYCLE_TIME:
        gi->cycleIntervalTime = value;
        break;
    case NO_TOTAL_VAN:
        gi->totalVan = value;
        break;
    default:

        break;
    }
}

void GUI_PrintParam(char *keyword, uint16_t value, char* unit, uint8_t row)
{
    
    if(!keyword) return;
    else LCDI2C_Print(keyword,POINTER_SLOT,row);
    char StringValue[10];
    if(!unit) sprintf(StringValue,"%u",value);
    sprintf(StringValue,"%u%s",value,unit);
    // pointer slot for pointer to text and slot for point to value
    LCDI2C_Print(StringValue,POINTER_SLOT + LENGTH_OF_PARAM + POINTER_SLOT,row);
}

void GUI_LoadPage()
{
    LCDI2C_Clear();
    vTaskDelay(20/portTICK_PERIOD_MS);
    // each parameter will be placed in each rows of LCD
    for(uint8_t i=0;i<LCD_ROWS;i++){
        //if not NULL of text
        if(!(param+i)->text_on_screen) return;
        GUI_PrintParam((param+i)->text_on_screen,(param +i)->Value,(param +i)->unit,i);
    }
    GUI_ShowPointer();
}

void GUI_LoadPageAtInit()
{
    uint8_t paramNum = GUINAV_GetParamNum();
    // each parameter will be placed in each rows of LCD
    for(uint8_t i=0;i<LCD_ROWS;i++){
        GUI_GetParam((param+i),paramNum+i);
        GUI_PrintParam((param+i)->text_on_screen,(param +i)->Value,(param +i)->unit,i);
    }
}
/*
    End Single function section
*/


/**
 * @brief Debug section
 * 
 */


void PrintNavigation()
{
    ESP_LOGI("GUI_NAV","page: %u",GUINAV_GetPage());
    ESP_LOGI("GUI_NAV","param: %u",GUINAV_GetParamNum());
    ESP_LOGI("GUI_NAV","pNow: %u",GUINAV_GetCurrentSelected());
    ESP_LOGI("GUI_NAV","x: %u",GUINAV_GetPointerPosX());
    ESP_LOGI("GUI_NAV","y: %u",GUINAV_GetPointerPosY());
}
/*
    End Debug section
*/



/**
 * @brief Test section
 * 
 */

void TestGUI()
{
    LCDI2C_Print("GUI test",0,0);
    LCDI2C_Print("Press 4 button",0,1);
    while(1){
        ReadGuiButton(BTN_MENU,EVT_BTN_MENU);
        ReadGuiButton(BTN_UP,EVT_BTN_UP);
        ReadGuiButton(BTN_DOWN_RIGHT,EVT_BTN_DOWN_RIGHT);
        ReadGuiButton(BTN_SET,EVT_BTN_SET);
        EventBits_t BitsToWaitFor = (EVT_BTN_DOWN_RIGHT 
                                    |EVT_BTN_MENU 
                                    |EVT_BTN_SET 
                                    |EVT_BTN_UP);
        EventBits_t e = xEventGroupWaitBits(evgGUI, BitsToWaitFor,pdTRUE,pdFALSE,0);
        if(e & EVT_BTN_MENU){
            LCDI2C_Print("MENU      ",0,1);
            PI_SetLevel(3);
        }
        if(e & EVT_BTN_SET){
            LCDI2C_Print("SET       ",0,1);
            PI_SetLevel(6);
        }
        
        if(e & EVT_BTN_UP){
            LCDI2C_Print("UP        ",0,1);
            PI_SetLevel(9);
        }

        if(e & EVT_BTN_DOWN_RIGHT){
            LCDI2C_Print("DOWN RIGHT    ",0,1);
            for(uint8_t i=0;i<10;i++){
                LedErrorWrite(1);
                LedStatusWrite(0);
                HC595_ShiftOut(NULL,2,1);
                vTaskDelay(50/portTICK_PERIOD_MS);
                LedErrorWrite(0);
                LedStatusWrite(1);
                HC595_ShiftOut(NULL,2,1);
                vTaskDelay(50/portTICK_PERIOD_MS);
            }
            vTaskDelay(500/portTICK_PERIOD_MS);
            for(uint8_t i=0;i<10;i++){
                LedErrorWrite(1);
                LedStatusWrite(0);
                HC595_ShiftOut(NULL,2,1);
                vTaskDelay(50/portTICK_PERIOD_MS);
                LedErrorWrite(0);
                LedStatusWrite(1);
                HC595_ShiftOut(NULL,2,1);
                vTaskDelay(50/portTICK_PERIOD_MS);
            }
            LCDI2C_Print("Full complete",0,0);
            vTaskDelay(1000/portTICK_PERIOD_MS);
            LCDI2C_Clear();
            break;
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void TestReadSingleButton(gpio_num_t gpio, EventBits_t e,char* logMessage)
{
    if(!gpio_get_level(gpio)){
        vTaskDelay(100/portTICK_PERIOD_MS);
        LCDI2C_Print(logMessage,0,2);
        ESP_LOGI("GUI","%s",logMessage);
        while (!gpio_get_level(gpio))
        {
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
        xEventGroupSetBits(evgGUI,e);
    }
}

void TestButton()
{
    LCDI2C_Print("Test Button",0,0);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    LCDI2C_Print("Press button",0,0);
    LCDI2C_Print("from left to right",0,1);
    while (1)
    {
        TestReadSingleButton(BTN_MENU,EVT_BTN_MENU,"MENU OK");
        TestReadSingleButton(BTN_SET,EVT_BTN_SET,"SET OK ");
        TestReadSingleButton(BTN_DOWN_RIGHT,EVT_BTN_DOWN_RIGHT,"DR OK  ");
        TestReadSingleButton(BTN_UP,EVT_BTN_UP,"UP OK  ");
        EventBits_t BitsToWaitFor = (EVT_BTN_DOWN_RIGHT 
                                    |EVT_BTN_MENU 
                                    |EVT_BTN_SET 
                                    |EVT_BTN_UP);
        EventBits_t e = xEventGroupWaitBits(evgGUI, BitsToWaitFor,pdTRUE,pdTRUE,0);
        if(CHECKFLAG(e,BitsToWaitFor)){
            LCDI2C_Print("Button test done",0,2);
            ESP_LOGI("GUI","Button test done");
            vTaskDelay(2000/portTICK_PERIOD_MS);//wait LCD to full clean
            LCDI2C_Clear();
            return;
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}

void TestLedStatusErr(uint8_t blinkNum,uint16_t delay)
{
    for(uint8_t i=0;i<blinkNum;i++){
        LedErrorWrite(1);
        HC595_ShiftOut(NULL,2,1);
        vTaskDelay(delay/portTICK_PERIOD_MS);
        LedErrorWrite(0);
        HC595_ShiftOut(NULL,2,1);
        vTaskDelay(delay/portTICK_PERIOD_MS);
    }
    for(uint8_t i=0;i<blinkNum;i++){
        LedStatusWrite(1);
        HC595_ShiftOut(NULL,2,1);
        vTaskDelay(delay/portTICK_PERIOD_MS);
        LedStatusWrite(0);
        HC595_ShiftOut(NULL,2,1);
        vTaskDelay(delay/portTICK_PERIOD_MS);
        
    }
    for(uint8_t i=0;i<blinkNum;i++){
        LedStatusWrite(1);
        LedErrorWrite(1);
        HC595_ShiftOut(NULL,2,1);
        vTaskDelay(delay/portTICK_PERIOD_MS);
        LedStatusWrite(0);
        LedErrorWrite(0);
        HC595_ShiftOut(NULL,2,1);
        vTaskDelay(delay/portTICK_PERIOD_MS);
    }
}

void GuiTestFull()
{
    // TestLedStatusErr(5,35);
    // TestButton();
    // TestGUI();
}

/*
    End Test section
*/


/**
 * @brief Init section
 * 
 */



void PressureIndicator_Init()
{
    ESP_LOGI("LedBar","init");
    PI_Init();
    PI_SetLevel(0);
}

esp_err_t LCD_init()
{
    esp_err_t err = ESP_OK;
    err = LCDI2C_Config(&lcdI2C);
    LCDI2C_TurnOnBackLight();
    // LCDI2C_Print("SpiritBoi",0,0);
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