#ifndef _BSP_SPI_H
#define _BSP_SPI_H
#include "spi.h"
#include "stdint.h"
#define DEVICE_SPI_CNT 1
//此处可以预定义一些接口ID
#define SPI_BMI088_PORT 0

class BSP_SPI_Typedef {
   private:
    SPI_HandleTypeDef *port;

   public:
    void init(SPI_HandleTypeDef *_port);
    void transmit_receive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
    void transmit(uint8_t *pTxData, uint16_t Size, uint32_t Timeout);
    void receive(uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
    static BSP_SPI_Typedef spi_ports[DEVICE_SPI_CNT];
};

void BSP_SPI_Init();

#endif