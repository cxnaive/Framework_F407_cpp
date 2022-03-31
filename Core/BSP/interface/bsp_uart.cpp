#include "bsp_uart.h"
BSP_UART_Typedef BSP_UART_Typedef::uart_ports[DEVICE_UART_CNT];

void BSP_UART_Init() {
    BSP_UART_Typedef::uart_ports[0].init(&huart3);
    BSP_UART_Typedef::uart_ports[0].init(&huart6);
}

void BSP_UART_Typedef::init(UART_HandleTypeDef *_port) {
    port = _port;
    call_backs.reserve(5);
    // HAL库的BUG处理，对于DMA需要先DeInit再Init，不然GG
    HAL_DMA_DeInit(port->hdmatx);
    HAL_DMA_DeInit(port->hdmarx);
    HAL_DMA_Init(port->hdmatx);
    HAL_DMA_Init(port->hdmarx);
    HAL_UART_DMAStop(port);
    //使能串口空闲中断
    __HAL_UART_ENABLE_IT(port, UART_IT_IDLE);
    //开启DMA接收
    HAL_UART_Receive_DMA(port, rx_buff, BSP_UART_DMA_BUFF_SIZE);
}

// 注册回调函数
void BSP_UART_Typedef::register_rx_callback(void *identifier, uart_rx_callback func) {
    call_backs.insert({identifier, func});
}

void BSP_UART_Typedef::unregister_rx_callback(void *identifier) {
    call_backs.erase(identifier);
}

void BSP_UART_Typedef::send_blocking(uint8_t *data, uint16_t len, uint32_t timeout) {
    HAL_UART_Transmit(port, data, len, timeout);
}
void BSP_UART_Typedef::send_it(uint8_t *data, uint16_t len) {
    HAL_UART_Transmit_IT(port, data, len);
}
void BSP_UART_Typedef::send_dma(uint8_t *data, uint16_t len) {
    HAL_UART_Transmit_DMA(port, data, len);
}

/**
 * @brief 串口空闲中断（中断回调）函数
 * @param 串口号
 * @retval None
 * @note  放在"stm32f4xx_it.c"里形如"void USART2_IRQHandler(void)"类的函数中，只要用了DMA接收的串口都放
 * 具体位置：系统调用的HAL_UART_IRQHandler函数下面，"USER CODE BEGIN USART1_IRQn 1"和"USER CODE END USART1_IRQn 1"两行注释之间
 */
void BSP_UART_IRQHandler(UART_HandleTypeDef *huart) {
    for (size_t uart_index = 0; uart_index < DEVICE_UART_CNT; ++uart_index) {
        // 判断串口
        if (huart != BSP_UART_Typedef::uart_ports[uart_index].port) continue;
        //判断是否是空闲中断
        if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
            __HAL_UART_CLEAR_IDLEFLAG(huart);  //清除空闲中断标志（否则会一直不断进入中断）
            // 下面进行空闲中断相关处理
            HAL_UART_DMAStop(huart);                                                              //暂时停止本次DMA传输，进行数据处理
            uint8_t data_length = BSP_UART_DMA_BUFF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);  //计算接收到的数据长度
            //调用回调函数对数据进行处理
            for (auto &func_pair : BSP_UART_Typedef::uart_ports[uart_index].call_backs) {
                auto &func = func_pair.second;
                if (func) {
                    func(&BSP_UART_Typedef::uart_ports[uart_index], BSP_UART_Typedef::uart_ports[uart_index].rx_buff, data_length);
                }
            }
            HAL_UART_Receive_DMA(huart, BSP_UART_Typedef::uart_ports[uart_index].rx_buff, BSP_UART_DMA_BUFF_SIZE);  //重启开始DMA传输 每次255字节数据
            return;
        }
    }
}