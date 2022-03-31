#ifndef _BSP_UART_H
#define _BSP_UART_H
#include <functional>
#include <unordered_map>
#include <vector>

#include "stdint.h"
#include "usart.h"
#include "warpper.h"

#define UART_REMOTE_PORT 0
#define UART_REFEREE_PORT 1
#define DEVICE_UART_CNT 2
#define BSP_UART_DMA_BUFF_SIZE 512  // DMA缓冲区大小，一次传输数据不应超过该长度

class BSP_UART_Typedef {
   public:
    typedef std::function<void(BSP_UART_Typedef* uart_device, uint8_t* data, uint32_t len)> uart_rx_callback;

   private:
    UART_HandleTypeDef* port;  //自定义uart编号
    std::unordered_map<void*, uart_rx_callback> call_backs;

   public:
    uint8_t rx_buff[BSP_UART_DMA_BUFF_SIZE];  // DMA接收数组，为了存放DMA搬运的串口数据
    void init(UART_HandleTypeDef* port);
    void register_rx_callback(void* identifier, uart_rx_callback func);
    void unregister_rx_callback(void* identifier);
    void send_blocking(uint8_t* data, uint16_t len, uint32_t timeout);
    void send_it(uint8_t* data, uint16_t len);
    void send_dma(uint8_t* data, uint16_t len);
    //友元函数
    friend void BSP_UART_IRQHandler(UART_HandleTypeDef* huart);
    static BSP_UART_Typedef uart_ports[DEVICE_UART_CNT];
};

void BSP_UART_Init();

#endif