#include "spi_eio.h"

#include "bsp_delay.h"
#include "bsp_gpio.h"
#include "string.h"

std::vector<spi_eio*> spi_eio::spi_eio_instances;

spi_eio::spi_eio(const spi_eio_config& _config) : monitor_item(_config.lost_callback, 10, this) {
    config = _config;
    rx_data.resize(config.eio_rx_data_len);
    spi_eio_instances.push_back(this);
}

spi_eio::~spi_eio() {
    for (auto it = spi_eio_instances.begin(); it != spi_eio_instances.end(); ++it) {
        if (*it == this) {
            spi_eio_instances.erase(it);
        }
    }
}

void spi_eio::transmit(const uint8_t* data, uint32_t len) {
    static std::vector<uint8_t> send_buffer;
    send_buffer.resize(len);
    memcpy(send_buffer.data(), data, len);
    config.spi_device->transmit(send_buffer.data(), len, 100);
    bsp_delay_us(20);
    config.tx_control->set(0);  //将移位寄存器的数据载入到存储寄存器中
    bsp_delay_us(20);
    config.tx_control->set(1);  //更新输出状态（提供一个脉冲）
}

void spi_eio::transmit(const std::vector<uint8_t>& data) {
    transmit(data.data(), data.size());
}

void spi_eio::receive() {
    config.rx_control->set(0);  //读取数据到寄存器
    bsp_delay_us(20);
    config.rx_control->set(1);  //停止读取并允许移位
    bsp_delay_us(20);
    config.spi_device->receive(rx_data.data(), config.eio_rx_data_len, 100);  //接收，提供时钟信号，从机接收到时钟信号时开始移位，并发送数据给主机
}

//使用线程方式持续接收
void SPI_eio_Loop() {
    for (spi_eio* now : spi_eio::spi_eio_instances) {
        now->receive();
    }
}