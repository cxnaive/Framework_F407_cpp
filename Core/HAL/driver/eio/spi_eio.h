#ifndef _SPI_EIO_H
#define _SPI_EIO_H

#include <vector>

#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "monitor.h"
#include "warpper.h"

struct spi_eio {
   public:
    struct spi_eio_config {
        BSP_SPI_Typedef* spi_device;
        BSP_GPIO_Typedef* tx_control;  //发送控制引脚
        BSP_GPIO_Typedef* rx_control;  //接收控制引脚
        // int eio_tx_data_len;           //发送数据所用字节数
        int eio_rx_data_len;  //接收数据所用字节数

        monitor::lost_callback lost_callback;
    };

   private:
    spi_eio_config config;
    static std::vector<spi_eio*> spi_eio_instances;

   public:
    friend void SPI_eio_Loop();
    std::vector<uint8_t> rx_data;
    monitor monitor_item;
    spi_eio(const spi_eio_config& _config);
    ~spi_eio();
    void transmit(const uint8_t* data, uint32_t len);
    void transmit(const std::vector<uint8_t>& data);
    void receive();
};

// void Spi_EIO_Transmit(spi_EIO* obj);
// void Spi_EIO_Receive(spi_EIO* obj);
// spi_EIO* Spi_EIO_Create(spi_eio_config* config);

#endif
