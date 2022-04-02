#include <bsp_time.h>

uint32_t BSP_sys_time_ms() {
    return HAL_GetTick();
}