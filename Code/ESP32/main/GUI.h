#ifndef __GUI_H__
#define __GUI_H__

typedef struct {
    uint16_t ex_works_setting;
    uint16_t max_range;
    uint16_t min_range;
    uint8_t unit;
}GUI_function;

typedef struct {
    uint8_t para_no;
    char text_on_screen[20];
    GUI_function fnc;    
}parameter_UI;

#endif