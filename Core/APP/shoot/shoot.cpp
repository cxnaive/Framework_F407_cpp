#include <bsp_time.h>
#include <pub_sub.h>
#include <shoot.h>
//摩擦轮半径(mm)
#define RADIUS 30

//摩擦轮周长(mm)
#define PERIMETER 94.25f  // 30 * 2 * pi

//拨弹电机减速比
#define MOTOR_DECELE_RATIO 36.0f

//拨弹轮每转一圈发弹数(个)
#define NUM_PER_CIRCLE 8

//每发射一颗小弹增加的热量
#define UNIT_HEAT_17MM 10

//每发射一颗大弹增加的热量
#define UNIT_HEAT_42MM 100

void shoot_motor_lost(void *motor) {
    //暂时没有作用
}

shoot::shoot() : friction_a([&] {
                     can_motor::can_motor_config config;
                     config.motor_model = can_motor::MODEL_3508;
                     config.can_device = &BSP_CanTypeDef::can_devices[0];
                     config.motor_set_id = 1;
                     config.motor_pid_model = can_motor::SPEED_LOOP;
                     config.position_fdb_model = can_motor::MOTOR_FDB;
                     config.speed_fdb_model = can_motor::MOTOR_FDB;
                     config.config_position = PID::pos_pid_config(2, 0, 0, 0, 5000);
                     config.config_speed = PID::pos_pid_config(4, 0.015, 0.8, 2000, 5000);
                     config.current_feedforward = NULL;
                     config.speed_feedforward = NULL;
                     config.speed_pid_fdb = NULL;
                     config.position_pid_fdb = NULL;
                     config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                     config.lost_callback = shoot_motor_lost;
                     return config;
                 }()),
                 friction_b([&] {
                     can_motor::can_motor_config config;
                     config.motor_model = can_motor::MODEL_3508;
                     config.can_device = &BSP_CanTypeDef::can_devices[0];
                     config.motor_set_id = 2;
                     config.motor_pid_model = can_motor::SPEED_LOOP;
                     config.position_fdb_model = can_motor::MOTOR_FDB;
                     config.speed_fdb_model = can_motor::MOTOR_FDB;
                     config.config_position = PID::pos_pid_config(2, 0, 0, 0, 5000);
                     config.config_speed = PID::pos_pid_config(4, 0.015, 0.8, 2000, 5000);
                     config.current_feedforward = NULL;
                     config.speed_feedforward = NULL;
                     config.speed_pid_fdb = NULL;
                     config.position_pid_fdb = NULL;
                     config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                     config.lost_callback = shoot_motor_lost;
                     return config;
                 }()),
                 load([&] {
                     can_motor::can_motor_config config;
                     config.motor_model = can_motor::MODEL_2006;
                     config.can_device = &BSP_CanTypeDef::can_devices[0];
                     config.motor_set_id = 3;
                     config.motor_pid_model = can_motor::POSITION_LOOP;
                     config.position_fdb_model = can_motor::MOTOR_FDB;
                     config.speed_fdb_model = can_motor::MOTOR_FDB;
                     config.config_position = PID::pos_pid_config(2, 0, 0, 0, 5000);
                     config.config_speed = PID::pos_pid_config(20, 0, 0, 2000, 600);
                     config.current_feedforward = NULL;
                     config.speed_feedforward = NULL;
                     config.speed_pid_fdb = NULL;
                     config.position_pid_fdb = NULL;
                     config.output_model = can_motor::MOTOR_OUTPUT_NORMAL;
                     config.lost_callback = shoot_motor_lost;
                     return config;
                 }()) {
    cmd_data = NULL;
    cooldown_start = cooldown_time = 0;
}

void shoot::update() {
    subscriber<cmd_shoot *> shoot_suber("cmd_shoot");
    if (shoot_suber.empty()) return;  //没有控制信息

    cmd_data = shoot_suber.pop();
    if (cmd_data->friction_mode == friction_stop) {
        load.stop();
        friction_a.stop();
        friction_b.stop();
    } else {
        load.enable();
        friction_a.enable();
        friction_b.enable();
        switch (cmd_data->bullet_speed) {
            case 30:
                // 弹速27.0-27.4的实测ref
                friction_a.speed_pid.ref = 41300;
                friction_b.speed_pid.ref = -41300;
                break;
            case 18:
                //待实测
                friction_a.speed_pid.ref = 18785;
                friction_b.speed_pid.ref = -18785;
            case 15:
                //待实测
                friction_a.speed_pid.ref = 15470;
                friction_b.speed_pid.ref = -15470;
            default:
                //待实测
                friction_a.speed_pid.ref = 15470;
                friction_b.speed_pid.ref = -15470;
                break;
        }
        load_update();
    }
}

void shoot::load_update() {
    if (cmd_data == NULL) return;
    if (cmd_data->heat_limit_remain < UNIT_HEAT_17MM) {
        cmd_data->bullet_mode = bullet_stop;
    }
    uint32_t time_now = BSP_sys_time_ms();
    static int load_delta_pos = 8192 * MOTOR_DECELE_RATIO / NUM_PER_CIRCLE;
    //仍然在冷却时间
    if (time_now < cooldown_start + cooldown_time) return;
    switch (cmd_data->bullet_mode) {
        case bullet_stop:
            //快速停止
            load.config.motor_pid_model = can_motor::POSITION_LOOP;
            load.position_pid.ref = load.real_position;  //待在原地
            break;
        case bullet_reverse:  //反转 解决卡弹
            load.config.motor_pid_model = can_motor::SPEED_LOOP;
            load.speed_pid.ref = 10 * 360 * MOTOR_DECELE_RATIO / NUM_PER_CIRCLE;
            break;
        case bullet_continuous:
            load.config.motor_pid_model = can_motor::SPEED_LOOP;
            load.speed_pid.ref = -cmd_data->fire_rate * 360 * MOTOR_DECELE_RATIO / NUM_PER_CIRCLE;
            break;
        case bullet_single:
            load.config.motor_pid_model = can_motor::POSITION_LOOP;
            load.position_pid.ref = load.real_position - load_delta_pos;
            cooldown_start = time_now;
            cooldown_time = 200;  //待测试
            break;
        case bullet_double:
            load.config.motor_pid_model = can_motor::POSITION_LOOP;
            load.position_pid.ref = load.real_position - load_delta_pos * 2;
            cooldown_start = time_now;
            cooldown_time = 300;  //待测试
            break;
        case bullet_trible:
            load.config.motor_pid_model = can_motor::POSITION_LOOP;
            load.position_pid.ref = load.real_position - load_delta_pos * 3;
            cooldown_start = time_now;
            cooldown_time = 400;  //待测试
            break;
        default:
            break;
    }
}