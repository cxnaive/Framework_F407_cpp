#include "bsp_pwm.h"

void BSP_PWM_Typedef::init(TIM_HandleTypeDef* _base, uint16_t _channel) {
    base = _base;
    channel = _channel;
}

void BSP_PWM_Typedef::pwm_start() {
    HAL_TIM_PWM_Start(base, channel);
}

void BSP_PWM_Typedef::pwm_stop() {
    HAL_TIM_PWM_Stop(base, channel);
}

void BSP_PWM_Typedef::start_ccr_dma(uint32_t* ccr_data, uint16_t len) {
    HAL_TIM_PWM_Start_DMA(base, channel, ccr_data, len);
}

void BSP_PWM_Typedef::set_ccr(uint32_t ccr_value) {
    __HAL_TIM_SetCompare(base, channel, ccr_value);
}

void BSP_PWM_Typedef::set_arr(uint32_t arr_value) {
    __HAL_TIM_SetAutoreload(base, arr_value);
}

BSP_PWM_Typedef BSP_PWM_Typedef::pwm_ports[DEVICE_PWM_CNT];

extern DMA_HandleTypeDef hdma_tim1_ch1;
void BSP_PWM_Init() {
    BSP_PWM_Typedef::pwm_ports[0].init(&htim10, TIM_CHANNEL_1);
    BSP_PWM_Typedef::pwm_ports[1].init(&htim4, TIM_CHANNEL_3);
    BSP_PWM_Typedef::pwm_ports[2].init(&htim1, TIM_CHANNEL_1);

    //处理HAL库DMA BUG
    HAL_DMA_DeInit(&hdma_tim1_ch1);
    HAL_DMA_Init(&hdma_tim1_ch1);
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim) {
    for (size_t i = 0; i < DEVICE_PWM_CNT; ++i) {
        if (htim == BSP_PWM_Typedef::pwm_ports[i].base) {
            HAL_TIM_PWM_Stop_DMA(BSP_PWM_Typedef::pwm_ports[i].base, BSP_PWM_Typedef::pwm_ports[i].channel);
        }
    }
}