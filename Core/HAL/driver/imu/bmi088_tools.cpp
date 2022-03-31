#include "bmi088.h"
#include "bmi088def.h"
#include "bmi088reg.h"
#include "bsp_gpio.h"
#include "bsp_spi.h"

uint8_t bmi088_imu::spi_send_recv_byte(uint8_t txdata) {
    uint8_t rx_data;
    config.spi_device->transmit_receive(&txdata, &rx_data, 1, 1000);
    return rx_data;
}

void bmi088_imu::spi_write_reg(uint8_t reg, uint8_t data) {
    spi_send_recv_byte(reg);
    spi_send_recv_byte(data);
}

void bmi088_imu::spi_read_multi(uint8_t reg, uint8_t* buf, uint8_t len) {
    spi_send_recv_byte(reg | 0x80);
    while (len != 0) {
        *buf = spi_send_recv_byte(reg | 0x80);
        ++buf;
        --len;
    }
}

void bmi088_imu::gyro_write(uint8_t reg, uint8_t data) {
    config.gyro_index->set(0);
    spi_write_reg(reg, data);
    config.gyro_index->set(1);
}
void bmi088_imu::gyro_read(uint8_t reg, uint8_t* buf) {
    config.gyro_index->set(0);
    spi_send_recv_byte(reg | 0x80);
    *buf = spi_send_recv_byte(0x55);
    config.gyro_index->set(1);
}
void bmi088_imu::gyro_read(uint8_t reg, uint8_t* buf, uint8_t len) {
    config.gyro_index->set(0);
    spi_read_multi(reg, buf, len);
    config.gyro_index->set(1);
}
void bmi088_imu::accel_write(uint8_t reg, uint8_t data) {
    config.gyro_index->set(0);
    spi_write_reg(reg, data);
    config.gyro_index->set(1);
}
void bmi088_imu::accel_read(uint8_t reg, uint8_t* buf) {
    config.gyro_index->set(0);
    spi_send_recv_byte(reg | 0x80);
    spi_send_recv_byte(0x55);
    *buf = spi_send_recv_byte(0x55);
    config.gyro_index->set(1);
}
void bmi088_imu::accel_read(uint8_t reg, uint8_t* buf, uint8_t len) {
    config.gyro_index->set(0);
    spi_send_recv_byte(reg | 0x80);
    spi_read_multi(reg, buf, len);
    config.gyro_index->set(1);
}