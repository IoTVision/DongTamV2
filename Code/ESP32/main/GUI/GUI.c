
#include "GUI.h"
#include "GUI_Navigation.h"

EventGroupHandle_t evgGUI;
LCDI2C lcdI2C;
TaskHandle_t taskGUIHandle;
GUI_Info guiInfo = {
    .dpHigh = 1300,
    .dpLow = 700,
    .dpAlarm = 2300,
    .cycleIntervalTime = 6,
    .pulseTime = 60,
    .intervalTime = 10,
    .totalVan = 0,
};

void GUI_LoadDefault()
{
    
}

void GUI_ShowPointer()
{
    LCDI2C_Print(">",GUINAV_GetPointerPosX(),GUINAV_GetPointerPosY());
}

void GUI_ClearPointer()
{
    LCDI2C_Print(" ",GUINAV_GetPointerPosX(),GUINAV_GetPointerPosY());
}

void PrintNavigation()
{
    ESP_LOGI("GUI_NAV","page: %u",GUINAV_GetPage());
    ESP_LOGI("GUI_NAV","param: %u",GUINAV_GetParam());
    ESP_LOGI("GUI_NAV","pNow: %u",GUINAV_GetCurrentSelected());
    ESP_LOGI("GUI_NAV","value: %u",GUINAV_GetValue());
    ESP_LOGI("GUI_NAV","x: %u",GUINAV_GetPointerPosX());
    ESP_LOGI("GUI_NAV","y: %u",GUINAV_GetPointerPosY());
}

void GUITask(void *pvParameter)
{
    EventBits_t e;
    while(1){
        if(xTaskNotifyWait(pdFALSE,pdTRUE,&e,10/portTICK_PERIOD_MS)){
            ESP_LOGI("GUITask","Receive notify %lu",e);
            GUI_ClearPointer();
            GUINAV_GetEvent(e);
            GUI_ShowPointer();
            PrintNavigation();
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
    vTaskDelay(Delay/portTICK_PERIOD_MS);
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
}

/*
    End Init section
*/