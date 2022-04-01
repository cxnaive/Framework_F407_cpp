#include <gimbal.h>
#include <pid.h>
#include <pub_sub.h>

// 云台水平并朝向底盘正前方时云台和底盘的编码器值
#define PITCH_MOTOR_ENCORDER_BIAS 841
#define YAW_MOTOR_ENCORDER_BIAS 3153
// 云台抬头/低头限位
#define PITCH_ENCORDER_HIGHEST (PITCH_MOTOR_ENCORDER_BIAS + 500)
#define PITCH_ENCORDER_LOWEST (PITCH_MOTOR_ENCORDER_BIAS - 500)

void gimbal_motor_lost(void* motor) {
    //暂时没有效用。
}

void gimbal_imu_lost(void* imu) {
    //暂时没有效用。
}

gimbal::gimbal() : imu([&] {
                       bmi088_imu::bmi088_config config;
                       config.accel_gpio = &BSP_GPIO_Typedef::gpio_ports[GPIO_BMI088_ACCEL_NS];
                       config.gyro_gpio = &BSP_GPIO_Typedef::gpio_ports[GPIO_BMI088_GYRO_NS];
                       config.pwm_device = &BSP_PWM_Typedef::pwm_ports[PWM_BMI088_HEAT_PORT];
                       config.spi_device = &BSP_SPI_Typedef::spi_ports[SPI_BMI088_PORT];
                       config.temp_target = 55.0f;
                       config.lost_callback = gimbal_imu_lost;
                       return config;
                   }()),
                   pitch([&] {
                       can_motor::can_motor_config config;
                       config.motor_model = can_motor::MODEL_6020;
                       config.can_device = &BSP_CanTypeDef::can_devices[0];
                       config.motor_set_id = 1;
                       config.motor_pid_model = can_motor::POSITION_LOOP;
                       config.position_fdb_model = can_motor::OTHER_FDB;
                       config.position_pid_fdb = &(imu.data.euler_8192[0]);  //根据实际情况 使用陀螺仪的Roll
                       config.speed_fdb_model = can_motor::OTHER_FDB;
                       config.speed_pid_fdb = &(imu.data.gyro_deg[0]);         // Roll轴角速度
                       config.output_model = can_motor::MOTOR_OUTPUT_REVERSE;  //陀螺仪正方向与电机正方向不一致，输出取反
                       config.lost_callback = gimbal_motor_lost;
                       config.current_feedforward = NULL;
                       config.speed_feedforward = NULL;
                       config.config_position = PID::pos_pid_config(1.4, 0.003, 1.6, 2500, 5000);
                       config.config_speed = PID::pos_pid_config(200, 0.7, 20, 5000, 25000);
                       return config;
                   }()),
                   yaw([&] {
                       can_motor::can_motor_config config;
                       config.motor_model = can_motor::MODEL_6020;
                       config.can_device = &BSP_CanTypeDef::can_devices[1];
                       config.motor_set_id = 1;
                       config.motor_pid_model = can_motor::POSITION_LOOP;
                       config.position_fdb_model = can_motor::OTHER_FDB;
                       config.position_pid_fdb = &(imu.data.yaw_8192_real);  // 陀螺仪模式反馈值更新 需参照C板实际安装方向 此处使用陀螺仪yaw轴Z
                       config.speed_fdb_model = can_motor::OTHER_FDB;
                       config.speed_pid_fdb = &(imu.data.gyro_deg[2]);  // Yaw轴角速度
                       config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                       config.lost_callback = gimbal_motor_lost;
                       config.current_feedforward = NULL;
                       config.speed_feedforward = NULL;
                       config.config_position = PID::pos_pid_config(1.6, 0.014, 0.1, 3200, 5000);
                       config.config_speed = PID::pos_pid_config(235, 3, 10, 2000, 22000);
                       return config;
                   }()) {
}

void gimbal::update() {
    // imu掉线检测，imu初始化完成检测
    if (!imu.monitor_item.is_online() || !imu.bias_init_success) return;

    // 在imu初始化成功之后回传云台信息
    static publisher<upload_gimbal*> gimbal_uploader("upload_gimbal");
    upload_data.gimbal_status = module_run;
    upload_data.gimbal_imu = &(imu.data);
    upload_data.yaw_encoder = &yaw.fdbPosition;
    gimbal_uploader.push(&upload_data);

    //获取cmd传来的指令
    static cmd_gimbal* cmd_data;
    static subscriber<cmd_gimbal*> gimbal_suber("cmd_gimbal");
    if (gimbal_suber.empty())
        return;  //目前没有指令
    else
        cmd_data = gimbal_suber.pop();

    switch (cmd_data->gimbal_mode) {
        case gimbal_stop:
            yaw.stop();
            pitch.stop();
            break;
        case gimbal_run:
            yaw.enable();
            pitch.enable();
            //切换陀螺仪控制
            yaw.config.speed_fdb_model = can_motor::OTHER_FDB;
            yaw.config.position_fdb_model = can_motor::OTHER_FDB;
            pitch.config.speed_fdb_model = can_motor::OTHER_FDB;
            pitch.config.position_fdb_model = can_motor::OTHER_FDB;
            yaw.position_pid.ref = cmd_data->yaw;
            pitch.position_pid.ref = cmd_data->pitch;
            break;
        case gimbal_middle:
            yaw.enable();
            pitch.enable();
            //切换编码器控制
            yaw.config.speed_fdb_model = can_motor::MOTOR_FDB;
            yaw.config.position_fdb_model = can_motor::MOTOR_FDB;
            pitch.config.speed_fdb_model = can_motor::MOTOR_FDB;
            pitch.config.position_fdb_model = can_motor::MOTOR_FDB;
            yaw.position_pid.ref = YAW_MOTOR_ENCORDER_BIAS;
            pitch.position_pid.ref = PITCH_MOTOR_ENCORDER_BIAS;
            break;
        default:
            break;
    }
}