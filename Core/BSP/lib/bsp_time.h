#ifndef _BSP_TIME_H
#define _BSP_TIME_H
#include <stdint.h>
#include "main.h"
uint32_t BSP_sys_time_ms(void) {
    return HAL_GetTick();
}

#endif