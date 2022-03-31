#include "warpper.h"
#include "bsp.h"
#include "app.h"
void main_init(){
    //初始化BSP层
    BSP_Layer_Init();
    APP_Layer_Init();
}