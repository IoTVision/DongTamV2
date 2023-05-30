#ifndef _GUI_NAVIGATION_H_
#define _GUI_NAVIGATION_H_
#include "GUI/GUI.h"
#include "ShareVar.h"

/*
Parameter should be padding left by 2 column for ">" pointer character 
because this ">" will be shown at column 1
*/ 
#define PARAM_PADDING_LEFT 1 


/*
NO_PARAM_START and NO_PARAM_END are used to detect GUI_NAV.param is reaching the boundary of ParamNum
We can use these two to set begin and end parameter without the need of hardcode set the name of parameter
Example: 
- First parameter is NO_PARAM_CODE =  NO_PARAM_START + 1
- Last parameter is NO_CYCLE_TIME  = NO_PARAM_END - 1
If we change order of these parameter, i.e Last parameter is NO_PULSE_TIME and the first is NO_DP_HIGH, 
in code we no need to set GUI_NAV.param = NO_DP_HIGH or GUI_NAV.param = NO_PULSE_TIME, 
it automatically set by using NO_PARAM_START and NO_PARAM_END
*/
typedef enum {
    NO_PARAM_START, // Not assign in actual code, use to set GUI_NAV.param to the start without hardcode
    NO_PARAM_CODE,
    NO_DP_LOW,
    NO_DP_HIGH,
    NO_DP_ALARM,
    NO_PULSE_TIME,
    NO_INTERVAL_TIME,
    NO_CYCLE_TIME,
    NO_PARAM_END, // Not assign in actual code, use to set GUI_NAV.param to the start without hardcode
}ParamNum; 

// current position of pointer
typedef enum {
    IS_KEYWORD = 1,
    IS_VALUE,
}PointerNow; 

typedef enum {
    PAGE_DEFAULT = 1,
    PAGE_1,
    PAGE_END,
}Page; 

typedef struct GUI_NAV{
    uint8_t value;
    uint8_t pX;
    uint8_t pY;
    Page page;
    PointerNow pNow;
    ParamNum param;
}GUI_NAV;

uint8_t GUINAV_GetPage();
uint8_t GUINAV_GetParam();
uint8_t GUINAV_GetValue();
uint8_t GUINAV_GetPointerPosX();
uint8_t GUINAV_GetPointerPosY();
uint8_t GUINAV_GetCurrentSelected();
void GUINAV_GetEvent(EventBits_t e);

#endif