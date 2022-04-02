#include <chassis.h>
#include <common.h>
#include <pub_sub.h>

#include <cmath>
// the radius of wheel(mm)，轮子半径
#define RADIUS 71  // 71.25

// the perimeter of wheel(mm)，轮子周长
#define PERIMETER 448  // 71.25*2pi

// wheel track distance(mm)，轮距
#define WHEELTRACK 340

// wheelbase distance(mm)，轴距
#define WHEELBASE 340

// gimbal is relative to chassis center x axis offset(mm)，云台相对于底盘中心的偏移，往右为正
#define ROTATE_X_OFFSET 0

// gimbal is relative to chassis center y axis offset(mm)，云台相对于底盘中心的偏移，往前为正
#define ROTATE_Y_OFFSET 0

// the deceleration ratio of chassis motor，底盘电机减速比
#define MOTOR_DECELE_RATIO 19.0f

#define RADIAN_COEF (180.0 / pi)

void chassis_imu_lost(void *imu) {
    //暂无作用
}
void chassis_supercap_lost(void *imu) {
    //暂无作用
}
void chassis_motor_lost(void *imu) {
    //暂无作用
}
chassis::chassis() : imu([&] {
                         bmi088_imu::bmi088_config config;
                         config.accel_gpio = &BSP_GPIO_Typedef::gpio_ports[GPIO_BMI088_ACCEL_NS];
                         config.gyro_gpio = &BSP_GPIO_Typedef::gpio_ports[GPIO_BMI088_GYRO_NS];
                         config.pwm_device = &BSP_PWM_Typedef::pwm_ports[PWM_BMI088_HEAT_PORT];
                         config.spi_device = &BSP_SPI_Typedef::spi_ports[SPI_BMI088_PORT];
                         config.temp_target = 55.0f;
                         config.lost_callback = chassis_imu_lost;
                         return config;
                     }()),
                     super_cap([&] {
                         super_cap_wuli::super_cap_wuli_config config;
                         config.can_device = &BSP_CanTypeDef::can_devices[0];
                         config.super_cap_wuli_rx_id = SUPER_CAP_WULI_RX_ID;
                         config.super_cap_wuli_tx_id = SUPER_CAP_WULI_TX_ID;
                         config.lost_callback = chassis_supercap_lost;
                         return config;
                     }()),
                     lf([&] {
                         can_motor::can_motor_config config;
                         config.motor_model = can_motor::MODEL_3508;
                         config.can_device = &BSP_CanTypeDef::can_devices[0];
                         config.motor_set_id = 4;
                         config.motor_pid_model = can_motor::SPEED_LOOP;
                         config.position_fdb_model = can_motor::MOTOR_FDB;
                         config.speed_fdb_model = can_motor::MOTOR_FDB;
                         config.position_pid_fdb = NULL;
                         config.speed_pid_fdb = NULL;
                         config.speed_feedforward = NULL;
                         config.current_feedforward = NULL;
                         config.config_position = PID::pos_pid_config(0, 0, 0, 0, 5000);
                         config.config_speed = PID::pos_pid_config(5, 0, 10, 0, 10000);
                         config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                         config.lost_callback = chassis_motor_lost;
                         return config;
                     }()),
                     rf([&] {
                         can_motor::can_motor_config config;
                         config.motor_model = can_motor::MODEL_3508;
                         config.can_device = &BSP_CanTypeDef::can_devices[0];
                         config.motor_set_id = 3;
                         config.motor_pid_model = can_motor::SPEED_LOOP;
                         config.position_fdb_model = can_motor::MOTOR_FDB;
                         config.speed_fdb_model = can_motor::MOTOR_FDB;
                         config.position_pid_fdb = NULL;
                         config.speed_pid_fdb = NULL;
                         config.speed_feedforward = NULL;
                         config.current_feedforward = NULL;
                         config.config_position = PID::pos_pid_config(0, 0, 0, 0, 5000);
                         config.config_speed = PID::pos_pid_config(5, 0, 10, 0, 10000);
                         config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                         config.lost_callback = chassis_motor_lost;
                         return config;
                     }()),
                     lb([&] {
                         can_motor::can_motor_config config;
                         config.motor_model = can_motor::MODEL_3508;
                         config.can_device = &BSP_CanTypeDef::can_devices[0];
                         config.motor_set_id = 1;
                         config.motor_pid_model = can_motor::SPEED_LOOP;
                         config.position_fdb_model = can_motor::MOTOR_FDB;
                         config.speed_fdb_model = can_motor::MOTOR_FDB;
                         config.position_pid_fdb = NULL;
                         config.speed_pid_fdb = NULL;
                         config.speed_feedforward = NULL;
                         config.current_feedforward = NULL;
                         config.config_position = PID::pos_pid_config(0, 0, 0, 0, 5000);
                         config.config_speed = PID::pos_pid_config(5, 0, 10, 0, 10000);
                         config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                         config.lost_callback = chassis_motor_lost;
                         return config;
                     }()),
                     rb([&] {
                         can_motor::can_motor_config config;
                         config.motor_model = can_motor::MODEL_3508;
                         config.can_device = &BSP_CanTypeDef::can_devices[0];
                         config.motor_set_id = 2;
                         config.motor_pid_model = can_motor::SPEED_LOOP;
                         config.position_fdb_model = can_motor::MOTOR_FDB;
                         config.speed_fdb_model = can_motor::MOTOR_FDB;
                         config.position_pid_fdb = NULL;
                         config.speed_pid_fdb = NULL;
                         config.speed_feedforward = NULL;
                         config.current_feedforward = NULL;
                         config.config_position = PID::pos_pid_config(0, 0, 0, 0, 5000);
                         config.config_speed = PID::pos_pid_config(5, 0, 10, 0, 10000);
                         config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                         config.lost_callback = chassis_motor_lost;
                         return config;
                     }()) {
    offset_x = ROTATE_X_OFFSET;
    offset_y = ROTATE_Y_OFFSET;
    cmd_data = NULL;
}

/**
 * @brief 正常运行模式下，麦轮底盘的解算函数
 * @param X方向的速度（向右为正，mm/s）,Y方向的速度（向前为正，mm/s）
 */
void chassis::mecanum_calculate(float vx, float vy, float rotate) {
    float r_x, r_y;
    float mecanum_speed[4];

    r_x = WHEELTRACK / 2 + offset_x;
    r_y = WHEELBASE / 2 - offset_y;
    mecanum_speed[0] = -vx - vy - rotate * (r_x + r_y) / RADIAN_COEF;
    r_x = WHEELTRACK / 2 - offset_x;
    r_y = WHEELBASE / 2 - offset_y;
    mecanum_speed[1] = -vx + vy - rotate * (r_x + r_y) / RADIAN_COEF;
    r_x = WHEELTRACK / 2 - offset_x;
    r_y = WHEELBASE / 2 + offset_y;
    mecanum_speed[2] = vx + vy - rotate * (r_x + r_y) / RADIAN_COEF;
    r_x = WHEELTRACK / 2 + offset_x;
    r_y = WHEELBASE / 2 + offset_y;
    mecanum_speed[3] = vx - vy - rotate * (r_x + r_y) / RADIAN_COEF;

    lf.speed_pid.ref = mecanum_speed[0] / PERIMETER * MOTOR_DECELE_RATIO * 360;  // rpm: *60  度/s: /360
    rf.speed_pid.ref = mecanum_speed[1] / PERIMETER * MOTOR_DECELE_RATIO * 360;
    rb.speed_pid.ref = mecanum_speed[2] / PERIMETER * MOTOR_DECELE_RATIO * 360;
    lb.speed_pid.ref = mecanum_speed[3] / PERIMETER * MOTOR_DECELE_RATIO * 360;
}

// 将基于offset的速度映射到实际底盘坐标系的方向上
void chassis::chassis_calculate() {
    float vx = cmd_data->speed.vx * cos(cmd_data->speed.offset_angle * DEG2RAD) - cmd_data->speed.vy * sin(cmd_data->speed.offset_angle * DEG2RAD);
    float vy = cmd_data->speed.vx * sin(cmd_data->speed.offset_angle * DEG2RAD) + cmd_data->speed.vy * cos(cmd_data->speed.offset_angle * DEG2RAD);
    if (cmd_data->mode == chassis_run)
        mecanum_calculate(vx, vy, cmd_data->speed.rotate);
    else if (cmd_data->mode == chassis_rotate_run) {
        float w = auto_rotate_param();
        mecanum_calculate(vx, vy, w);
    } else if (cmd_data->mode == chassis_run_follow_offset) {
        float w = 0.05f * (cmd_data->speed.offset_angle) * fabs(cmd_data->speed.offset_angle);  // 采用二次函数
        mecanum_calculate(vx, vy, w);
    }
    // 缓启动 斜坡
    // 功率控制
}

float chassis::auto_rotate_param() {
    return 150;  //可以改为变速
}

void chassis::update() {
    //检查imu在线并初始化完成
    if (!imu.monitor_item.is_online() || !imu.bias_init_success) return;

    //发送回传数据指针
    static publisher<upload_chassis *> chassis_puber("upload_chassis");
    upload_data.chassis_imu = &imu.data;
    upload_data.chassis_status = module_run;
    chassis_puber.push(&upload_data);

    //获得cmd命令
    static subscriber<cmd_chassis *> chassis_suber("cmd_chassis");
    if (chassis_suber.empty()) return;  //未收到指令
    cmd_data = chassis_suber.pop();

    if (cmd_data->mode == chassis_stop) {
        lf.stop();
        lb.stop();
        rf.stop();
        rb.stop();
    } else {
        lf.enable();
        lb.enable();
        rf.enable();
        rb.enable();
        chassis_calculate();
    }
}