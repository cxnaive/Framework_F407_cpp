#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H
#include "gpio.h"
#include "stdint.h"

#define DEVICE_GPIO_CNT 2
//此处可以预定义一些接口ID
#define GPIO_BMI088_ACCEL_NS 0
#define GPIO_BMI088_GYRO_NS 1

class BSP_GPIO_Typedef {
   private:
    GPIO_TypeDef* base;
    uint16_t pin;

   public:
    void init(GPIO_TypeDef* _base, uint16_t _pin);
    uint8_t read(uint8_t* data = nullptr);
    void set(uint8_t status);
    //静态数据成员
    //定义要使用的io口
    static BSP_GPIO_Typedef gpio_ports[DEVICE_GPIO_CNT];
};

void BSP_GPIO_Init();

#endif