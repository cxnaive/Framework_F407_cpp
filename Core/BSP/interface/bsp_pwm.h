#ifndef _BSP_TIM_H
#define _BSP_TIM_H
#include "dma.h"
#include "stdint.h"
#include "tim.h"
//此处可以预定义一些接口ID
#define PWM_BMI088_HEAT_PORT 0
#define PWM_BUZZER_PORT 1
#define DEVICE_PWM_CNT 3

class BSP_PWM_Typedef {
   private:
    TIM_HandleTypeDef* base;
    uint16_t channel;

   public:
    void init(TIM_HandleTypeDef* _base, uint16_t _channel);
    void pwm_start();
    void pwm_stop();
    void start_ccr_dma(uint32_t* ccr_data, uint16_t len);
    void set_ccr(uint32_t ccr_value);
    void set_arr(uint32_t arr_value);
    //友元函数
    friend void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim);
    //定义实例
    static BSP_PWM_Typedef pwm_ports[DEVICE_PWM_CNT];
};

void BSP_PWM_Init();
#endif