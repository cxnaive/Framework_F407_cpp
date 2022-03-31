#include "bmi088.h"

#include "bmi088def.h"
#include "bmi088reg.h"
#include "bsp_delay.h"
#include "bsp_log.h"
#include "bsp_random.h"
#include "common.h"
#include "stdio.h"
#include "string.h"
////BMI088全局配置
#define BMI088_ACCEL_SEN BMI088_ACCEL_3G_SEN
#define BMI088_GYRO_SEN BMI088_GYRO_2000_SEN
#define BMI088_BIAS_INIT_DISCARD 1000
#define BMI088_BIAS_INIT_COUNT 2000
//加热PID配置
#define HEAT_PID_KP 1600.0f        // kp of temperature control PID
#define HEAT_PID_KI 0.2f           // ki of temperature control PID
#define HEAT_PID_KD 0.0f           // kd of temperature control PID
#define HEAT_PID_MAX_OUT 1000.0f   // max out of temperature control PID
#define HEAT_PID_MAX_ERROR 900.0f  // max out of temperature control PID
// BMI088加速度计配置数组
static uint8_t BMI088_accel_config[BMI088_WRITE_ACCEL_REG_NUM][3] = {{BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON, BMI088_ACC_PWR_CTRL_ERROR},
                                                                     {BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE, BMI088_ACC_PWR_CONF_ERROR},
                                                                     {BMI088_ACC_CONF, BMI088_ACC_NORMAL | BMI088_ACC_800_HZ | BMI088_ACC_CONF_MUST_Set, BMI088_ACC_CONF_ERROR},
                                                                     {BMI088_ACC_RANGE, BMI088_ACC_RANGE_3G, BMI088_ACC_RANGE_ERROR},
                                                                     {BMI088_INT1_IO_CTRL, BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP | BMI088_ACC_INT1_GPIO_LOW, BMI088_INT1_IO_CTRL_ERROR},
                                                                     {BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT, BMI088_INT_MAP_DATA_ERROR}

};
// BMI088陀螺仪配置数组
static uint8_t BMI088_gyro_config[BMI088_WRITE_GYRO_REG_NUM][3] = {{BMI088_GYRO_RANGE, BMI088_GYRO_2000, BMI088_GYRO_RANGE_ERROR},
                                                                   {BMI088_GYRO_BANDWIDTH, BMI088_GYRO_1000_116_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set, BMI088_GYRO_BANDWIDTH_ERROR},
                                                                   {BMI088_GYRO_LPM1, BMI088_GYRO_NORMAL_MODE, BMI088_GYRO_LPM1_ERROR},
                                                                   {BMI088_GYRO_CTRL, BMI088_DRDY_ON, BMI088_GYRO_CTRL_ERROR},
                                                                   {BMI088_GYRO_INT3_INT4_IO_CONF, BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW, BMI088_GYRO_INT3_INT4_IO_CONF_ERROR},
                                                                   {BMI088_GYRO_INT3_INT4_IO_MAP, BMI088_GYRO_DRDY_IO_INT3, BMI088_GYRO_INT3_INT4_IO_MAP_ERROR}

};

std::vector<bmi088_imu*> bmi088_imu::bmi088_instances;

bmi088_imu::bmi088_imu(const bmi088_config& _config) : heat_pid(PID::pos_pid_config(HEAT_PID_KP, HEAT_PID_KI, HEAT_PID_KD, HEAT_PID_MAX_ERROR, HEAT_PID_MAX_OUT)),
                                                       mahony_solver(2 * 1.0, 2 * 0.0001, 500.0f),
                                                       madgwick_solver(0.2, 500.0f),
                                                       monitor_item(_config.lost_callback, 5, this) {
    init_error = BMI088_NO_ERROR;
    bias_init_success = 0;
    do {
        accel_init();
        gyro_init();
    } while (init_error);
    heat_init();
    memset(gyrobias, 0, sizeof(float) * 3);
    bmi088_instances.push_back(this);
}

bmi088_imu::~bmi088_imu(){
    for(auto it = bmi088_instances.begin();it != bmi088_instances.end();++it){
        if(*it == this){
            bmi088_instances.erase(it);
            return;
        }
    }
}

//加速度计初始化
void bmi088_imu::accel_init() {
    uint8_t res = 0;
    // dummy read
    accel_read(BMI088_ACC_CHIP_ID, &res);
    bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    // software reset
    accel_write(BMI088_ACC_SOFTRESET, BMI088_ACC_SOFTRESET_VALUE);
    bsp_delay_ms(100);
    // check normal

    // dummy read
    accel_read(BMI088_ACC_CHIP_ID, &res);
    bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    // get chip ID
    accel_read(BMI088_ACC_CHIP_ID, &res);
    bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    if (res != BMI088_ACC_CHIP_ID_VALUE) {
        init_error |= BMI088_NO_SENSOR;
        return;
    }

    for (int i = 0; i < BMI088_WRITE_ACCEL_REG_NUM; ++i) {
        accel_write(BMI088_accel_config[i][0], BMI088_accel_config[i][1]);
        bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        accel_read(BMI088_accel_config[i][0], &res);
        bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
        if (res != BMI088_accel_config[i][1]) {
            init_error |= BMI088_accel_config[i][2];
            return;
        }
    }
    printf_log("BMI088 accel init success!\n");
}

// BMI088陀螺仪初始化
void bmi088_imu::gyro_init() {
    uint8_t res = 0;
    gyro_read(BMI088_GYRO_CHIP_ID, &res);
    bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    gyro_write(BMI088_GYRO_SOFTRESET, BMI088_GYRO_SOFTRESET_VALUE);
    bsp_delay_ms(100);

    gyro_read(BMI088_GYRO_CHIP_ID, &res);
    bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

    if (res != BMI088_GYRO_CHIP_ID_VALUE) {
        init_error |= BMI088_NO_SENSOR;
        return;
    }
    for (int i = 0; i < BMI088_WRITE_GYRO_REG_NUM; ++i) {
        gyro_write(BMI088_gyro_config[i][0], BMI088_gyro_config[i][1]);
        bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);

        gyro_read(BMI088_gyro_config[i][0], &res);
        bsp_delay_us(BMI088_COM_WAIT_SENSOR_TIME);
        if (res != BMI088_gyro_config[i][1]) {
            init_error |= BMI088_gyro_config[i][2];
            return;
        }
    }
    printf_log("BMI088 gyro init success!\n");
}

// BMI088温控初始化
void bmi088_imu::heat_init() {
    config.pwm_device->pwm_start();
    heat_pid.ref = config.temp_target;
}

void bmi088_imu::read_raw() {
    uint8_t buf[8] = {0};
    int16_t tmp;

    //加速度
    accel_read(BMI088_ACCEL_XOUT_L, buf, 6);
    tmp = (int16_t)((buf[1]) << 8) | buf[0];
    data.accel[0] = tmp * BMI088_ACCEL_SEN;
    tmp = (int16_t)((buf[3]) << 8) | buf[2];
    data.accel[1] = tmp * BMI088_ACCEL_SEN;
    tmp = (int16_t)((buf[5]) << 8) | buf[4];
    data.accel[2] = tmp * BMI088_ACCEL_SEN;

    //陀螺仪
    gyro_read(BMI088_GYRO_CHIP_ID, buf, 8);
    tmp = (int16_t)((buf[3]) << 8) | buf[2];
    data.gyro[0] = tmp * BMI088_GYRO_SEN;
    tmp = (int16_t)((buf[5]) << 8) | buf[4];
    data.gyro[1] = tmp * BMI088_GYRO_SEN;
    tmp = (int16_t)((buf[7]) << 8) | buf[6];
    data.gyro[2] = tmp * BMI088_GYRO_SEN;

    //温度
    accel_read(BMI088_TEMP_M, buf, 2);
    tmp = (int16_t)((buf[0] << 3) | (buf[1] >> 5));
    if (tmp > 1023) tmp -= 2048;
    temperature = tmp * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
}

void bmi088_imu::heat_control() {
    heat_pid.fdb = temperature;
    heat_pid.calc();
    config.pwm_device->set_ccr(heat_pid.output);
}

void bmi088_imu::update() {
    read_raw();
    heat_control();
    monitor_item.reset();
    if (temperature < config.temp_target - 2) {
        bias_init_success = 0;
        return;
    }
    if (!bias_init_success) {
        static uint16_t init_count = 0;
        ++init_count;
        if (init_count < BMI088_BIAS_INIT_DISCARD)
            return;
        else if (init_count < BMI088_BIAS_INIT_COUNT + BMI088_BIAS_INIT_DISCARD) {
            gyrobias[0] += data.gyro[0];
            gyrobias[1] += data.gyro[1];
            gyrobias[2] += data.gyro[2];
        } else if (init_count == BMI088_BIAS_INIT_COUNT + BMI088_BIAS_INIT_DISCARD) {
            gyrobias[0] /= BMI088_BIAS_INIT_COUNT;
            gyrobias[1] /= BMI088_BIAS_INIT_COUNT;
            gyrobias[2] /= BMI088_BIAS_INIT_COUNT;
            bias_init_success = 1;
        } else {
            init_count = 0;
        }
        return;
    }
    data.gyro[0] -= gyrobias[0];
    data.gyro[1] -= gyrobias[1];
    data.gyro[2] -= gyrobias[2];
    mahony_solver.update(data.gyro[0], data.gyro[1], data.gyro[2], data.accel[0], data.accel[1], data.accel[2]);
    data.update_euler(mahony_solver.euler);
}

void Imu_Loop() {
    for (auto imu : bmi088_imu::bmi088_instances) {
        imu->update();
    }
}