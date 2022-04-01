#ifndef __BMI088_H
#define __BMI088_H
// #include "stm32f4xx_hal.h"
#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"
#include "bsp_gpio.h"
#include "bsp_pwm.h"
#include "bsp_spi.h"
#include "common.h"
#include "imu_data.h"
#include "monitor.h"
#include "pid.h"
#include "stdint.h"
#include "warpper.h"

class bmi088_imu {
   public:
    struct bmi088_config {
        BSP_SPI_Typedef* spi_device;
        BSP_PWM_Typedef* pwm_device;
        BSP_GPIO_Typedef* accel_gpio;
        BSP_GPIO_Typedef* gyro_gpio;
        float temp_target;
        monitor::lost_callback lost_callback;
    };

   private:
    float temperature;
    bmi088_config config;
    PID heat_pid;
    MahonyAHRS mahony_solver;
    MadgwickAHRS madgwick_solver;
    static std::vector<bmi088_imu*> bmi088_instances;
    void accel_init();
    void gyro_init();
    void heat_init();

    uint8_t spi_send_recv_byte(uint8_t txdata);
    void spi_write_reg(uint8_t reg, uint8_t data);
    void spi_read_multi(uint8_t reg, uint8_t* buf, uint8_t len);
    void gyro_write(uint8_t reg, uint8_t data);
    void gyro_read(uint8_t reg, uint8_t* buf);
    void gyro_read(uint8_t reg, uint8_t* buf, uint8_t len);
    void accel_write(uint8_t reg, uint8_t data);
    void accel_read(uint8_t reg, uint8_t* buf);
    void accel_read(uint8_t reg, uint8_t* buf, uint8_t len);
    void update();
    void read_raw();
    void heat_control();

   public:
    friend void Imu_Loop();
    uint8_t init_error;
    uint8_t bias_init_success;
    imu_data data;
    float gyrobias[3];
    monitor monitor_item;

    bmi088_imu(const bmi088_config& _config);
    ~bmi088_imu();
};

// void BMI088_Driver_Init();
// void BMI088_Update_All();
// // Public methods
// BMI088_imu* BMI088_Create(BMI088_config* config);
// void BMI088_Update(BMI088_imu* obj);

#endif