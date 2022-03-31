#include "usart.h"
#ifdef __cplusplus
extern "C" {
#endif

//此处定义所有的提供给C接口的函数

//主要是需要在freertos/定时器中断内循环调用的函数

//初始化函数
void main_init();
// 串口DMA接收处理
void BSP_UART_IRQHandler(UART_HandleTypeDef* huart);
// 监视器loop
void Monitor_Loop();
// 蜂鸣器loop
void Buzzer_Loop();
// 陀螺仪loop
void Imu_Loop();
// 超级电容Loop
void Supercap_wuli_Loop();
// SPI扩展IO Loop
void SPI_eio_Loop();
// 电机Loop
void Motor_Loop();
// APP层总Loop
void APP_Loop();
#ifdef __cplusplus
};
#endif