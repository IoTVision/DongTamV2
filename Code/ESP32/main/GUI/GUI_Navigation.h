#ifndef _GUI_NAVIGATION_H_
#define _GUI_NAVIGATION_H_
#include "../ShareVar.h"
#include "GUI/GUI.h"
#include "BoardParameter.h"

/**
 * @brief POINTER_SLOT Ô dùng để hiển thị con trỏ trên màn hình, sẽ chiếm dụng một cột 
 * trước tên thông số và 1 cột trước giá trị của các thông số trên màn hình
 * 
 */
#define POINTER_SLOT 1 

/**
 * @brief Cho biết hiện tại con trỏ đang trỏ tới tên thông số hay giá trị của nó
 * 
 */
typedef enum {
    IS_KEYWORD = 1,
    IS_VALUE,
}PointerNow; 

/**
 * @brief Trang hiển thị, hiện tại không dùng
 * 
 */
typedef enum {
    PAGE_START,
    PAGE_SETTING,
    PAGE_CONTROL,
    PAGE_RUN,
    PAGE_END,
}Page; 

/**
 * @brief Điều hướng hiển thị màn hình LCD
 * @param pX Vị trí cột hiển thị con trỏ trên màn hình
 * @param pY Vị trí hàng hiển thị con trỏ 
 * @param page chuyển trang tiếp theo
 * @param pNow Cho biết con trỏ đang trỏ tới tên thông số(key) hay giá trị của thông số đó(value)
 * @param param thông số hiện tại đang trỏ tới để thao tác, lấy từ BoardParameter.h
 */
typedef struct GUI_NAV{
    uint8_t pX;
    uint8_t pY;
    Page page;
    PointerNow pNow;
    ParamIndex param;
}GUI_NAV;

uint8_t GUINAV_GetPage();
/**
 * @brief Trả về thứ tự (index) của biến orderToDisplay trong GUI.c mà con trỏ đang trỏ tới, 
 * không phải thứ tự các thông số trong BoardParameter
 * Ví dụ: con trỏ GUI đang trỏ tới phần tử thứ 0 trong orderToDisplay của GUI.c 
 * thì có giá trị là INDEX_PARAM_CODE trong BoardPameter.h vì đây là giá trị mapping 
 * cho giống với bảng điều khiển của Đồng Tâm, còn 
 * Để lấy được thông số thực sự mà con trỏ đang trỏ tới thì phải viết như sau:
 * orderToDisplay[GUINAV_GetOrderToDisplayIndex]
 */
 
ParamIndex GUINAV_GetOrderToDisplayIndex();
// Vị trí con trỏ trên màn hình LCD theo cột
uint8_t GUINAV_GetPointerPosX();
// Vị trí con trỏ trên màn hình LCD theo hàng 
uint8_t GUINAV_GetPointerPosY();
// Cho biết con trỏ đang trỏ tới tên thông số hay giá trị thông số
uint8_t GUINAV_GetCurrentSelected();
void GUINAV_GetEvent(EventBits_t e);
void GUI_LoadPageSetting();
void GUI_Manage();



#endif