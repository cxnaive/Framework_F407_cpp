#include "bsp_gpio.h"

BSP_GPIO_Typedef BSP_GPIO_Typedef::gpio_ports[DEVICE_GPIO_CNT];
void BSP_GPIO_Typedef::init(GPIO_TypeDef* _base, uint16_t _pin) {
    base = _base;
    pin = _pin;
}

void BSP_GPIO_Init() {
    BSP_GPIO_Typedef::gpio_ports[0].init(GPIOA, GPIO_PIN_4);
    BSP_GPIO_Typedef::gpio_ports[1].init(GPIOB, GPIO_PIN_0);
}

void BSP_GPIO_Typedef::set(uint8_t status) {
    HAL_GPIO_WritePin(base, pin, (GPIO_PinState)status);
}

uint8_t BSP_GPIO_Typedef::read(uint8_t* data) {
    uint8_t result = HAL_GPIO_ReadPin(base, pin);
    if (data != nullptr) *data = result;
    return result;
}