#include "bsp_spi.h"

BSP_SPI_Typedef BSP_SPI_Typedef::spi_ports[DEVICE_SPI_CNT];
void BSP_SPI_Typedef::init(SPI_HandleTypeDef *_port) {
    port = _port;
}
void BSP_SPI_Typedef::transmit_receive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout) {
    HAL_SPI_TransmitReceive(port, pTxData, pRxData, Size, Timeout);
}
void BSP_SPI_Typedef::transmit(uint8_t *pTxData, uint16_t Size, uint32_t Timeout) {
    HAL_SPI_Transmit(port, pTxData, Size, Timeout);
}
void BSP_SPI_Typedef::receive(uint8_t *pRxData, uint16_t Size, uint32_t Timeout) {
    HAL_SPI_Receive(port, pRxData, Size, Timeout);
}

void BSP_SPI_Init() {
    BSP_SPI_Typedef::spi_ports[0].init(&hspi1);
}