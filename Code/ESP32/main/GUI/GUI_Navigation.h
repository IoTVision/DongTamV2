#ifndef _GUI_NAVIGATION_H_
#define _GUI_NAVIGATION_H_
#include "../ShareVar.h"
#include "GUI/GUI.h"

/*
Parameter should be padding left by 2 column for ">" pointer character 
because this ">" will be shown at column 1
*/ 
#define POINTER_SLOT 1 


/*
NO_PARAM_START and NO_PARAM_END are used to detect GUI_NAV.param is reaching the boundary of ParamSetting
We can use these two to set begin and end parameter without the need of hardcode set the name of parameter
Example: 
- First parameter is NO_PARAM_CODE =  NO_PARAM_START + 1
- Last parameter is NO_CYCLE_TIME  = NO_PARAM_END - 1
If we change order of these parameter, i.e Last parameter is NO_PULSE_TIME and the first is NO_DP_HIGH, 
in code we no need to set GUI_NAV.param = NO_DP_HIGH or GUI_NAV.param = NO_PULSE_TIME, 
it automatically set by using NO_PARAM_START and NO_PARAM_END
*/
typedef enum {
    SETTING_NO_PARAM_START, // Not assign in actual code, use to set GUI_NAV.param to the start without hardcode
    NO_PARAM_CODE,
    NO_DP_LOW,
    NO_DP_HIGH,
    NO_DP_ALARM,
    NO_PULSE_TIME,
    NO_INTERVAL_TIME,
    NO_CYCLE_TIME,
    NO_TOTAL_VAN,
    NO_ODC_DOWN_TIME_CY,
    NO_ODC_HIGH,
    NO_ODC_LOW,
    NO_OPERATE_HOURS,
    NO_ODC_CLEANING_MODE,
    NO_SER_RUN_HOURS_ALARM,
    NO_DP_MODE,
    SETTING_NO_PARAM_END, // Not assign in actual code, use to set GUI_NAV.param to the start without hardcode
}ParamSetting; 



// current position of pointer
typedef enum {
    IS_KEYWORD = 1,
    IS_VALUE,
}PointerNow; 

typedef enum {
    PAGE_START,
    PAGE_SETTING,
    PAGE_CONTROL,
    PAGE_RUN,
    PAGE_END,
}Page; 

typedef struct GUI_NAV{
    uint8_t pX;
    uint8_t pY;
    Page page;
    PointerNow pNow;
    ParamSetting param;
}GUI_NAV;

uint8_t GUINAV_GetPage();
uint8_t GUINAV_GetParamNum();
uint8_t GUINAV_GetPointerPosX();
uint8_t GUINAV_GetPointerPosY();
uint8_t GUINAV_GetCurrentSelected();
void GUINAV_GetEvent(EventBits_t e);
void GUI_LoadPageSetting();
void GUI_Manage();



#endif