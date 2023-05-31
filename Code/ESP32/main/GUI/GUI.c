
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

void GUITask(void *pvParameter)
{
    EventBits_t e;
    while(1){
        if(xTaskNotifyWait(pdFALSE,pdTRUE,&e,10/portTICK_PERIOD_MS)){
            ESP_LOGI("GUITask","Notify");
            GUI_ClearPointer();
            GUINAV_GetEvent(e);
            GUI_ShowPointer();
            // PrintNavigation();
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
    if(pNow == IS_KEYWORD){
    }
    else if (pNow == IS_VALUE){
        uint16_t valLowLimit = param[pY].lowLimit;
        uint16_t valHighLimit = param[pY].highLimit;
        uint16_t scale =  param[pY].scaleValue;
        uint16_t value = param[pY].Value;// save previous value to clear value slot before print new value to slot
        uint16_t temp = value;
        uint8_t lengthPrevValue = 1;
        if(value < 10) lengthPrevValue = 1;
        else if(value < 100) lengthPrevValue = 2; // 2 digit to clear
        else if(value < 1000) lengthPrevValue = 3; // 3 digit to clear
        else if(value < 10000) lengthPrevValue = 4; // 4 digit to clear
        else if(value < 100000) lengthPrevValue = 5; // 5 digit to clear
        EventBits_t BitToWait = EVT_INCREASE_VALUE|EVT_DECREASE_VALUE|EVT_SAVE_VALUE_TO_FLASH ;
        EventBits_t e = xEventGroupWaitBits(evgGUI,BitToWait, pdTRUE,pdFALSE,0);
        if(CHECKFLAG(e,EVT_INCREASE_VALUE)) temp +=scale;
        else if(CHECKFLAG(e,EVT_DECREASE_VALUE)) temp -=scale;
        else if(CHECKFLAG(e,EVT_SAVE_VALUE_TO_FLASH)) ESP_LOGW("GUIManage","Save value to flash");
        // if value is above limit, set it to limit and set eventbit in evgGUI
        if(temp >= valHighLimit){
            value = valHighLimit;
            xEventGroupSetBits(evgGUI,EVT_VALUE_ABOVE_THRESHOLD);
            // ESP_LOGI("GUI_Manage","high limit");
        } 
        else if(temp <= valLowLimit){
            value = valLowLimit;
            xEventGroupSetBits(evgGUI,EVT_VALUE_BELOW_THRESHOLD);
            // ESP_LOGI("GUI_Manage","low limit");
        }
        else {
            value = temp;
            //if none of conditions above is passed, it mean value is not saturated anymore, so check and clear bit control threshold
            if(CHECKFLAG(e,EVT_VALUE_ABOVE_THRESHOLD)) xEventGroupClearBits(evgGUI,EVT_VALUE_ABOVE_THRESHOLD);
            else if(CHECKFLAG(e,EVT_VALUE_BELOW_THRESHOLD)) xEventGroupClearBits(evgGUI,EVT_VALUE_BELOW_THRESHOLD);
            // ESP_LOGI("GUI_Manage","not limit");
        }
        char s[8]={0};
        uint8_t i=0;
        do{
            strcat(s," ");
            i++;
        } while(i < lengthPrevValue);
        LCDI2C_Print(s,pX+POINTER_SLOT,pY);
        sprintf(s,"%u",value);
        LCDI2C_Print(s,pX+POINTER_SLOT,pY);
        param[pY].Value = value;
    }

}


/**
 * @brief Single function section
 *  
 */

void BtnHandleWhenHolding(gpio_num_t gpio, EventBits_t e){

#define BTN_HOLD_DELAY_MAX 1000
#define BTN_HOLD_DELAY_MIN 100
#define BTN_HOLD_DELAY_DECREASE_STEP 300
#define DELAY_COUNT_LOOP_THRESHOLD 2
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
    }
    /*if user keep holding down button, check button is UP or DOWN-RIGHT and if only currently selected is value 
    to make delay shorter, the rest is just delay
    */
    if(((gpio == BTN_UP) || (gpio == BTN_DOWN_RIGHT)) && (GUINAV_GetCurrentSelected() == IS_VALUE)) {
        vTaskDelay(Delay/portTICK_PERIOD_MS);
        xTaskNotify(taskGUIHandle,e,eSetValueWithoutOverwrite);
        DelayCountLoop+=1;
    }
    else {
        // button SET and MENU are nothing to hanle, just delay and return
        vTaskDelay(50/portTICK_PERIOD_MS);
        return;
    }
    if(DelayCountLoop >= DELAY_COUNT_LOOP_THRESHOLD && Delay > BTN_HOLD_DELAY_MIN){
        DelayCountLoop=0;
        Delay-=BTN_HOLD_DELAY_DECREASE_STEP;
        // Saturate low delay speed
        if(Delay < 50) Delay = 50;
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
        //Send notify to GUITask without overwrite value
        xTaskNotify(taskGUIHandle,e,eSetValueWithoutOverwrite);
        // Send RESET_DELAY to reset Delay and DelayCountLoop, have extra delay 50ms
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
    case NO_DP_HIGH:
        AssignParam(param,TEXT_DP_HIGH,guiInfo.dpHigh,250,4000,50,"Pa");
        break;
    case NO_DP_LOW:
        AssignParam(param,TEXT_DP_LOW,guiInfo.dpLow,250,4000,50,"Pa");
        break;
    case NO_DP_ALARM:
        AssignParam(param,TEXT_DP_ALARM,guiInfo.dpAlarm,300,5000,100,"Pa");
        break;
    case NO_CYCLE_TIME:
        AssignParam(param,TEXT_DP_ALARM,guiInfo.dpAlarm,2,100,1,"s");
        break;
    case NO_INTERVAL_TIME:
        AssignParam(param,TEXT_INTERVAL_TIME,guiInfo.intervalTime,4,500,1,"s");
        break;
    case NO_PULSE_TIME:
        AssignParam(param,TEXT_PULSE_TIME,guiInfo.pulseTime,30,300,30,"ms");
        break;
    case NO_TOTAL_VAN:
        AssignParam(param,TEXT_TOTAL_VAN,guiInfo.totalVan,0,15,1,NULL);
        break;
    default:
        param->text_on_screen = "Not available";
        param->Value = 0;
        param->highLimit = 0;
        param->lowLimit = 0;
        param->scaleValue = 0;
        param->unit = NULL;
        break;
    }
}

void GUI_PrintParam(char *keyword, uint16_t value, uint8_t row)
{
    LCDI2C_Print(keyword,POINTER_SLOT,row);
    char StringValue[10];
    sprintf(StringValue,"%u",value);
    // pointer slot for pointer to text and slot for point to value
    LCDI2C_Print(StringValue,POINTER_SLOT + LENGTH_OF_PARAM + POINTER_SLOT,row);
}

void GUI_LoadPageSetting()
{
    uint8_t paramNum = GUINAV_GetParamNum();
    // each parameter will be placed in each rows of LCD
    for(uint8_t i=0;i<LCD_ROWS;i++){
        GUI_GetParam((param+i),paramNum+i);
        GUI_PrintParam((param+i)->text_on_screen,(param +i)->Value,i);
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
    TestLedStatusErr(5,35);
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
    GUI_LoadPageSetting();
}

/*
    End Init section
*/